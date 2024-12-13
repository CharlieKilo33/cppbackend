#pragma once
#include <map>
#include <optional>
#include <tuple>
#include <unordered_set>

#include "../model/road.h"
#include "../model/typedef.h"

namespace model {

// Класс который описывает все дороги имеющиеся на карте.
// Всю карту дробим на секторы, и в каждом храним дороги, которые через него проходят.
class RoadLayout {
 public:
  using Roads = std::vector<Road>;

  /*Конструкторы все дефорлтные*/
  RoadLayout() = default;
  RoadLayout(const RoadLayout& other);
  RoadLayout(RoadLayout&& other);
  RoadLayout& operator=(const RoadLayout& other);
  RoadLayout& operator=(RoadLayout&& other);
  virtual ~RoadLayout() = default;

  void AddRoad(const Road& road);  // добавить дорогу

  const Roads& GetRoads() const noexcept;  // Геттер

  std::tuple<Position, Speed> GetValidMove(const Position& current_pos,  // Сам ход
                                           const Position& target_pos,
                                           const Speed& current_spd);

 private:
  struct MapCoord {
    int64_t x;
    int64_t y;

    bool operator==(const MapCoord& other) const {
      return (x == other.x) && (y == other.y);
    };
  };

  using MatrixMap = std::map<int64_t, std::map<int64_t, std::unordered_set<size_t> > >;
  MatrixMap matrixMap_;  // Карта дорог
  Roads roads_;          // Вектор дорог

  /*Получение структуры MapCoord*/
  std::optional<const MapCoord> GetDestinationRoadsOfRoute(
      std::optional<const MapCoord> start, std::optional<const MapCoord> end,
      const Speed& current_spd);

  /*Получение координат из позиции*/
  std::optional<const MapCoord> GetCoordinatesOfPosition(const Position& pos);

  /*Полчение направления (север юг итд)*/
  const Direction SpeedToDirection(const Speed& speed);

  /*Получение координат сектора*/
  const std::unordered_map<Direction, Position> MatrixCoordinateToPosition(
      const MapCoord& coord, const Position& target_pos);

  /*Проверка пересечения множеств*/
  bool IsCrossedSets(const std::unordered_set<size_t>& lhs,
                     const std::unordered_set<size_t>& rhs);

  /*Проверка координат на валиднойть*/
  bool IsValidCoordinates(const MapCoord& coord);

  /*Поиск самой удалённой точки*/
  const Position GetFarthestPoinOfRoute(const MapCoord& roads_coord,
                                        const Position& current_pos,
                                        const Speed& current_spd);

  /*Валидация позиции*/
  bool IsValidPosition(const std::unordered_set<size_t>& roads, const Position& pos);

  /*Валидация позиции относительно дороги*/
  bool IsValidPositionOnRoad(const Road& road, const Position& pos);

  /*Копирование через метод AddRoad*/
  void CopyAll(const Roads& roads);
};

}  // namespace model