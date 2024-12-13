#pragma once
#include <string>

#include "../model/game_session.h"
#include "../other/tagged.h"

namespace app {

class Player {
  inline static size_t cntMaxId = 0;

 public:
  using Id = util::Tagged<size_t, Player>;
  Player(std::string name) : id_(Id{Player::cntMaxId++}), name_(name) {};
  Player(Id id, std::string name) : id_(id), name_(name) {};
  Player(const Player& other) = default;
  Player(Player&& other) = default;
  Player& operator=(const Player& other) = default;
  Player& operator=(Player&& other) = default;
  virtual ~Player() = default;

  void SetGameSession(std::shared_ptr<GameSession> session);  // Задать игровую сессию
  void SetDog(const std::string& dog_name, const model::Map& map,
              bool randomize_spawn_points);  // Сеттер собаки

  const Id& GetId() const;                      // Геттер на айди
  const std::string& GetName() const;           // Геттер на имя
  const GameSession::Id& GetSessionId() const;  // Геттер на сессию
  std::shared_ptr<GameSession> GetSession();  // Геттер на сессию указатель
  std::shared_ptr<model::Dog> GetDog();  // Геттер на собаку

  void MoveDog(
      const std::chrono::milliseconds& diff_time);  // Метод который двигает собаку

 private:
  void PutDogInRndPosition(
      const model::Map& map);  // Разместить на рандомной позиции собаку
  void PutDogInStartPosition(
      const model::Map& map);  // Разместить на переданной позиции собаку

  Id id_;                                 // айди
  std::string name_;                      // имя
  std::shared_ptr<GameSession> session_;  // сессия в игре
  std::shared_ptr<model::Dog> dog_;       // собака игрока
};

}  // namespace app