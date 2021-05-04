#include "meshclickdraw.h"
#include <iomanip>
#include <sstream>

std::vector<float> bezier(std::vector<float> control_points) {
    // control_points should have 5 points in it, define the two curves with this
    std::vector<float> points_on_curve;
    float ctrl_0_x = control_points[0]; // will be control pt 1
    float ctrl_0_y = control_points[1];
    float ctrl_1_x = control_points[2]; // will be control pt 3
    float ctrl_1_y = control_points[3];
    float ctrl_2_x = control_points[4];
    float ctrl_2_y = control_points[5]; // will be control pt 5
    float ctrl_3_x = control_points[6]; // last two will be the middle points
    float ctrl_3_y = control_points[7];
    float ctrl_4_x = control_points[8];
    float ctrl_4_y = control_points[9];
    for (int i = 0; i <= 100; i += 5) {
        float u = i / 100.0;
        float b_0_2 = pow((1 - u), 2);
        float b_1_2 = 2 * u * (1 - u);
        float b_2_2 = pow(u, 2);
        float point_x = b_0_2 * ctrl_0_x + b_1_2 * ctrl_3_x + b_2_2 * ctrl_1_x;
        float point_y = b_0_2 * ctrl_0_y + b_1_2 * ctrl_3_y + b_2_2 * ctrl_1_y;
        points_on_curve.push_back(point_x);
        points_on_curve.push_back(point_y);
    }
    for (int i = 0; i <= 100; i += 5) {
        float u = i / 100.0;
        float b_0_2 = pow((1 - u), 2);
        float b_1_2 = 2 * u * (1 - u);
        float b_2_2 = pow(u, 2);
        float point_x = b_0_2 * ctrl_1_x + b_1_2 * ctrl_4_x + b_2_2 * ctrl_2_x;
        float point_y = b_0_2 * ctrl_1_y + b_1_2 * ctrl_4_y + b_2_2 * ctrl_2_y;
        points_on_curve.push_back(point_x);
        points_on_curve.push_back(point_y);
    }
    return points_on_curve;
}

void Initialize(std::vector<float> &col, std::vector<float> &vtx) {
    vtx.clear();
    col.clear();
    // control pt 1 default
    vtx.push_back((float)200);
    vtx.push_back((float)200);
    col.push_back(0);
    col.push_back(0);
    col.push_back(0);
    col.push_back(1);
    // control pt 3 default
    vtx.push_back((float)380);
    vtx.push_back((float)340);
    col.push_back(0);
    col.push_back(0);
    col.push_back(0);
    col.push_back(1);
    // control pt 5 default
    vtx.push_back((float)600);
    vtx.push_back((float)375);
    col.push_back(0);
    col.push_back(0);
    col.push_back(0);
    col.push_back(1);
    // control pt 2 default
    vtx.push_back((float)300);
    vtx.push_back((float)200);
    col.push_back(0);
    col.push_back(0);
    col.push_back(0);
    col.push_back(1);
    // control pt 4 default
    vtx.push_back((float)400);
    vtx.push_back((float)375);
    col.push_back(0);
    col.push_back(0);
    col.push_back(0);
    col.push_back(1);
    std::cout << "Please define middle point for arcs, when arc visible, press 'SPACE' to confirm, "
                 "'R' to redo."
              << std::endl;
}

