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

namespace {
    // Константы для индексов компонентов пути
    constexpr size_t kApiIndex = 0;
    constexpr size_t kVersionIndex = 1;
    constexpr size_t kMapsIndex = 2;
    constexpr size_t kMapIdIndex = 3;

    // Константы для размеров
    constexpr size_t kExpectedPathSizeForMap = 4;
    constexpr size_t kMinPathSizeForBadRequest = 3;
    constexpr size_t kMaxPathSizeForBadRequest = 4;
}

template <typename Body, typename Allocator>
bool IsGetMapListRequest(
    const http::request<Body, http::basic_fields<Allocator>>& req) {
    return (req.target() == "/api/v1/maps") || (req.target() == "/api/v1/maps/");
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
    bool isValidPath = (pathComponents.size() == kExpectedPathSizeForMap) &&
                       (pathComponents[kApiIndex] == "api") &&
                       (pathComponents[kVersionIndex] == "v1") &&
                       (pathComponents[kMapsIndex] == "maps");
    bool mapExists = (game.FindMap(model::Map::Id(std::string(pathComponents[kMapIdIndex]))) != nullptr);
    return isValidPath && mapExists;
}

template <typename Body, typename Allocator>
StringResponse HandleGetMapById(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const model::Game& game) {
    StringResponse response(http::status::ok, req.version());
    auto id = SplitStr(req.target())[kMapIdIndex];
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
    bool isApiPath = (!pathComponents.empty()) && (pathComponents[kApiIndex] == "api");
    bool isInvalidSize = (pathComponents.size() > kMaxPathSizeForBadRequest) ||
                         (pathComponents.size() < kMinPathSizeForBadRequest);
    bool isInvalidVersion = (pathComponents.size() >= 2) && (pathComponents[kVersionIndex] != "v1");
    bool isInvalidMaps = (pathComponents.size() >= 3) && (pathComponents[kMapsIndex] != "maps");
    return isApiPath && (isInvalidSize || isInvalidVersion || isInvalidMaps);
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
    bool isValidPath = (pathComponents.size() == kExpectedPathSizeForMap) &&
                       (pathComponents[kApiIndex] == "api") &&
                       (pathComponents[kVersionIndex] == "v1") &&
                       (pathComponents[kMapsIndex] == "maps");
    bool mapNotFound = (game.FindMap(model::Map::Id(std::string(pathComponents[kMapIdIndex]))) == nullptr);
    return isValidPath && mapNotFound;
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
