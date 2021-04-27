cmake_minimum_required(VERSION 3.8)
project(nullPtr_Beta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(../../public/src ${CMAKE_BINARY_DIR}/public) 
add_subdirectory(meshclickdraw)
add_subdirectory(generate_mesh)
add_subdirectory(draw_mesh)
add_subdirectory(generate_simulation)

add_executable(main MACOSX_BUNDLE main.cpp)
target_link_libraries(main meshclickdraw drawMesh genSim)

