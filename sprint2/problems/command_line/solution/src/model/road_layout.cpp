#include "../model/road_layout.h"

#include <stdint.h>

#include <cmath>
#include <iostream>
#include <set>

namespace model {

const int CELL_FACTOR = 20;
const double ACCURACY = 0.001;
const double OFFSET = 0.4;

RoadLayout::RoadLayout(const RoadLayout& other) { CopyAll(other.roads_); };
RoadLayout::RoadLayout(RoadLayout&& other) {
  matrixMap_ = std::move(other.matrixMap_);
  roads_ = std::move(other.roads_);
};

RoadLayout& RoadLayout::operator=(const RoadLayout& other) {
  if (this != &other) {
    CopyAll(other.roads_);
  }
  return *this;
};

RoadLayout& RoadLayout::operator=(RoadLayout&& other) {
  if (this != &other) {
    matrixMap_ = std::move(other.matrixMap_);
    roads_ = std::move(other.roads_);
  }
  return *this;
};

void RoadLayout::AddRoad(const Road& road) {
  const int64_t SCALLED_OFFSET = OFFSET * CELL_FACTOR;
  size_t index = roads_.size();
  roads_.emplace_back(road);
  if (road.IsHorizontal()) {
    int64_t start =
        (road.GetStart().x < road.GetEnd().x) ? road.GetStart().x : road.GetEnd().x;
    int64_t end =
        (road.GetStart().x < road.GetEnd().x) ? road.GetEnd().x : road.GetStart().x;
    start = start * CELL_FACTOR - SCALLED_OFFSET;
    end = end * CELL_FACTOR + SCALLED_OFFSET;
    int64_t y = road.GetStart().y * CELL_FACTOR;
    for (int64_t x = start; x <= end; ++x) {
      for (int i = -(SCALLED_OFFSET); i <= SCALLED_OFFSET; ++i) {
        matrixMap_[x][y + i].insert(index);
      }
    }
  } else {
    int64_t start =
        (road.GetStart().y < road.GetEnd().y) ? road.GetStart().y : road.GetEnd().y;
    int64_t end =
        (road.GetStart().y < road.GetEnd().y) ? road.GetEnd().y : road.GetStart().y;
    start = start * CELL_FACTOR - SCALLED_OFFSET;
    end = end * CELL_FACTOR + SCALLED_OFFSET;
    int64_t x = road.GetStart().x * CELL_FACTOR;
    for (int64_t y = start; y <= end; ++y) {
      for (int i = -SCALLED_OFFSET; i <= SCALLED_OFFSET; ++i) {
        matrixMap_[x + i][y].insert(index);
      }
    }
  }
};

const RoadLayout::Roads& RoadLayout::GetRoads() const noexcept { return roads_; };

std::tuple<Position, Speed> RoadLayout::GetValidMove(const Position& current_pos,
                                                     const Position& target_pos,
                                                     const Speed& current_spd) {
  Speed speed = {0, 0};
  auto start_roads = GetCoordinatesOfPosition(current_pos);
  auto end_roads = GetCoordinatesOfPosition(target_pos);
  if (end_roads) {
    if (!IsValidPosition(matrixMap_[end_roads.value().x][end_roads.value().y],
                         target_pos)) {
      end_roads = std::nullopt;
    } else if (start_roads == end_roads) {
      return std::tie(target_pos, current_spd);
    }
  }
  auto dest = GetDestinationRoadsOfRoute(start_roads, end_roads, current_spd);
  Position position;
  if (dest && IsValidPosition(matrixMap_[dest.value().x][dest.value().y], target_pos)) {
    position = target_pos;
    speed = current_spd;
  } else {
    position = GetFarthestPoinOfRoute(dest.value(), current_pos, current_spd);
  }
  return std::tie(position, speed);
};

std::optional<const RoadLayout::MapCoord> RoadLayout::GetDestinationRoadsOfRoute(
    std::optional<const MapCoord> start, std::optional<const MapCoord> end,
    const Speed& old_velocity) {
  const MapCoord start_coord = start.value();
  MapCoord current_coord = start_coord;
  if (old_velocity.vx != 0) {
    int direction = std::signbit(old_velocity.vx) ? -1 : 1;
    int64_t end_x{0};
    if (end) {
      end_x = end.value().x * CELL_FACTOR;
      end_x = (direction > 0) ? (end_x < LLONG_MAX ? end_x + 1 : LLONG_MAX) : end_x - 1;
    } else {
      end_x = (direction > 0) ? LLONG_MAX : -(OFFSET * CELL_FACTOR) - 1;
    }
    int64_t index{0};
    for (index = start_coord.x; index != end_x; index += direction) {
      if (IsValidCoordinates({index, start_coord.y}) &&
          IsCrossedSets(matrixMap_[start_coord.x][start_coord.y],
                        matrixMap_[index][start_coord.y])) {
        current_coord = {index, start_coord.y};
      } else {
        break;
      }
    }
    return current_coord;
  } else if (old_velocity.vy != 0) {
    int direction = std::signbit(old_velocity.vy) ? -1 : 1;
    int64_t end_y{0};
    if (end) {
      end_y = end.value().y * CELL_FACTOR;
      end_y = (direction > 0) ? (end_y < LLONG_MAX ? end_y + 1 : LLONG_MAX) : end_y - 1;
    } else {
      end_y = (direction > 0) ? LLONG_MAX : -(OFFSET * CELL_FACTOR) - 1;
    }
    int64_t index{0};
    for (index = start_coord.y; index != end_y; index += direction) {
      if (IsValidCoordinates({start_coord.x, index}) &&
          IsCrossedSets(matrixMap_[start_coord.x][start_coord.y],
                        matrixMap_[start_coord.x][index])) {
        current_coord = {start_coord.x, index};
      } else {
        break;
      }
    }
    return current_coord;
  }
  return std::nullopt;
};

std::optional<const RoadLayout::MapCoord> RoadLayout::GetCoordinatesOfPosition(
    const Position& pos) {
  if (pos.x < -OFFSET - ACCURACY || pos.y < -OFFSET - ACCURACY) {
    return std::nullopt;
  }
  int64_t x_index =
      (pos.x >= 0) ? std::floor(pos.x * CELL_FACTOR) : std::ceil(pos.x * CELL_FACTOR);
  int64_t y_index =
      (pos.y >= 0) ? std::floor(pos.y * CELL_FACTOR) : std::ceil(pos.y * CELL_FACTOR);
  if (matrixMap_.contains(x_index)) {
    if (matrixMap_[x_index].contains(y_index)) {
      return MapCoord{x_index, y_index};
    }
  }
  return std::nullopt;
};

bool RoadLayout::IsCrossedSets(const std::unordered_set<size_t>& lhs,
                               const std::unordered_set<size_t>& rhs) {
  for (auto item : lhs) {
    if (rhs.contains(item)) {
      return true;
    }
  }
  return false;
};

bool RoadLayout::IsValidCoordinates(const MapCoord& coord) {
  if (matrixMap_.contains(coord.x)) {
    return matrixMap_[coord.x].contains(coord.y);
  }
  return false;
};

const Position RoadLayout::GetFarthestPoinOfRoute(const MapCoord& roads_coord,
                                                  const Position& current_pos,
                                                  const Speed& current_spd) {
  Position res_position{current_pos};
  auto cell_pos = MatrixCoordinateToPosition(roads_coord, current_pos);
  auto direction = SpeedToDirection(current_spd);
  for (auto road_ind : matrixMap_[roads_coord.x][roads_coord.y]) {
    auto start_position = cell_pos.at(OPOSITE_DIRECTION.at(direction));
    auto end_position = cell_pos.at(direction);
    if (IsValidPositionOnRoad(roads_[road_ind], start_position)) {
      if (IsValidPositionOnRoad(roads_[road_ind], end_position)) {
        return end_position;
      }
      res_position = start_position;
    }
  }
  return res_position;
};

const std::unordered_map<Direction, Position> RoadLayout::MatrixCoordinateToPosition(
    const MapCoord& coord, const Position& target_pos) {
  std::unordered_map<Direction, Position> res;
  int64_t x_inc_e = (coord.x < 0) ? 0 : 1;
  int64_t y_inc_s = (coord.y < 0) ? 0 : 1;
  int64_t x_inc_w = (coord.x < 0) ? -1 : 0;
  int64_t y_inc_n = (coord.y < 0) ? -1 : 0;
  res[Direction::NORTH] = Position{target_pos.x, (static_cast<double>(coord.y + y_inc_n) /
                                                  static_cast<double>(CELL_FACTOR))};
  res[Direction::SOUTH] = Position{target_pos.x, (static_cast<double>(coord.y + y_inc_s) /
                                                  static_cast<double>(CELL_FACTOR))};
  res[Direction::WEST] = Position{
      (static_cast<double>(coord.x + x_inc_w) / static_cast<double>(CELL_FACTOR)),
      target_pos.y};
  res[Direction::EAST] = Position{
      (static_cast<double>(coord.x + x_inc_e) / static_cast<double>(CELL_FACTOR)),
      target_pos.y};
  res[Direction::UNKNOWN] = Position{target_pos.x, target_pos.y};
  return res;
}

const Direction RoadLayout::SpeedToDirection(const Speed& speed) {
  Speed spd{0, 0};
  if (speed.vx != 0) {
    spd.vx = std::signbit(speed.vx) ? -1 : 1;
  }
  if (speed.vy != 0) {
    spd.vy = std::signbit(speed.vy) ? -1 : 1;
  }
  return SPEED_TO_DIRECTION.at(spd);
}

bool RoadLayout::IsValidPosition(const std::unordered_set<size_t>& roads,
                                 const Position& pos) {
  for (auto road_index : roads) {
    if (IsValidPositionOnRoad(roads_[road_index], pos)) {
      return true;
    }
  }
  return false;
};

bool RoadLayout::IsValidPositionOnRoad(const Road& road, const Position& pos) {
  double start_x, end_x, start_y, end_y;
  if (road.IsHorizontal()) {
    start_x =
        (road.GetStart().x < road.GetEnd().x) ? (road.GetStart().x) : (road.GetEnd().x);
    end_x =
        (road.GetStart().x < road.GetEnd().x) ? (road.GetEnd().x) : (road.GetStart().x);
    start_y = road.GetStart().y - OFFSET;
    end_y = road.GetStart().y + OFFSET;

    start_x -= OFFSET;
    end_x += OFFSET;
  } else {
    start_y =
        (road.GetStart().y < road.GetEnd().y) ? (road.GetStart().y) : (road.GetEnd().y);
    end_y =
        (road.GetStart().y < road.GetEnd().y) ? (road.GetEnd().y) : (road.GetStart().y);
    start_x = road.GetStart().x - OFFSET;
    end_x = road.GetStart().x + OFFSET;

    start_y -= OFFSET;
    end_y += OFFSET;
  }
  return ((pos.x > start_x) || (std::abs(pos.x - start_x) < ACCURACY)) &&
         ((pos.x < end_x) || (std::abs(pos.x - end_x) < ACCURACY)) &&
         ((pos.y > start_y) || (std::abs(pos.y - start_y) < ACCURACY)) &&
         ((pos.y < end_y) || (std::abs(pos.y - end_y) < ACCURACY));
};

void RoadLayout::CopyAll(const RoadLayout::Roads& roads) {
  for (auto& road : roads) {
    AddRoad(road);
  }
};
}  // namespace model