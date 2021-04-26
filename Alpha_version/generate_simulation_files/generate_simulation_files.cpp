#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#include <exception>
#include <filesystem>
#include <gmsh.h>
#include <math.h>
#include <set>
#include <fssimplewindow.h>
#include "ysclass.h"
#include <vector>

void generate_mesh(double radius, int numSamples){
    // by Shu using Gmsh
    //generate_mesh() implements gmsh, but with some unexpected issue with commandline
    //when gmsh is used, can not call fluent later on
    //this function works, just not with run_simualtion() at the same time
    gmsh::initialize();

    double lc = 1; // 1e-2; // Target mesh size

    double bboxHeightLimit = 50.0;
    double bboxWidthLimit = 80.0;
    double lowerOutletLimit = 12.5;
    double wallThickness = 2.0; // can be changed
    double upperOutletLimit = lowerOutletLimit + wallThickness;

    double ctrlOneX = 20.0; // Three equal segments
    double ctrlThreeX = 40.0;
    double ctrlThreeY = (radius + lowerOutletLimit) / 2; // can be changed
    double ctrlFiveX = 60.0;

    double ctrlInterval = (ctrlThreeX - ctrlOneX) / (double)(numSamples + 1);

    for (int i = 0; i < numSamples; i++) {
        gmsh::initialize();

        double safeOffset = 0.0;

        double ctrlTwoX = ctrlOneX + (i + 1) * ctrlInterval + safeOffset;
        double ctrlFourX = ctrlFiveX - (i + 1) * ctrlInterval - safeOffset;

        double theta = atan2(radius - lowerOutletLimit, ctrlFourX - ctrlTwoX);

        double ctrlThreeXUpper = ctrlThreeX + wallThickness * sin(theta);
        double ctrlThreeYUpper = ctrlThreeY + wallThickness * cos(theta);

        // Define fixed point coordinates
        gmsh::model::geo::addPoint(bboxWidthLimit, bboxHeightLimit, 0, lc, 1); // Upper right corner
        gmsh::model::geo::addPoint(0, bboxHeightLimit, 0, lc, 2);              // Upper left corner
        gmsh::model::geo::addPoint(0, 0, 0, lc, 3);                            // Lower left corner
        gmsh::model::geo::addPoint(bboxWidthLimit, 0, 0, lc, 4);               // Lower right corner
        gmsh::model::geo::addPoint(bboxWidthLimit, lowerOutletLimit, 0, lc, 5); // Lower outlet
        gmsh::model::geo::addPoint(bboxWidthLimit, upperOutletLimit, 0, lc, 6); // Upper outlet

        // Bezier control points, start from left
        gmsh::model::geo::addPoint(ctrlOneX, radius, 0, lc, 7);             // Lower crtl 1
        gmsh::model::geo::addPoint(ctrlTwoX, radius, 0, lc, 8);             // Lower crtl 2
        gmsh::model::geo::addPoint(ctrlThreeX, ctrlThreeY, 0, lc, 9);       // Lower crtl 3
        gmsh::model::geo::addPoint(ctrlFourX, lowerOutletLimit, 0, lc, 10); // Lower crtl 4
        gmsh::model::geo::addPoint(ctrlFiveX, lowerOutletLimit, 0, lc, 11); // Lower crtl 5

        gmsh::model::geo::addPoint(ctrlOneX, radius + wallThickness, 0, lc, 12); // Upper crtl 1
        gmsh::model::geo::addPoint(ctrlTwoX, radius + wallThickness, 0, lc, 13); // Upper crtl 2
        gmsh::model::geo::addPoint(ctrlThreeXUpper, ctrlThreeYUpper, 0, lc, 14); // Upper crtl 3
        gmsh::model::geo::addPoint(ctrlFourX, upperOutletLimit, 0, lc, 15);      // Upper crtl 4
        gmsh::model::geo::addPoint(ctrlFiveX, upperOutletLimit, 0, lc, 16);      // Upper crtl 5

        gmsh::model::geo::addPoint(ctrlOneX, radius + wallThickness / 2.0, 0, lc,
                                   17); // tip circular center

        // Define boundary lines
        gmsh::model::geo::addLine(1, 2, 1);
        gmsh::model::geo::addLine(2, 3, 2);
        gmsh::model::geo::addLine(3, 4, 3);
        gmsh::model::geo::addLine(4, 5, 4);
        gmsh::model::geo::addLine(6, 1, 5);

        // Define nozzle wall using Bezier curves and lines
        gmsh::model::geo::addBezier({7, 8, 9}, 6);
        gmsh::model::geo::addBezier({9, 10, 11}, 7);
        gmsh::model::geo::addBezier({12, 13, 14}, 8);
        gmsh::model::geo::addBezier({14, 15, 16}, 9);
        gmsh::model::geo::addCircleArc(12, 17, 7, 10);
        gmsh::model::geo::addLine(11, 5, 11);
        gmsh::model::geo::addLine(16, 6, 12);

        // Define surfaces
        gmsh::model::geo::addCurveLoop({1, 2, 3, 4, -11, -7, -6, -10, 8, 9, 12, 5}, 1);
        gmsh::model::geo::addPlaneSurface({1}, 1);

        // Synchronize model before mesh
        gmsh::model::geo::synchronize();

        // Define physical groups
        int fluid_tag = gmsh::model::addPhysicalGroup(2, {1}, 11);
        gmsh::model::setPhysicalName(2, fluid_tag, "fluid");

        int inlet_tag = gmsh::model::addPhysicalGroup(1, {4}, 12);
        gmsh::model::setPhysicalName(1, inlet_tag, "inlet");

        int outlet_tag = gmsh::model::addPhysicalGroup(1, {1, 2, 5}, 13);
        gmsh::model::setPhysicalName(1, outlet_tag, "outlet");

        int wall_tag = gmsh::model::addPhysicalGroup(1, {6, 7, 8, 9, 10, 11, 12}, 14);
        gmsh::model::setPhysicalName(1, wall_tag, "wall");

        int axis_tag = gmsh::model::addPhysicalGroup(1, {3}, 15);
        gmsh::model::setPhysicalName(1, axis_tag, "axis");

        // Define Boundary Layer
        gmsh::model::mesh::field::add("BoundaryLayer", 1);
        gmsh::model::mesh::field::setNumber(1, "hfar", 0.05);
        gmsh::model::mesh::field::setNumber(1, "hwall_n", 0.05);
        gmsh::model::mesh::field::setNumber(1, "thickness", 3);
        gmsh::model::mesh::field::setNumber(1, "ratio", 1.1);
        gmsh::model::mesh::field::setNumber(1, "AnisoMax", 10);
        gmsh::model::mesh::field::setNumber(1, "Quads", 1);

        gmsh::model::mesh::field::setNumbers(1, "CurvesList", {6, 7, 8, 9, 10, 11, 12});
        gmsh::model::mesh::field::setNumbers(1, "PointsList", {5, 6, 7, 12});

        gmsh::model::mesh::field::setAsBoundaryLayer(1);

        // Set mesh option and generate mesh
        gmsh::option::setNumber("Mesh.MeshSizeFactor", 1);
        gmsh::model::mesh::generate(2);

        // Save generated mesh to disk as Nastran Bulk Data file
        gmsh::option::setNumber("Mesh.SaveElementTagType", 2);
        std::string dirName("output/");
        std::filesystem::create_directory(dirName); // create output directory for generated mesh
        std::string fileName("m_" + std::to_string(radius) + "_" + std::to_string(i) + ".bdf");
        gmsh::write(dirName + fileName);

        gmsh::finalize();
    }

    std::cout << "All mesh successfully generated!" << std::endl;
}

