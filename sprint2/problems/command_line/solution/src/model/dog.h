#pragma once
#include <chrono>
#include <string>
#include <unordered_map>

#include "../other/tagged.h"

#pragma once

#include <string>
#include <unordered_map>

namespace model {

struct Point {
  int x, y;
};

struct Size {
  int width, height;
};

struct Rectangle {
  Point position;
  Size size;
};

struct Offset {
  int dx, dy;
};

enum class Direction { NORTH, SOUTH, WEST, EAST, UNKNOWN };

struct Position {
  double x, y;
};

struct Speed {
  double vx, vy;
};

bool operator==(const Speed& lhs, const Speed& rhs);

struct SpeedHasher {
  size_t operator()(const Speed& spd) const {
    size_t sd = 17;
    return std::hash<double>{}(spd.vy) * sd + std::hash<double>{}(spd.vx);
  }
};

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

const std::unordered_map<Speed, Direction, SpeedHasher> SPEED_TO_DIRECTION = {
    {{0, -1}, Direction::NORTH},
    {{0, 1}, Direction::SOUTH},
    {{-1, 0}, Direction::WEST},
    {{1, 0}, Direction::EAST},
    {{0, 0}, Direction::UNKNOWN}};

const std::unordered_map<Direction, Direction> OPOSITE_DIRECTION = {
    {Direction::NORTH, Direction::SOUTH},
    {Direction::SOUTH, Direction::NORTH},
    {Direction::WEST, Direction::EAST},
    {Direction::EAST, Direction::WEST},
    {Direction::UNKNOWN, Direction::UNKNOWN}};

class Dog {
  inline static size_t cntMaxId = 0;

 public:
  using Id = util::Tagged<size_t, Dog>;

  Dog(std::string name) : id_(Id{Dog::cntMaxId++}), name_(name) {};

  Dog(Id id, std::string name) : id_(id), name_(name) {};

  const Id& GetId() const;
  const std::string& GetName() const;

  const Direction GetDirection() const;
  void SetDirection(Direction direction);

  const Position& GetPosition() const;
  void SetPosition(Position position);

  const Speed& GetSpeed() const;
  void SetSpeed(Speed velocity);

  void Move(Direction direction, double speed);
  Position CalculateNewPosition(const std::chrono::milliseconds& diff_time);

 private:
  Id id_;
  std::string name_;
  Direction direction_{Direction::NORTH};
  Position position_{0.0, 0.0};
  Speed speed_{0.0, 0.0};
};

}  // namespace model