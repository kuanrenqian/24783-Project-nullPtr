#include "meshclickdraw.h"

std::vector<float> bezier(std::vector<float> control_points) {
    //control_points should have 5 points in it, define the two curves with this
    std::vector <float> points_on_curve;
    float ctrl_0_x = control_points[0]; //will be control pt 1
    float ctrl_0_y = control_points[1];
    float ctrl_1_x = control_points[2]; //will be control pt 3
    float ctrl_1_y = control_points[3];
    float ctrl_2_x = control_points[4];
    float ctrl_2_y = control_points[5]; //will be control pt 5
    float ctrl_3_x = control_points[6];//last two will be the middle points
    float ctrl_3_y = control_points[7];
    float ctrl_4_x = control_points[8];
    float ctrl_4_y = control_points[9];
    for (float u = 0; u <= 1; u+=0.05) {
      float b_0_2 = pow((1 - u),2);
      float b_1_2 = 2 * u * (1 - u);
      float b_2_2 = pow(u,2);
      float point_x = b_0_2 * ctrl_0_x + b_1_2 * ctrl_3_x + b_2_2 * ctrl_1_x;
      float point_y = b_0_2 * ctrl_0_y + b_1_2 * ctrl_3_y + b_2_2 * ctrl_1_y;
      points_on_curve.push_back(point_x);
      points_on_curve.push_back(point_y);
    }
    for (float u = 0; u <= 1; u+=0.05) {
      float b_0_2 = pow((1 - u),2);
      float b_1_2 = 2 * u * (1 - u);
      float b_2_2 = pow(u,2);
      float point_x = b_0_2 * ctrl_1_x + b_1_2 * ctrl_4_x + b_2_2 * ctrl_2_x;
      float point_y = b_0_2 * ctrl_1_y + b_1_2 * ctrl_4_y + b_2_2 * ctrl_2_y;
      points_on_curve.push_back(point_x);
      points_on_curve.push_back(point_y);
    }
    return points_on_curve;
}

void Initialize(std::vector<float> &col, std::vector<float> &vtx)
{
  vtx.clear();
  col.clear();
  //control pt 1 fixed
  vtx.push_back((float)200);
  vtx.push_back((float)200);
  col.push_back(0);
  col.push_back(0);
  col.push_back(0);
  col.push_back(1);
  //control pt 3 fixed
  vtx.push_back((float)380);
  vtx.push_back((float)340);
  col.push_back(0);
  col.push_back(0);
  col.push_back(0);
  col.push_back(1);
  //control pt 5 fixed
  vtx.push_back((float)600);
  vtx.push_back((float)375);
  col.push_back(0);
  col.push_back(0);
  col.push_back(0);
  col.push_back(1);
  std::cout << "Please define middle point for arcs, when arc visible, press 'SPACE' to confirm, 'R' to redo." << std::endl;
}

void RunOneStep(std::vector<float> &col, std::vector<float> &vtx, bool &term, std::vector <float> &allctrl, std::vector<float> &col_layout, std::vector<float> &vtx_layout)
{
  auto key=FsInkey();
  if(FSKEY_ESC==key)
  {
    term=true;
  }

  if ((vtx.size() == 6) || (vtx.size() == 8)) {
    int lb,mb,rb,mx,my;
    auto evt=FsGetMouseEvent(lb,mb,rb,mx,my);
    if(FSMOUSEEVENT_LBUTTONDOWN==evt)
    {
      float ptx = (float)mx;
      float pty = (float)my;
      //not sure if you have to change this invalid case? 1 seems really small
      if (vtx.size() == 6 && (abs(ptx - vtx[0]) < 1 || abs(pty - vtx[1]) < 1 || abs(ptx - vtx[2]) < 1 || abs(pty - vtx[3]) < 1)) {
        std::cout << "Invalid point (distance between adjacent control points must be at least 1): Please try again." << std::endl;
        return;
      } else if (vtx.size() == 8 && (abs(ptx - vtx[2]) < 1 || abs(pty - vtx[3]) < 1 || abs(ptx - vtx[4]) < 1 || abs(pty - vtx[5]) < 1)) {
        std::cout << "Invalid point (distance between adjacent control points must be at least 1): Please try again." << std::endl;
        return;
      } else {
        vtx.push_back(ptx);
        vtx.push_back(pty);
        col.push_back(0);
        col.push_back(0);
        col.push_back(0);
        col.push_back(1);
      }
    }
  } else if (vtx.size() == 10) {
    allctrl = vtx;
    vtx = bezier(vtx);
    col.clear();
    for (int i = 0; i < vtx.size(); i+=2) {
      col.push_back(0);
      col.push_back(0);
      col.push_back(0);
      col.push_back(1);
    }
  } 
  else {
    if(FSKEY_SPACE==key)
    {
      term=true;
    } else if (FSKEY_R==key) {
      vtx.clear();
      col.clear();
      allctrl.clear();
      Initialize(col,vtx);
    }
  }

  // added code to draw layout
  vtx_layout.push_back(0);    vtx_layout.push_back(0);
  col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(1);
  vtx_layout.push_back(800);  vtx_layout.push_back(0);
  col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(1);
  vtx_layout.push_back(800);  vtx_layout.push_back(375);
  col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(1);
  vtx_layout.push_back(600);  vtx_layout.push_back(375);
  col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(1);
  vtx_layout.push_back(800);  vtx_layout.push_back(375);
  col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(1);
  vtx_layout.push_back(800);  vtx_layout.push_back(500);
  col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(1);
  vtx_layout.push_back(0);    vtx_layout.push_back(500);
  col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(1);
  vtx_layout.push_back(0);    vtx_layout.push_back(0);
  col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(0);    col_layout.push_back(1);
}

