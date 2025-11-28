/**
 * @file MuAlpha5p.cc
 * @brief Definition of the custom μ⁻α⁺ bound state (5p excited state) particle.
 */

#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "MuAlpha5p.hh"

MuAlpha5p* MuAlpha5p::muAlphaDefinition = nullptr;

// -----------------------------------------------------------------------------
/**
 * @brief Initializes and returns the singleton definition of the μHe⁺ 5p particle.
 * 
 * Sets up mass, charge, and lifetime appropriate for a muon-alpha bound state in an excited state.
 * This model treats the excited μHe⁺ ion as a distinct, stable entity until it undergoes stripping.
 * 
 * @return MuAlpha5p* Pointer to particle definition.
 */
// -----------------------------------------------------------------------------
MuAlpha5p* MuAlpha5p::Definition() {
    if (muAlphaDefinition) return muAlphaDefinition;

    // Estimated mass of the μ⁻α⁺ bound system (alpha ~3727 MeV, muon ~105 MeV)
    G4double mass = 3727.379 * MeV;

    // Charge of the muon-alpha bound system (assume it's still singly ionized)
    G4double charge = +1. * eplus;

    // Lifetime of excited 5p state (placeholder; can be updated with real decay data)
    // G4double lifetime = 2.2 * microsecond;  // Same as free muon for now
    G4double lifetime = 50 * picosecond;  // Same as free muon for now

	muAlphaDefinition = (MuAlpha5p*) new G4ParticleDefinition(
		"muAlpha5p",         // name
		mass,                // mass [MeV/c^2]
		0.0 * MeV,           // width
		charge,              // charge
		0,                   // spin (2*spin)
		0,                   // parity
		0,                   // C-conjugation
		0,                   // isospin
		0,                   // isospin3
		0,                   // G-parity
		"exotic",            // type
		0,                   // lepton number
		0,                   // baryon number
		1000666,             // encoding (PDG-style or arbitrary unique ID)
		true,                // stable
		lifetime,            // lifetime [s]
		nullptr,             // decay table
		false,               // short-lived
		"muAlpha5p",         // subtype
		0,                   // anti-particle encoding
		0.0                  // magnetic moment
	);

    return muAlphaDefinition;
}
// -----------------------------------------------------------------------------