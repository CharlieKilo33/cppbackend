#include "../model/player.h"

namespace model {

void Player::SetGameSession(std::weak_ptr<GameSession> session) { session_ = session; };

void Player::SetDog(std::weak_ptr<Dog> dog) { dog_ = dog; };

const Player::Id& Player::GetId() const { return id_; };

const std::string& Player::GetName() const { return name_; };

const model::GameSession::Id& Player::GetSessionId() const {
  return session_.lock()->GetId();
};

std::weak_ptr<Dog> Player::GetDog() { return dog_; };

std::weak_ptr<GameSession> Player::GetSession() { return session_; };

void Player::MoveDog(const std::chrono::milliseconds& diff_time) {
  auto dog = dog_.lock();
  auto [new_pos, new_speed] = session_.lock()->GetMap()->GetMove(
      dog->GetPosition(), dog->CalculateNewPosition(diff_time), dog->GetSpeed());
  dog->SetPosition(new_pos);
  dog->SetSpeed(new_speed);
};

}  // namespace model