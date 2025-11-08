cmake_minimum_required(VERSION 3.16)
project(hyperlog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -DNDEBUG -pthread")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

find_package(nlohmann_json CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

file(GLOB_RECURSE WAL_SOURCES "src/engine/wal/*.cpp" "src/util/*.cpp")
add_library(wal_core STATIC ${WAL_SOURCES})
target_include_directories(wal_core PUBLIC src include)
target_link_libraries(wal_core PUBLIC pthread)

file(GLOB_RECURSE SERVER_SOURCES "src/*.cpp")
list(FILTER SERVER_SOURCES EXCLUDE REGEX "src/engine/wal/.*|src/util/.*")
add_executable(hyperlog ${SERVER_SOURCES})
target_include_directories(hyperlog PRIVATE src include ${JSONCPP_INCLUDE_DIRS})
target_link_directories(hyperlog PRIVATE ${JSONCPP_LIBRARY_DIRS})
target_link_libraries(hyperlog PRIVATE wal_core nlohmann_json::nlohmann_json ${JSONCPP_LIBRARIES})

add_executable(bench benchmark/bench.cpp)
target_link_libraries(bench PRIVATE wal_core)

add_custom_target(run
    COMMAND ./hyperlog
    DEPENDS hyperlog
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)