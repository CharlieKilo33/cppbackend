cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.78.0 COMPONENTS log log_setup REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
	src/main.cpp
	src/app/app.h
	src/app/app.cpp
	src/json/boost_json.cpp
	src/json/json_handler.h
	src/json/json_handler.cpp
	src/json/json_loader.h
	src/json/json_loader.cpp
	src/logger/logger.h
	src/logger/logger.cpp
	src/logger/logging_handler.h
	src/logger/logging_handler.cpp
	src/model/dog.h
	src/model/dog.cpp
	src/model/player.h
	src/model/player.cpp
	src/model/game_session.h
	src/model/game_session.cpp
	src/model/game.h
	src/model/game.cpp
	src/model/map.h
	src/model/map.cpp
	src/model/player_tokens.h
	src/model/player_tokens.cpp
	src/model/const.h
	src/model/typedef.h
	src/model/typedef.cpp
	src/model/road.h
	src/model/road.cpp
	src/model/road_layout.h
	src/model/road_layout.cpp
	src/other/data.h
	src/other/tagged.h
	src/other/sdk.h
	src/other/util.h
	src/other/util.cpp
	src/other/file_system.h
	src/other/file_system.cpp
	src/request_handler/request_handler.h
	src/request_handler/request_handler.cpp
	src/request_handler/api_request_handler.h
	src/request_handler/api_request_handler_proxy.h
	src/request_handler/static_file_request_handler.h
	src/request_handler/static_file_request_handler_proxy.h
	src/server/http_server.h
	src/server/http_server.cpp
)

target_include_directories(game_server PRIVATE
    src
    src/server
    src/other
    src/model
    src/json
    src/request_handler
	src/logger
	src/app
)

target_link_libraries(game_server PRIVATE Threads::Threads jsoncpp Boost::log Boost::log_setup)
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)