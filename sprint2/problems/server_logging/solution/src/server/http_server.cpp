#include "http_server.h"

#include <boost/asio/dispatch.hpp>

namespace http_server {
void SessionBase::Run() {
  // Вызываем метод Read, используя executor объекта stream_.
  // Таким образом вся работа со stream_ будет выполняться, используя его
  // executor
  net::dispatch(stream_.get_executor(),
                beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
}

void SessionBase::Read() {
  using namespace std::literals;
  // Очищаем запрос от прежнего значения (метод Read может быть вызван
  // несколько раз)
  request_ = {};
  stream_.expires_after(30s);
  // Считываем request_ из stream_, используя buffer_ для хранения считанных
  // данных
  http::async_read(stream_, buffer_, request_,
                   // По окончании операции будет вызван метод OnRead
                   beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
}

void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
  using namespace std::literals;
  if (ec == http::error::end_of_stream) {
    // Нормальная ситуация - клиент закрыл соединение
    return Close();
  }
  if (ec) {
    LOG_ERROR(ec.value(), ec.message(), "read");
    return ReportError(ec, "read"sv);
  }
  std::string ip(stream_.socket().remote_endpoint().address().to_string());
  std::string uri(request_.target());
  std::string method(request_.method_string());
  LOG_REQUEST_RECEIVED(ip, uri, method);
  response_timer_.Start();
  HandleRequest(std::move(request_));
}

void SessionBase::Close() {
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
  if (ec) {
    ReportError(ec, ec.message());
  }
}

// void SessionBase::OnWrite(bool close, beast::error_code ec,
//                           [[maybe_unused]] std::size_t bytes_written) {
//   if (ec) {
//     LOG_ERROR(ec.value(), ec.message(), "write");
//     return ReportError(ec, "write"sv);
//   }

//   if (close) {
//     // Семантика ответа требует закрыть соединение
//     return Close();
//   }

//   // Считываем следующий запрос
//   std::string ip(stream_.socket().remote_endpoint(Server has).address().to_string());
//   std::string content_type(safe_response->at(http::field::content_type));
//   LOG_RESPONSE_SENT(ip, response_timer_.End(),
//   static_cast<int>(safe_response->result()),
//                     content_type);
//   Read();
// }

}  // namespace http_server
