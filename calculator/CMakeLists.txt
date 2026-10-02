cmake_minimum_required(VERSION 3.20)

project(Calculator LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

foreach(SOURCE ${SOURCES})
    get_filename_component(TARGET_NAME "${SOURCE}" NAME_WE)
    add_executable("${TARGET_NAME}" "${SOURCE}")

endforeach()