void RunOneStep(std::vector<float> &col, std::vector<float> &vtx, bool &term,
                std::vector<float> &allctrl, std::vector<float> &col_layout,
                std::vector<float> &vtx_layout, int &mouseLockOn) {
    auto key = FsInkey();
    if (FSKEY_ESC == key) {
        term = true;
    }

    enum CONTROL { // Control points mouse lock on ID
        ONE = 1,
        TWO,
        THREE,
        FOUR,
        FIVE
    };

    vtx = bezier(allctrl);
    col.clear();
    for (int i = 0; i < vtx.size(); i += 2) {
        col.push_back(0);
        col.push_back(0);
        col.push_back(0);
        col.push_back(1);
    }

    // Handle mouse drag and drop
    int lb, mb, rb, mx, my;
    auto evt = FsGetMouseEvent(lb, mb, rb, mx, my);
    int mousePrecision = 5;
    if (FSMOUSEEVENT_LBUTTONDOWN == evt) {
        if (mx > (allctrl[6] - mousePrecision) && mx < (allctrl[6] + mousePrecision) &&
            my > (allctrl[7] - mousePrecision) && my < (allctrl[7] + mousePrecision)) { // Ctrl 2
            mouseLockOn = CONTROL::TWO;
        } else if (mx > (allctrl[8] - mousePrecision) && mx < (allctrl[8] + mousePrecision) &&
                   my > (allctrl[9] - mousePrecision) &&
                   my < (allctrl[9] + mousePrecision)) { // Ctrl 4
            mouseLockOn = CONTROL::FOUR;
        } else if (mx > (allctrl[0] - mousePrecision) && mx < (allctrl[0] + mousePrecision) &&
                   my > (allctrl[1] - mousePrecision) &&
                   my < (allctrl[1] + mousePrecision)) { // Ctrl 1
            mouseLockOn = CONTROL::ONE;
        } else if (mx > (allctrl[2] - mousePrecision) && mx < (allctrl[2] + mousePrecision) &&
                   my > (allctrl[3] - mousePrecision) &&
                   my < (allctrl[3] + mousePrecision)) { // Ctrl 3
            mouseLockOn = CONTROL::THREE;
        } else if (mx > (allctrl[4] - mousePrecision) && mx < (allctrl[4] + mousePrecision) &&
                   my > (allctrl[mousePrecision] - mousePrecision) &&
                   my < (allctrl[mousePrecision] + mousePrecision)) { // Ctrl 5
            mouseLockOn = CONTROL::FIVE;
        }
    }
    if (FSMOUSEEVENT_LBUTTONUP == evt) {
        mouseLockOn = 0;
    }

    int distancePadding = 10;          // Distance constraint between adjacent ctrl points
    if (mouseLockOn == CONTROL::TWO) { // Ctrl 2
        // Fixed y value to guarantee continuity
        allctrl[7] = (float)my;
        float boundingLeft = allctrl[0] + distancePadding;
        float boundingRight = allctrl[2] - distancePadding;
        if (mx < boundingLeft) {
            allctrl[6] = boundingLeft;
        } else if (mx > boundingRight) {
            allctrl[6] = boundingRight;
        } else {
            allctrl[6] = (float)mx;
        }
    } else if (mouseLockOn == CONTROL::FOUR) { // Ctrl 4
        // Fixed y value to guarantee continuity
        allctrl[9] = (float)my;
        float boundingLeft = allctrl[2] + distancePadding;
        float boundingRight = allctrl[4] - distancePadding;
        if (mx < boundingLeft) {
            allctrl[8] = boundingLeft;
        } else if (mx > boundingRight) {
            allctrl[8] = boundingRight;
        } else {
            allctrl[8] = (float)mx;
        }
    } else if (mouseLockOn == CONTROL::ONE) { // Ctrl 1
        allctrl[1] = (float)my;
        // allctrl[7] = (float)my; // Ctrl 2 y constraint
        float boundingLeft = distancePadding;
        float boundingRight = allctrl[6] - distancePadding;
        if (mx < boundingLeft) {
            allctrl[0] = boundingLeft;
        } else if (mx > boundingRight) {
            allctrl[0] = boundingRight;
        } else {
            allctrl[0] = (float)mx;
        }
    } else if (mouseLockOn == CONTROL::THREE) { // Ctrl 3
        allctrl[3] = (float)my;
        float boundingLeft = allctrl[6] + distancePadding;
        float boundingRight = allctrl[8] - distancePadding;
        if (mx < boundingLeft) {
            allctrl[2] = boundingLeft;
        } else if (mx > boundingRight) {
            allctrl[2] = boundingRight;
        } else {
            allctrl[2] = (float)mx;
        }
    } else if (mouseLockOn == CONTROL::FIVE) { // Ctrl 5
        // Fixed y value
        float boundingLeft = allctrl[8] + distancePadding;
        float boundingRight = 800.0 - distancePadding;
        if (mx < boundingLeft) {
            allctrl[4] = boundingLeft;
        } else if (mx > boundingRight) {
            allctrl[4] = boundingRight;
        } else {
            allctrl[4] = (float)mx;
        }
    }

    // Handle user reset or confirmation
    if (FSKEY_SPACE == key) {
        term = true;
    } else if (FSKEY_R == key) {
        vtx.clear();
        col.clear();
        allctrl.clear();
        Initialize(col, allctrl);
    }

    // added code to draw layout
    vtx_layout.clear();
    col_layout.clear();
    vtx_layout.push_back(0);
    vtx_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(1);
    vtx_layout.push_back(800);
    vtx_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(1);
    vtx_layout.push_back(800);
    vtx_layout.push_back(375);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(1);
    vtx_layout.push_back(allctrl[4]);
    vtx_layout.push_back(375);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(1);
    vtx_layout.push_back(800);
    vtx_layout.push_back(375);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(1);
    vtx_layout.push_back(800);
    vtx_layout.push_back(500);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(1);
    vtx_layout.push_back(0);
    vtx_layout.push_back(500);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(1);
    vtx_layout.push_back(0);
    vtx_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(0);
    col_layout.push_back(1);
}

std::string parseFloatStr(float num) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << num;
    return stream.str();
}

