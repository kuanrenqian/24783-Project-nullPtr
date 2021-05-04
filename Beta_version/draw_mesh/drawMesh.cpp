#include "drawMesh.h"
#include "genSim.h"

// const int wid = 900;
const int hei = 800;
vector<point> verts;
vector<tvi> tvis;
vector<qvi> qvis;

std::vector <float> vtx_tri;
std::vector <float> col_tri;

std::vector <float> vtx_quad;
std::vector <float> col_quad;

std::vector<float> aspectRatio;

// 2d point
struct point {
    float x, y;
};

// triangle vertex indices
struct tvi {
    int a, b, c;
};

// quad vertex indices
struct qvi {
    int a, b, c, d;
};

// TODO: try gl_points instead of gl_lines to see at least the vertices are at the right place
void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    glColor3ub(0, 0, 0);
    glBegin(GL_LINE_LOOP);

    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);

    glEnd();
}

void DrawCircle(int cx, int cy, int rad, int fill) {
    if (0 != fill)
    {
        glBegin(GL_POLYGON);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }

    int i;
    for (i = 0; i < 64; i++)
    {
        double angle = (double)i * 3.1415926 / 32.0;
        double x = (double)cx + cos(angle) * (double)rad;
        double y = (double)cy + sin(angle) * (double)rad;
        glVertex2d(x, y);
    }

    glEnd();
}

void DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    glLineWidth(0.005);
    glColor3ub(0, 0, 0);
    glBegin(GL_LINE_LOOP);

    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);

    glEnd();
}

int read_mesh(string pathToFile, float &xmin, float &xmax, float &ymin, float &ymax) {
    verts.clear();
    tvis.clear();
    qvis.clear();

    fstream mFile;
    mFile.open(pathToFile, ios::in);

    if (mFile.fail()) {
        cerr << "Failed to open file " << pathToFile << " !! Most likely cause is bad control points -> failed mesh" << endl;
        return 1;
    }
    if (mFile.is_open()) {
        string line;
        while (getline(mFile, line)) {
            // find type of line
            int start = 0;
            int end = line.find(" ");
            string type = line.substr(start, end - start);
            // type 0
            if (type == "GRID") {
                // find x coord
                start = 24; // Start index of coordinates
                string x_str = line.substr(start, 8);
                // find y coord
                string y_str = line.substr(start + 8, 8);
                // string to float stof
                if (x_str == "0.00E+00") {
                    x_str = "0.0";
                }
                if (y_str == "0.00E+00") {
                    y_str = "0.0";
                }
                float x = stof(x_str);
                float y = stof(y_str);
                point p;
                p.x = x;
                p.y = y;
                verts.push_back(p);
            }
            xmin = 0;
            xmax = 80;
            ymin = 0;
            ymax = 50;
            // type 1
            if (type == "CTRIA3") {
                // first vertex's index
                start = 24; // TODO: Hardcoded
                end = line.find(" ", start);
                string a_str = line.substr(start, end - start);
                // second idx
                start = 32; // TODO: Hardcoded
                end = line.find(" ", start);
                string b_str = line.substr(start, end - start);
                // third idx
                start = 40; // TODO: Hardcoded
                end = line.find(" ", start);
                string c_str = line.substr(start, end - start);

                // stoi
                tvi i; // a,b,c vertex indices
                i.a = stoi(a_str);
                i.b = stoi(b_str);
                i.c = stoi(c_str);
                tvis.push_back(i);
            }
            // type 2
            if (type == "CQUAD4") {
                // first vertex's index
                start = 24; // TODO: Hardcoded
                end = line.find(" ", start);
                string a_str = line.substr(start, end - start);
                // second idx
                start = 32; // TODO: Hardcoded
                end = line.find(" ", start);
                string b_str = line.substr(start, end - start);
                // third idx
                start = 40; // TODO: Hardcoded
                end = line.find(" ", start);
                string c_str = line.substr(start, end - start);
                // fourth idx
                start = 48; // TODO: Hardcoded
                end = line.find(" ", start);
                string d_str = line.substr(start, end - start);

                // stoi
                qvi i; // abcd vertex indices
                i.a = stoi(a_str);
                i.b = stoi(b_str);
                i.c = stoi(c_str);
                i.d = stoi(d_str);
                qvis.push_back(i);
            }
        }
        mFile.close();
    }
    return 0;
}

