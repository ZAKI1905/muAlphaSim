#pragma once

#include "G4ElectroMagneticField.hh"
#include "G4ThreeVector.hh"
#include "RevolvedFieldFromPOS.hh"

/**
 * @class RevolvedG4Field
 * @brief A Geant4-compatible electromagnetic field that wraps a revolved 2D E-field.
 *
 * This class implements G4ElectroMagneticField and delegates field lookups to
 * a precomputed axisymmetric electric field (loaded via RevolvedFieldFromPOS).
 * Only the electric field is set (magnetic field is zero).
 */
class RevolvedG4Field : public G4ElectroMagneticField {
public:
    /**
     * @brief Constructor from .pos field filename.
     * @param filename Path to a Gmsh .pos file containing axisymmetric E-field data.
     */
    explicit RevolvedG4Field(const std::string& filename);

    /**
     * @brief Override Geant4 field query.
     * @param point Cartesian coordinates of query point (x,y,z,t).
     * @param field Output array to store [Ex, Ey, Ez, Bx, By, Bz].
     */
    void GetFieldValue(const double point[4], double* field) const override;

	/**
	 * @brief Indicates whether the field contributes to energy change of the particle.
	 * @return true, because electric fields do work on charged particles.
	 */
	G4bool DoesFieldChangeEnergy() const override { return true; }

private:
    RevolvedFieldFromPOS fieldMap; ///< Internal field map used for E-field lookup
};