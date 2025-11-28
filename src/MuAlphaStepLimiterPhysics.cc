#include "G4StepLimiter.hh"
#include "G4ProcessManager.hh"
#include "G4Decay.hh"

#include "MuAlphaStepLimiterPhysics.hh"
#include "MuAlpha5p.hh"
// -----------------------------------------------------------------------------
/**
 * @file MuAlphaStepLimiterPhysics.cc
 * @brief Implements the MuAlphaStepLimiterPhysics class to manage mu-α step limiting and decay processes.
 */
// -----------------------------------------------------------------------------
MuAlphaStepLimiterPhysics::MuAlphaStepLimiterPhysics(const G4String& name)
    : G4VPhysicsConstructor(name) {}

// -----------------------------------------------------------------------------
/**
 * @brief Constructs the mu-α particle and registers its processes.
 * 
 * This method ensures that the MuAlpha5p particle is defined and that
 * the step limiter and decay processes are added to its process manager.
 */
 // -----------------------------------------------------------------------------
void MuAlphaStepLimiterPhysics::ConstructParticle()
{
    // Ensure MuAlpha5p gets constructed
    MuAlpha5p::Definition();
}

// -----------------------------------------------------------------------------
/**
 * @brief Constructs the processes for the mu-α particle.
 * 
 * This method adds a step limiter and decay process to the mu-α particle's
 * process manager, allowing for step limiting and decay handling.
 */
 // -----------------------------------------------------------------------------
void MuAlphaStepLimiterPhysics::ConstructProcess()
{
    auto* muAlpha = MuAlpha5p::Definition();
    G4ProcessManager* pmanager = muAlpha->GetProcessManager();
    if (pmanager) 
	{
        auto* stepLimiter = new G4StepLimiter();
        pmanager->AddDiscreteProcess(stepLimiter);

		// Add decay process
		G4Decay* decay = new G4Decay();
		pmanager->AddProcess(decay);
		pmanager->SetProcessOrdering(decay, idxPostStep);
		pmanager->SetProcessOrdering(decay, idxAtRest);

	}
}
// -----------------------------------------------------------------------------