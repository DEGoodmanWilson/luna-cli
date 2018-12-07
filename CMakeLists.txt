cmake_minimum_required(VERSION 3.12)
project(luna-cli)

set(CMAKE_CXX_STANDARD 17)

##### External dependencies

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

##### Options

macro(luna_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

message(STATUS "Luna CMake Options")
message(STATUS "=======================================================")
luna_option(LUNA_ETC_PATH "Path to find template and other auxilliary files"  "/usr/local/etc/luna")
message(STATUS "  LUNA_VERSION: " ${CONAN_PACKAGE_VERSION})
message(STATUS "=======================================================")

add_definitions(-DLUNA_ETC_PATH=\"${LUNA_ETC_PATH}\")
add_definitions(-DLUNA_NAME=\"luna\")
add_definitions(-DLUNA_VERSION=\"${CONAN_PACKAGE_VERSION}\")


##### Executable

add_executable(luna main.cpp)

target_link_libraries(luna
        PRIVATE ${CONAN_LIBS}
        )