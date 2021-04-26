#include "genSim.h"

void generate_mesh_exe_radius(float radius){
    //This is a version of generate_mesh function, but using genMesh.exe instead of 
    //directly using gmsh. This solves the issue with commandline problem
    printf ("Checking if system is available...");
    if (system(NULL)) puts ("Ok");
    else exit (EXIT_FAILURE);
    printf ("Executing genMesh in batch mode...\n");
    string command = ".\\generate_mesh\\Release\\genMesh.exe " + to_string(radius);
    const char *command_char = command.c_str();
    system (command_char);
    
}

void generate_mesh_exe_ctrlPoints(float c1x, float c1y, float c2x, float c2y, float c3x, float c3y, float c4x, float c4y, float c5x, float c5y, int numSimu){
    //This is a version of generate_mesh function, but using genMesh.exe instead of 
    //directly using gmsh. This solves the issue with commandline problem
    printf ("Checking if system is available...");
    if (system(NULL)) puts ("Ok");
    else exit (EXIT_FAILURE);
    printf ("Executing genMesh in batch mode...\n");
    string command = ".\\generate_mesh\\Release\\genMesh.exe " + to_string(c1x) + " " + to_string(c1y) + " " + to_string(c2x) + " " + to_string(c2y) + " "
     + to_string(c3x) + " " + to_string(c3y) + " " + to_string(c4x) + " " + to_string(c4y) + " " + to_string(c5x) + " " + to_string(c5y) + " " + to_string(numSimu);
    cout << command << endl;

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

void parula_colormap(float input, float max_input, float &R, float &G, float &B){
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

float read_and_plot_testResultTXT(int numSimu, vector <float> &vtx2d, vector <float> &col2d, int Xoffset, int Yoffset){
    // this function is float only for ctest purpose
    vector <float> temp_rgb;
    vtx2d.clear();
    col2d.clear();
    
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
    float xmin,xmax,ymin,ymax;
    // read data while end of file is not reached
    while(myfile.eof()!=true){
        myfile >> nodeID >> xx >> yy >> pp >> vv;
        x = stof(xx)*10;
        y = stof(yy)*10;
        p = stof(pp);
        v = stof(vv);
        
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

        vtx2d.push_back(x);  vtx2d.push_back(y);
        temp_rgb.push_back(p);  temp_rgb.push_back(p); temp_rgb.push_back(p);
    }
    for(int i=0; i<vtx2d.size()-1; i+=2){
        vtx2d[i] = (vtx2d[i]-xmin)/xmax*300+Xoffset; 
        vtx2d[i+1] = (vtx2d[i+1]-ymin)/ymax*187.5+Yoffset;
    }
    
    float max_val = *max_element(temp_rgb.begin(), temp_rgb.end());

    float r,g,b,val;
    for(int i=0; i<temp_rgb.size()-3; i+=3){
        val = temp_rgb[i];
        parula_colormap(val,max_val,r,g,b);
        col2d.push_back(r);  col2d.push_back(g); col2d.push_back(b); col2d.push_back(1);
    }

    temp_rgb.clear();
    
    return xmax;
}

void Draw_Simulation_Result(int numSimu, int x_off, int y_off){
    // This function draws simulation results read by read_and_plot_testResultTXT()
    // draws GL_POINTS
    std::vector <float> vtx2d;
	std::vector <float> col2d;

    read_and_plot_testResultTXT(numSimu, vtx2d, col2d, x_off, y_off);

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
}

void Draw_All_Simulation_Results(int batch){
    int row, column;
    int x_off, y_off;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(int i=0; i<batch; ++i){
        row = i/4;
        column = i%4;
        y_off = row*187.5;
        x_off = column*300;
        Draw_Simulation_Result(i, x_off, y_off);
    }
    // <<< draw here
    FsSwapBuffers();
    FsSleep(25);
}

// int compareFiles(FILE *fp1, FILE *fp2)
// {

//     char ch1 = getc(fp1);
//     char ch2 = getc(fp2);

//     int error = 0, pos = 0, line = 1;
  
//     while (ch1 != EOF && ch2 != EOF)
//     {
//         pos++;

//         if (ch1 == '\n' && ch2 == '\n')        {
//             line++;
//             pos = 0;
//         }
  

//         if (ch1 != ch2)        {
//             cout << "!!" << endl;
//             cout << ch1 << " " << ch2 << endl;
//             return 1;
//         }
  
//         ch1 = getc(fp1);
//         ch2 = getc(fp2);
//     }
//     return 0;
// }

// int GUI_test1(){
//     std::vector <float> vtx2d;
// 	std::vector <float> col2d;
//     float xmax = read_and_plot_testResultTXT(0, vtx2d, col2d, 0, 0);
//     cout << vtx2d.size() << endl;
//     return vtx2d.size();
// }

// vector <float> GUI_test2(){
//     std::vector <float> vtx2d;
// 	std::vector <float> col2d;
//     float xmax = read_and_plot_testResultTXT(0, vtx2d, col2d, 0, 0);
//     return col2d;
// }

// float GUI_test3(){
//     std::vector <float> vtx2d;
// 	std::vector <float> col2d;
//     return read_and_plot_testResultTXT(0, vtx2d, col2d, 0, 0);
// }