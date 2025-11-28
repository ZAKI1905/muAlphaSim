/**
 * @file      RunAction.hh
 * @brief     User run-level hooks: histogram I/O **and** global tallies.
 *
 * ────────────────────────────────────────────────────────────────────────────
 *  Design summary (2025-06-04 redesign)
 *  ────────────────────────────────────────────────────────────────────────────
 *  • Exactly **one** util::DataLogger lives on the **master** thread.  
 *  • Every RunAction (master + each worker) owns a *vector* of
 *      `G4Accumulable<unsigned>` for  
 *        –   per-cone ionisations / captures,  
 *        –   per-panel ionisations / captures.  
 *    These are registered with `G4AccumulableManager` in the ctor.
 *  • Workers **only** increment their thread-local accumulables
 *    (zero locking, zero I/O).  
 *  • In `EndOfRunAction` the master merges accumulables, then asks
 *    `DataLogger` to write `run.json` + footer of `events.tsv`.
 */

#pragma once

/*─────────────────────────── Geant4 core ───────────────────────────────*/
#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4Run.hh"

/*──────────────────────────── std / project ────────────────────────────*/
#include <vector>
#include <string>
#include "GeometryConfig.hh"

namespace util { class DataLogger; }

/*======================================================================*/
/*  class RunAction                                                     */
/*======================================================================*/
class RunAction : public G4UserRunAction
{
  public:
    RunAction(const geom::GeometryConfig& cfg,
              util::DataLogger*           logger);                 ///< ctor
    ~RunAction() override = default;                               ///< dtor

    /* Geant4 entry points */
    void BeginOfRunAction(const G4Run* run) override;
    void   EndOfRunAction(const G4Run* run) override;

    /*────────── fast access for SteppingAction (thread-local) ─────────*/
    inline G4Accumulable<unsigned>& ConeIon  (std::size_t i) { return coneIon_[i]; }
    inline G4Accumulable<unsigned>& ConeCap  (std::size_t i) { return coneCap_[i]; }
    inline G4Accumulable<unsigned>& PanelIon (std::size_t i){ return panelIon_[i]; }
    inline G4Accumulable<unsigned>& PanelCap (std::size_t i){ return panelCap_[i]; }

    /** @brief Write a one-page run summary to the console (master only). */
    static void PrintRunSummary(unsigned long nEvents,
                                unsigned long nCap,
                                unsigned long nIon);
  private:
    /*──── immutable per-run data ────────────────────────────────────*/
    const geom::GeometryConfig cfg_;
    util::DataLogger*          logger_;          ///< belongs to master only
    const std::size_t          nCones_;
    const std::size_t          nPanels_;

    /*──── thread-local accumulables (registered in ctor) ────────────*/
    std::vector<G4Accumulable<unsigned>> coneIon_;
    std::vector<G4Accumulable<unsigned>> coneCap_;
    std::vector<G4Accumulable<unsigned>> panelIon_;
    std::vector<G4Accumulable<unsigned>> panelCap_;


};