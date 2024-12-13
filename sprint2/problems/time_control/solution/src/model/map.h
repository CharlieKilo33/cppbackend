#pragma once
#include <boost/json.hpp>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../model/const.h"
#include "../model/dog.h"
#include "../model/road_layout.h"
#include "../model/typedef.h"
#include "../other/tagged.h"

// Тут тоже всё переделываем на tag_invoke
namespace model {

const double DOG_SPEED_INIT = 1.0;

namespace json = boost::json;

class Building {
 public:
  explicit Building(Rectangle bounds) noexcept : bounds_{bounds} {}

  const Rectangle& GetBounds() const noexcept { return bounds_; }

 private:
  Rectangle bounds_;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Building& building);
Building tag_invoke(json::value_to_tag<Building>, const json::value& jv);

class Office {
 public:
  using Id = util::Tagged<std::string, Office>;

  Office(Id id, Point position, Offset offset) noexcept
      : id_{std::move(id)}, position_{position}, offset_{offset} {}

  const Id& GetId() const noexcept { return id_; }

  Point GetPosition() const noexcept { return position_; }

  Offset GetOffset() const noexcept { return offset_; }

 private:
  Id id_;
  Point position_;
  Offset offset_;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Office& office);
Office tag_invoke(json::value_to_tag<Office>, const json::value& jv);

class Map {
 public:
  using Id = util::Tagged<std::string, Map>;
  using Roads = std::vector<Road>;
  using Buildings = std::vector<Building>;
  using Offices = std::vector<Office>;

  Map(Id id, std::string name) noexcept : id_(std::move(id)), name_(std::move(name)) {}

  const Id& GetId() const noexcept { return id_; }

  const std::string& GetName() const noexcept { return name_; }

  const Buildings& GetBuildings() const noexcept { return buildings_; }

  const Roads& GetRoads() const noexcept { return roadLayout_.GetRoads(); }

  const Offices& GetOffices() const noexcept { return offices_; }

  void AddRoad(const Road& road) { roadLayout_.AddRoad(road); }

  void AddRoads(Roads& roads) {
    for (const auto& item : roads) {
      AddRoad(item);
    }
  }

  void AddBuilding(const Building& building) { buildings_.emplace_back(building); }

  void AddBuildings(Buildings& buildings) {
    for (const auto& item : buildings) {
      AddBuilding(item);
    }
  }

  void AddOffice(Office office);

  void AddOffices(Offices& offices) {
    for (auto item : offices) {
      AddOffice(item);
    }
  }

  void SetDogSpeed(double speed) { dogSpeed_ = std::abs(speed); };

  double GetDogSpeed() const noexcept { return dogSpeed_; };

  std::tuple<Position, Speed> GetMove(const Position& current_pos,
                                      const Position& target_pos,
                                      const Speed& current_spd);

 private:
  using OfficeIdToIndex =
      std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

  Id id_;
  std::string name_;
  RoadLayout roadLayout_;
  Buildings buildings_;

  OfficeIdToIndex warehouse_id_to_index_;
  Offices offices_;

  double dogSpeed_{DOG_SPEED_INIT};
};

void tag_invoke(json::value_from_tag, json::value& jv, const Map& map);
Map tag_invoke(json::value_to_tag<Map>, const json::value& jv);

}  // namespace model