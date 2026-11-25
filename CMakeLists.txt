cmake_minimum_required(VERSION 3.16)

project(OrneredeBrowser VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Network
)

add_executable(ornerede
    main.cpp
    browser.cpp
    browser.h
)

target_link_libraries(ornerede PRIVATE
    Qt6::Widgets
    Qt6::Network
)
