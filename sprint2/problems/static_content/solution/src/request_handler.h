#pragma once

#include <filesystem>

#include "http_server.h"
#include "model.h"
#include "response_file_handler.h"
#include "response_handler.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace fs = std::filesystem;

class RequestHandler {
 public:
  explicit RequestHandler(model::Game& game, fs::path staticPath)
      : game_{game}, staticPath_{fs::weakly_canonical(staticPath)} {}

  RequestHandler(const RequestHandler&) = delete;
  RequestHandler& operator=(const RequestHandler&) = delete;

  template <typename Body, typename Allocator, typename Send>
  void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
    assert(&game_ != nullptr);
    assert(!staticPath_.empty());

    try {
      // Проверка на неверный запрос
      if (responseHandler::IsApiRequest(req)) {
        if (responseHandler::IsBadRequest(req)) {
          send(responseHandler::HandleBadRequest(req));
        }
        // Проверка на запрос списка карт
        else if (responseHandler::IsGetMapListRequest(req)) {
          if (req.method() == http::verb::get) {
            send(responseHandler::HandleGetMapList(req, game_));
          } else {
            send(responseHandler::HandleBadRequest(req));
          }
        }
        // Проверка на запрос карты по ID
        else if (responseHandler::IsGetMapByIdRequest(req, game_)) {
          if (req.method() == http::verb::get) {
            send(responseHandler::HandleGetMapById(req, game_));
          } else {
            send(responseHandler::HandleBadRequest(req));
          }
        }
        // Проверка на случай, если карта не найдена
        else if (responseHandler::IsMapNotFoundRequest(req, game_)) {
          if (req.method() == http::verb::get) {
            send(responseHandler::HandleMapNotFound(req));
          } else {
            send(responseHandler::HandleBadRequest(req));
          }
        } else {
          send(responseHandler::HandlePageNotFound(req));
        }
      } else {
        if (responseHandler::StaticContentFileNotFoundCheck(req, staticPath_)) {
          if (req.method() == http::verb::get || req.method() == http::verb::head)
            send(responseHandler::StaticContentFileNotFound(req, staticPath_));
          else
            send(responseHandler::HandleBadRequest(req));
        } else if (responseHandler::LeaveStaticContentRootDirCheck(req, staticPath_)) {
          if (req.method() == http::verb::get || req.method() == http::verb::head)
            send(responseHandler::LeaveStaticContentRootDir(req, staticPath_));
          else
            send(responseHandler::HandleBadRequest(req));
        } else if (responseHandler::GetStaticContentFileCheck(req, staticPath_)) {
          if (req.method() == http::verb::get || req.method() == http::verb::head)
            send(responseHandler::GetStaticContentFile(req, staticPath_));
          else
            send(responseHandler::HandleBadRequest(req));
        }
        // Обработка случая, если страница не найдена
        else {
          send(responseHandler::HandlePageNotFound(req));
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "Error processing HTTP request: " << e.what() << std::endl;
      send(responseHandler::HandleBadRequest(req));
    }
  }

 private:
  model::Game& game_;
  fs::path staticPath_;
};

}  // namespace http_handler
