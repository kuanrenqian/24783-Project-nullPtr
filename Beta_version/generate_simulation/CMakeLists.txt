add_library(genSim genSim.cpp genSim.h)
target_link_libraries(genSim ysclass fssimplewindow ysbitmapfont)
target_compile_features(genSim PRIVATE cxx_std_17)
target_include_directories(genSim PUBLIC .)

add_executable(genSim_test_1 MACOSX_BUNDLE genSim_test_1.cpp)
target_link_libraries(genSim_test_1 genSim)
add_test(genSim_test_1 genSim_test_1)

add_executable(genSim_test_2 MACOSX_BUNDLE genSim_test_2.cpp)
target_link_libraries(genSim_test_2 genSim)
add_test(genSim_test_2 genSim_test_2)

add_executable(genSim_test_3 MACOSX_BUNDLE genSim_test_3.cpp)
target_link_libraries(genSim_test_3 genSim)
add_test(genSim_test_3 genSim_test_3)

add_executable(GUI_test_1 MACOSX_BUNDLE GUI_test_1.cpp)
target_link_libraries(GUI_test_1 genSim)
add_test(GUI_test_1 GUI_test_1)

add_executable(GUI_test_2 MACOSX_BUNDLE GUI_test_2.cpp)
target_link_libraries(GUI_test_2 genSim)
add_test(GUI_test_2 GUI_test_2)

add_executable(GUI_test_3 MACOSX_BUNDLE GUI_test_3.cpp)
target_link_libraries(GUI_test_3 genSim)
add_test(GUI_test_3 GUI_test_3)