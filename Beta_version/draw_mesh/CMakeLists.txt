add_library(drawMesh drawMesh.cpp drawMesh.h)
target_link_libraries(drawMesh fssimplewindow ysbitmapfont)
target_include_directories(drawMesh PUBLIC .)