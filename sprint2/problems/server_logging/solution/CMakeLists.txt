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
	src/server/http_server.cpp
	src/server/http_server.h
	src/other/sdk.h
	src/model/model.h
	src/model/model.cpp
	src/other/tagged.h
	src/json/boost_json.cpp
	src/json/json_loader.h
	src/json/json_loader.cpp
	src/request_handler/request_handler.cpp
	src/request_handler/request_handler.h
	src/response_handler/response_file_handler.cpp
	src/response_handler/response_file_handler.h
	src/response_handler/response_handler.cpp
	src/response_handler/response_handler.h
	src/json/json_handler.cpp
	src/json/json_handler.h
	src/logger/logger.h
	src/logger/logger.cpp
	src/other/data.h
)

target_include_directories(game_server PRIVATE
    src
    src/server
    src/other
    src/model
    src/json
    src/request_handler
    src/response_handler
	src/logger
)

target_link_libraries(game_server PRIVATE Threads::Threads jsoncpp Boost::log Boost::log_setup)