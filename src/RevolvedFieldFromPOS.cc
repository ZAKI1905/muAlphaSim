// RevolvedFieldFromPOS.cc
// Parses axisymmetric .pos field and projects into Geant4 3D cartesian field

#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>

#include "RevolvedFieldFromPOS.hh"

// -----------------------------------------------------------------------------
/**
 * @brief Constructor that loads the .pos file containing VT entries.
 * @param filename Path to the Gmsh .pos file
 */
// -----------------------------------------------------------------------------
RevolvedFieldFromPOS::RevolvedFieldFromPOS(const std::string& filename) 
{
    LoadPOS(filename);
}

// -----------------------------------------------------------------------------
/**
 * @brief Load and parse the .pos file containing Gmsh VT (vector triangle) entries.
 *
 * Each line of the .pos file contains a triangle in the form:
 * VT(x1,y1,z1, x2,y2,z2, x3,y3,z3){Ex1,Ey1,Ez1, Ex2,Ey2,Ez2, Ex3,Ey3,Ez3};
 * We extract the 3 vertices and their electric field values, convert them to
 * cylindrical coordinates (r, z), and store them for nearest-neighbor lookup.
 *
 * @param filename Path to the .pos file
 */
// -----------------------------------------------------------------------------
void RevolvedFieldFromPOS::LoadPOS(const std::string& filename) 
{
    std::ifstream in(filename);
    std::string line;

    while (std::getline(in, line)) {
        if (line.find("View") != std::string::npos) continue;
        if (line.find("VT(") == std::string::npos) continue;

        // Extract coordinate and field substrings
        size_t p1 = line.find("(");
        size_t p2 = line.find(")", p1);
        std::string coordsStr = line.substr(p1 + 1, p2 - p1 - 1);

        size_t b1 = line.find("{");
        size_t b2 = line.find("}", b1);
        std::string fieldStr = line.substr(b1 + 1, b2 - b1 - 1);

        std::istringstream coords(coordsStr);
        std::istringstream fields(fieldStr);

        double x[3], y[3], z[3], Ex[3], Ey[3], Ez[3];
        for (int i = 0; i < 3; ++i)
            coords >> x[i] >> y[i] >> z[i];
        for (int i = 0; i < 3; ++i)
            fields >> Ex[i] >> Ey[i] >> Ez[i];

        for (int i = 0; i < 3; ++i) {
            double r = std::sqrt(x[i] * x[i] + y[i] * y[i]);
            fieldPoints.push_back({r, z[i], G4ThreeVector(Ex[i], 0.0, Ez[i])});
        }
    }

    std::cout << "Loaded " << fieldPoints.size() << " field points from POS." << std::endl;
}

// -----------------------------------------------------------------------------
/**
 * @brief Get the interpolated electric field at a 3D position using nearest neighbor.
 *
 * Converts (x, y, z) to cylindrical (r, z), finds the closest field point in the
 * preloaded axisymmetric dataset, and rotates the (Er, Ez) into Cartesian coordinates.
 *
 * @param pos The 3D position in Cartesian coordinates (x, y, z)
 * @return Electric field vector (Ex, Ey, Ez) at the requested point
 */
// -----------------------------------------------------------------------------
G4ThreeVector RevolvedFieldFromPOS::GetField(const G4ThreeVector& pos) const 
{
    double x = pos.x(), y = pos.y(), z = pos.z();
    double r = std::sqrt(x * x + y * y);

    double minDist2 = 1e9;
    G4ThreeVector E_nearest(0, 0, 0);

    for (const auto& pt : fieldPoints) {
        double dr = pt.r - r;
        double dz = pt.z - z;
        double dist2 = dr * dr + dz * dz;
        if (dist2 < minDist2) {
            minDist2 = dist2;
            E_nearest = pt.E;
        }
    }

    // Project radial field into x-y plane
    if (r < 1e-9) return G4ThreeVector(0, 0, E_nearest.z());
    double Ex = E_nearest.x() * x / r;
    double Ey = E_nearest.x() * y / r;
    double Ez = E_nearest.z();
    return G4ThreeVector(Ex, Ey, Ez);
}
// -----------------------------------------------------------------------------