cmake_minimum_required(VERSION 3.30)
project(physicTesting)

set(CMAKE_CXX_STANDARD 20)

set(SDL2_INCLUDE_DIR ${CMAKE_BINARY_DIR}/SDL2/include)
set(QUADTREE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/QuadTree)
set(SDL2_LIB_DIR ${CMAKE_BINARY_DIR}/SDL2/lib)

include_directories(${SDL2_INCLUDE_DIR} ${QUADTREE_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(physicTesting ${CMAKE_SOURCE_DIR}/src/main.cpp)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2 SDL2_ttf)

file(COPY ${SDL2_LIB_DIR}/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${SDL2_LIB_DIR}/SDL2_ttf.dll DESTINATION ${CMAKE_BINARY_DIR})