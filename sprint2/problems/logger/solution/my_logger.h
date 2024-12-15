#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "/var/log/sample_log_";
        ss << std::put_time(std::gmtime(&t_c), "%Y_%m_%d");
        ss << ".log";
        return ss.str();
    }

    template<typename T, class... Ts>
    void Log(std::stringstream& ss, T value, const Ts&... args) {
        ss << value;
        Log(ss, args...);
    }

    void Log(std::stringstream& ss) {
    }

    Logger() = default;
    Logger(const Logger&) = delete;

public:
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    // Выведите в поток все аргументы.
    template<class... Ts>
    void Log(const Ts&... args) {
        
        std::stringstream ss;
        
        ss << GetTimeStamp() << ": ";
        Log(ss, args...);
        
        const std::lock_guard<std::mutex> lock(mutex_);
        std::ofstream log_file_{GetFileTimeStamp(), std::ios::app };
        log_file_ << ss.str() << std::endl;
    }

    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts) {
        std::lock_guard<std::mutex> lock(mutex_);
        manual_ts_ = std::move(ts);
    };

private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    std::mutex mutex_;
};
