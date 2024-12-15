#include "http_server.h"

#include <boost/asio/dispatch.hpp>
#include <iostream>

namespace http_server {

void ErrorMessage(beast::error_code ec, std::string_view w) {
  BOOST_LOG_TRIVIAL(error) << logger::CreateLogMessage(
      "error"sv, logger::ExceptionLog(0, ec.what(), w));
}

void SessionBase::Run() {
  net::dispatch(stream_.get_executor(),
                beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
}

SessionBase::SessionBase(tcp::socket&& socket) : stream_(std::move(socket)) {}

void SessionBase::Read() {
  using namespace std::literals;

  request_ = {};
  stream_.expires_after(std::chrono::seconds(20));
  http::async_read(stream_, buffer_, request_,
                   beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
}

const std::string& SessionBase::GetRemoteIp() {
  static std::string remote_ip;
  try {
    auto temp = stream_.socket().remote_endpoint().address().to_string();
    remote_ip = temp;
  } catch (const boost::system::system_error& e) {
    BOOST_LOG_TRIVIAL(error) << logger::CreateLogMessage(
        "error"sv, logger::ExceptionLog(0, e.what(), "Remote Ip Error"sv));
  }
  return remote_ip;
};

void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
  using namespace std::literals;
  if (ec == http::error::end_of_stream) {
    return Close();
  }
  if (ec) {
    return ErrorMessage(ec, "Read Error"sv);
  }
  HandleRequest(std::move(request_));
}

void SessionBase::Close() {
  BOOST_LOG_TRIVIAL(info) << logger::CreateLogMessage(
      "info"sv, logger::ExceptionLog(0, "Client connection closed", ""));
  stream_.socket().shutdown(tcp::socket::shutdown_send);
}

void SessionBase::OnWrite(bool close, beast::error_code ec,
                          [[maybe_unused]] std::size_t bytes_written) {
  if (ec) {
    return ErrorMessage(ec, "Write Error"sv);
  }

  if (close) {
    return Close();
  }

  Read();
}

void SessionBase::SetReqRecieveTime(const boost::posix_time::ptime& reqTime) {
  reqRecieveTime_ = reqTime;
}

int64_t SessionBase::GetDurReceivedRequest(const boost::posix_time::ptime& currTime) {
  boost::posix_time::time_duration dur = currTime - reqRecieveTime_;
  return dur.total_milliseconds();
}

}  // namespace http_server