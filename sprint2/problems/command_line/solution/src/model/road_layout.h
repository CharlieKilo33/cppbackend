#pragma once
#include <map>
#include <optional>
#include <tuple>
#include <unordered_set>

#include "../model/road.h"

namespace model {

// Класс который описывает все дороги имеющиеся на карте.
// Всю карту дробим на секторы, и в каждом храним дороги, которые через него проходят.
class RoadLayout {
 public:
  using Roads = std::vector<Road>;

  RoadLayout() = default;
  RoadLayout(const RoadLayout& other);
  RoadLayout(RoadLayout&& other);
  RoadLayout& operator=(const RoadLayout& other);
  RoadLayout& operator=(RoadLayout&& other);
  virtual ~RoadLayout() = default;

  void AddRoad(const Road& road);

  const Roads& GetRoads() const noexcept;

  std::tuple<Position, Speed> GetValidMove(const Position& current_pos,
                                           const Position& target_pos,
                                           const Speed& current_spd);

 private:
  struct MapCoord {
    int64_t x = 0;
    int64_t y = 0;

    bool operator==(const MapCoord& other) const {
      return (x == other.x) && (y == other.y);
    };
  };

  using MatrixMap = std::map<int64_t, std::map<int64_t, std::unordered_set<size_t> > >;
  MatrixMap matrixMap_;
  Roads roads_;

  std::optional<const MapCoord> GetDestinationRoadsOfRoute(
      std::optional<const MapCoord> start, std::optional<const MapCoord> end,
      const Speed& current_spd);

  std::optional<const MapCoord> GetCoordinatesOfPosition(const Position& pos);

  const Direction SpeedToDirection(const Speed& speed);

  const std::unordered_map<Direction, Position> MatrixCoordinateToPosition(
      const MapCoord& coord, const Position& target_pos);

  bool IsCrossedSets(const std::unordered_set<size_t>& lhs,
                     const std::unordered_set<size_t>& rhs);

  bool IsValidCoordinates(const MapCoord& coord);

  const Position GetFarthestPoinOfRoute(const MapCoord& roads_coord,
                                        const Position& current_pos,
                                        const Speed& current_spd);

  bool IsValidPosition(const std::unordered_set<size_t>& roads, const Position& pos);

  bool IsValidPositionOnRoad(const Road& road, const Position& pos);

  void CopyAll(const Roads& roads);
};

}  // namespace model