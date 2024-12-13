#pragma once
#include <chrono>
#include <string>
#include <unordered_map>

#include "../model/typedef.h"
#include "../other/tagged.h"

namespace model {

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

  const Id& GetId() const;             // Геттер на айди
  const std::string& GetName() const;  // Геттер на имя

  const Direction GetDirection() const;    // Геттер на направление
  void SetDirection(Direction direction);  // Сеттер на направление

  const Position& GetPosition() const;  // Геттер на позицию
  void SetPosition(Position position);  // Сеттер на позицию

  const Speed& GetSpeed() const;  // Геттер на скорость
  void SetSpeed(Speed velocity);  // Сеттер на скорость

  void Move(Direction direction, double speed);  // Двигать собаку
  Position CalculateNewPosition(
      const std::chrono::milliseconds& diff_time);  // Новая позиция собаки

 private:
  Id id_;                                  // айди
  std::string name_;                       // имя
  Direction direction_{Direction::NORTH};  // направление
  Position position_{0.0, 0.0};            // позиция
  Speed speed_{0.0, 0.0};                  // скорость
};

}  // namespace model