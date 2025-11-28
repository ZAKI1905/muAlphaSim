/**
 * @file MuAlpha5p.hh
 * @brief Declaration of the custom muon-alpha (μα) particle in the 5p state.
 *        This particle is treated as a bound system for initialization and tracking in Geant4.
 */

#pragma once

#include "G4ParticleDefinition.hh"

/**
 * @class MuAlpha5p
 * @brief Custom Geant4 particle representing a μ⁻α⁺ bound state in an excited 5p state.
 */
class MuAlpha5p : public G4ParticleDefinition {
public:
    /**
     * @brief Accessor for the singleton instance of the particle.
     * @return Pointer to the MuAlpha5p definition.
     */
    static MuAlpha5p* Definition();

private:
    /// Singleton instance of the particle
    static MuAlpha5p* muAlphaDefinition;

    /// Constructor is private to enforce singleton pattern
    MuAlpha5p() = default;
};