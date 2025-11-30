cmake_minimum_required(VERSION 3.10)
project(DartVision LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(rgb565 test/rgb565.cpp)
add_executable(lab test/lab.cpp)
add_executable(threshold test/threshold.cpp)
