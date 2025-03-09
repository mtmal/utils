cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0079 NEW)

# set the project name
project(Utils)

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-fPIC -g -pedantic -Wall -Wextra")

# Include directories
include_directories(src)

add_executable(test_listeners tests/test_listeners.cpp)
target_link_libraries(test_listeners pthread)

add_executable(test_thread tests/test_thread.cpp)
target_link_libraries(test_thread pthread)
