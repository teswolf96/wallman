cmake_minimum_required(VERSION 3.7)
project(wallman)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c config_parse.h config_parse.c vector.h config_parse.h config_save.c)
add_executable(wallman ${SOURCE_FILES})