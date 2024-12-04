#pragma once
#include <boost/beast/http.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace responseHandler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;
using StringResponse = http::response<http::string_body>;
using namespace std::literals;
namespace fs = std::filesystem;

const std::unordered_map<std::string, std::string> CONTENT_TYPE = {
    {".htm", "text/html"},       {".html", "text/html"},
    {".css", "text/css"},        {".txt", "text/plain"},
    {".js", "text/javascript"},  {".json", "application/json"},
    {".xml", "application/xml"}, {".png", "image/png"},
    {".jpg", "image/jpeg"},      {".jpe", "image/jpeg"},
    {".jpeg", "image/jpeg"},     {".gif", "image/gif"},
    {".bmp", "image/bmp"},       {".ico", "image/vnd.microsoft.icon"},
    {".tiff", "image/tiff"},     {".tif", "image/tiff"},
    {".svg", "image/svg+xml"},   {".svgz", "image/svg+xml"},
    {".mp3", "audio/mpeg"}};

const std::string INDEX_FILE_NAME{"index.html"};

bool IsSubPath(std::filesystem::path path, std::filesystem::path base);
std::string URLDecode(const std::string& src);
std::string ToLowerCase(const std::string& str);

template <typename Body, typename Allocator>
bool StaticContentFileNotFoundCheck(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const std::filesystem::path& static_base_path) {
  try {
    fs::path req_path(req.target().substr(1));
    req_path = fs::weakly_canonical(req_path);

    fs::path path(static_base_path);
    path /= req_path;

    bool exists = true;
    if (path.has_extension()) {
      if (!fs::exists(path)) {
        exists = false;
        std::cerr << "File does not exist" << std::endl;
      }
    }

    return !exists;
  } catch (const std::exception& e) {
    std::cerr << "Error checking static content file: " << e.what() << std::endl;
    return true;
  }
}

template <typename Body, typename Allocator>
StringResponse StaticContentFileNotFound(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const std::filesystem::path& static_base_path) {
  StringResponse response(http::status::not_found, req.version());
  response.set(http::field::content_type, "text/plain");
  response.body() = "Static content file not found.";
  response.content_length(response.body().size());
  response.keep_alive(req.keep_alive());
  return response;
};

template <typename Body, typename Allocator>
bool LeaveStaticContentRootDirCheck(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const std::filesystem::path& static_base_path) {
  if (static_base_path.empty()) {
    std::cerr << "static_base_path is empty. This should not happen." << std::endl;
    return false;
  }

  fs::path path(static_base_path);
  std::string_view req_target_str = req.target().substr(1);
  std::string req_target_decoded = URLDecode(req_target_str.data());
  std::string req_target_lower = ToLowerCase(req_target_decoded);
  fs::path req_path(req_target_lower);
  if (req_path.empty()) {
    std::cout << "Request path is empty" << std::endl;
    return false;
  }

  try {
    req_path = fs::weakly_canonical(path / req_path);
    bool is_subpath = IsSubPath(req_path, static_base_path);
    return !is_subpath;
  } catch (const std::exception& e) {
    std::cerr << "Error checking for leaving static content root directory: " << e.what()
              << std::endl;
    return false;
  }
};

template <typename Body, typename Allocator>
StringResponse LeaveStaticContentRootDir(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const std::filesystem::path& static_base_path) {
  StringResponse response(http::status::bad_request, req.version());
  response.set(http::field::content_type, "text/plain");
  response.body() = "Leave static content root directory.";
  response.content_length(response.body().size());
  response.keep_alive(req.keep_alive());
  return response;
};

template <typename Body, typename Allocator>
bool GetStaticContentFileCheck(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const std::filesystem::path& static_base_path) {
  return true;
};

template <typename Body, typename Allocator>
http::response<http::file_body> GetStaticContentFile(
    const http::request<Body, http::basic_fields<Allocator>>& req,
    const std::filesystem::path& static_base_path) {
  http::response<http::file_body> response;
  response.version(req.version());
  response.result(http::status::ok);

  fs::path path(static_base_path);
  fs::path req_path(req.target().substr(1));
  
  if (req_path.empty() || req_path == "/") {
    std::filesystem::path index_path{INDEX_FILE_NAME};
    path = std::filesystem::weakly_canonical(path / index_path);
  } else if (!req_path.has_extension() || IsSubPath(req_path, static_base_path)) {
    std::filesystem::path index_path{INDEX_FILE_NAME};
    path = std::filesystem::weakly_canonical(path / index_path);
  } else {
    path = fs::weakly_canonical(path / req_path);
  }

  if (CONTENT_TYPE.contains(path.extension().string())) {
    response.insert(http::field::content_type,
                    CONTENT_TYPE.at(path.extension().string()));
  } else {
    response.insert(http::field::content_type, "application/octet-stream");
  }

  http::file_body::value_type file;

  std::string path_str = path.string();
  const char* path_ptr = path_str.c_str();

  if (sys::error_code ec; file.open(path_ptr, beast::file_mode::read, ec), ec) {
    std::cout << "Error to open file "sv
              << std::endl;
  } else {
    response.body() = std::move(file);
  }
  response.prepare_payload();
  return response;
};

}  // namespace responseHandler