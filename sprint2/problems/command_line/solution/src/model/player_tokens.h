#pragma once
#include <memory>
#include <random>
#include <string>
#include <unordered_map>

#include "../model/player.h"
#include "../other/tagged.h"

namespace auth {

struct TokenTag {};  // Пустышка для таггеда

using Token = util::Tagged<std::string, TokenTag>;
using TokenHasher = util::TaggedHasher<Token>;

class PlayerTokens {
 public:
  /*Все конструкторы дефолтные*/
  PlayerTokens() = default;
  PlayerTokens(const PlayerTokens& other) = default;
  PlayerTokens(PlayerTokens&& other) = default;
  PlayerTokens& operator=(const PlayerTokens& other) = default;
  PlayerTokens& operator=(PlayerTokens&& other) = default;
  virtual ~PlayerTokens() = default;

  Token AddPlayer(std::weak_ptr<app::Player> player);  // Добавить игорька
  std::weak_ptr<app::Player> FindPlayerByToken(Token token);  // Найти по токену

 private:
  std::unordered_map<Token, std::weak_ptr<app::Player>, TokenHasher>
      tokenToPlayer_;  // Тут храним указатели на игорьков по токенам

  /*Тут всё для рандомайзера*/
  std::random_device random_device_;
  std::mt19937_64 generator1_{[this] {
    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
    return dist(random_device_);
  }()};
  std::mt19937_64 generator2_{[this] {
    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
    return dist(random_device_);
  }()};
};

}  // namespace auth