cmake_minimum_required(VERSION 3.10)

project(task1)

find_library(GMSH_LIB 
    gmsh
    HINTS "./lib"
)
if(NOT GMSH_LIB)
  message(FATAL_ERROR "Could not find libgmsh")
endif()

add_executable(genMesh genMesh.cpp include/gmsh.h include/gmshc.h)
target_include_directories(genMesh PUBLIC ./include)
target_link_libraries(genMesh ${GMSH_LIB})
target_compile_features(genMesh PRIVATE cxx_std_17)