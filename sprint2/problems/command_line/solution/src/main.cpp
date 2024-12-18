#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <filesystem>
#include <iostream>
#include <thread>

#include "app/app.h"
#include "json/json_loader.h"
#include "logger/logger.h"
#include "other/sdk.h"
#include "request_handler/request_handler.h"
#include "ticker/command.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;

namespace {

// Запускает функцию fn на num_threads потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned num_threads, const Fn& fn) {
  num_threads = std::max(1u, num_threads);
  std::vector<std::jthread> workers;
  workers.reserve(num_threads - 1);
  // Запускаем num_threads-1 рабочих потоков, выполняющих функцию fn
  while (--num_threads) {
    workers.emplace_back(fn);
  }
  fn();
}

}  // namespace

int main(int argc, const char* argv[]) {
  logger::InitLogger();
  programm_option::Args args = programm_option::ParseCommandLine(argc, argv);
  try {
    // 1. Загружаем карту из файла и построить модель игры
    model::Game game = json_loader::LoadGame(args.config_file);
    // 2. Устанавливаем путь до статического контента.
    std::filesystem::path staticContentPath{args.www_root};
    // 3. Инициализируем io_context
    const unsigned num_threads = std::thread::hardware_concurrency();
    net::io_context ioc(num_threads);
    app::Application application(std::move(game), args.tick_period,
                                 args.randomize_spawn_points, ioc);

    // 4. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
          if (!ec) {
            BOOST_LOG_TRIVIAL(info) << logger::CreateLogMessage(
                "server was forcibly closed."sv, logger::ExitCodeLog(0));
            ioc.stop();
          }
        });

    // 5. Создаём обработчик HTTP-запросов и связываем его с моделью игры, задаем путь до
    // статического контента.
    http_handler::RequestHandler handler{application, staticContentPath};

    // 6. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
    const auto address = net::ip::make_address("0.0.0.0");
    constexpr net::ip::port_type port = 8080;
    http_server::ServeHttp(ioc, {address, port}, [&handler](auto&& req, auto&& send) {
      handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
    });

    // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
    BOOST_LOG_TRIVIAL(info) << logger::CreateLogMessage(
        "server started"sv, logger::ServerAddrPortLog(address.to_string(), port));

    // 7. Запускаем обработку асинхронных операций
    RunWorkers(std::max(1u, num_threads), [&ioc] { ioc.run(); });
  } catch (const std::exception& ex) {
    BOOST_LOG_TRIVIAL(error) << logger::CreateLogMessage(
        "error"sv, logger::ExceptionLog(EXIT_FAILURE, "Server not started"sv, ex.what()));
    return EXIT_FAILURE;
  }
}