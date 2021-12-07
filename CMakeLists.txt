cmake_minimum_required(VERSION 3.16)
project(Lab3)

set(CMAKE_CXX_STANDARD 14)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(Lab3 main.cpp expression.cpp individual_task.cpp individual_task.h expression.h)

target_link_libraries(Lab3 sfml-graphics sfml-audio)

