cmake_minimum_required(VERSION 3.10)
project(internship LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(nlohmann_json REQUIRED)
find_package(date REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${nlohmann_json_INCLUDE_DIRS})
include_directories(${date_INCLUDE_DIRS})

add_executable(internship src/internship.cpp src/main.cpp)
target_link_libraries(internship nlohmann_json::nlohmann_json)
target_link_libraries(internship date::date)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(internship_test src/test/internship_test.cpp src/internship.cpp)
target_link_libraries(internship_test GTest::GTest GTest::Main)
target_link_libraries(internship_test nlohmann_json::nlohmann_json)
target_link_libraries(internship_test date::date)
gtest_discover_tests(internship_test)
