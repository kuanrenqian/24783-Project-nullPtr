#ifndef MESHCLICKDRAW_IS_INCLUDED
#define MESHCLICKDRAW_IS_INCLUDED

#include <fssimplewindow.h>
#include <iostream>
#include "ysglfontdata.h"
#include <vector>
#include <math.h>
#include <string>


std::vector<float> bezier(std::vector<float> control_points);

void Initialize(std::vector<float> &col, std::vector<float> &vtx);

void RunOneStep(std::vector<float> &col, std::vector<float> &vtx, bool &term, std::vector <float> &allctrl, std::vector<float> &col_layout, std::vector<float> &vtx_layout);

void Draw_coordinates();

void Draw(std::vector<float> &col, std::vector<float> &vtx, std::vector<float> &col_layout, std::vector<float> &vtx_layout);

std::vector<float> runClickDraw();

void crtlPts_mapping(std::vector <float> &orderedControl);

#endif