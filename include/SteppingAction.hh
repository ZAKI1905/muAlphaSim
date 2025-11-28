/**
 * @file    SteppingAction.hh
 * @brief   Per-step logic – decide capture vs ionisation for a mu-α track.
 *
 * Design (2025-06-04):
 *   •  Worker threads do not talk to util::DataLogger.
 *   •  Instead they increment `G4Accumulable<unsigned>` counters exposed by
 *      their thread-local `RunAction`:
 *          ‣  coneIon / coneCap   (size = #cones)
 *          ‣  panelIon / panelCap (size = #panels)
 *   •  The master thread merges accumulables and writes the JSON/TSV summary.
 */

#ifndef STEPPING_ACTION_HH
#define STEPPING_ACTION_HH

/*────────────────────────── Geant4 core ───────────────────────────────*/
#include "G4UserSteppingAction.hh"
#include "G4Threading.hh"

/*──────────────────────────── std / proj ─────────────────────────────*/
#include <cstddef>
#include <cmath>

class DetectorConstruction;   // fwd
class RunAction;              // fwd

/**
 * @class SteppingAction
 * @brief Implements custom stepping rules for mu-α tracks.
 */
class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(const DetectorConstruction* det, RunAction* run);

    /* Geant4 hook */
    void UserSteppingAction(const G4Step* step) override;

    /* Event-scoped flags --------------------------------------------------*/
    static void   ResetEventFlags();
    static bool   EventHadCapture();
    static bool   EventHadIonization();

  private:
    /* geometry handle (for cone LV + ConeInfo look-ups) */
    const DetectorConstruction* fDet {nullptr};

    /* thread-local RunAction → exposes G4Accumulables */
    RunAction* runAction_{nullptr};

    /* event flags (thread-local) */
    static G4ThreadLocal bool gEventCaptureOccurred;
    static G4ThreadLocal bool gEventIonizationOccurred;
};

#endif /* STEPPING_ACTION_HH */