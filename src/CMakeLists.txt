add_subdirectory( CommunicationInterfaces )
add_subdirectory( PowerSupplyInterfaces )
add_subdirectory( Util )

add_library(bchelper bchelper.cpp)
add_library(bzhelper bzhelper.cpp)


## Battery Cycling Executables
add_executable(bc bcmain.cpp)
target_link_libraries(bc bchelper Psu keithley hameg hp66332 e5270 w5000)
add_executable(bc_debug bcmain.cpp)
target_link_libraries(bc_debug bchelper Psu_debug keithley_debug hameg_debug hp66332_debug e5270_debug w5000_debug)

## Imepedance Measurement Executables
add_executable(bz bzmain.cpp)
target_link_libraries(bz bzhelper Psu keithley hameg hp66332 e5270 w5000)
add_executable(bz_debug bzmain.cpp)
target_link_libraries(bz_debug bzhelper Psu_debug keithley_debug hameg_debug hp66332_debug e5270_debug w5000_debug)

set_target_properties( bz bc bz_debug bc_debug
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
