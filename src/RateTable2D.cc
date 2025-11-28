/**
 * @file      RateTable2D.cc
 * @brief     Implementation of RateTable2D for unstructured (rho, z, rate) data.
 *
 * This file contains the definitions for loading, storing, and interpolating
 * unstructured 2D data in cylindrical (rho, z) coordinates.
 */

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>

#include "RateTable2D.hh"

// -----------------------------------------------------------------------------
// Constructor: Load file and store raw points
// -----------------------------------------------------------------------------
RateTable2D::RateTable2D(const std::string& filename, char delim)
{
    std::ifstream fin(filename);
    if (!fin)
        throw std::runtime_error("RateTable2D: cannot open file " + filename);

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;  // Skip comments
        std::replace(line.begin(), line.end(), delim, ' ');
        std::istringstream iss(line);
        double r, z, w;
        if (iss >> r >> z >> w) {
            mPoints.push_back({r, z, w});
        }
    }

    if (mPoints.empty())
        throw std::runtime_error("RateTable2D: no data loaded from file.");
}

// -----------------------------------------------------------------------------
// Inside: returns true if (rho, z) is within the bounding box of the dataset
// -----------------------------------------------------------------------------
bool RateTable2D::Inside(double rho, double z) const
{
    if (mPoints.empty()) return false;

    double minRho = std::numeric_limits<double>::max();
    double maxRho = std::numeric_limits<double>::lowest();
    double minZ = std::numeric_limits<double>::max();
    double maxZ = std::numeric_limits<double>::lowest();

    for (const auto& pt : mPoints) {
        minRho = std::min(minRho, pt.rho);
        maxRho = std::max(maxRho, pt.rho);
        minZ = std::min(minZ, pt.z);
        maxZ = std::max(maxZ, pt.z);
    }

    // Debug output for bounding box
    // std::cout << "[DEBUG] Bounding box: "
    //        << "Rho [" << minRho << ", " << maxRho << "] m, "
    //        << "Z [" << minZ << ", " << maxZ << "] m" 
    //        << "Given Point (rho, z): (" << rho << 
    //        ", " << z  << ")" << std::endl;

    // Check if (rho, z) is within the bounding box
    return (rho >= minRho && rho <= maxRho && z >= minZ && z <= maxZ);
}

// -----------------------------------------------------------------------------
// GetBoundingBox: computes the bounding box of the data points
// -----------------------------------------------------------------------------
void RateTable2D::GetBoundingBox(double& minRho, double& maxRho,
                                 double& minZ, double& maxZ) const
{
    minRho = std::numeric_limits<double>::max();
    maxRho = std::numeric_limits<double>::lowest();
    minZ   = std::numeric_limits<double>::max();
    maxZ   = std::numeric_limits<double>::lowest();

    for (const auto& pt : mPoints) {
        minRho = std::min(minRho, pt.rho);
        maxRho = std::max(maxRho, pt.rho);
        minZ   = std::min(minZ, pt.z);
        maxZ   = std::max(maxZ, pt.z);
    }
}

// -----------------------------------------------------------------------------
// Interp: returns bilinear interpolation if possible, else nearest neighbor
// -----------------------------------------------------------------------------
double RateTable2D::Interp(double rho, double z) const
{
    std::vector<RatePoint> neighbors;
    if (FindBilinearNeighbors(rho, z, neighbors)) {
        return BilinearInterp(neighbors, rho, z);
    }

    // fallback to nearest neighbor
    double minDist2 = std::numeric_limits<double>::max();
    double bestValue = 0.0;
    for (const auto& pt : mPoints) {
        double drho = pt.rho - rho;
        double dz = pt.z - z;
        double dist2 = drho * drho + dz * dz;
        if (dist2 < minDist2) {
            minDist2 = dist2;
            bestValue = pt.rate;
        }
    }
    return bestValue;
}

// -----------------------------------------------------------------------------
// FindBilinearNeighbors: Find the 4 corners of a rectangle enclosing the point
// -----------------------------------------------------------------------------
bool RateTable2D::FindBilinearNeighbors(double rho, double z,
                                        std::vector<RatePoint>& neighbors) const
{
    neighbors.clear();

    // Create lists of points that lie on the same rows and columns
    std::vector<double> rho_vals, z_vals;
    for (const auto& pt : mPoints) {
        rho_vals.push_back(pt.rho);
        z_vals.push_back(pt.z);
    }

    std::sort(rho_vals.begin(), rho_vals.end());
    std::sort(z_vals.begin(), z_vals.end());
    auto it_rho = std::lower_bound(rho_vals.begin(), rho_vals.end(), rho);
    auto it_z   = std::lower_bound(z_vals.begin(), z_vals.end(), z);

    if (it_rho == rho_vals.begin() || it_rho == rho_vals.end()) return false;
    if (it_z == z_vals.begin()   || it_z == z_vals.end())   return false;

    double r1 = *(it_rho - 1), r2 = *it_rho;
    double z1 = *(it_z - 1), z2 = *it_z;

    // Find 4 corners (r1,z1), (r2,z1), (r1,z2), (r2,z2)
    int found = 0;
    for (const auto& pt : mPoints) {
        if (std::fabs(pt.rho - r1) < 1e-12 && std::fabs(pt.z - z1) < 1e-12)
            { neighbors.push_back(pt); ++found; continue; }
        if (std::fabs(pt.rho - r2) < 1e-12 && std::fabs(pt.z - z1) < 1e-12)
            { neighbors.push_back(pt); ++found; continue; }
        if (std::fabs(pt.rho - r1) < 1e-12 && std::fabs(pt.z - z2) < 1e-12)
            { neighbors.push_back(pt); ++found; continue; }
        if (std::fabs(pt.rho - r2) < 1e-12 && std::fabs(pt.z - z2) < 1e-12)
            { neighbors.push_back(pt); ++found; continue; }
    }
    return (found == 4);
}

// -----------------------------------------------------------------------------
// BilinearInterp: perform bilinear interpolation on rectangle of 4 values
// -----------------------------------------------------------------------------
double RateTable2D::BilinearInterp(const std::vector<RatePoint>& N,
                                   double rho, double z) const
{
    // Unpack
    double x1 = N[0].rho, x2 = N[1].rho;
    double y1 = N[0].z,   y2 = N[2].z;

    double f11 = N[0].rate;
    double f21 = N[1].rate;
    double f12 = N[2].rate;
    double f22 = N[3].rate;

    double denom = (x2 - x1) * (y2 - y1);
    if (denom == 0.0) return 0.5 * (f11 + f22);  // degenerate rectangle

    double a = (x2 - rho) * (y2 - z) / denom;
    double b = (rho - x1) * (y2 - z) / denom;
    double c = (x2 - rho) * (z - y1) / denom;
    double d = (rho - x1) * (z - y1) / denom;

    return a * f11 + b * f21 + c * f12 + d * f22;
}
// -----------------------------------------------------------------------------