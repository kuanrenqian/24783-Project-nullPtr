#include <chrono>
#include <exception>
#include <filesystem>
#include <gmsh.h>
#include <iostream>
#include <math.h>
#include <set>
#include <string>
#include <vector>

class CtrlPoint {
public:
    CtrlPoint(double x, double y) : x(x), y(y) {}
    double x, y;
};

/**
 * @brief Get current time in milliseconds since epoch.
 *
 * @return long long current time in milliseconds.
 */
long long currentTimeMillis() {
    auto time = std::chrono::system_clock::now();
    auto since_epoch = time.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch).count();
}

/**
 * @brief Process the geometry and generate a 2D mesh.
 *
 */
void process_geo() {
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
    gmsh::model::mesh::field::setNumber(1, "hwall_n", 0.1);
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
}

/**
 * @brief Generate mesh file based on Bezier control points.
 *
 * @param ctrlPoints Array of 5 Bezier control points.
 */
void generate_mesh(const std::vector<CtrlPoint> &ctrlPoints, int numSamples) {
    if (ctrlPoints.size() != 5) {
        std::cout << "Number of control points should be 5." << std::endl;
        return;
    }
    CtrlPoint ctrlOne = ctrlPoints.at(0);
    CtrlPoint ctrlTwo = ctrlPoints.at(1);
    CtrlPoint ctrlThree = ctrlPoints.at(2);
    CtrlPoint ctrlFour = ctrlPoints.at(3);
    CtrlPoint ctrlFive = ctrlPoints.at(4);

    gmsh::initialize();

    double outer_mesh_size = 2.0; // Mesh size factor
    double inner_mesh_size = 0.5;

    double bboxHeightLimit = 50.0;
    double bboxWidthLimit = 80.0;
    double lowerOutletLimit = 12.5;
    double wallThickness = 2.0; // can be changed
    double upperOutletLimit = lowerOutletLimit + wallThickness;

    double theta = atan2(ctrlOne.y - lowerOutletLimit, ctrlFour.x - ctrlTwo.x);

    double ctrlThreeXUpper = ctrlThree.x + wallThickness * sin(theta);
    double ctrlThreeYUpper = ctrlThree.y + wallThickness * cos(theta);

    // Define fixed point coordinates
    gmsh::model::geo::addPoint(bboxWidthLimit, bboxHeightLimit, 0, outer_mesh_size, 1);  // Upper right corner
    gmsh::model::geo::addPoint(0, bboxHeightLimit, 0, outer_mesh_size, 2);               // Upper left corner
    gmsh::model::geo::addPoint(0, 0, 0, outer_mesh_size, 3);                             // Lower left corner
    gmsh::model::geo::addPoint(bboxWidthLimit, 0, 0, outer_mesh_size, 4);                // Lower right corner
    gmsh::model::geo::addPoint(bboxWidthLimit, lowerOutletLimit, 0, outer_mesh_size, 5); // Lower outlet
    gmsh::model::geo::addPoint(bboxWidthLimit, upperOutletLimit, 0, outer_mesh_size, 6); // Upper outlet

    // Bezier control points, start from left
    gmsh::model::geo::addPoint(ctrlOne.x, ctrlOne.y, 0, inner_mesh_size, 7);     // Lower crtl 1
    gmsh::model::geo::addPoint(ctrlTwo.x, ctrlTwo.y, 0, inner_mesh_size, 8);     // Lower crtl 2
    gmsh::model::geo::addPoint(ctrlThree.x, ctrlThree.y, 0, inner_mesh_size, 9); // Lower crtl 3
    gmsh::model::geo::addPoint(ctrlFour.x, ctrlFour.y, 0, inner_mesh_size, 10);  // Lower crtl 4
    gmsh::model::geo::addPoint(ctrlFive.x, ctrlFive.y, 0, inner_mesh_size, 11);  // Lower crtl 5

    gmsh::model::geo::addPoint(ctrlOne.x, ctrlOne.y + wallThickness, 0, inner_mesh_size, 12);   // Upper crtl 1
    gmsh::model::geo::addPoint(ctrlTwo.x, ctrlTwo.y + wallThickness, 0, inner_mesh_size, 13);   // Upper crtl 2
    gmsh::model::geo::addPoint(ctrlThreeXUpper, ctrlThreeYUpper, 0, inner_mesh_size, 14);       // Upper crtl 3
    gmsh::model::geo::addPoint(ctrlFour.x, ctrlFour.y + wallThickness, 0, inner_mesh_size, 15); // Upper crtl 4
    gmsh::model::geo::addPoint(ctrlFive.x, ctrlFive.y + wallThickness, 0, inner_mesh_size, 16); // Upper crtl 5

    gmsh::model::geo::addPoint(ctrlOne.x, ctrlOne.y + wallThickness / 2.0, 0, 1.0,
                               17); // tip circular center

    process_geo();

    // Save generated mesh to disk as Nastran Bulk Data file
    gmsh::option::setNumber("Mesh.SaveElementTagType", 2);
    std::string dirName("output/");
    std::filesystem::create_directory(dirName); // create output directory for generated mesh
    std::string fileName("m_" + std::to_string(numSamples) + ".bdf");
    // std::string fileName("m_bezier.bdf");
    gmsh::write(dirName + fileName);

    gmsh::finalize();
    std::cout << "Mesh successfully generated!" << std::endl;
}

