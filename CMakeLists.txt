cmake_minimum_required(VERSION 3.0.0)
project(Masters VERSION 2.0.0)

SET(CMAKE_CXX_FLAGS  "-Weffc++  -Wextra -pedantic -Wall -std=c++11 -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable")

include_directories(include)
include_directories(src)

add_library(bchelper src/bchelper.cpp)
add_library(bzhelper src/bzhelper.cpp)
add_library(visawrapper src/visawrapper.cpp)
add_library(visawrapper_debug src/visawrapper.cpp)
add_library(keithley src/KeithleyInterface.cpp)
add_library(keithley_debug src/KeithleyInterface.cpp)
add_library(hameg src/HamegInterface.cpp)
add_library(hameg_debug src/HamegInterface.cpp)
add_library(hp66332 src/HP66332Interface.cpp)
add_library(hp66332_debug src/HP66332Interface.cpp)
add_library(e5270 src/E5270Interface.cpp)
add_library(e5270_debug src/E5270Interface.cpp)
add_library(psu src/PSUInterface.cpp)
add_library(psu_debug src/PSUInterface.cpp)

target_link_libraries(visawrapper ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/lib/msc/visa32.lib ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/Lib_x64/msc/visa64.lib)
target_link_libraries(visawrapper_debug ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/lib/msc/visa32.lib ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/Lib_x64/msc/visa64.lib)
target_link_libraries(keithley visawrapper)
target_link_libraries(keithley_debug visawrapper_debug)
target_link_libraries(hameg visawrapper)
target_link_libraries(hameg_debug visawrapper_debug)
target_link_libraries(hp66332 visawrapper)
target_link_libraries(hp66332_debug visawrapper_debug)
target_link_libraries(e5270 visawrapper)
target_link_libraries(e5270_debug visawrapper_debug)
target_link_libraries(psu visawrapper)
target_link_libraries(psu_debug visawrapper_debug)

target_compile_definitions(visawrapper_debug PUBLIC DEBUG=1)
target_compile_definitions(keithley_debug PUBLIC DEBUG=1)
target_compile_definitions(hameg_debug PUBLIC DEBUG=1)
target_compile_definitions(hp66332_debug PUBLIC DEBUG=1)
target_compile_definitions(e5270_debug PUBLIC DEBUG=1)
target_compile_definitions(psu_debug PUBLIC DEBUG=1)

## Battery Cycling Executables
# BCK and debug
add_executable(bck bck.cpp)
target_link_libraries(bck bchelper visawrapper keithley psu)
add_executable(bck_debug bck.cpp)
target_link_libraries(bck_debug bchelper visawrapper_debug keithley_debug psu_debug)

# BCH and debug
add_executable(bch bch.cpp)
target_link_libraries(bch bchelper visawrapper hameg psu)
add_executable(bch_debug bch.cpp)
target_link_libraries(bch_debug bchelper visawrapper_debug hameg_debug psu_debug)

# BC66332 and debug
add_executable(bc66332 bc66332.cpp)
target_link_libraries(bc66332 bchelper visawrapper hp66332 psu)
add_executable(bc66332_debug bc66332.cpp)
target_link_libraries(bc66332_debug bchelper visawrapper_debug hp66332_debug psu_debug)

## Imepedance Measurement Executables
# BZK and debug
add_executable(bzk bzk.cpp)
target_link_libraries(bzk bzhelper visawrapper keithley psu)
add_executable(bzk_debug bzk.cpp)
target_link_libraries(bzk_debug bzhelper visawrapper_debug keithley_debug psu_debug)

# BZH and debug
add_executable(bzh bzh.cpp)
target_link_libraries(bzh bzhelper visawrapper hameg psu)
add_executable(bzh_debug bzh.cpp)
target_link_libraries(bzh_debug bzhelper visawrapper_debug hameg_debug psu_debug)

# BZ66332 and debug
add_executable(bz66332 bz66332.cpp)
target_link_libraries(bz66332 bzhelper visawrapper hp66332 psu)
add_executable(bz66332_debug bz66332.cpp)
target_link_libraries(bz66332_debug bzhelper visawrapper_debug hp66332_debug psu_debug)

# BZ5270 and debug
add_executable(bz5270 bz5270.cpp)
target_link_libraries(bz5270 bzhelper visawrapper e5270 psu)
add_executable(bz5270_debug bz5270.cpp)
target_link_libraries(bz5270_debug bzhelper visawrapper_debug e5270_debug psu_debug)

