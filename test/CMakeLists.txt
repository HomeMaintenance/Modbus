add_executable(ModbusDevice_test test.cpp TestDevice.h TestDevice.cpp)
target_link_libraries(ModbusDevice_test PUBLIC ModbusDevice)
target_include_directories(ModbusDevice_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
