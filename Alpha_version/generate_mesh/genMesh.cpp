#include <exception>
#include <filesystem>
#include <gmsh.h>
#include <iostream>
#include <math.h>
#include <set>
#include <string>

void generate_mesh(double radius, int numSamples){

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

int main(int argc, char **argv) {

    double radius = atof(argv[1]);
    int numSamples = 1;
    generate_mesh(radius, numSamples);
    return 0;
}

// int main(int argc, char **argv) {

    // if (argc <= 2) {
    //     std::cout << "Usage: ./genMesh.exe <radius> <num_samples>" << std::endl;
    //     exit(0);
    // }

    // double radius = 0.0;
    // try {
    //     radius = std::stod(argv[1]);
    // } catch (std::exception &e) {
    //     std::cout << "Invalid radius arg: " << e.what() << std::endl;
    //     exit(1);
    // }
    // if (radius < 25.0 || radius > 35.0) {
    //     std::cout << "radius must be between 25.0 and 35.0" << std::endl;
    //     exit(1);
    // }

    // int numSamples = 20; // For numerical stability purpose, max out at 20
    // try {
    //     numSamples = std::stoi(argv[2]);
    // } catch (std::exception &e) {
    //     std::cout << "Invalid num_samples arg: " << e.what() << std::endl;
    //     exit(1);
    // }
    // if (numSamples < 1 || numSamples > 20) {
    //     std::cout << "Number of samples must be between 1 and 20" << std::endl;
    //     exit(1);
    // }

    // gmsh::initialize();

    // double lc = 1; // 1e-2; // Target mesh size

    // double bboxHeightLimit = 50.0;
    // double bboxWidthLimit = 80.0;
    // double lowerOutletLimit = 12.5;
    // double wallThickness = 2.0; // can be changed
    // double upperOutletLimit = lowerOutletLimit + wallThickness;

    // double ctrlOneX = 20.0; // Three equal segments
    // double ctrlThreeX = 40.0;
    // double ctrlThreeY = (radius + lowerOutletLimit) / 2; // can be changed
    // double ctrlFiveX = 60.0;

    // double ctrlInterval = (ctrlThreeX - ctrlOneX) / (double)(numSamples + 1);

    // for (int i = 0; i < numSamples; i++) {
    //     gmsh::initialize();

    //     double safeOffset = 0.0;

    //     double ctrlTwoX = ctrlOneX + (i + 1) * ctrlInterval + safeOffset;
    //     double ctrlFourX = ctrlFiveX - (i + 1) * ctrlInterval - safeOffset;

    //     double theta = atan2(radius - lowerOutletLimit, ctrlFourX - ctrlTwoX);

    //     double ctrlThreeXUpper = ctrlThreeX + wallThickness * sin(theta);
    //     double ctrlThreeYUpper = ctrlThreeY + wallThickness * cos(theta);

    //     // Define fixed point coordinates
    //     gmsh::model::geo::addPoint(bboxWidthLimit, bboxHeightLimit, 0, lc, 1); // Upper right corner
    //     gmsh::model::geo::addPoint(0, bboxHeightLimit, 0, lc, 2);              // Upper left corner
    //     gmsh::model::geo::addPoint(0, 0, 0, lc, 3);                            // Lower left corner
    //     gmsh::model::geo::addPoint(bboxWidthLimit, 0, 0, lc, 4);               // Lower right corner
    //     gmsh::model::geo::addPoint(bboxWidthLimit, lowerOutletLimit, 0, lc, 5); // Lower outlet
    //     gmsh::model::geo::addPoint(bboxWidthLimit, upperOutletLimit, 0, lc, 6); // Upper outlet

    //     // Bezier control points, start from left
    //     gmsh::model::geo::addPoint(ctrlOneX, radius, 0, lc, 7);             // Lower crtl 1
    //     gmsh::model::geo::addPoint(ctrlTwoX, radius, 0, lc, 8);             // Lower crtl 2
    //     gmsh::model::geo::addPoint(ctrlThreeX, ctrlThreeY, 0, lc, 9);       // Lower crtl 3
    //     gmsh::model::geo::addPoint(ctrlFourX, lowerOutletLimit, 0, lc, 10); // Lower crtl 4
    //     gmsh::model::geo::addPoint(ctrlFiveX, lowerOutletLimit, 0, lc, 11); // Lower crtl 5

    //     gmsh::model::geo::addPoint(ctrlOneX, radius + wallThickness, 0, lc, 12); // Upper crtl 1
    //     gmsh::model::geo::addPoint(ctrlTwoX, radius + wallThickness, 0, lc, 13); // Upper crtl 2
    //     gmsh::model::geo::addPoint(ctrlThreeXUpper, ctrlThreeYUpper, 0, lc, 14); // Upper crtl 3
    //     gmsh::model::geo::addPoint(ctrlFourX, upperOutletLimit, 0, lc, 15);      // Upper crtl 4
    //     gmsh::model::geo::addPoint(ctrlFiveX, upperOutletLimit, 0, lc, 16);      // Upper crtl 5

    //     gmsh::model::geo::addPoint(ctrlOneX, radius + wallThickness / 2.0, 0, lc,
    //                                17); // tip circular center

    //     // Define boundary lines
    //     gmsh::model::geo::addLine(1, 2, 1);
    //     gmsh::model::geo::addLine(2, 3, 2);
    //     gmsh::model::geo::addLine(3, 4, 3);
    //     gmsh::model::geo::addLine(4, 5, 4);
    //     gmsh::model::geo::addLine(6, 1, 5);

    //     // Define nozzle wall using Bezier curves and lines
    //     gmsh::model::geo::addBezier({7, 8, 9}, 6);
    //     gmsh::model::geo::addBezier({9, 10, 11}, 7);
    //     gmsh::model::geo::addBezier({12, 13, 14}, 8);
    //     gmsh::model::geo::addBezier({14, 15, 16}, 9);
    //     gmsh::model::geo::addCircleArc(12, 17, 7, 10);
    //     gmsh::model::geo::addLine(11, 5, 11);
    //     gmsh::model::geo::addLine(16, 6, 12);

    //     // Define surfaces
    //     gmsh::model::geo::addCurveLoop({1, 2, 3, 4, -11, -7, -6, -10, 8, 9, 12, 5}, 1);
    //     gmsh::model::geo::addPlaneSurface({1}, 1);

    //     // Synchronize model before mesh
    //     gmsh::model::geo::synchronize();

    //     // Define physical groups
    //     int fluid_tag = gmsh::model::addPhysicalGroup(2, {1}, 11);
    //     gmsh::model::setPhysicalName(2, fluid_tag, "fluid");

    //     int inlet_tag = gmsh::model::addPhysicalGroup(1, {4}, 12);
    //     gmsh::model::setPhysicalName(1, inlet_tag, "inlet");

    //     int outlet_tag = gmsh::model::addPhysicalGroup(1, {1, 2, 5}, 13);
    //     gmsh::model::setPhysicalName(1, outlet_tag, "outlet");

    //     int wall_tag = gmsh::model::addPhysicalGroup(1, {6, 7, 8, 9, 10, 11, 12}, 14);
    //     gmsh::model::setPhysicalName(1, wall_tag, "wall");

    //     int axis_tag = gmsh::model::addPhysicalGroup(1, {3}, 15);
    //     gmsh::model::setPhysicalName(1, axis_tag, "axis");

    //     // Define Boundary Layer
    //     gmsh::model::mesh::field::add("BoundaryLayer", 1);
    //     gmsh::model::mesh::field::setNumber(1, "hfar", 0.05);
    //     gmsh::model::mesh::field::setNumber(1, "hwall_n", 0.05);
    //     gmsh::model::mesh::field::setNumber(1, "thickness", 3);
    //     gmsh::model::mesh::field::setNumber(1, "ratio", 1.1);
    //     gmsh::model::mesh::field::setNumber(1, "AnisoMax", 10);
    //     gmsh::model::mesh::field::setNumber(1, "Quads", 1);

    //     gmsh::model::mesh::field::setNumbers(1, "CurvesList", {6, 7, 8, 9, 10, 11, 12});
    //     gmsh::model::mesh::field::setNumbers(1, "PointsList", {5, 6, 7, 12});

    //     gmsh::model::mesh::field::setAsBoundaryLayer(1);

    //     // Set mesh option and generate mesh
    //     gmsh::option::setNumber("Mesh.MeshSizeFactor", 1);
    //     gmsh::model::mesh::generate(2);

    //     // Save generated mesh to disk as Nastran Bulk Data file
    //     gmsh::option::setNumber("Mesh.SaveElementTagType", 2);
    //     std::string dirName("output/");
    //     std::filesystem::create_directory(dirName); // create output directory for generated mesh
    //     std::string fileName("m_" + std::to_string(radius) + "_" + std::to_string(i) + ".bdf");
    //     gmsh::write(dirName + fileName);

    //     gmsh::finalize();
    // }

    // std::cout << "All mesh successfully generated!" << std::endl;

    // This should be called when you are done using the Gmsh C++ API:
    // gmsh::finalize();

//     return 0;
// }