#pragma once
#include <random>
#include <string_view>
#include <vector>

namespace requestHandler {

	std::vector<std::string_view> SplitStr(std::string_view str);
	std::string GetBearerToken(std::string_view bearer_string);

}//namespace requestHandler

namespace randomgen {

	double RandomDouble(const double thl, const double thh);
	int RandomInt(const int thl, const int thh);

}//namespace random