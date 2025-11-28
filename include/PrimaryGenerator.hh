/**
 * @file PrimaryGenerator.hh
 * @brief Declares a primary particle generator for emitting muAlpha5p particles.
 */

#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

/**
 * @class PrimaryGenerator
 * @brief Custom primary generator class to shoot a single muAlpha5p particle per event.
 *
 * Uses G4ParticleGun to initialize the particle's position, direction, and energy.
 */
class PrimaryGenerator : public G4VUserPrimaryGeneratorAction {
public:
    /// Constructor
    PrimaryGenerator();

    /// Destructor
    ~PrimaryGenerator() override;

    /**
     * @brief Called for each event to generate a primary vertex.
     * @param event Pointer to the G4Event object for the current event.
     */
    void GeneratePrimaries(G4Event* event) override;

private:
    G4ParticleGun* fParticleGun; ///< Particle gun instance used for emission
};