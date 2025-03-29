cmake_minimum_required(VERSION 3.0)
project(TestDemo)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../csettings)

file(GLOB SRC ${CMAKE_CURRENT_LIST_DIR}/../csettings/*.c)

add_executable(TestDemo main.c ${SRC})