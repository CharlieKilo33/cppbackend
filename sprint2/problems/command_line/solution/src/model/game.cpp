#include "../model/game.h"

#include <string>

namespace model {

using namespace std::literals;

void Game::AddMap(Map map) {
  const size_t index = maps_.size();
  if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
    throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
  } else {
    try {
      if (initDogSpeed_ != DOG_SPEED_INIT && map.GetDogSpeed() == DOG_SPEED_INIT) {
        map.SetDogSpeed(initDogSpeed_);
      }
      maps_.push_back(std::make_shared<Map>(std::move(map)));
    } catch (...) {
      map_id_to_index_.erase(it);
      throw;
    }
  }
}

void Game::AddMaps(const std::vector<Map>& maps) {
  for (auto item : maps) {
    AddMap(item);
  }
};

const Game::Maps& Game::GetMaps() const noexcept { return maps_; }

const std::shared_ptr<Map> Game::FindMap(const Map::Id& id) const noexcept {
  if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
    return maps_.at(it->second);
  }
  return nullptr;
};

void Game::AddGameSession(std::shared_ptr<GameSession> session) {
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

std::shared_ptr<GameSession> Game::FindGameSessionBy(const Map::Id& id) const noexcept {
  if (auto it = map_id_to_session_index_.find(id); it != map_id_to_session_index_.end()) {
    return sessions_.at(it->second);
  }
  return nullptr;
};

void Game::SetInitDogSpeed(double speed) {
  initDogSpeed_ = std::abs(speed);
  if (initDogSpeed_ == DOG_SPEED_INIT) {
    return;
  }
  for (auto map : maps_) {
    if (map->GetDogSpeed() == DOG_SPEED_INIT) {
      map->SetDogSpeed(initDogSpeed_);
    }
  }
};

double Game::GetInitDogSpeed() const noexcept { return initDogSpeed_; };

}  // namespace model