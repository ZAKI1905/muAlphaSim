// RevolvedFieldFromPOS.hh
// Parses axisymmetric 2D electric field from .pos file (VT entries)
// and projects it into 3D space via revolution.

#pragma once

#include "G4ThreeVector.hh"
#include <vector>
#include <string>

/**
 * @class RevolvedFieldFromPOS
 * @brief Loads a Gmsh .pos file with axisymmetric electric field (VT format)
 *        and provides field interpolation at arbitrary 3D positions.
 *
 * This class reads a Gmsh-generated .pos file containing vector triangle (VT)
 * electric field data from a 2D axisymmetric simulation (r-z plane). It stores
 * the values and interpolates the nearest electric field at a given 3D point
 * by assuming azimuthal symmetry and revolving the radial component into the
 * (x, y, z) space used by Geant4.
 *
 * Example usage:
 *   RevolvedFieldFromPOS field("ElectricField.pos");
 *   G4ThreeVector E = field.GetField(G4ThreeVector(x, y, z));
 */
class RevolvedFieldFromPOS {
public:
    /**
     * @brief Constructor: loads a Gmsh .pos file with VT field data.
     * @param filename Path to the .pos file containing VT entries.
     */
    RevolvedFieldFromPOS(const std::string& filename);

    /**
     * @brief Interpolates the electric field at a given 3D position.
     *        Assumes azimuthal symmetry.
     * @param pos Position in Cartesian space (x, y, z).
     * @return Electric field vector (Ex, Ey, Ez) at that point.
     */
    G4ThreeVector GetField(const G4ThreeVector& pos) const;

private:
    /// Struct to store one field vector at a node in (r, z) space
    struct FieldVertex {
        double r;                ///< Radial position
        double z;                ///< Z position
        G4ThreeVector E;         ///< Electric field vector (Er, 0, Ez)
    };

    std::vector<FieldVertex> fieldPoints; ///< List of sampled VT field points

    /**
     * @brief Parses the Gmsh .pos file (VT format) and loads vector data.
     * @param filename Path to the .pos file.
     */
    void LoadPOS(const std::string& filename);
};
