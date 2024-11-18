#pragma once
#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <memory>

#include "hotdog.h"
#include "result.h"

namespace net = boost::asio;
using namespace std::chrono;
using namespace std::literals;
using Timer = net::steady_timer;

// Функция-обработчик операции приготовления хот-дога
using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;

class ThreadChecker {
   public:
    explicit ThreadChecker(std::atomic_int& counter) : counter_{counter} {}

    ThreadChecker(const ThreadChecker&) = delete;
    ThreadChecker& operator=(const ThreadChecker&) = delete;

    ~ThreadChecker() {
        // assert выстрелит, если между вызовом конструктора и деструктора
        // значение expected_counter_ изменится
        assert(expected_counter_ == counter_);
    }

   private:
    std::atomic_int& counter_;
    int expected_counter_ = ++counter_;
};

class Logger {
   public:
    explicit Logger(std::string id) : id_(std::move(id)) {}

    void LogMessage(std::string_view message) const {
        std::osyncstream os{std::cout};
        os << id_ << "> ["sv
           << duration<double>(steady_clock::now() - start_time_).count()
           << "s] "sv << message << std::endl;
    }

   private:
    std::string id_;
    steady_clock::time_point start_time_{steady_clock::now()};
};

class Order : public std::enable_shared_from_this<Order> {
   public:
    Order(int id, net::io_context& io, Store& store_, GasCooker& gas_cooker,
          HotDogHandler handler)
        : id_{id},
          io_{io},
          hot_dog_handler_{std::move(handler)},
          bread_(store_.GetBread()),
          sausage_(store_.GetSausage()),
          gas_cooker_(gas_cooker) {}

    // Запускает асинхронное выполнение заказа
    void Execute() {
        logger_.LogMessage("Order has been started."sv);
        FrySausage();
        BakeBread();
    }

   private:
    net::io_context& io_;
    int id_;
    HotDogHandler hot_dog_handler_;
    Logger logger_{std::to_string(id_)};
    Timer bread_timer_{io_};
    Timer sausage_timer_{io_};
    std::shared_ptr<Bread> bread_;
    std::shared_ptr<Sausage> sausage_;
    GasCooker& gas_cooker_;
    std::atomic_int counter_{0};

    void BakeBread() {
        logger_.LogMessage("Start baking Bread"sv);
        bread_->StartBake(gas_cooker_, [self = shared_from_this(),
                                        bread = bread_] {
            self->bread_timer_.expires_from_now(1000ms);
            self->bread_timer_.async_wait([self, bread](sys::error_code ec) {
                bread->StopBaking();
                self->OnBaking(ec);
            });
        });
    }

    void OnBaking(sys::error_code ec) {
        ThreadChecker checker{counter_};
        if (ec) {
            logger_.LogMessage("Baking bread error : "s + ec.what());
        } else {
            logger_.LogMessage("Bread has been baked."sv);
        }
        CheckReadiness(ec);
    }

    void FrySausage() {
        logger_.LogMessage("Start frying sausage"sv);
        sausage_->StartFry(gas_cooker_,
                           [self = shared_from_this(), sausage = sausage_]() {
                               self->sausage_timer_.expires_from_now(1500ms);
                               self->sausage_timer_.async_wait(
                                   [self, sausage](sys::error_code ec) {
                                       sausage->StopFry();
                                       self->OnFrySausage(ec);
                                   });
                           });
    }

    void OnFrySausage(sys::error_code ec) {
        ThreadChecker checker{counter_};
        if (ec) {
            logger_.LogMessage("Frying sausage error: "s + ec.what());
        } else {
            logger_.LogMessage("Sausage has been fried."sv);
        }
        CheckReadiness(ec);
    }

    void CheckReadiness(sys::error_code ec) {
        if (bread_->IsCooked() && sausage_->IsCooked()) {
            hot_dog_handler_(Result{HotDog{id_, sausage_, bread_}});
        }
    }
};

// Класс "Кафетерий". Готовит хот-доги
class Cafeteria {
   public:
    explicit Cafeteria(net::io_context& io) : io_{io} {}

    // Асинхронно готовит хот-дог и вызывает handler, как только хот-дог будет
    // готов. Этот метод может быть вызван из произвольного потока
    void OrderHotDog(HotDogHandler handler) {
        net::dispatch(strand_, [this, handler] {
            std::make_shared<Order>(++next_order_id_, io_, store_, *gas_cooker_,
                                    std::move(handler))
                ->Execute();
        });
    }

   private:
    net::io_context& io_;
    // Используется для создания ингредиентов хот-дога
    Store store_;
    using Strand = net::strand<net::io_context::executor_type>;
    Strand strand_{net::make_strand(io_)};
    // Газовая плита. По условию задачи в кафетерии есть только одна газовая
    // плита на 8 горелок Используйте её для приготовления ингредиентов
    // хот-дога. Плита создаётся с помощью make_shared, так как GasCooker
    // унаследован от enable_shared_from_this.
    std::shared_ptr<GasCooker> gas_cooker_ = std::make_shared<GasCooker>(io_);
    int next_order_id_ = 0;
};
