/**
 * @file PhysicsList.cc
 * @brief Basic implementation using the QGSP_BERT modular physics list.
 */

#include "G4SystemOfUnits.hh"
#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4IonPhysics.hh"

#include "PhysicsList.hh"
#include "MuAlphaStepLimiterPhysics.hh"

// -----------------------------------------------------------------------------
/**
 * @brief Constructs and registers standard physics modules.
 */
// -----------------------------------------------------------------------------
PhysicsList::PhysicsList() {
    // Default cut value
    defaultCutValue = 0.7 * mm;

    // Register standard EM physics
    RegisterPhysics(new G4EmStandardPhysics());

    // Register hadron physics
    RegisterPhysics(new G4HadronPhysicsQGSP_BERT());

    // Register decay physics
    RegisterPhysics(new G4DecayPhysics());

    // Optional: Ion physics (if needed for fusion nuclei, etc.)
    RegisterPhysics(new G4IonPhysics());

    RegisterPhysics(new MuAlphaStepLimiterPhysics());
}
// -----------------------------------------------------------------------------