#include "../ticker/error_handler.h"

namespace errorHandler {

using namespace std::literals;

void ErrorLog(beast::error_code ec, std::string_view where) {
  BOOST_LOG_TRIVIAL(error) << logger::CreateLogMessage(
      "error"sv, logger::ExceptionLogData(0, ec.what(), where));
}

}  // namespace errorHandler