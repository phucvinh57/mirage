cmake_minimum_required(VERSION 3.16)
project(mirage)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FindPkgConfig)
pkg_check_modules(MIRAL miral REQUIRED IMPORTED_TARGET)
add_executable(mirage
    mirage.cc
    mirage_config.cc
    mirage_keybind.cc
    mirage_window_manager.cpp)
target_compile_definitions(mirage PRIVATE MIR_LOG_COMPONENT="mirage")
set(MIRAGE_HOME_CONFIG "$ENV{HOME}/.config/mirage.config")
add_custom_command(
    OUTPUT "${MIRAGE_HOME_CONFIG}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/.config"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/mirage.config"
            "${MIRAGE_HOME_CONFIG}"
    DEPENDS "${CMAKE_SOURCE_DIR}/mirage.config")
add_custom_target(mirage-config DEPENDS "${MIRAGE_HOME_CONFIG}")
add_dependencies(mirage mirage-config)
target_link_libraries(mirage PkgConfig::MIRAL)
