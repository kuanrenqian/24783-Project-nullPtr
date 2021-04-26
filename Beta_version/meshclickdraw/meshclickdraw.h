#ifndef MESHCLICKDRAW_IS_INCLUDED
#define MESHCLICKDRAW_IS_INCLUDED

#include <fssimplewindow.h>
#include <iostream>
#include "ysglfontdata.h"
#include <vector>
#include <math.h>

std::vector<float> bezier(std::vector<float> control_points);

void Initialize(std::vector<float> &col, std::vector<float> &vtx);

void RunOneStep(std::vector<float> &col, std::vector<float> &vtx, bool &term, std::vector <float> &allctrl);

void Draw(std::vector<float> &col, std::vector<float> &vtx);

std::vector<float> runClickDraw();

void crtlPts_mapping(std::vector <float> &orderedControl);

#endif