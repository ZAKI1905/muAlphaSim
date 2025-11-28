/**
 * @file PhysicsList.hh
 * @brief Declares a basic physics list wrapper using the QGSP_BERT reference list.
 */

#pragma once

#include "G4VModularPhysicsList.hh"
#include "G4VPhysicsConstructor.hh"

/**
 * @class PhysicsList
 * @brief Wraps a standard Geant4 reference physics list (QGSP_BERT).
 *
 * You can later add custom physics processes (e.g. muon-alpha stripping) to this class.
 */
class PhysicsList : public G4VModularPhysicsList {
public:
    PhysicsList();
    ~PhysicsList() override = default;
};