/**
 * @brief Automatically generate mesh files based on nozzle radius and target sample numbers.
 *
 * @param radius Radius of the nozzle inlet.
 * @param numSamples Number of samples to generate.
 */
void generate_mesh(double radius, int numSamples) {

    double outer_mesh_size = 2.0; // Mesh size factor
    double inner_mesh_size = 0.5;

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
        gmsh::model::geo::addPoint(bboxWidthLimit, bboxHeightLimit, 0, outer_mesh_size, 1); // Upper right corner
        gmsh::model::geo::addPoint(0, bboxHeightLimit, 0, outer_mesh_size, 2);              // Upper left corner
        gmsh::model::geo::addPoint(0, 0, 0, outer_mesh_size, 3);                            // Lower left corner
        gmsh::model::geo::addPoint(bboxWidthLimit, 0, 0, outer_mesh_size, 4);               // Lower right corner
        gmsh::model::geo::addPoint(bboxWidthLimit, lowerOutletLimit, 0, outer_mesh_size, 5); // Lower outlet
        gmsh::model::geo::addPoint(bboxWidthLimit, upperOutletLimit, 0, outer_mesh_size, 6); // Upper outlet

        // Bezier control points, start from left
        gmsh::model::geo::addPoint(ctrlOneX, radius, 0, inner_mesh_size, 7);             // Lower crtl 1
        gmsh::model::geo::addPoint(ctrlTwoX, radius, 0, inner_mesh_size, 8);             // Lower crtl 2
        gmsh::model::geo::addPoint(ctrlThreeX, ctrlThreeY, 0, inner_mesh_size, 9);       // Lower crtl 3
        gmsh::model::geo::addPoint(ctrlFourX, lowerOutletLimit, 0, inner_mesh_size, 10); // Lower crtl 4
        gmsh::model::geo::addPoint(ctrlFiveX, lowerOutletLimit, 0, inner_mesh_size, 11); // Lower crtl 5

        gmsh::model::geo::addPoint(ctrlOneX, radius + wallThickness, 0, inner_mesh_size, 12); // Upper crtl 1
        gmsh::model::geo::addPoint(ctrlTwoX, radius + wallThickness, 0, inner_mesh_size, 13); // Upper crtl 2
        gmsh::model::geo::addPoint(ctrlThreeXUpper, ctrlThreeYUpper, 0, inner_mesh_size, 14); // Upper crtl 3
        gmsh::model::geo::addPoint(ctrlFourX, upperOutletLimit, 0, inner_mesh_size, 15);      // Upper crtl 4
        gmsh::model::geo::addPoint(ctrlFiveX, upperOutletLimit, 0, inner_mesh_size, 16);      // Upper crtl 5

        gmsh::model::geo::addPoint(ctrlOneX, radius + wallThickness / 2.0, 0, 1.0,
                                   17); // tip circular center

        process_geo(); // Form shape and generate 2D mesh

        // Save generated mesh to disk as Nastran Bulk Data file
        gmsh::option::setNumber("Mesh.SaveElementTagType", 2);
        std::string dirName("output/");
        std::filesystem::create_directory(dirName); // create output directory for generated mesh
        // std::string fileName("m_" + std::to_string(radius) + "_" + std::to_string(i) + ".bdf");
        std::string fileName("m_" + std::to_string(radius) + ".bdf");
        gmsh::write(dirName + fileName);

        gmsh::finalize();
    }

    std::cout << "All mesh successfully generated!" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 2 && argc != 12) {
        std::cout
            << "Usage:\n\tSingle radius input: .\\genMesh.exe <radius>\n\tControl points input: "
               ".\\genMesh.exe <c1x> <c1y> ... <c5x> <c5y>"
            << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        double radius = atof(argv[1]);
        generate_mesh(radius, 1);
    } else { // Bezier control input
        CtrlPoint p1(atof(argv[1]), atof(argv[2]));
        CtrlPoint p2(atof(argv[3]), atof(argv[4]));
        CtrlPoint p3(atof(argv[5]), atof(argv[6]));
        CtrlPoint p4(atof(argv[7]), atof(argv[8]));
        CtrlPoint p5(atof(argv[9]), atof(argv[10]));
        int numSamples = atoi(argv[11]);
        generate_mesh({p1, p2, p3, p4, p5}, numSamples);
    }
    return 0;
}
