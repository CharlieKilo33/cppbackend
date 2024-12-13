#include "../model/player.h"
#include <random>

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

namespace model {

void Player::SetGameSession(std::shared_ptr<GameSession> session) { session_ = session; };

void Player::SetDog(const std::string& dog_name, const model::Map& map,
                    bool randomize_spawn_points) {
  dog_ = std::make_shared<model::Dog>(dog_name);
  if (randomize_spawn_points) {
    PutDogInRndPosition(map);
  } else {
    PutDogInStartPosition(map);
  }
};

const Player::Id& Player::GetId() const { return id_; };

const std::string& Player::GetName() const { return name_; };

const GameSession::Id& Player::GetSessionId() const { return session_->GetId(); };

std::shared_ptr<model::Dog> Player::GetDog() { return dog_; };

std::shared_ptr<GameSession> Player::GetSession() { return session_; };

void Player::MoveDog(const std::chrono::milliseconds& diff_time) {
  auto [new_pos, new_speed] = session_->GetMap()->GetMove(
      dog_->GetPosition(), dog_->CalculateNewPosition(diff_time), dog_->GetSpeed());
  dog_->SetPosition(new_pos);
  dog_->SetSpeed(new_speed);
};

void Player::PutDogInRndPosition(const model::Map& map) {
  double x, y;
  auto roads = map.GetRoads();
  int road_index = RandomInt(0, roads.size() - 1);
  auto road = roads[road_index];
  if (road.IsHorizontal()) {
    x = RandomDouble(road.GetStart().x, road.GetEnd().x);
    y = road.GetStart().y;
  } else {
    y = RandomDouble(road.GetStart().y, road.GetEnd().y);
    x = road.GetStart().x;
  }
  dog_->SetPosition({x, y});
};

void Player::PutDogInStartPosition(const model::Map& map) {
  double x, y;
  auto roads = map.GetRoads();
  auto road = roads[0];
  dog_->SetPosition(
      {static_cast<double>(road.GetStart().x), static_cast<double>(road.GetStart().y)});
};

}  // namespace model