#pragma once
#include <string_view>

#include "../logger/logger.h"

namespace errorHandler {

namespace beast = boost::beast;

void ErrorLog(beast::error_code ec, std::string_view what);

}  // namespace errorHandler