/**
 * @file    ActionInitialization.cc
 * @brief   Instantiate PrimaryGenerator, RunAction, SteppingAction per thread.
 */

#include "ActionInitialization.hh"

/*────────────────────────── Geant4 includes ─────────────────────────*/
#include "G4AnalysisManager.hh"

/*────────────────────────── project headers ─────────────────────────*/
#include "PrimaryGenerator.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "DataLogger.hh"

/*════════════════════════════════════════════════════════════════════*/
/*  ctor / dtor                                                       */
/*════════════════════════════════════════════════════════════════════*/
ActionInitialization::ActionInitialization(DetectorConstruction*       det,
                                           const geom::GeometryConfig& cfg)
: fDet_{det}
, cfg_{cfg}
{
    /* Create the single master-owned DataLogger right here */
    logger_ = new util::DataLogger("results");
}

ActionInitialization::~ActionInitialization()
{
    /* Logger is never deleted until process exit – harmless. */
}

/*════════════════════════════════════════════════════════════════════*/
/*  BuildForMaster – master thread gets RunAction only                */
/*════════════════════════════════════════════════════════════════════*/
void ActionInitialization::BuildForMaster() const
{
    SetUserAction(new RunAction(cfg_, logger_));

    /* ROOT file settings (shared for all threads) */
    auto* mgr = G4AnalysisManager::Instance();
    mgr->SetDefaultFileType("root");
    mgr->SetNtupleMerging(true);
    mgr->SetFileName(
        "/Users/keeper/Documents/geant4-projects/muAlphaSim/output/root/"
        "muon_ionization_output.root");
}

/*════════════════════════════════════════════════════════════════════*/
/*  Build – called once per *worker* thread                           */
/*════════════════════════════════════════════════════════════════════*/
void ActionInitialization::Build() const
{
    /* 1)  Primary generator */
    SetUserAction(new PrimaryGenerator());

    /* 2)  RunAction (thread-local but shares same logger pointer) */
    auto* runAction = new RunAction(cfg_, logger_);
    SetUserAction(runAction);

    /* 3)  SteppingAction needs geometry + this thread’s RunAction */
    SetUserAction(new SteppingAction(fDet_, runAction));

    /* 4)  Optional per-event bookkeeping */
    SetUserAction(new EventAction());

    /* 5)  ROOT settings (same as master) */
    auto* mgr = G4AnalysisManager::Instance();
    mgr->SetDefaultFileType("root");
    mgr->SetNtupleMerging(true);
    mgr->SetFileName(
        "/Users/keeper/Documents/geant4-projects/muAlphaSim/output/root/"
        "muon_ionization_output.root");
}