void generate_mesh_exe(float radius, int numSamples){
    //This is a version of generate_mesh function, but using genMesh.exe instead of 
    //directly using gmsh. This solves the issue with commandline problem
    printf ("Checking if system is available...");
    if (system(NULL)) puts ("Ok");
    else exit (EXIT_FAILURE);
    printf ("Executing genMesh in batch mode...\n");
    string command = ".\\generate_mesh\\Release\\genMesh.exe " + to_string(radius) + " " + to_string(numSamples);
    const char *command_char = command.c_str();
    system (command_char);
}

void generate_simulation_files(string InFile, int numSimu){
    // This function generate simulation journal files for fluent to read
    // settings are for a 2D axis-symmetry fliud problem with pressure BCs
    string axisymetric = "y";
    string steady = "y";
    string pressure_based = "y";
    string laminar = "y";
    string hyb_initialization = "y";
    int iteration = 2000;

    ofstream file;
    // mesh input
    file.open ("./output/simulation_input_journal" + to_string(numSimu) + ".jou");
    file << "sync-chdir ./output\n";
    file << "/file/import/nastran/bulkdata " + InFile + "\n";
    // simulation setup
    file << "/define/models/axisymmetric " + axisymetric + "\n";
    file << "/define/models/steady " + steady + "\n";
    file << "/define/models/solver/pressure-based " + pressure_based + "\n";
    file << "/define/models/viscous/laminar " + laminar + "\n";
    // materail assignment
    file << "/define/materials/change-create air air_test n n n n n n n n\n";
    file << "/define/boundary-conditions/zone-type solid-11 fluid\n";
    file << "/define/boundary-conditions fluid solid-11 y air_test n n n n n n\n";
    // Boundary conditions (semi-hardcoding)
    file << "/define/boundary-condition/modify-zone/zone-type wall-15 axis\n";
    file << "/define/boundary-condition/modify-zone/zone-type wall-13 pressure-inlet\n";
    file << "/define/boundary-condition/modify-zone/zone-type wall-12 pressure-outlet\n";
    file << "/define/boundary-conditions/pressure-inlet wall-13 y n 1000 n 0 n y\n";
    // solver setup
    file << "/solve/set/p-v-coupling 20\n";
    file << "/solve/set/discretization-scheme pressure 10\n";
    file << "/solve/set/discretization-scheme mom 1\n";
    file << "/solve/set/gradient-scheme n y\n";
    file << "/solve/initialize/hyb-initialization " + hyb_initialization + "\n";
    file << "/solve/iterate " + to_string(iteration) + "\n";

    file << "/file/export/ascii test_result_" + to_string(numSimu) + ".txt default_interior-11 () n pressure velocity-magnitude () n\n";
    file << "/exit yes\n";

    file.close();

}

