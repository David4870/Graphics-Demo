cmake_minimum_required(VERSION 3.28.1)

project(Graphics-Demo)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)