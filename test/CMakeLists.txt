add_executable(test test.cpp TestDevice.h TestDevice.cpp)
target_link_libraries(test PUBLIC ModbusDevice)
target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
