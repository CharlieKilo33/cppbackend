#pragma once
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../model/game.h"
#include "../model/player.h"
#include "../model/player_tokens.h"
#include "../other/tagged.h"
#include "../ticker/ticker.h"

namespace app {

namespace net = boost::asio;

class Application {
 public:
  using StrandApp = net::strand<net::io_context::executor_type>;

  Application(model::Game game, size_t tick_period, bool randomize_pos,
              net::io_context& ioc)
      : game_(std::move(game)),
        tickPeriod_{tick_period},
        randomizePosition_{randomize_pos},
        ioc_(ioc),
        strand_(std::make_shared<StrandApp>(net::make_strand(ioc))) {
    if (tickPeriod_.count() != 0) {
      ticker_ = std::make_shared<tickerTime::Ticker>(
          strand_, tickPeriod_,
          std::bind(&Application::UpdateTime, this, std::placeholders::_1));
      ticker_->Start();
    }
  };

  Application(const Application& other) = delete;
  Application(Application&& other) = delete;
  Application& operator=(const Application& other) = delete;
  Application& operator=(Application&& other) = delete;

  virtual ~Application() = default;

  const model::Game::Maps& ListMap() const noexcept;
  const std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept;
  std::tuple<auth::Token, model::Player::Id> JoinGame(const std::string& player_name,
                                                      const model::Map::Id& id);
  const std::vector<std::weak_ptr<model::Player> >& GetPlayersFromSession(
      auth::Token token);
  bool CheckPlayerByToken(auth::Token token);
  void MovePlayer(const auth::Token& token, model::Direction direction);
  std::shared_ptr<StrandApp> GetStrand();
  bool CheckTimeManage();
  void UpdateTime(const std::chrono::milliseconds& time);
  void AddGameSession(std::shared_ptr<model::GameSession> session);
  std::shared_ptr<model::GameSession> GameSessionById(
      const model::Map::Id& id) const noexcept;

 private:
  using SessionIdHasher = util::TaggedHasher<model::GameSession::Id>;
  using SessionIdToIndex =
      std::unordered_map<model::GameSession::Id,
                         std::vector<std::weak_ptr<model::Player> >, SessionIdHasher>;

  using MapIdHasher = util::TaggedHasher<model::Map::Id>;
  using MapIdToSessionIndex = std::unordered_map<model::Map::Id, size_t, MapIdHasher>;

  model::Game game_;
  std::chrono::milliseconds tickPeriod_;
  bool randomizePosition_;
  std::vector<std::shared_ptr<model::Player> > players_;
  SessionIdToIndex sessionID_;
  auth::PlayerTokens playerTokens_;
  net::io_context& ioc_;
  std::shared_ptr<StrandApp> strand_;
  std::shared_ptr<tickerTime::Ticker> ticker_;
  std::vector<std::shared_ptr<model::GameSession> > sessions_;
  MapIdToSessionIndex map_id_to_session_index_;

  std::shared_ptr<model::Player> CreatePlayer(const std::string& player_name);
  void BindPlayerInSession(std::shared_ptr<model::Player> player,
                           std::shared_ptr<model::GameSession> session);
};

}  // namespace app