void run_simulation_files(int numSimu){
    // This function calls fluent to run a double precision, no gui simulation using generated journal file
    printf ("Checking if system is available...");
    if (system(NULL)) puts ("Ok");
    else exit (EXIT_FAILURE);
    printf ("Executing Fluent in batch mode...\n");
    string command = "fluent 2ddp -g -i ./output/simulation_input_journal" + to_string(numSimu) + ".jou";
    const char *command_char = command.c_str();
    system (command_char);
}

float parula_colormap(float input, float max_input, float &R, float &G, float &B){
    // This function generate RGB based on input pressure or velocity value
    // predefined colormap parula (from Matlab)
    std::vector <float> parulaR{    
        0.2422,
        0.2667,
        0.2797,
        0.2796,
        0.2567,
        0.1867,
        0.1700,
        0.1356,
        0.0831,
        0.0040,
        0.1466,
        0.2291,
        0.3802,
        0.5675,
        0.7455,
        0.8934,
        0.9961,
        0.9857,
        0.9595,
        0.9769};
    std::vector <float> parulaG{    
        0.1504,
        0.2028,
        0.2699,
        0.3435,
        0.4185,
        0.4981,
        0.5681,
        0.6315,
        0.6879,
        0.7296,
        0.7597,
        0.7880,
        0.8026,
        0.7936,
        0.7657,
        0.7348,
        0.7445,
        0.8228,
        0.9058,
        0.9839};
    std::vector <float> parulaB{
        0.6603,
        0.8087,
        0.9141,
        0.9710,
        0.9962,
        0.9841,
        0.9374,
        0.8954,
        0.8494,
        0.7701,
        0.6797,
        0.5757,
        0.4448,
        0.2998,
        0.1789,
        0.1712,
        0.2362,
        0.1841,
        0.1463,
        0.0805};

    // scale input value using max_input, and assign RGB accordingly
    int i = (int)(input/max_input*20);
    R = parulaR[i];
    G = parulaG[i];
    B = parulaB[i];

}

