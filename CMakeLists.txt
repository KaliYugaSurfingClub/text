cmake_minimum_required(VERSION 3.28)
project(text LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

include_directories(../core)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(core)
add_subdirectory(benchmark)

target_link_libraries(${PROJECT_NAME} PRIVATE core)
