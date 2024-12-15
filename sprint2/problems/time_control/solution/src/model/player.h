#pragma once
#include <string>

#include "../model/game_session.h"
#include "../other/tagged.h"

namespace model {

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

  void SetGameSession(std::weak_ptr<GameSession> session);  // Задать игровую сессию
  void SetDog(std::weak_ptr<Dog> dog);                      // Сеттер собаки

  const Id& GetId() const;                             // Геттер на айди
  const std::string& GetName() const;                  // Геттер на имя
  const model::GameSession::Id& GetSessionId() const;  // Геттер на сессию
  std::weak_ptr<GameSession> GetSession();  // Геттер на сессию указатель
  std::weak_ptr<Dog> GetDog();              // Геттер на собаку

  void MoveDog(
      const std::chrono::milliseconds& diff_time);  // Метод который двигает собаку

 private:
  Id id_;                               // айди
  std::string name_;                    // имя
  std::weak_ptr<GameSession> session_;  // сессия в игре
  std::weak_ptr<Dog> dog_;              // собака игрока
};

}  // namespace model