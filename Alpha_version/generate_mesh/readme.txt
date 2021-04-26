OS: Windows 10
C++ compiler: Microsoft (R) C/C++ Optimizing Compiler Version 19.28.29913 for x64

Directory structure:
    include/gmsh.h: C++ wrapper of the C API
    include/gmshc.h: C API header
    lib/gmsh.lib: import library (Windows only)
    gmsh-4.8.dll: Dynamic link library
    genMesh.cpp: Main source code
    genMesh.exe: Executable binary

To compile, open x64 Native Tools Command Prompt for VS 2019 and change to the root directory. Then do:
    cl.exe /Iinclude genMesh.cpp .\lib\gmsh.lib
Or use cmake, in x64 Native Tools Command Prompt for VS 2019
    cd build
    cmake ..
    cmake --build . --config Release --target genMesh
IMPORTANT: make sure to use --config Release

Once compiled, run the executable:
    .\genMesh.exe <radius> <num_samples>
Make sure gmsh-4.8.dll is in the same directory as mesh.exe. <radius> is the radius of the nozzle,
the range should be between 25.0 and 35.0. <num_samples> is the number of generated mesh files, the
range should be between 1 and 20 for numerical stability purpose.

The generated mesh Nastran Bulk Datafile will be saved in a new directory:
    .\output\
The naming convention is as follows:
    m_radius_sampleNumber
For example, with radius 30 and 3 samples, the generated mesh files will be:
    .\output\m_30.000000_0.bdf
    .\output\m_30.000000_1.bdf
    .\output\m_30.000000_2.bdf
