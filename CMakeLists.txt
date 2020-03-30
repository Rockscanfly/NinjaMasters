cmake_minimum_required(VERSION 3.0.0)
project(Masters VERSION 3.0.0)

SET(CMAKE_CXX_FLAGS  "-Weffc++  -Wextra -pedantic -Wall -std=c++11 -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/CommunicationInterfaces)
include_directories(${CMAKE_SOURCE_DIR}/include/PowerSupplyInterfaces)

add_subdirectory(src)


