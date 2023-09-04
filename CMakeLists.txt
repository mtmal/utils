cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0079 NEW)

# set the project name
project(JetracerUtils)

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-fPIC -g -pedantic -Wall -Wextra")

# Include directories
include_directories(src)

# Build the actual library
add_library(JetracerUtils SHARED src/ScopedLock.cpp)
target_link_libraries(JetracerUtils pthread)

add_executable(test_Listeners tests/test_Listeners.cpp)
target_link_libraries(test_Listeners JetracerUtils)

add_executable(test_Thread tests/test_Thread.cpp)
target_link_libraries(test_Thread pthread)
