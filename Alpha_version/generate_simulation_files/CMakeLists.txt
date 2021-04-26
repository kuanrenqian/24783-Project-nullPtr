find_library(GMSH_LIB 
    gmsh
    HINTS "./lib"
)
if(NOT GMSH_LIB)
  message(FATAL_ERROR "Could not find libgmsh")
endif()

add_executable(genSim MACOSX_BUNDLE generate_simulation_files.cpp include/gmsh.h include/gmshc.h)
target_include_directories(genSim PUBLIC ./include)
target_link_libraries(genSim ${GMSH_LIB} ysclass fssimplewindow)
target_compile_features(genSim PRIVATE cxx_std_17)


