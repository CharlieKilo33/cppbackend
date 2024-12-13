#include "../model/game_session.h"

namespace randomgen {

double RandomDouble(const double thl, const double thh) {
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<double> distr(thl, thh);
  return distr(eng);
};

int RandomInt(const int thl, const int thh) {
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_int_distribution<int> distr(thl, thh);
  return distr(eng);
};
}  // namespace randomgen

namespace model {

const GameSession::Id& GameSession::GetId() const noexcept { return id_; }

const std::shared_ptr<model::Map> GameSession::GetMap() { return map_; };

std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() { return strand_; };
}  // namespace model