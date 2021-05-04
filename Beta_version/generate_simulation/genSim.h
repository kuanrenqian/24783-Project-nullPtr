#ifndef GENMESH_IS_INCLUDED
#define GENMESH_IS_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <filesystem>
#include <math.h>
#include <set>
#include <fssimplewindow.h>
#include "ysclass.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "ysglfontdata.h"
// #include "drawmesh.h"
// #include <unordered_set>

// typedef std::unordered_set<float> vtxSet;

// #include <conio.h>
// #include <process.h>
// #include <unistd.h>
using namespace std;

void generate_mesh_exe_radius(float radius); // Radius input version

void generate_mesh_exe_ctrlPoints(float c1x, float c1y, float c2x, float c2y, float c3x, float c3y, float c4x, float c4y, float c5x, float c5y, int numSimu);

void generate_simulation_files(string InFile, int numSimu);

void check_output_folder();

int run_simulation_files(int numSimu, int nprocs);

void parula_colormap(float input, float max_input, float &R, float &G, float &B);

float read_and_plot_testResultTXT(int numSimu, vector <float> &vtx2d, vector <float> &col2d, int Xoffset, int Yoffset, float &maxP);

void label_cbar(int Xoffset, int Yoffset, float maxP);

void Draw_Simulation_Result(int numSimu, int x_off, int y_off);

void Draw_All_Simulation_Results(int batch);

int compareFiles(FILE *fp1, FILE *fp2);

int GUI_test1();

vector <float> GUI_test2();

float GUI_test3();

#endif