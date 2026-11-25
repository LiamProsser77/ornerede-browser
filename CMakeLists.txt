cmake_minimum_required(VERSION 3.16)

project(OrneredeBrowser VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(ornerede main.cpp)

target_link_libraries(ornerede PRIVATE Qt6::Widgets)
