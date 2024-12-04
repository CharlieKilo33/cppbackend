#pragma once

#include <string>

#include "../model/model.h"

namespace jsonOperation {

std::string SerializeGameToJson(const model::Game& game);
std::string SerializeMapToJson(const model::Map& map);
std::string CreatePageNotFoundResponse();
std::string CreateBadRequestResponse();
std::string CreateMapNotFoundResponse();

}  // namespace jsonOperation
