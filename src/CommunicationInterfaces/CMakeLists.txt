# Communications Interfaces
add_library(visawrapper visawrapper.cpp)
add_library(visawrapper_debug visawrapper.cpp)
add_library(gpibdevice GpibDevice.cpp)
add_library(gpibdevice_debug GpibDevice.cpp)


target_link_libraries(visawrapper ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/lib/msc/visa32.lib ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/Lib_x64/msc/visa64.lib)
target_link_libraries(visawrapper_debug ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/lib/msc/visa32.lib ${CMAKE_SOURCE_DIR}/include/VISA/WinNT/Lib_x64/msc/visa64.lib)

target_compile_definitions(visawrapper_debug PUBLIC DEBUG=1)