void Draw_coordinates(){
  // draw coordinates
  glRasterPos2d(200,180);
  glColor3ub(0, 0, 0);
  YsGlDrawFontBitmap10x14("(20,30)");
  glRasterPos2d(380,320);
  glColor3ub(0, 0, 0);
  YsGlDrawFontBitmap10x14("(40,160)");
  glRasterPos2d(600,355);
  glColor3ub(0, 0, 0);
  YsGlDrawFontBitmap10x14("(60,12.5)");
}

void Draw(std::vector<float> &col, std::vector<float> &vtx, std::vector<float> &col_layout, std::vector<float> &vtx_layout)
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_SMOOTH);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glPointSize(10);
  glColorPointer(4,GL_FLOAT,0,col.data());
  glVertexPointer(2,GL_FLOAT,0,vtx.data());
  if (vtx.size() == 6 || vtx.size() == 8) {
    glDrawArrays(GL_POINTS,0,vtx.size()/2);
  }
  else {
    glDrawArrays(GL_LINE_STRIP,0,vtx.size()/2);
  }

  // draw layout
  glColorPointer(4,GL_FLOAT,0,col_layout.data());
  glVertexPointer(2,GL_FLOAT,0,vtx_layout.data());
  glDrawArrays(GL_LINE_STRIP,0,vtx_layout.size()/2);
  Draw_coordinates();

  glRasterPos2d(250,50);
  glColor3ub(0, 0, 0);
  YsGlDrawFontBitmap16x20("Define 2 middle points for arcs");
  glRasterPos2d(250,80);
  YsGlDrawFontBitmap16x20("SPACE to confirm, R to redo.");
  glRasterPos2d(600,450);
  YsGlDrawFontBitmap12x16("Vaccum tube inlet");

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  FsSwapBuffers();
}

std::vector<float> runClickDraw()
{
  // FsOpenWindow(0,0,800,600,1);
  bool term=false;
  std::vector <float> vtx,col,allctrl;
  std::vector <float> vtxLayout,colLayout;
  Initialize(col,vtx);
  while (!term) {
    FsPollDevice();
    RunOneStep(col,vtx,term,allctrl,colLayout,vtxLayout);
    Draw(col,vtx,colLayout,vtxLayout);
  }

  std::vector <float> orderedControl;
  orderedControl.push_back(allctrl[0]);
  orderedControl.push_back(allctrl[1]);
  orderedControl.push_back(allctrl[6]);
  orderedControl.push_back(allctrl[7]);
  orderedControl.push_back(allctrl[2]);
  orderedControl.push_back(allctrl[3]);
  orderedControl.push_back(allctrl[8]);
  orderedControl.push_back(allctrl[9]);
  orderedControl.push_back(allctrl[4]);
  orderedControl.push_back(allctrl[5]);

  crtlPts_mapping(orderedControl);

  std::cout << "Control points in order are as follows:" << std::endl;
  for (int i = 0; i < orderedControl.size(); i+=2) {
    std::cout << "(" << orderedControl[i] << ", " << orderedControl[i+1] << ")" << std::endl;
  }
  return orderedControl;
}

// added on 04/25/2021 to convert control points for genMesh
void crtlPts_mapping(std::vector <float> &orderedControl){
  for(int i=0; i<orderedControl.size(); i+=2){
    orderedControl[i] /= 10;
    orderedControl[i+1] = 50-orderedControl[i+1]/10;
  }
}