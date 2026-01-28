cmake_minimum_required(VERSION 3.15)
project(injector LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(injector injector.cpp process_utils.cpp)

add_library(test_dll SHARED test_dll.cpp)

