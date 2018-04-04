cmake_minimum_required(VERSION 3.7)
project(wallman)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra -Wpedantic -pedantic -Wno-sign-compare -Wformat-security")
set(SOURCE_FILES main.c config_parse.h config_parse.c vector.h  config_save.c config_parse.h )
add_executable(wallman ${SOURCE_FILES})
