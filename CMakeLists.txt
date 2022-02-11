cmake_minimum_required(VERSION 3.20)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(my_modbus)

add_library(
    ModbusConversions
    Conversions.h
    Conversions.cpp
)
target_include_directories(ModbusConversions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
    ModbusDevice
    ModbusDevice.h
    ModbusDevice.cpp
    ModbusRegister.h
    ModbusRegister.cpp
)
target_include_directories(ModbusDevice PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
find_package(libmodbus CONFIG REQUIRED)
target_link_libraries(ModbusDevice PUBLIC modbus)
target_link_libraries(ModbusDevice PUBLIC ModbusConversions)
target_link_libraries(ModbusDevice PUBLIC ObserverModel)
