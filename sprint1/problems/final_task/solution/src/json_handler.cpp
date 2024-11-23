#include "json_handler.h"

#include <json/json.h>

#include <map>
#include <sstream>

namespace jsonOperation {

std::string SerializeGameToJson(const model::Game& game) {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    for (const auto& item : game.GetMaps()) {
        Json::Value mapJson;
        mapJson["id"] = (*item.GetId()).c_str();
        mapJson["name"] = item.GetName().c_str();
        root.append(std::move(mapJson));
    }

    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
}

void AppendRoadsToJson(const model::Map& map, Json::Value& root) {
    Json::Value roadsJson;

    for (const auto& road : map.GetRoads()) {
        Json::Value roadJson;
        roadJson["x0"] = road.GetStart().x;
        roadJson["y0"] = road.GetStart().y;
        if (road.IsVertical())
            roadJson["y1"] = road.GetEnd().y;
        else
            roadJson["x1"] = road.GetEnd().x;
        roadsJson.append(std::move(roadJson));
    }

    root["roads"] = roadsJson;
}

void AppendBuildingsToJson(const model::Map& map, Json::Value& root) {
    Json::Value buildingsJson;

    for (const auto& building : map.GetBuildings()) {
        Json::Value buildingJson;
        buildingJson["x"] = building.GetBounds().position.x;
        buildingJson["y"] = building.GetBounds().position.y;
        buildingJson["w"] = building.GetBounds().size.width;
        buildingJson["h"] = building.GetBounds().size.height;
        buildingsJson.append(std::move(buildingJson));
    }

    root["buildings"] = buildingsJson;
}

void AppendOfficesToJson(const model::Map& map, Json::Value& root) {
    Json::Value officesJson;

    for (const auto& office : map.GetOffices()) {
        Json::Value officeJson;
        officeJson["id"] = (*office.GetId()).c_str();
        officeJson["x"] = office.GetPosition().x;
        officeJson["y"] = office.GetPosition().y;
        officeJson["offsetX"] = office.GetOffset().dx;
        officeJson["offsetY"] = office.GetOffset().dy;
        officesJson.append(std::move(officeJson));
    }

    root["offices"] = officesJson;
}

std::string SerializeMapToJson(const model::Map& map) {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    root["id"] = (*map.GetId()).c_str();
    root["name"] = map.GetName().c_str();
    AppendRoadsToJson(map, root);
    AppendBuildingsToJson(map, root);
    AppendOfficesToJson(map, root);

    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
}

std::string CreateMapNotFoundResponse() {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    root["code"] = "mapNotFound";
    root["message"] = "Map not found";

    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
}

std::string CreateBadRequestResponse() {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    root["code"] = "badRequest";
    root["message"] = "Bad request";

    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
}

std::string CreatePageNotFoundResponse() {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    root["code"] = "pageNotFound";
    root["message"] = "Page not found";

    std::stringstream jsonString;
    writer->write(root, &jsonString);
    return jsonString.str();
}

}  // namespace jsonOperation
