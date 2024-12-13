#include "../model/dog.h"

namespace model {

bool operator==(const Speed& lhs, const Speed& rhs) {
  return lhs.vx == rhs.vx && lhs.vy == rhs.vy;
}

const double CONVERT_MS_TO_S = 1000.0;

const Dog::Id& Dog::GetId() const { return id_; };

const std::string& Dog::GetName() const { return name_; };

void Dog::SetDirection(Direction direction) { direction_ = std::move(direction); };

const Direction Dog::GetDirection() const { return direction_; };

void Dog::SetPosition(Position position) { position_ = std::move(position); };

const Position& Dog::GetPosition() const { return position_; };

void Dog::SetSpeed(Speed velocity) { speed_ = velocity; };

const Speed& Dog::GetSpeed() const { return speed_; };

void Dog::Move(Direction direction, double speed) {
  switch (direction) {
    case Direction::NORTH: {
      SetDirection(direction);
      SetSpeed({0, -speed});
      break;
    }
    case Direction::SOUTH: {
      SetDirection(direction);
      SetSpeed({0, speed});
      break;
    }
    case Direction::WEST: {
      SetDirection(direction);
      SetSpeed({-speed, 0});
      break;
    }
    case Direction::EAST: {
      SetDirection(direction);
      SetSpeed({speed, 0});
      break;
    }
    case Direction::UNKNOWN: {
      SetSpeed({0, 0});
      break;
    }
  }
};

Position Dog::CalculateNewPosition(const std::chrono::milliseconds& diff_time) {
  Position pos = GetPosition();
  const Speed& speed = GetSpeed();
  pos.x += speed.vx * diff_time.count() / CONVERT_MS_TO_S;  // 1000 - перевод из мс в с
  pos.y += speed.vy * diff_time.count() / CONVERT_MS_TO_S;
  return pos;
};

}  // namespace model