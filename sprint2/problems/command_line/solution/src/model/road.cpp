#include "../model/road.h"

namespace model {

const std::string ROADS = "roads";
const std::string ROAD_XO = "x0";
const std::string ROAD_YO = "y0";
const std::string ROAD_X1 = "x1";
const std::string ROAD_Y1 = "y1";

void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Road& road) {
  if (road.IsHorizontal()) {
    jv = {{ROAD_XO, boost::json::value_from(road.GetStart().x)},
          {ROAD_YO, boost::json::value_from(road.GetStart().y)},
          {ROAD_X1, boost::json::value_from(road.GetEnd().x)}};
  } else {
    jv = {{ROAD_XO, boost::json::value_from(road.GetStart().x)},
          {ROAD_YO, boost::json::value_from(road.GetStart().y)},
          {ROAD_Y1, boost::json::value_from(road.GetEnd().y)}};
  }
};

Road tag_invoke(boost::json::value_to_tag<Road>, const boost::json::value& jv) {
  Point start;
  start.x = boost::json::value_to<int>(jv.as_object().at(ROAD_XO));
  start.y = boost::json::value_to<int>(jv.as_object().at(ROAD_YO));
  int end;
  try {
    end = boost::json::value_to<int>(jv.as_object().at(ROAD_X1));
    return Road(Road::HORIZONTAL, start, end);
  } catch (...) {
    end = boost::json::value_to<int>(jv.as_object().at(ROAD_Y1));
    return Road(Road::VERTICAL, start, end);
  }
};

}  // namespace model