void Draw_coordinates(std::vector<float> &allctrl) {
    std::stringstream stream;
    // draw coordinates
    int marginBottom = 20;
    glRasterPos2d(allctrl[0], allctrl[1] - marginBottom); // Ctrl 1
    glColor3ub(0, 0, 0);
    std::string ctrlOneLabel = "(" + parseFloatStr(allctrl[0] / 10.0) + "," +
                               parseFloatStr(50.0 - allctrl[1] / 10.0) + ")";
    YsGlDrawFontBitmap10x14(ctrlOneLabel.c_str());

    glRasterPos2d(allctrl[2], allctrl[3] - marginBottom); // Ctrl 3
    glColor3ub(0, 0, 0);
    std::string ctrlThreeLabel = "(" + parseFloatStr(allctrl[2] / 10.0) + "," +
                               parseFloatStr(50.0 - allctrl[3] / 10.0) + ")";
    YsGlDrawFontBitmap10x14(ctrlThreeLabel.c_str());

    glRasterPos2d(allctrl[4], allctrl[5] - marginBottom); // Ctrl 5
    glColor3ub(0, 0, 0);
    std::string ctrlFiveLabel = "(" + parseFloatStr(allctrl[4] / 10.0) + "," +
                               parseFloatStr(50.0 - allctrl[5] / 10.0) + ")";
    YsGlDrawFontBitmap10x14(ctrlFiveLabel.c_str());
}

void Draw(std::vector<float> &col, std::vector<float> &vtx, std::vector<float> &allctrl,
          std::vector<float> &col_layout, std::vector<float> &vtx_layout) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glPointSize(10);
    glColorPointer(4, GL_FLOAT, 0, col.data());

    // control points
    glVertexPointer(2, GL_FLOAT, 0, allctrl.data());
    glDrawArrays(GL_POINTS, 0, allctrl.size() / 2);

    // Bezier curve
    glVertexPointer(2, GL_FLOAT, 0, vtx.data());
    glDrawArrays(GL_LINE_STRIP, 0, vtx.size() / 2);

    // draw layout
    glColorPointer(4, GL_FLOAT, 0, col_layout.data());
    glVertexPointer(2, GL_FLOAT, 0, vtx_layout.data());
    glDrawArrays(GL_LINE_STRIP, 0, vtx_layout.size() / 2);
    Draw_coordinates(allctrl);

    glRasterPos2d(250, 50);
    glColor3ub(0, 0, 0);
    YsGlDrawFontBitmap16x20("Define 2 middle points for arcs");
    glColor3ub(0, 0, 255);
    glRasterPos2d(250, 80);
    YsGlDrawFontBitmap16x20("SPACE");
    glColor3ub(0, 0, 0);
    glRasterPos2d(320, 80);
    YsGlDrawFontBitmap16x20(" to confirm, R to redo.");
    glRasterPos2d(650, 450);
    YsGlDrawFontBitmap12x16("Tube inlet");

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    FsSwapBuffers();
}

std::vector<float> runClickDraw(std::vector<float> &allctrl) {
    bool term = false;
    std::vector<float> vtx, col;
    std::vector<float> vtxLayout, colLayout;
    if (allctrl.size() == 0) {
        Initialize(col, allctrl);
    } else {
        for (int i = 0; i < allctrl.size(); i++) { // convert to window scale
            if (i % 2 == 0) { // x coord
                allctrl[i] *= 10.0;
            } else { // y coord
                allctrl[i] = (50.0 - allctrl[i]) * 10.0;
            }
        }

        // convert to window order
        std::swap(allctrl[2], allctrl[4]);
        std::swap(allctrl[3], allctrl[5]);
        std::swap(allctrl[4], allctrl[6]);
        std::swap(allctrl[5], allctrl[7]);
        std::swap(allctrl[4], allctrl[8]);
        std::swap(allctrl[5], allctrl[9]);
    }
    int mouseLockOn = 0;
    while (!term) {
        FsPollDevice();
        RunOneStep(col, vtx, term, allctrl, colLayout, vtxLayout, std::ref(mouseLockOn));
        Draw(col, vtx, allctrl, colLayout, vtxLayout);
    }
    std::vector<float> orderedControl;
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
    for (int i = 0; i < orderedControl.size(); i += 2) {
        std::cout << "(" << orderedControl[i] << ", " << orderedControl[i + 1] << ")" << std::endl;
    }
    return orderedControl;
}

// added on 04/25/2021 to convert control points for genMesh
void crtlPts_mapping(std::vector<float> &orderedControl) {
    for (int i = 0; i < orderedControl.size(); i += 2) {
        orderedControl[i] /= 10;
        orderedControl[i + 1] = 50 - orderedControl[i + 1] / 10;
    }
}