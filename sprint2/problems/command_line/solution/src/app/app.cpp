#include "../app/app.h"

#include <algorithm>
#include <cassert>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../model/dog.h"

namespace app {

using namespace std::literals;

Application::Application(model::Game game, size_t tick_period, bool randomize_pos,
                         net::io_context& ioc)
    : game_(std::move(game)),
      tickPeriod_{tick_period},
      randomizePosition_{randomize_pos},
      ioc_(ioc),
      strand_(std::make_shared<StrandApp>(net::make_strand(ioc))) {
  {
    if (tickPeriod_.count() != 0) {
      ticker_ = std::make_shared<tickerTime::Ticker>(
          strand_, tickPeriod_,
          std::bind(&Application::UpdateTime, this, std::placeholders::_1));
      ticker_->Start();
    }
  }
}

const model::Game::Maps& Application::ListMap() const noexcept {
  return game_.GetMaps();
};

const std::shared_ptr<model::Map> Application::FindMap(
    const model::Map::Id& id) const noexcept {
  return game_.FindMap(id);
};

std::tuple<auth::Token, model::Player::Id> Application::JoinGame(
    const std::string& name, const model::Map::Id& id) {
  auto player = CreatePlayer(name);
  auto token = playerTokens_.AddPlayer(player);

  auto session = GameSessionById(id);
  if (!session) {
    session = std::make_shared<model::GameSession>(game_.FindMap(id), ioc_);
    AddGameSession(session);
  }
  BindPlayerInSession(player, session);
  return std::tie(token, player->GetId());
};

std::shared_ptr<model::Player> Application::CreatePlayer(const std::string& player_name) {
  auto player = std::make_shared<model::Player>(player_name);
  players_.push_back(player);
  return player;
};

void Application::BindPlayerInSession(std::shared_ptr<model::Player> player,
                                      std::shared_ptr<model::GameSession> session) {
  sessionID_[session->GetId()].push_back(player);
  player->SetGameSession(session);
  player->SetDog(player->GetName(), *(session->GetMap()), randomizePosition_);
};

const std::vector<std::weak_ptr<model::Player> >& Application::GetPlayersFromSession(
    auth::Token token) {
  static const std::vector<std::weak_ptr<model::Player> > emptyPlayerList;
  auto player = playerTokens_.FindPlayerByToken(token).lock();
  auto session_id = player->GetSessionId();
  if (!sessionID_.contains(session_id)) {
    return emptyPlayerList;
  }
  return sessionID_[session_id];
};

bool Application::CheckPlayerByToken(auth::Token token) {
  return !playerTokens_.FindPlayerByToken(token).expired();
};

void Application::UpdateTime(const std::chrono::milliseconds& time) {
  for (auto player : players_) {
    player->MoveDog(time);
  }
};

std::shared_ptr<Application::StrandApp> Application::GetStrand() { return strand_; };

void Application::MovePlayer(const auth::Token& token, model::Direction direction) {
  auto player = playerTokens_.FindPlayerByToken(token).lock();
  auto dog = player->GetDog();
  double speed = player->GetSession()->GetMap()->GetDogSpeed();
  dog->Move(direction, speed);
};

bool Application::CheckTimeManage() { return tickPeriod_.count() == 0; };

void Application::AddGameSession(std::shared_ptr<model::GameSession> session) {
  const size_t index = sessions_.size();
  if (auto [it, inserted] =
          map_id_to_session_index_.emplace(session->GetMap()->GetId(), index);
      !inserted) {
    throw std::invalid_argument("Game session with map id "s +
                                *(session->GetMap()->GetId()) + " already exists"s);
  } else {
    try {
      sessions_.push_back(session);
    } catch (...) {
      map_id_to_session_index_.erase(it);
      throw;
    }
  }
};

std::shared_ptr<model::GameSession> Application::GameSessionById(
    const model::Map::Id& id) const noexcept {
  if (auto it = map_id_to_session_index_.find(id); it != map_id_to_session_index_.end()) {
    return sessions_.at(it->second);
  }
  return nullptr;
};

}  // namespace app
