cmake_minimum_required(VERSION 3.20)

project(Calculator LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(Calculator main.c)