cmake_minimum_required(VERSION 3.12)
project(luna-cli)

set(CMAKE_CXX_STANDARD 17)
## because I simply cannot get `set(CMAKE_CXX_STANDARD 17) to work
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

##### External dependencies

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

##### Options

add_definitions(-DLUNA_NAME=\"luna\")
add_definitions(-DLUNA_VERSION=\"${CONAN_PACKAGE_VERSION}\")

##### Executable

add_executable(luna main.cpp)

target_link_libraries(luna
        PRIVATE ${CONAN_LIBS}
        )