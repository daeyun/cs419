cmake_minimum_required(VERSION 3.7)
project(cs419_cpp)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_subdirectory(src)
add_subdirectory(tests)

add_executable(cs419_cpp main.cc)
target_link_libraries(cs419_cpp raytracing)
