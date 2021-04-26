#include "drawMesh.h"

const int wid = 1200;
const int hei = 1000;
vector<point> verts;
vector<tvi> tvis;
vector<qvi> qvis;

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
    glColor3ub(0, 0, 255);
    glBegin(GL_LINE_LOOP);

    glVertex2f(x1, hei - y1);
    glVertex2f(x2, hei - y2);
    glVertex2f(x3, hei - y3);

    glEnd();
}

void DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    glColor3ub(0, 128, 255);
    glBegin(GL_LINE_LOOP);

    glVertex2f(x1, hei - y1);
    glVertex2f(x2, hei - y2);
    glVertex2f(x3, hei - y3);
    glVertex2f(x4, hei - y4);

    glEnd();
}

void read_mesh(string pathToFile, float &xmin, float &xmax, float &ymin, float &ymax){
    verts.clear();
    tvis.clear();
    qvis.clear();

    fstream mFile;
    mFile.open(pathToFile, ios::in);

    if (mFile.fail()) {
        cerr << "Failed to open file " << pathToFile << " !! Most likely cause is bad control points -> failed mesh" << endl;
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
                // scale to window
                if (x > xmax) {
                    xmax = x;
                }
                if (y > ymax) {
                    ymax = y;
                }
                if (x < xmin) {
                    xmin = x;
                }
                if (y < ymin) {
                    ymin = y;
                }
            }
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
}

void Draw_mesh(string pathToFile, float Xoffset, float Yoffset){
    // vars
    float xmax, ymax, xmin, ymin;

    // data processing
    // How to make sure your file can be opened in XCode in a Mac
    // https://stackoverflow.com/questions/23438393/new-to-xcode-cant-open-files-in-c/23449331
   
    read_mesh(pathToFile,xmin,xmax,ymin,ymax);
    xmin = 0;
    xmax = 80;
    ymin = 0;
    ymax = 50;

    // draw
    // draw here >>>
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
        DrawTriangle(x1, y1, x2, y2, x3, y3); 
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
        DrawQuad(x1, y1, x2, y2, x3, y3, x4, y4);
    }
}

void Draw_All_mesh(vector<string> AllPathToFile){
    int numFiles = AllPathToFile.size();
    int row, column;
    int x_off, y_off;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(int i=0; i<numFiles; ++i){
        row = i/4;
        column = i%4;
        y_off = row*187.5;
        x_off = column*300;
        Draw_mesh(AllPathToFile[i], x_off, y_off);
    }
    // <<< draw here
    FsSwapBuffers();
    FsSleep(25);
}