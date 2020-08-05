# Communications Interfaces
add_library(visawrapper visawrapper.cpp)
add_library(visawrapper_debug visawrapper.cpp)
add_library(gpibdevice GpibDevice.cpp)
add_library(gpibdevice_debug GpibDevice.cpp)
add_library(linuxdevice LinuxSerialDevice.cpp)
add_library(linuxdevice_debug LinuxSerialDevice.cpp)
add_library(serialdevice SerialDevice.cpp)
add_library(serialdevice_debug SerialDevice.cpp)

# IF (WIN32)
#     target_link_libraries(visawrapper ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/lib/msc/visa32.lib ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/Lib_x64/msc/visa64.lib)
#     target_link_libraries(visawrapper_debug ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/lib/msc/visa32.lib ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/Lib_x64/msc/visa64.lib)
# ENDIF()
IF (WIN32)
    target_link_libraries(visawrapper ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/lib/msc/agvisa32.lib ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/Lib_x64/msc/visa64.lib)
    target_link_libraries(visawrapper_debug ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/lib/msc/agvisa32.lib ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/Lib_x64/msc/visa64.lib)
ENDIF()

target_link_libraries(gpibdevice visawrapper serialdevice)
target_link_libraries(gpibdevice_debug visawrapper_debug serialdevice_debug)
target_link_libraries(linuxdevice serialdevice timerutil)
target_link_libraries(linuxdevice_debug serialdevice_debug timerutil)


target_compile_definitions(visawrapper_debug PUBLIC DEBUG=1)
target_compile_definitions(gpibdevice_debug PUBLIC DEBUG=1)

target_compile_definitions(linuxdevice_debug PUBLIC DEBUG=1)
target_compile_definitions(serialdevice_debug PUBLIC DEBUG=1)
