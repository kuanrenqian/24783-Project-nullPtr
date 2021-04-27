#include <iostream>
#include "meshclickdraw.h"
#include "drawMesh.h"
#include "genSim.h"
using namespace std;

int main(){
    int batch; // # of simulation to run
    string fileName;

    // FsSimpleWindows size
    int wid = 900;
    int hei = 800;

    // // termination state
    bool termination = false;

    // print info and takes input (to be integrated with click-draw)
    cout << "Remember to copy mesh.exe and gmsh.dll to genMesh.exe directory!" << endl;
    cout << "Make sure build/output is empty! Fluent does not overwrite by default!" << endl;
    cout << "Input batch size: [1~20, int]" << endl;
    cin >> batch;

    batch = (int)batch;

    // // open window
    FsOpenWindow(0,0,wid,hei,1);

    // initialize variables
    vector<float> control_pts; // define curvature of the vaccum tube
    vector<string> AllPathToFile; // vector of meshFile names (.bdf) for displaying
    string pathToFile; // single meshFile name (.bdf)
    AllPathToFile.clear();
    control_pts.clear();
    // generate and display all mesh
    for (int i=0; i<batch; ++i){
        
        // meshclickdraw - this component handles its own while loop (at the moment)
        control_pts = runClickDraw();

        // genenerate mesh
        generate_mesh_exe_ctrlPoints(control_pts[0],control_pts[1],control_pts[2],control_pts[3],control_pts[4],control_pts[5],control_pts[6],control_pts[7],control_pts[8],control_pts[9], i);

        // draw mesh
        fileName = "m_"+to_string(i)+".bdf";
        pathToFile = "./output/"+fileName;
        AllPathToFile.push_back(pathToFile);
        // display info in commandline
        cout << "****************************************************************************" << endl;
        cout << "Displaying mesh #" << i << endl;
        cout << "Press SPACE to continue... (if nothing happens, click window and press SPACE" << endl;
        // Drawing mesh
        while(termination!=true){
            FsPollDevice();
            auto key=FsInkey();
            if(FSKEY_SPACE==key){
                termination=true;
            }
            Draw_All_mesh(AllPathToFile);
        }
        termination = false;

        // generate simulation
        generate_simulation_files(fileName,i);
    }

    // run simulation
    for (int i=0; i<batch; ++i){
        run_simulation_files(i);
    }

    // read simulation results from testResults.txt into vtx and col
    // GUI visulization
    termination = false;
    while(termination!=true){
        FsPollDevice();
        auto key=FsInkey();
        if(FSKEY_SPACE==key){
            termination=true;
        }
        Draw_All_Simulation_Results(batch);        
    }
    return 0;
}