int Draw_mesh(string pathToFile, float Xoffset, float Yoffset) {
    // vars
    float xmax, ymax, xmin, ymin;

    // data processing
    // How to make sure your file can be opened in XCode in a Mac
    // https://stackoverflow.com/questions/23438393/new-to-xcode-cant-open-files-in-c/23449331
   
    if (read_mesh(pathToFile,xmin,xmax,ymin,ymax) != 0) {
        return 1;
    }
    // hard coding dimension - some potential issue with reading max from .bdf

    // draw here >>>
    float R,G,B;
    for (int i = 0; i < tvis.size(); i++) {
        // scale to window
        float x1 = verts[tvis[i].a - 1].x;
        float y1 = verts[tvis[i].a - 1].y;
        float x2 = verts[tvis[i].b - 1].x;
        float y2 = verts[tvis[i].b - 1].y;
        float x3 = verts[tvis[i].c - 1].x;
        float y3 = verts[tvis[i].c - 1].y;
        // hard coded 300&187.5 maybe fixing in the future
        x1 = (x1-xmin)/xmax*300+Xoffset;   y1 = (y1-ymin)/ymax*187.5+Yoffset;
        x2 = (x2-xmin)/xmax*300+Xoffset;   y2 = (y2-ymin)/ymax*187.5+Yoffset;
        x3 = (x3-xmin)/xmax*300+Xoffset;   y3 = (y3-ymin)/ymax*187.5+Yoffset;

        // calculating and drawing skewness
        vtx_tri.push_back(x1); vtx_tri.push_back(hei-y1); vtx_tri.push_back(0);
        vtx_tri.push_back(x2); vtx_tri.push_back(hei-y2); vtx_tri.push_back(0);
        vtx_tri.push_back(x3); vtx_tri.push_back(hei-y3); vtx_tri.push_back(0);

        float skewness = calc_Skewness(x1, y1, x2, y2, x3, y3);
        if(skewness > 0.5){
            R=1;    G=0;    B=0;
        }
        else{
            parula_colormap(skewness,1,R,G,B);
        }         
        col_tri.push_back(R); col_tri.push_back(G); col_tri.push_back(B); col_tri.push_back(1);
        col_tri.push_back(R); col_tri.push_back(G); col_tri.push_back(B); col_tri.push_back(1);
        col_tri.push_back(R); col_tri.push_back(G); col_tri.push_back(B); col_tri.push_back(1);

    }

    for (int i = 0; i < qvis.size(); i++) {
        // scale to window
        float x1 = verts[qvis[i].a - 1].x;
        float y1 = verts[qvis[i].a - 1].y;
        float x2 = verts[qvis[i].b - 1].x;
        float y2 = verts[qvis[i].b - 1].y;
        float x3 = verts[qvis[i].c - 1].x;
        float y3 = verts[qvis[i].c - 1].y;
        float x4 = verts[qvis[i].d - 1].x;
        float y4 = verts[qvis[i].d - 1].y;
        x1 = (x1-xmin)/xmax*300+Xoffset;   y1 = (y1-ymin)/ymax*187.5+Yoffset;
        x2 = (x2-xmin)/xmax*300+Xoffset;   y2 = (y2-ymin)/ymax*187.5+Yoffset;
        x3 = (x3-xmin)/xmax*300+Xoffset;   y3 = (y3-ymin)/ymax*187.5+Yoffset;        
        x4 = (x4-xmin)/xmax*300+Xoffset;   y4 = (y4-ymin)/ymax*187.5+Yoffset;
        // DrawQuad(x1, y1, x2, y2, x3, y3, x4, y4);
        vtx_quad.push_back(x1); vtx_quad.push_back(hei-y1); vtx_quad.push_back(0);
        vtx_quad.push_back(x2); vtx_quad.push_back(hei-y2); vtx_quad.push_back(0);
        vtx_quad.push_back(x3); vtx_quad.push_back(hei-y3); vtx_quad.push_back(0);
        vtx_quad.push_back(x4); vtx_quad.push_back(hei-y4); vtx_quad.push_back(0);

        aspectRatio.push_back(calc_aspectRatio(x1, y1, x2, y2, x3, y3, x4, y4));
    }

    float maxAR = *max_element(aspectRatio.begin(), aspectRatio.end());
    for (int i = 0; i < qvis.size(); i++) {
        parula_colormap(aspectRatio[i],maxAR,R,G,B);
        col_quad.push_back(R); col_quad.push_back(G); col_quad.push_back(B); col_quad.push_back(1);
        col_quad.push_back(R); col_quad.push_back(G); col_quad.push_back(B); col_quad.push_back(1);
        col_quad.push_back(R); col_quad.push_back(G); col_quad.push_back(B); col_quad.push_back(1);
        col_quad.push_back(R); col_quad.push_back(G); col_quad.push_back(B); col_quad.push_back(1);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, col_tri.data());
    glVertexPointer(3, GL_FLOAT, 0, vtx_tri.data());
    glDrawArrays(GL_TRIANGLES, 0, vtx_tri.size()/3);

    glColorPointer(4, GL_FLOAT, 0, col_quad.data());
    glVertexPointer(3, GL_FLOAT, 0, vtx_quad.data());
    glDrawArrays(GL_QUADS, 0, vtx_quad.size()/4);

    for (int i = 0; i < vtx_tri.size();i+=9) {
        float x1 = vtx_tri[i];      float y1 = vtx_tri[i+1];    // skipping z
        float x2 = vtx_tri[i+3];    float y2 = vtx_tri[i+4];    // skipping z
        float x3 = vtx_tri[i+6];    float y3 = vtx_tri[i+7];    // skipping z
        DrawTriangle(x1, y1, x2, y2, x3, y3); 
    }

    for (int i = 0; i < vtx_quad.size();i+=12) {
        float x1 = vtx_quad[i];      float y1 = vtx_quad[i+1];  // skipping z
        float x2 = vtx_quad[i+3];    float y2 = vtx_quad[i+4];  // skipping z
        float x3 = vtx_quad[i+6];    float y3 = vtx_quad[i+7];  // skipping z
        float x4 = vtx_quad[i+9];    float y4 = vtx_quad[i+10]; // skipping z
        DrawQuad(x1, y1, x2, y2, x3, y3, x4, y4);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glColor3ub(0, 0, 0);
    glRasterPos2d(250,50);
    YsGlDrawFontBitmap16x20("Displaying mesh");
    glColor3ub(0, 0, 255);
    glRasterPos2d(250,80);
    YsGlDrawFontBitmap16x20("SPACE");
    glColor3ub(0, 0, 0);
    glRasterPos2d(320,80);
    YsGlDrawFontBitmap16x20(" to continue");
    return 0;
}

int Draw_All_mesh(vector<string> AllPathToFile) {
    int numFiles = AllPathToFile.size();
    int row, column;
    int x_off, y_off;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(int i=0; i<numFiles; ++i){
        row = i/3;
        column = i%3;
        y_off = row*187.5;
        x_off = column*300;
        if (Draw_mesh(AllPathToFile[i], x_off, y_off) != 0) {
            return 1;
        }
    }
    legend_skewness_aspectRatio();
    
    FsSwapBuffers();
    FsSleep(25);

    return 0;
}

float lengthSquare(pair<float,float> X, pair<float,float> Y) {
    float xDiff = X.first - Y.first;
    float yDiff = X.second - Y.second;
    return xDiff*xDiff + yDiff*yDiff;
}

void getAngle(pair<float,float> A, pair<float,float> B, pair<float,float> C, vector<float> &angles) {
    // Square of lengths be a2, b2, c2
    float a2 = lengthSquare(B,C);
    float b2 = lengthSquare(A,C);
    float c2 = lengthSquare(A,B);
    // length of sides be a, b, c
    float a = sqrt(a2);
    float b = sqrt(b2);
    float c = sqrt(c2);

    // From Cosine law
    float alpha = acos((b2 + c2 - a2)/(2*b*c));
    float betta = acos((a2 + c2 - b2)/(2*a*c));
    float gamma = acos((a2 + b2 - c2)/(2*a*b));

    // Converting to degree
    angles[0] = alpha * 180 / PI;
    angles[1] = betta * 180 / PI;
    angles[2] = gamma * 180 / PI;

} 

float calc_Skewness(float x1, float y1, float x2, float y2, float x3, float y3) {

    pair<float,float> a = make_pair(x1,y1);
    pair<float,float> b = make_pair(x2,y2);
    pair<float,float> c = make_pair(x3,y3);
    vector<float> angles (3);
    getAngle(a,b,c,angles);
    sort(angles.begin(), angles.end());

    return max((angles[2]-angles[1])/(180-angles[1]),(angles[1]-angles[0])/angles[1]);
}

float calc_aspectRatio(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

    pair<float,float> a = make_pair(x1,y1);
    pair<float,float> b = make_pair(x2,y2);
    pair<float,float> c = make_pair(x3,y3);
    pair<float,float> d = make_pair(x4,y4);
    vector<float> edgeLength (4);
    // length of 4 sides
    edgeLength[0] = sqrt(lengthSquare(a,b));
    edgeLength[1] = sqrt(lengthSquare(b,c));
    edgeLength[2] = sqrt(lengthSquare(c,d));
    edgeLength[3] = sqrt(lengthSquare(d,a));
    sort(edgeLength.begin(), edgeLength.end());

    return edgeLength[2]/edgeLength[1];
}

void legend_skewness_aspectRatio(){
    glColor3ub(0, 0, 0);
    glRasterPos2d(250,110);
    YsGlDrawFontBitmap10x14("Triangle - Skewness, Quads - Aspect Ratio");
}