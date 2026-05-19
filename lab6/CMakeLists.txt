cmake_minimum_required(VERSION 3.10)
project(Lab6OpenCV)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")

add_executable(lab6 ${SOURCES})

target_link_libraries(lab6 ${OpenCV_LIBS})
