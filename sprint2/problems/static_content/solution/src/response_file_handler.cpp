#include "response_file_handler.h"

namespace responseHandler {
bool IsSubPath(std::filesystem::path path, std::filesystem::path base) {
  path = std::filesystem::weakly_canonical(path);
  base = std::filesystem::weakly_canonical(base);

  for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
    if (p == path.end() || *p != *b) {
      return false;
    }
  }
  return true;
}

std::string URLDecode(const std::string& src) {
  std::string dest;
  dest.reserve(src.length());

  for (size_t i = 0; i < src.length(); ++i) {
    if (src[i] == '%') {
      if (i + 2 < src.length()) {
        std::string hex = src.substr(i + 1, 2);
        char decoded_char = static_cast<char>(std::stoi(hex, nullptr, 16));
        dest += decoded_char;
        i += 2;
      } else {
        dest += src[i];
      }
    } else if (src[i] == '+') {
      dest += ' ';
    } else {
      dest += src[i];
    }
  }

  return dest;
}

std::string ToLowerCase(const std::string& str) {
  std::string lower_str = str;
  std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lower_str;
}

}  // namespace responseHandler
