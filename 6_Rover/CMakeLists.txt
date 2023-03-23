cmake_minimum_required(VERSION 3.16.3)
project(Rover)

set(CMAKE_CXX_STANDARD 20)

add_executable(rover main.cpp)
add_executable(rover_example rover_example.cc)
