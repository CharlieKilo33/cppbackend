#include "dog.h"

namespace model {

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

}  // namespace model