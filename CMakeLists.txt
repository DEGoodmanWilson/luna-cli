cmake_minimum_required(VERSION 3.12)
project(luna-cli)

set(CMAKE_CXX_STANDARD 17)

##### External dependencies

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

add_executable(luna main.cpp)

target_link_libraries(luna
        PRIVATE ${CONAN_LIBS}
        )