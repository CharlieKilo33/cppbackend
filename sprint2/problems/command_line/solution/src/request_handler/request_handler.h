#pragma once
#include <boost/beast/http.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>

#include "../app/app.h"
#include "../request_handler/api_request_handler.h"
#include "../request_handler/static_file_request_handler.h"
#include "../server/http_server.h"

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;

class RequestHandler {
 public:
  explicit RequestHandler(app::Application& application,
                          std::filesystem::path staticContentPath)
      : application_{application}, staticContentPath_{staticContentPath} {}

  RequestHandler(const RequestHandler&) = delete;
  RequestHandler& operator=(const RequestHandler&) = delete;

  template <typename Body, typename Allocator, typename Send>
  void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
    if (requestHandler::ApiRequestHandlerProxy<
            http::request<Body, http::basic_fields<Allocator>>, Send>::GetInstance()
            .Execute(req, application_, std::move(send))) {
      return;
    } else if (requestHandler::StaticFileRequestHandlerProxy<
                   http::request<Body, http::basic_fields<Allocator>>,
                   Send>::GetInstance()
                   .Execute(req, staticContentPath_, std::move(send))) {
      return;
    };
    requestHandler::PageNotFound(req, application_, send);
  }

 private:
  app::Application& application_;
  std::filesystem::path staticContentPath_;
};

}  // namespace http_handler

