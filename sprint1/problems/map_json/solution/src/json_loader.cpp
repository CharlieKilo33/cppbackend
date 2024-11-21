#include "json_loader.h"

#include <boost/json.hpp>
#include <fstream>
#include <iostream>

namespace json_loader {

namespace json = boost::json;

std::string ReadJSONFile(const std::filesystem::path& json_path) {
    std::ifstream file(json_path);
    if (!file.is_open()) {
        std::cout << "Can not open current file" << std::endl;
        std::exit(1);
    }

    std::string tmp;
    std::string jsonTxt;
    while (getline(file, tmp)) {
        jsonTxt += tmp;
    }
    file.close();
    return jsonTxt;
};

void AddRoads(model::Map& map, const json::value& mapItem) {
    if (!mapItem.as_object().contains("roads")) return;
    for (auto item : mapItem.as_object().at("roads").as_array()) {
        model::Coord x{static_cast<int>(item.as_object().at("x0").as_int64())};
        model::Coord y{static_cast<int>(item.as_object().at("y0").as_int64())};
        model::Point startPoint(x, y);
        if (item.as_object().contains("x1")) {
            model::Coord end{
                static_cast<int>(item.as_object().at("x1").as_int64())};
            map.AddRoad(model::Road(model::Road::HORIZONTAL, startPoint, end));
        } else {
            model::Coord end{
                static_cast<int>(item.as_object().at("y1").as_int64())};
            map.AddRoad(model::Road(model::Road::VERTICAL, startPoint, end));
        }
    }
};

void AddBuildings(model::Map& map, const json::value& mapItem) {
    if (!mapItem.as_object().contains("buildings")) return;
    for (auto item : mapItem.as_object().at("buildings").as_array()) {
        model::Coord x{static_cast<int>(item.as_object().at("x").as_int64())};
        model::Coord y{static_cast<int>(item.as_object().at("y").as_int64())};
        model::Coord w{static_cast<int>(item.as_object().at("w").as_int64())};
        model::Coord h{static_cast<int>(item.as_object().at("h").as_int64())};
        model::Rectangle rectangle(model::Point(x, y), model::Size(w, h));
        map.AddBuilding(model::Building(rectangle));
    }
};

void AddOffices(model::Map& map, const json::value& mapItem) {
    if (!mapItem.as_object().contains("offices")) return;
    for (auto item : mapItem.as_object().at("offices").as_array()) {
        model::Office::Id id(
            std::string(item.as_object().at("id").as_string()));
        model::Coord x{static_cast<int>(item.as_object().at("x").as_int64())};
        model::Coord y{static_cast<int>(item.as_object().at("y").as_int64())};
        model::Coord dx{
            static_cast<int>(item.as_object().at("offsetX").as_int64())};
        model::Coord dy{
            static_cast<int>(item.as_object().at("offsetY").as_int64())};
        map.AddOffice(
            model::Office(id, model::Point(x, y), model::Offset(dx, dy)));
    }
};

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    std::string jsonStr{ReadJSONFile(json_path)};
    auto mapJSON = json::parse(jsonStr);
    model::Game game;

    for (auto mapItem : mapJSON.as_object().at("maps").as_array()) {
        std::string id(mapItem.as_object().at("id").as_string());
        std::string name(mapItem.as_object().at("name").as_string());
        model::Map map(model::Map::Id(id), name);
        AddRoads(map, mapItem);
        AddBuildings(map, mapItem);
        AddOffices(map, mapItem);
        game.AddMap(std::move(map));
    }
    return game;
};

}  // namespace json_loader