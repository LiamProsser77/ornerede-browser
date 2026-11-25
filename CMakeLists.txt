cmake_minimum_required(VERSION 3.16)

project(OrneredeBrowser VERSION 0.0.1 LANGUAGES CXX)

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
    proxy.cpp
    proxy.h
)

target_link_libraries(ornerede PRIVATE
    Qt6::Widgets
    Qt6::Network
)

# Copy the browser logo into the build directory
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Ornerede.png"
    "${CMAKE_CURRENT_BINARY_DIR}/Ornerede.png"
    COPYONLY
)
