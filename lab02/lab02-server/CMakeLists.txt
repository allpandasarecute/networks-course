cmake_minimum_required(VERSION 3.0)
project(lab02-server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET_NAME "${PROJECT_NAME}")

add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE src/main.cpp)

target_include_directories(${TARGET_NAME} PRIVATE inc)
target_link_libraries(${TARGET_NAME} ${CMAKE_SOURCE_DIR}/libs/librestbed.a)
