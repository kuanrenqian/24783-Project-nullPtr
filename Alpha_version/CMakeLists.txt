cmake_minimum_required(VERSION 3.8)
project(nullPtr_alpha)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(generate_mesh)
add_subdirectory(generate_simulation_files)
add_subdirectory(draw_mesh)

add_subdirectory(../../public/src ${CMAKE_BINARY_DIR}/public) 
