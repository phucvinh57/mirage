cmake_minimum_required(VERSION 3.16)
project(demo-mir-compositor)
set(CMAKE_CXX_STANDARD 23)

include(FindPkgConfig)
pkg_check_modules(MIRAL miral REQUIRED IMPORTED_TARGET)
add_executable(miracle miracle.cc miracle_config.cc)
target_compile_definitions(miracle PRIVATE MIR_LOG_COMPONENT="miracle")
set(MIRACLE_HOME_CONFIG "$ENV{HOME}/.config/miracle.config")
add_custom_command(
    OUTPUT "${MIRACLE_HOME_CONFIG}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/.config"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/miracle.config"
            "${MIRACLE_HOME_CONFIG}"
    DEPENDS "${CMAKE_SOURCE_DIR}/miracle.config")
add_custom_target(miracle-config DEPENDS "${MIRACLE_HOME_CONFIG}")
add_dependencies(miracle miracle-config)
target_link_libraries(miracle PkgConfig::MIRAL)
