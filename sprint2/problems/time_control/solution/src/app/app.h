#pragma once
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../model/game.h"
#include "../model/player.h"
#include "../model/player_tokens.h"
#include "../other/tagged.h"

namespace app {

namespace net = boost::asio;

class Application {
 public:
  using StrandApp = net::strand<net::io_context::executor_type>;

  Application(model::Game game, net::io_context& ioc)
      : game_(std::move(game)),
        ioc_(ioc),
        strand_(std::make_shared<StrandApp>(net::make_strand(ioc))) {};  // конструктор.

  /*Запретить все копирования, присваивания и мувы*/
  Application(const Application& other) = delete;
  Application(Application&& other) = delete;
  Application& operator=(const Application& other) = delete;
  Application& operator=(Application&& other) = delete;

  virtual ~Application() = default;

  std::shared_ptr<StrandApp> GetStrand();  // Геттер, на всякий случай...
  bool CheckPlayerByToken(model::Token token);  // Чекнуть, если такой в игре
  const model::Game::Maps& ListMap() const noexcept;  // Выдать список карт
  const std::shared_ptr<model::Map> FindMap(
      const model::Map::Id& id) const noexcept;  // Найти карту

  std::tuple<model::Token, model::Player::Id> JoinGame(
      const std::string& player_name, const model::Map::Id& id);  // Залогинить игрока
  const std::vector<std::weak_ptr<model::Player> >& GetPlayersFromSession(
      model::Token token);  // Посмотреть сколько играют

  void MovePlayer(const model::Token& token,
                  model::Direction direction);  // Передвижение игрока
  void UpdateGameState(const std::chrono::milliseconds& time);  // апдейтим состояние игры
 private:
  using IdHasher = util::TaggedHasher<model::GameSession::Id>;
  using IdToIndex =
      std::unordered_map<model::GameSession::Id,
                         std::vector<std::weak_ptr<model::Player> >, IdHasher>;

  model::Game game_;                                      // Объект игры
  std::vector<std::shared_ptr<model::Player> > players_;  // Вектор игорьков
  IdToIndex sessionID_;                                   // ID сессии
  model::PlayerTokens playerTokens_;
  net::io_context& ioc_;
  std::shared_ptr<StrandApp> strand_;  // На всякий случай...

  std::shared_ptr<model::Player> CreatePlayer(
      const std::string& player_name);  // Создать игорька
  void BindPlayerInSession(
      std::shared_ptr<model::Player> player,  // Забиндить игорька в сессию
      std::shared_ptr<model::GameSession> session);
};

}  // namespace app
