#include <iomanip>
#include <sstream>

#include "../model/player_tokens.h"

namespace auth {

const size_t HALF_TOKEN_SIZE = 16;

Token PlayerTokens::AddPlayer(std::weak_ptr<app::Player> player) {
  std::stringstream ss;
  ss << std::setw(HALF_TOKEN_SIZE) << std::setfill('0') << std::hex << generator1_();
  ss << std::setw(HALF_TOKEN_SIZE) << std::setfill('0') << std::hex << generator2_();
  Token token{ss.str()};
  tokenToPlayer_[token] = player;
  return token;
};

std::weak_ptr<app::Player> PlayerTokens::FindPlayerByToken(Token token) {
  if (!tokenToPlayer_.contains(token)) {
    return std::weak_ptr<app::Player>();
  }
  return tokenToPlayer_[token];
};

}  // namespace auth