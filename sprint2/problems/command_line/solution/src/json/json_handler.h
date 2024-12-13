#pragma once
#include "../app/app.h"
#include <string>
#include <tuple>
#include <optional>

namespace jsonOperation {

	const std::string RESPONSE_CODE = "code";
	const std::string RESPONSE_MESSAGE = "message";
	const std::string RESPONSE_AUTHTOKEN = "authToken";
	const std::string RESPONSE_PLAYERID = "playerId";
	const std::string RESPONSE_NAME = "name";
	const std::string RESPONSE_PLAYERS = "players";
	const std::string RESPONSE_DOG_POSITION = "pos";
	const std::string RESPONSE_DOG_SPEED = "speed";
	const std::string RESPONSE_DOG_DIRECTION = "dir";

	const std::string REQUEST_NAME = "userName";
	const std::string REQUEST_MAPID = "mapId";
	const std::string REQUEST_MOVE = "move";
	const std::string REQUEST_TIME_DELTA = "timeDelta";

	std::string GameToJson(const model::Game::Maps& game);
	std::string MapToJson(const model::Map& map);
	std::string PageNotFound();
	std::string BadRequest();
	std::string MapNotFound();

	
	std::string JoinToGameInvalidArgument();
	std::string JoinToGameMapNotFound();
	std::string JoinToGameEmptyPlayerName();
	std::string PlayersListOnMap(const std::vector< std::weak_ptr<model::Player> >& players);
	std::string InvalidMethod();
	std::string EmptyAuthorization();
	std::string UnknownToken();
	std::string JoinToGame(const std::string& token, size_t player_id);
	std::string GameState(const std::vector< std::weak_ptr<model::Player> >& players);
	std::string PlayerAction();
	std::string PlayerInvalidAction();
	std::string InvalidContentType();
	std::string SetDeltaTime();
	std::string InvalidDeltaTime();
	std::string InvalidEndpoint();

	std::string OnlyPostMethodAllowed();															
	std::optional< std::tuple<std::string, model::Map::Id> > ParseJoinToGame(const std::string& msg);
	std::optional<std::string> ParsePlayerActionRequest(const std::string& msg);
	std::optional<int> ParseSetDeltaTimeRequest(const std::string& msg);

}