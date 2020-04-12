cmake_minimum_required(VERSION 3.0.0)
project(Masters VERSION 4.0.0)

#SET(CMAKE_CXX_FLAGS  "-Weffc++  -Wextra -pedantic -Wall -std=c++11 -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable")

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
# CHECK_INCLUDE_FILE(/opt/vc/include/bcm_host.h BCMHOST)
CHECK_INCLUDE_FILE_CXX(/opt/vc/include/bcm_host.h BCMHOST)

if(BCMHOST)
    add_compile_definitions(BCMHOST 1)
    message("Compiling for a raspberry pi")
endif()

SET(CMAKE_CXX_FLAGS  "-Wall -Wextra -pedantic -std=c++11 -Weffc++ -Werror -Wno-unused-parameter -Wno-unused-variable")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/CommunicationInterfaces)
include_directories(${CMAKE_SOURCE_DIR}/include/PowerSupplyInterfaces)

add_subdirectory(src)


