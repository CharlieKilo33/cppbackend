#include "../model/map.h"

#include <stdexcept>

namespace model {
using namespace std::literals;

void Map::AddOffice(Office office) {
  if (warehouse_id_to_index_.contains(office.GetId())) {
    throw std::invalid_argument("Duplicate office");
  }

  const size_t index = offices_.size();
  Office& o = offices_.emplace_back(std::move(office));
  try {
    warehouse_id_to_index_.emplace(o.GetId(), index);
  } catch (...) {
    offices_.pop_back();  // Возвращаем всё назад, из-за исключения
    throw;
  }
}

void tag_invoke(json::value_from_tag, json::value& jv, const Building& building) {
  jv = {{BUILDING_X, json::value_from(building.GetBounds().position.x)},
        {BUILDING_Y, json::value_from(building.GetBounds().position.y)},
        {BUILDING_WIDTH, json::value_from(building.GetBounds().size.width)},
        {BUILDING_HEIGHT, json::value_from(building.GetBounds().size.height)}};
};

Building tag_invoke(json::value_to_tag<Building>, const json::value& jv) {
  Point point;
  point.x = json::value_to<int>(jv.as_object().at(BUILDING_X));
  point.y = json::value_to<int>(jv.as_object().at(BUILDING_Y));
  Size size;
  size.width = json::value_to<int>(jv.as_object().at(BUILDING_WIDTH));
  size.height = json::value_to<int>(jv.as_object().at(BUILDING_HEIGHT));
  return Building(Rectangle(point, size));
};

void tag_invoke(json::value_from_tag, json::value& jv, const Office& office) {
  jv = {{OFFICE_ID, json::value_from(*(office.GetId()))},
        {OFFICE_X, json::value_from(office.GetPosition().x)},
        {OFFICE_Y, json::value_from(office.GetPosition().y)},
        {OFFICE_OFFSET_X, json::value_from(office.GetOffset().dx)},
        {OFFICE_OFFSET_Y, json::value_from(office.GetOffset().dy)}};
};

Office tag_invoke(json::value_to_tag<Office>, const json::value& jv) {
  Office::Id id{json::value_to<std::string>(jv.as_object().at(OFFICE_ID))};
  Point position;
  position.x = json::value_to<int>(jv.as_object().at(OFFICE_X));
  position.y = json::value_to<int>(jv.as_object().at(OFFICE_Y));
  Offset offset;
  offset.dx = json::value_to<int>(jv.as_object().at(OFFICE_OFFSET_X));
  offset.dy = json::value_to<int>(jv.as_object().at(OFFICE_OFFSET_Y));
  return Office(id, position, offset);
};

void tag_invoke(json::value_from_tag, json::value& jv, const Map& map) {
  jv = {{MAP_ID, json::value_from(*(map.GetId()))},
        {MAP_NAME, json::value_from(map.GetName())},
        {ROADS, json::value_from(map.GetRoads())},
        {BUILDINGS, json::value_from(map.GetBuildings())},
        {OFFICES, json::value_from(map.GetOffices())}};
};

Map tag_invoke(json::value_to_tag<Map>, const json::value& jv) {
  Map::Id id{json::value_to<std::string>(jv.as_object().at(MAP_ID))};
  std::string name = json::value_to<std::string>(jv.as_object().at(MAP_NAME));
  Map map(id, name);
  std::vector<Road> roads = json::value_to<std::vector<Road>>(jv.as_object().at(ROADS));
  map.AddRoads(roads);
  std::vector<Building> buildings =
      json::value_to<std::vector<Building>>(jv.as_object().at(BUILDINGS));
  map.AddBuildings(buildings);
  std::vector<Office> offices =
      json::value_to<std::vector<Office>>(jv.as_object().at(OFFICES));
  map.AddOffices(offices);
  try {
    double dog_speed = json::value_to<double>(jv.as_object().at(MAP_DOG_SPEED));
    map.SetDogSpeed(dog_speed);
  } catch (...) {
  }
  return map;
};

std::tuple<Position, Speed> Map::GetMove(const Position& current_pos,
                                         const Position& target_pos,
                                         const Speed& current_spd) {
  return roadLayout_.GetValidMove(current_pos, target_pos, current_spd);
};

}  // namespace model