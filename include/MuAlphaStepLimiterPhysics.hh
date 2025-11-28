#pragma once

#include "G4VPhysicsConstructor.hh"

class MuAlphaStepLimiterPhysics : public G4VPhysicsConstructor {
public:
    MuAlphaStepLimiterPhysics(const G4String& name = "MuAlphaStepLimiter");
    ~MuAlphaStepLimiterPhysics() override = default;

    void ConstructParticle() override;
    void ConstructProcess() override;
};