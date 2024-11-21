#pragma once

#include "http_server.h"
#include "model.h"
#include "response_handler.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;

class RequestHandler {
   public:
    explicit RequestHandler(model::Game& game) : game_{game} {}

    // Запрет копирования
    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    // Обработка HTTP-запроса
    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req,
                    Send&& send) {
        // Проверка на неверный запрос
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
        }
        // Обработка случая, если страница не найдена
        else {
            send(responseHandler::HandlePageNotFound(req));
        }
    }

   private:
    model::Game& game_;  // Ссылка на объект игры
};

}  // namespace http_handler
