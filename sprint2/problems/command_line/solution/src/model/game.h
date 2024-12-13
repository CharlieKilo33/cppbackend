#pragma once
#include <memory>

#include "../model/game_session.h"
#include "../model/map.h"
#include "../model/player.h"

namespace model {

class Game {
 public:
  using Maps = std::vector<std::shared_ptr<Map> >;

  void AddMap(Map map);

  void AddMaps(const std::vector<Map>& maps);

  const Maps& GetMaps() const noexcept;

  const std::shared_ptr<Map> FindMap(const Map::Id& id) const noexcept;

  void SetInitDogSpeed(double speed);
  double GetInitDogSpeed() const noexcept;

 private:
  using MapIdHasher = util::TaggedHasher<Map::Id>;
  using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

  std::vector<std::shared_ptr<Map> > maps_;
  MapIdToIndex map_id_to_index_;
  double initDogSpeed_{DOG_SPEED_INIT};
};

}  // namespace model