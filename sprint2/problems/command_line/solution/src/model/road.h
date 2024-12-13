#pragma once
#include <boost/json.hpp>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../model/const.h"
#include "../model/dog.h"
#include "../model/typedef.h"
#include "../other/tagged.h"

namespace model {

class Road {
  struct HorizontalTag {
    explicit HorizontalTag() = default;
  };

  struct VerticalTag {
    explicit VerticalTag() = default;
  };

 public:
  constexpr static HorizontalTag HORIZONTAL{};
  constexpr static VerticalTag VERTICAL{};

  Road(HorizontalTag, Point start, Coord end_x) noexcept
      : start_{start}, end_{end_x, start.y} {}

  Road(VerticalTag, Point start, Coord end_y) noexcept
      : start_{start}, end_{start.x, end_y} {}

  bool IsHorizontal() const noexcept { return start_.y == end_.y; }

  bool IsVertical() const noexcept { return start_.x == end_.x; }

  Point GetStart() const noexcept { return start_; }

  Point GetEnd() const noexcept { return end_; }

 private:
  Point start_;
  Point end_;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Road& road);
Road tag_invoke(boost::json::value_to_tag<Road>, const boost::json::value& jv);

}  // namespace model