void read_and_plot_testResultTXT(int numSimu, vector <float> &vtx2d, vector <float> &col2d_rgb, vector <float> &col2d){
    // This function reads simulation results generated by fluent and plot the results
    fstream myfile;
    myfile.open("./output/test_result_" + to_string(numSimu) + ".txt", std::ios_base::in);
    if (!myfile)  {                     // if it does not work
        cerr << "Can't open Data! ./output/test_result_" + to_string(numSimu)+ "\n";
    }

    // read header (first line)
    string nodeID, xx, yy, pp, vv;
    myfile >> nodeID >> xx >> yy >> pp >> vv;

    float x,y,p,v;
    // read data while end of file is not reached
    while(myfile.eof()!=true){
        myfile >> nodeID >> xx >> yy >> pp >> vv;
        x = stof(xx)*10;
        y = stof(yy)*10;
        p = stof(pp);
        v = stof(vv);
        
        vtx2d.push_back(x);  vtx2d.push_back(y);
        col2d_rgb.push_back(p);  col2d_rgb.push_back(p); col2d_rgb.push_back(p);
    }

    float max_val = *max_element(col2d_rgb.begin(), col2d_rgb.end());

    float r,g,b,val;
    for(int i=0; i<col2d_rgb.size()-3; i+=3){
        val = col2d_rgb[i];
        parula_colormap(val,max_val,r,g,b);
        col2d.push_back(r);  col2d.push_back(g); col2d.push_back(b); col2d.push_back(1);
    }
}

void Draw(vector <float> &vtx2d, vector <float> &col2d){
    // This function draws simulation results read by read_and_plot_testResultTXT()
    // draws GL_POINTS
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(4);
	glColorPointer(4,GL_FLOAT,0,col2d.data());
	glVertexPointer(2,GL_FLOAT,0,vtx2d.data());
	glDrawArrays(GL_POINTS,0,vtx2d.size()/2);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_POINT_SMOOTH);

	FsSwapBuffers();
    FsSleep(20);
}

int main(){

    float minRadius, maxRadius, radius;
    int batch;
    string fileName;

    cout << "Input Radius lower range: [25~35, float]" << endl;
    cin >> minRadius;
    cout << "Input Radius upper range: [25~35, float]" << endl;
    cin >> maxRadius;
    cout << "Input batch size: [1~20, int]" << endl;
    cin >> batch;

    for (int numSimu=1; numSimu<=batch; ++numSimu){

        radius = minRadius+(maxRadius-minRadius)/batch*(numSimu-1);
        cout << radius << endl;
        generate_mesh_exe(radius, 0);

        fileName = "m_"+to_string(radius)+"_0.bdf";
        generate_simulation_files(fileName,numSimu);

        run_simulation_files(numSimu);
    }

    // bool term = false;

    // FsPollDevice();

    // auto key=FsInkey();
	// if(FSKEY_ESC==key)
	// {
	// 	term=true;
	// }

	// std::vector <float> vtx2d;	// Extruded Shape Triangles
	// std::vector <float> col2d_rgb;
	// std::vector <float> col2d;

    // // Test for read_and_plot_testResultTXT()
    // read_and_plot_testResultTXT(1,vtx2d,col2d_rgb,col2d);

    // FsOpenWindow(0,0,800,500,1);
    // while(term!=true){
    //     Draw(vtx2d, col2d);        
    // }

    return 0;
}