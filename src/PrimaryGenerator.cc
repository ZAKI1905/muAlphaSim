/**
 * @file PrimaryGenerator.cc
 * @brief Defines a simple particle gun to emit muAlpha5p particles into the simulation.
 */

#include "G4MuonMinus.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "Randomize.hh"

#include "PrimaryGenerator.hh"
#include "MuAlpha5p.hh"


// -----------------------------------------------------------------------------
/**
 * @brief Constructor for the primary generator.
 *
 * Configures the G4ParticleGun to fire one muAlpha5p particle from a fixed location
 * in the +z direction with a user-defined kinetic energy.
 */
// -----------------------------------------------------------------------------
PrimaryGenerator::PrimaryGenerator() 
{
    fParticleGun = new G4ParticleGun(1); // One particle per event

    auto* particle = MuAlpha5p::Definition(); ///< Custom particle definition
    fParticleGun->SetParticleDefinition(particle);
}

// -----------------------------------------------------------------------------
/**
 * @brief Destructor: cleans up the particle gun.
 */
PrimaryGenerator::~PrimaryGenerator() 
{
    delete fParticleGun;
}

// -----------------------------------------------------------------------------
/**
 * @brief Generates a primary particle for the event.
 * @param event Pointer to the current event.
 */
void PrimaryGenerator::GeneratePrimaries(G4Event* event) 
{
    // --- Spatial distribution (Gaussian beam around y-z plane at x = -10 µm)
    G4double sigma_r = 100 * nm;
    G4double r = sigma_r * std::sqrt(-2.0 * std::log(G4UniformRand()));
    G4double phi = 2.0 * CLHEP::pi * G4UniformRand();

    G4double x = -2.0 * um;  // Starting x-plane
    G4double y = r * std::cos(phi);
    G4double z_offset = 975 * nm;  // Shift beam to target the tip of the cone
    G4double z = z_offset + r * std::sin(phi);

    fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));

    // --- Momentum direction (narrow cone around +x)
    G4double angular_spread = 2.0 * CLHEP::deg;

    G4double theta = angular_spread * G4UniformRand(); // [0, θ_max]
    G4double psi   = 2.0 * CLHEP::pi * G4UniformRand(); // full azimuthal angle

    // Convert spherical deviation from x-axis
    G4ThreeVector direction(
        std::cos(theta),
        std::sin(theta) * std::cos(psi),
        std::sin(theta) * std::sin(psi)
    );

    fParticleGun->SetParticleMomentumDirection(direction);

    fParticleGun->GeneratePrimaryVertex(event);

    // G4cout << "[DEBUG] GeneratePrimaries called. Firing: muAlpha5p from "
    //        << fParticleGun->GetParticlePosition()
    //        << ", direction: " << direction
    //        << ", energy: " << fParticleGun->GetParticleEnergy()
    //        << G4endl;
}
// -----------------------------------------------------------------------------