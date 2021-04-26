#ifndef DRAWMESH_IS_INCLUDED
#define DRAWMESH_IS_INCLUDED

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#define GL_SILENCE_DEPRECATION
#include <fssimplewindow.h>
using namespace std;

// 2d point
struct point;
// triangle vertex indices
struct tvi;
// quad vertex indices
struct qvi;
// TODO: try gl_points instead of gl_lines to see at least the vertices are at the right place
void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);

void DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

void read_mesh(string pathToFile, float &xmin, float &xmax, float &ymin, float &ymax);

void Draw_mesh(string pathToFile, float Xoffset, float Yoffset);

void Draw_All_mesh(vector<string> AllPathToFile);
#endif