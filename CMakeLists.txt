cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0079 NEW)

# set the project name
project(JetracerUtils)

set(CMAKE_CXX_FLAGS "-fPIC -g -pedantic -Wall -Wextra")

include_directories(src)

# Build the actual library
add_library(JetracerUtils SHARED src/ScopedLock.cpp)
target_link_libraries(JetracerUtils pthread)


add_executable(test_Listeners tests/test_Listeners.cpp)
target_link_libraries(test_Listeners JetracerUtils)
