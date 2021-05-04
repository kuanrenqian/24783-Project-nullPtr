#include <iostream>
#include "meshclickdraw.h"
#include "drawMesh.h"
#include "genSim.h"
#include <chrono>
using namespace std;
#include <future>

// FsSimpleWindows size
const int wid = 900;
const int hei = 800;

void commandline_instruction(int &batch, int &nprocs){
    cout << "#################################################################" << endl;
    cout << "#   24-783 Project - Automated Training Data Generation for     #" << endl;
    cout << "#   Geometrical Machine Learning                                #" << endl;
    cout << "#   Team: nullPtr                                               #" << endl;
    cout << "#   Team member: Allen Jiang, Bryant Porras, Kuanren Qian,      #" << endl;
    cout << "#                Shu You, Vimalesh Vasu                         #" << endl;
    cout << "#   Date: 04/27/2021                                            #" << endl;
    cout << "#################################################################" << endl;
    cout << endl;
    cout << "Description: " << endl;
    cout << "This program is a automated pipeline for generating" << endl;
    cout << "vacuum tube geometries, meshing, simulation, and visulization." << endl;
    cout << endl;
    cout << "Requirements:" << endl;
    cout << "1. Valid installation of Fluent with correct environmental path" << endl;
    cout << "    (run C:/Program Files/ANSYS Inc/v202/fluent/ntbin/win64/setenv.exe)" << endl;
    cout << "2. Access to YS public library" << endl;
    cout << "    (Change public folder path in CMakefiles as needed)" << endl;
    cout << endl;
    cout << "Usage:" << endl;
    cout << "1. ./release/main.exe" << endl;
    cout << "2. enter batch size in commandline window" << endl;
    cout << "3. Follow on screen instruction and draw tube shape" << endl;
    cout << "4. Check mesh" << endl;
    cout << "5. Repeat step 3 and 4 until batch size is reached" << endl;
    cout << "6. Program will automatically call fluent to run all simulations" << endl;
    cout << "7. GUI visualization of all simulation  results" << endl;
    cout << endl;
    cout << "Batch size can be arbitrary int values, but only first 12 will be displayed" << endl;
    cout << "Input batch size (int): " << endl;
    cin >> batch;
    cout << "# of processors to use (int): " << endl;
    cin >> nprocs;

    batch = (int)batch;
    nprocs = (int)nprocs;
}

int main(){
    int batch; // # of simulation to run
    int nprocs; // # of processors to use
    string fileName;

    // termination state
    bool termination = false;

    // print info and takes input (to be integrated with click-draw)
    commandline_instruction(batch,nprocs);

    // open window
    FsOpenWindow(0,0,wid,hei,1);

    // initialize variables
    vector<float> control_pts; // define curvature of the vaccum tube
    vector<string> AllPathToFile; // vector of meshFile names (.bdf) for displaying
    string pathToFile; // single meshFile name (.bdf)
    AllPathToFile.clear();
    control_pts.clear();

    check_output_folder(); // this clears output folder

    int redo = 0;
    // generate and display all mesh
    for (int i=0; i<batch; ++i){
        
        // meshclickdraw - this component handles its own while loop (at the moment)
        control_pts = runClickDraw(control_pts);

        // genenerate mesh
        generate_mesh_exe_ctrlPoints(control_pts[0],control_pts[1],control_pts[2],control_pts[3],control_pts[4],control_pts[5],control_pts[6],control_pts[7],control_pts[8],control_pts[9], i);

        // draw mesh
        fileName = "m_"+to_string(i)+".bdf";
        pathToFile = "./output/"+fileName;
        if (std::find(AllPathToFile.begin(), AllPathToFile.end(), pathToFile) == AllPathToFile.end()) {
            AllPathToFile.push_back(pathToFile);
        }
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
            if (Draw_All_mesh(AllPathToFile) != 0) {
                // Mesh failed, require user redraw
                redo = 1;
                break;
            }
        }
        if (redo) {
            i--;
            redo = 0;
            continue;
        }
        termination = false;

        // generate simulation
        generate_simulation_files(fileName,i);
    }

    auto start = std::chrono::high_resolution_clock::now();
    // run simulation (requires licensed installation of Fluent software with correct environmental path)
    for (int i=0; i<batch; ++i){
        int highlightCounter = 0;
        int radius = 5;
        int margin = 20;
        auto future = std::async(std::launch::async, run_simulation_files, i, nprocs);
        while (future.wait_for(100ms) != std::future_status::ready) {
            // Simulation still running
            FsPollDevice();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glRasterPos2d(230, hei / 2 - 50);
            glColor3ub(0, 0, 0);
            std::string status = "Simulation " + std::to_string(i+1) + " of " + std::to_string(batch) 
                                    + " Running";
            YsGlDrawFontBitmap16x20(status.c_str());

            for (int j = 0; j < 4; j++) {
                int offset = margin * (2 - j);
                DrawCircle(wid/2 - offset, hei/2, radius, j == highlightCounter);
            }
            
            FsSwapBuffers();
            FsSleep(25);
            highlightCounter = (highlightCounter + 1) % 4;
        }
        std::cout << "Finished simulation # " << i << std::endl;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = stop-start;
    cout << "Simulation duration: " << diff.count() << "seconds for " << batch << " simulations." << endl;

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