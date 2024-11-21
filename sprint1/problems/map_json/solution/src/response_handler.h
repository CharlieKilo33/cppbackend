#pragma once

#include <boost/beast/http.hpp>
#include <vector>

#include "json_handler.h"
#include "model.h"

namespace responseHandler {

namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>;

std::vector<std::string_view> SplitStr(std::string_view str);

template <typename Body, typename Allocator>
bool IsGetMapListRequest(
    const http::request<Body, http::basic_fields<Allocator>>& req) {
    return (req.target() == "/api/v1/maps") ||
           (req.target() == "/api/v1/maps/");
}

template <typename Body, typename Allocator>
StringResponse HandleGetMapList(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const model::Game& game) {
    StringResponse response(http::status::ok, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = jsonOperation::SerializeGameToJson(game);
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
}

template <typename Body, typename Allocator>
bool IsGetMapByIdRequest(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const model::Game& game) {
    auto pathComponents = SplitStr(req.target());
    return (pathComponents.size() == 4) && (pathComponents[0] == "api") &&
           (pathComponents[1] == "v1") && (pathComponents[2] == "maps") &&
           (game.FindMap(model::Map::Id(std::string(pathComponents[3]))) !=
            nullptr);
}

template <typename Body, typename Allocator>
StringResponse HandleGetMapById(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const model::Game& game) {
    StringResponse response(http::status::ok, req.version());
    auto id = SplitStr(req.target())[3];
    response.set(http::field::content_type, "application/json");
    response.body() = jsonOperation::SerializeMapToJson(
        *game.FindMap(model::Map::Id(std::string(id))));
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
}

template <typename Body, typename Allocator>
bool IsBadRequest(
    const http::request<Body, http::basic_fields<Allocator>>& req) {
    auto pathComponents = SplitStr(req.target());
    return (!pathComponents.empty()) && (pathComponents[0] == "api") &&
           ((pathComponents.size() > 4) || (pathComponents.size() < 3) ||
            ((pathComponents.size() >= 2) && (pathComponents[1] != "v1")) ||
            ((pathComponents.size() >= 3) && (pathComponents[2] != "maps")));
}

template <typename Body, typename Allocator>
StringResponse HandleBadRequest(
    const http::request<Body, http::basic_fields<Allocator>>& req) {
    StringResponse response(http::status::bad_request, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = jsonOperation::CreateBadRequestResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
}

template <typename Body, typename Allocator>
bool IsMapNotFoundRequest(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const model::Game& game) {
    auto pathComponents = SplitStr(req.target());
    return (pathComponents.size() == 4) && (pathComponents[0] == "api") &&
           (pathComponents[1] == "v1") && (pathComponents[2] == "maps") &&
           (game.FindMap(model::Map::Id(std::string(pathComponents[3]))) ==
            nullptr);
}

template <typename Body, typename Allocator>
StringResponse HandleMapNotFound(
    const http::request<Body, http::basic_fields<Allocator>>& req) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = jsonOperation::CreateMapNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
}

template <typename Body, typename Allocator>
StringResponse HandlePageNotFound(
    const http::request<Body, http::basic_fields<Allocator>>& req) {
    StringResponse response(http::status::not_found, req.version());
    response.set(http::field::content_type, "application/json");
    response.body() = jsonOperation::CreatePageNotFoundResponse();
    response.content_length(response.body().size());
    response.keep_alive(req.keep_alive());
    return response;
}

}  // namespace responseHandler
