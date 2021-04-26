cmake_minimum_required(VERSION 3.8)
project(draw_mesh)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(drawMesh MACOSX_BUNDLE main.cpp)
target_link_libraries(drawMesh fssimplewindow)

add_subdirectory(../../../public/src ${CMAKE_BINARY_DIR}/public) 