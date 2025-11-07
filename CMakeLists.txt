cmake_minimum_required(VERSION 3.16)
project(hyperlog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -DNDEBUG -pthread")

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories(src include)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})
link_directories(${JSONCPP_LIBRARY_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(hyperlog ${SOURCES})
target_link_libraries(hyperlog PRIVATE nlohmann_json::nlohmann_json ${JSONCPP_LIBRARIES} pthread)

add_custom_target(run
    COMMAND ./hyperlog
    DEPENDS hyperlog
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)