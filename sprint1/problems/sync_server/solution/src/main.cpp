#ifdef WIN32
#include <sdkddkver.h>
#endif
// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <optional>
#include <thread>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;

// Структура ContentType задаёт область видимости для констант,
// задающий значения HTTP-заголовка Content-Type
struct ContentType {
  ContentType() = delete;
  constexpr static std::string_view TEXT_HTML = "text/html"sv;
  // При необходимости внутрь ContentType можно добавить и другие типы контента
};

http::response<http::string_body> MakeStringResponse(
    http::status status, std::string_view body, unsigned http_version,
    bool keep_alive, bool bad_request = false,
    std::string_view content_type = ContentType::TEXT_HTML) {
  http::response<http::string_body> response(status, http_version);
  response.set(http::field::content_type, content_type);
  if (bad_request) {
    response.set(http::field::allow, "GET, HEAD");
  }
  response.body() = body;
  response.content_length(body.size());
  response.keep_alive(keep_alive);
  return response;
}

http::response<http::string_body> HandleRequest(
    http::request<http::string_body>&& req) {
  const auto text_response = [&req](http::status status, std::string_view text,
                                    bool bad_request) {
    return MakeStringResponse(status, text, req.version(), req.keep_alive(),
                              bad_request);
  };

  if (req.method() != http::verb::get && req.method() != http::verb::head) {
    return text_response(http::status::method_not_allowed, "Invalid method", true);
  }

  std::string text = "Hello, ";
  std::string target(req.target());
  text += target.erase(0, 1);
  return text_response(http::status::ok, text, false);
}

void DumpRequest(const http::request<http::string_body>& req) {
  std::cout << req.method_string() << ' ' << req.target() << std::endl;

  for (const auto& header : req) {
    std::cout << "  "sv << header.name_string() << ": "sv << header.value()
              << std::endl;
  }
}

std::optional<http::request<http::string_body>> ReadRequest(
    tcp::socket& socket, beast::flat_buffer& buffer) {
  beast::error_code ec;
  http::request<http::string_body> req;

  http::read(socket, buffer, req, ec);

  if (ec == http::error::end_of_stream) {
    return std::nullopt;
  }

  if (ec) {
    throw std::runtime_error("Failed to read request: "s.append(ec.message()));
  }
  return req;
}

template <typename RequestHandler>
void HandleConnection(tcp::socket& socket, RequestHandler&& handle_request) {
  try {
    beast::flat_buffer buffer;

    while (auto request = ReadRequest(socket, buffer)) {
      DumpRequest(*request);
      auto response = handle_request(*std::move(request));
      http::write(socket, response);

      if (response.need_eof()) {
        break;
      }
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  beast::error_code ec;
  socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
  // Выведите строчку "Server has started...", когда сервер будет готов
  // принимать подключения
  net::io_context ioc;

  const auto address = net::ip::make_address("0.0.0.0");
  constexpr unsigned short port = 8080;

  tcp::acceptor acceptor(ioc, {address, port});
  std::cout << "Server has started..."sv << std::endl;

  while (true) {
    tcp::socket socket(ioc);
    acceptor.accept(socket);

    std::thread t(
        [](tcp::socket socket) {
          // Вызываем HandleConnection, передавая ей функцию-обработчик
          // запроса
          HandleConnection(socket, HandleRequest);
        },
        std::move(socket));
    t.detach();
  }
}
