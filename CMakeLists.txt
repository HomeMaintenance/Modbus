cmake_minimum_required(VERSION 3.20)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(my_modbus)

include(FetchContent)
FetchContent_Declare(
    ObserverModel
    GIT_REPOSITORY https://github.com/HomeMaintenance/ObserverModel.git
    GIT_TAG main
)
FetchContent_MakeAvailable(ObserverModel)

add_subdirectory(test)

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

if(PROJECT_IS_TOP_LEVEL)
    find_package(Doxygen
        REQUIRED dot
        OPTIONAL_COMPONENTS mscgen dia
    )
    set(DOXYGEN_EXTRACT_PRIVATE TRUE)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED FALSE)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C TRUE)
    set(DOXYGEN_EXCLUDE_PATTERNS */build/*)

    doxygen_add_docs(doxygen_target
        ${CMAKE_SOURCE_DIR}
        COMMENT "Run doxygen_target"
    )
endif()
