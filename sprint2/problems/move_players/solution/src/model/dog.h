#pragma once
#include <string>
#include <unordered_map>

#include "../other/tagged.h"

namespace model {

enum class Direction { NORTH, SOUTH, WEST, EAST, UNKNOWN };

const std::unordered_map<Direction, std::string> DIRECTION_TO_JSON = {
    {Direction::NORTH, "U"},
    {Direction::SOUTH, "D"},
    {Direction::WEST, "L"},
    {Direction::EAST, "R"},
    {Direction::UNKNOWN, ""}};

const std::unordered_map<std::string, Direction> JSON_TO_DIRECTION = {
    {"U", Direction::NORTH},
    {"D", Direction::SOUTH},
    {"L", Direction::WEST},
    {"R", Direction::EAST},
    {"", Direction::UNKNOWN}};

struct Position {
  double x, y;
};

struct Speed {
  double vx, vy;
};

class Dog {
  inline static size_t cntMaxId = 0;

 public:
  using Id = util::Tagged<size_t, Dog>;

  Dog(std::string name) : id_(Id{Dog::cntMaxId++}), name_(name) {};

  Dog(Id id, std::string name) : id_(id), name_(name) {};

  /*Кострукторы копирования все дефолтные*/
  Dog(const Dog& other) = default;
  Dog(Dog&& other) = default;
  Dog& operator=(const Dog& other) = default;
  Dog& operator=(Dog&& other) = default;
  virtual ~Dog() = default;

  const Id& GetId() const;
  const std::string& GetName() const;

  const Direction GetDirection() const;
  void SetDirection(Direction direction);

  const Position& GetPosition() const;
  void SetPosition(Position position);

  const Speed& GetSpeed() const;
  void SetSpeed(Speed velocity);

  void Move(Direction direction, double speed);

 private:
  Id id_;
  std::string name_;
  Direction direction_{Direction::NORTH};
  Position position_{0.0, 0.0};
  Speed speed_{0.0, 0.0};
};

}  // namespace model