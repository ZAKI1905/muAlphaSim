/**
 * @file    SteppingAction.cc
 * @brief   Capture / Ionization logic for each simulation step.
 *
 *  A. If pre-step point is inside the cone logical volume  → “captured”.
 *  B. Else, if (rho,z) lies inside the tabulated domain      → sample ADK.
 *  C. Otherwise                                             do nothing.
 */

// #include <cmath>    /* std::hypot */

// #include "G4Step.hh"
// #include "Randomize.hh"
// #include "G4ThreeVector.hh"
// #include "G4SystemOfUnits.hh"
// #include "G4AutoLock.hh"
// #include "G4RunManager.hh"
// #include "G4VUserTrackInformation.hh"

// #include "SteppingAction.hh"
// #include "DetectorConstruction.hh"
// #include "RateTableSingleton.hh"
// #include "MuAlpha5p.hh"
// #include "HistogramManager.hh"
// #include "RateTableSingleton.hh"
#include "RunAction.hh"

#include "SteppingAction.hh"

/*────────────────────────────── Geant4 ───────────────────────────────*/
#include "G4AutoLock.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
// #include "G4UniformRand.hh"

/*──────────────────────────── project ────────────────────────────────*/
#include "DetectorConstruction.hh"
// #include "HistogramManager.hh"
#include "MuAlpha5p.hh"
#include "RateTableSingleton.hh"
#include "RunAction.hh"


/*====================================================================*/
/*  thread-local event flags                                           */
/*====================================================================*/
G4ThreadLocal bool SteppingAction::gEventCaptureOccurred    = false;
G4ThreadLocal bool SteppingAction::gEventIonizationOccurred = false;

/* mutex to protect ROOT histogram fills (simple, coarse lock) */
// namespace { G4Mutex gHistMutex = G4MUTEX_INITIALIZER; }

// ----------------------------------------------------------------------
// Static counters for ionization and capture events
// ----------------------------------------------------------------------
// G4ThreadLocal size_t SteppingAction::fIonizationCount = 0;  ///< count of ionization events
// G4ThreadLocal size_t SteppingAction::fCaptureCount = 0;     ///< count of capture events
// G4ThreadLocal bool SteppingAction::gEventCaptureOccurred    = false;
// G4ThreadLocal bool SteppingAction::gEventIonizationOccurred = false;
/*====================================================================*/
/*  ctor                                                              */
/*====================================================================*/
SteppingAction::SteppingAction(const DetectorConstruction* det,
                               RunAction*                  run)
: fDet(det)
, runAction_(run)
{}
/*====================================================================*/
/*  static helpers                                                    */
/*====================================================================*/
void SteppingAction::ResetEventFlags()
{
    gEventCaptureOccurred    = false;
    gEventIonizationOccurred = false;
}
bool SteppingAction::EventHadCapture()    { return gEventCaptureOccurred; }
bool SteppingAction::EventHadIonization() { return gEventIonizationOccurred; }

// -----------------------------------------------------------------------------
/// Print current counts (for summary at end of run)
/* static */
void RunAction::PrintRunSummary(unsigned long nEvents,
                                unsigned long nCap,
                                unsigned long nIon)
{
    auto fmt_pct = [](double x) { return 100.0 * x; };

    const double fIon = nEvents ? double(nIon) / nEvents : 0.0;
    const double fCap = nEvents ? double(nCap) / nEvents : 0.0;

    auto err95 = [nEvents](unsigned long k, double p)
    {
        if (k == 0 || k == nEvents) return 3.0 / nEvents;
        return std::sqrt(p * (1.0 - p) / nEvents);
    };

    const double dIon = err95(nIon, fIon);
    const double dCap = err95(nCap, fCap);

    G4cout << "\n"
           << "        ============ Global Event Summary ============\n"
           << "         Number of events in this run : " << nEvents << "\n"
           << "         Total Captures               : " << nCap << "\n"
           << "         Total Ionizations            : " << nIon << "\n"
           << "         Ionization Fraction (%)      : "
           << fmt_pct(fIon) << "  ± " << fmt_pct(dIon) << "\n"
           << "         Capture    Fraction (%)      : "
           << fmt_pct(fCap) << "  ± " << fmt_pct(dCap) << "\n"
           << "         Ionization / Capture Ratio   : "
           << (nCap ? double(nIon) / nCap : 0.0) << "\n"
           << "        ==============================================\n" << G4endl;
}

// -----------------------------------------------------------------------------
// /// @brief Increment the capture event count.
// /// This method is called whenever a capture event occurs.
// void SteppingAction::IncrementCaptureCount() 
// { 
//     ++fCaptureCount; 
//     ++gTotalCaptureCount;  // also increment global count
// }

// // -----------------------------------------------------------------------------
// /// @brief Increment the ionization event count.
// /// This method is called whenever an ionization event occurs.
// void SteppingAction::IncrementIonizationCount() 
// { 
//     ++fIonizationCount; 
//     ++gTotalIonizationCount;  // also increment global count
// }

// -----------------------------------------------------------------------------
/*====================================================================*/
/*  UserSteppingAction                                                */
/*====================================================================*/
/**
 * @brief Per-step hook — see header for flow chart.
 * @param step pointer supplied by Geant4 each step
 */
void SteppingAction::UserSteppingAction(const G4Step* step)
{
    /* ignore everything except mu-α */
    auto track = step->GetTrack();
    if (track->GetDefinition() != MuAlpha5p::Definition()) return;

    /* cone id + logical volume lookup */
    auto touch  = step->GetPreStepPoint()->GetTouchableHandle();
    auto* lv    = touch->GetVolume()->GetLogicalVolume();
    int  copyNo = touch->GetVolume()->GetCopyNo();  // 0-based cone index

    /*──────────────────────────────────────────────── CAPTURE ────────*/
    if (lv == fDet->GetConeLogical())
    {
        /* bookkeeping */
        gEventCaptureOccurred = true;
        runAction_->ConeCap(copyNo) += 1;                 // per-cone
        runAction_->PanelCap(fDet->GetConesInfo()[copyNo]
                             .panelIdx) += 1;             // per-panel

        /* histogram (protected) */
        // {
        //     G4AutoLock lk(&gHistMutex);
        //     const auto& C = fDet->GetConesInfo()[copyNo].baseCentre;
        //     const auto  P = track->GetPosition();
        //     const double rho = 1e-3 * std::hypot(P.x() - C.x(),
        //                                          P.y() - C.y()); // m
        //     const double z   = 1e-3 * (P.z() - C.z());          // m
        //     HistogramManager::RecordCapture(rho, z);
        // }

        track->SetTrackStatus(fStopAndKill);
        return;
    }

   
    // //-------------------------------------------------------------------
    // //  A. CAPTURE  – was the PRE-step point inside a cone LV?
    // //-------------------------------------------------------------------
    // if (lv == fDet->GetConeLogical())
    // {
    //     // G4cout << "[DEBUG] SteppingAction::UserSteppingAction: mu-α captured in cone "
    //     //       << (copyNo + 1) << " at " << track->GetPosition() << G4endl;
    //     //-----------------------------------------------------------------
    //     // 1)  Look-up static cone metadata
    //     //-----------------------------------------------------------------
    //     //  • gCone[copyNo] was filled once per cone by the builder
    //     //  • baseCentre is in global metres (x, y, z0)
    //     //-----------------------------------------------------------------
    //     const geom::ConeInfo& ci      = fDet->GetConesInfo()[copyNo];
    //     const auto&     C       = ci.baseCentre;          // alias

    //     //-----------------------------------------------------------------
    //     // 2)  Local cylindrical coordinates at the capture point
    //     //-----------------------------------------------------------------
    //     const G4ThreeVector& P  = track->GetPosition();   // global [mm]
    //     const double rho_local  = 1e-3 * std::hypot(P.x() - C.x(),
    //                                             P.y() - C.y()); // → m
    //     const double z_local    = 1e-3 * (P.z() - C.z());          // → m

    //     //-----------------------------------------------------------------
    //     // 3)  Book-keeping & output
    //     //-----------------------------------------------------------------
    //     IncrementCaptureCount();
    //     HistogramManager::RecordCapture(rho_local, z_local);
    //     gEventCaptureOccurred = true;

    //     // // When a capture happens:
    //     // logger_->coneCaptureCounts_[copyNo].fetch_add(1, std::memory_order_relaxed);
    //     // logger_->panelCaptureCounts_[ci.panelIdx].fetch_add(1, std::memory_order_relaxed);

    //     if (!logger_->IsInitialized()) 
    //     {
    //         // Skip this step — logger is not ready yet!
    //         G4cout << "[SteppingAction] WARNING: DataLogger is not initialized, skipping capture logging." << G4endl << std::flush;
    //         return;
    //     }

    //     {
    //         std::lock_guard<std::mutex> lock(logger_->counterMutex_);
    //         G4cout << "[SteppingAction] Incrementing capture counts for cone " 
    //                << (copyNo + 1) << " and panel " << (ci.panelIdx + 1) << G4endl;
    //         logger_->coneCaptureCounts_[copyNo]++;
    //         logger_->panelCaptureCounts_[ci.panelIdx]++;
    //     }


    //     /* Kill the track */
    //     track->SetTrackStatus(fStopAndKill);
    //     return;                                     // done with this step
    // }

    /*────────────────────────────────────────────── IonizATION ──────*/
    // ---------------------------------------------------------------------------
    // B. IonizATION  – path–integrated probability over one shell crossing
    // ---------------------------------------------------------------------------

    /* ------------------------------------------------------------------------
    * Helper in UserInformation (per track)
    * --------------------------------------------------------------------- */
    struct ShellSegment : public G4VUserTrackInformation
    {
        G4ThreeVector entryPos;     ///< first point inside shell  [mm]
        G4double      entryTime{};  ///< global time at entry      [s]
        bool          inside{false};
        G4ThreeVector lastInsidePos;
        int           coneIdx{-1};  ///< copyNo at entry (for ConeInfo)
    };

    /* Retrieve / create segment record ------------------------------------ */
    auto* info = dynamic_cast<ShellSegment*>(track->GetUserInformation());
    if (!info) 
    {
        info = new ShellSegment;
        track->SetUserInformation(info);
    }

    /* Quick flags --------------------------------------------------------- */
    // auto* preLV   = touch->GetVolume()->GetLogicalVolume();
    bool  inShell = (lv == fDet->GetInShellLogical())  ||
                    (lv == fDet->GetMidShellLogical()) ||
                    (lv == fDet->GetOutShellLogical());

    /* -------- case 1 : entering shell ----------------------------------- */
    if (inShell && !info->inside)
    {
        info->inside      = true;
        info->entryPos    = track->GetPosition();       // [mm]
        info->entryTime   = track->GetGlobalTime();     // [s]
        info->coneIdx     = copyNo;                     // remember which cone
        info->lastInsidePos = info->entryPos;
        return;
    }
 
    /* -------- case 2 : still inside ------------------------------------- */
    if (inShell && info->inside)
    {
        info->lastInsidePos = track->GetPosition();
        return;
    }

    /* -------- case 3 : leaving shell ------------------------------------ */
    if (!inShell && info->inside)
    {
        /* geometry constants for *this* cone ------------------------------- */
        const geom::ConeInfo& ci = fDet->GetConesInfo()[ info->coneIdx ];
        const auto& C            = ci.baseCentre;              // global metres

        /* segment endpoints in metres -------------------------------------- */
        const G4ThreeVector P0 = info->entryPos     * 1e-3;
        const G4ThreeVector P1 = track->GetPosition()* 1e-3;

        const double T0 = info->entryTime;
        const double T1 = track->GetGlobalTime();

        /* fixed mid-point resolution --------------------------------------- */
        constexpr int N = 20;
        double        sumλ = 0.0;

        for (int i = 0; i < N; ++i)
        {
            const double s  = (i + 0.5) / N;
            const auto   P  = P0 + s * (P1 - P0);        // global [m]

            const double ρ  = std::hypot(P.x() - C.x(),  // cone-local ρ
                                        P.y() - C.y());
            const double z  = P.z() - C.z();             // cone-local z

            if (RateTable().Inside(ρ, z))
                sumλ += RateTable().Interp(ρ, z);        // λ [s⁻¹]
        }

        const double dt   = (T1 - T0) / N;
        const double Pint = 1.0 - std::exp(-sumλ * dt);

        info->inside = false;                          // reset for reuse

        /* Bernoulli trial --------------------------------------------------- */
        if (G4UniformRand() < Pint)
        {
            /* stop track exactly here ----------------------------------------- */
            track->SetStepLength(0.0);
            track->SetTrackStatus(fKillTrackAndSecondaries);

            /* entry / exit in cone frame (metres) ----------------------------- */
            const double ρ_entry = std::hypot(P0.x() - C.x(),
                                            P0.y() - C.y());
            const double z_entry = P0.z() - C.z();

            const G4ThreeVector Pexit = info->lastInsidePos * 1e-3;
            const double z_exit  = Pexit.z() - C.z();     // ρ_exit≈75 nm → omit

            /* book-keeping + histogram --------------------------------------- */
            gEventIonizationOccurred = true;
            
            // G4cout << "[SteppingAction] mu-α ionized in cone " 
            //        << (info->coneIdx + 1) << ", copyNo +1= " << copyNo +1
            //     //    << " at " << Pexit << " (ρ=" << ρ_entry << " m, z=" << z_entry << " m)" 
            //        << G4endl;

            runAction_->ConeIon(info->coneIdx) += 1;
            runAction_->PanelIon(fDet->GetConesInfo()[info->coneIdx].panelIdx) += 1;
            // {
            //     G4AutoLock lk(&gHistMutex);
            //     HistogramManager::RecordIonization(ρ_entry, z_entry); // Not really working! Histograms are empty!
            // }


            // When an ionization happens:
            // logger_->coneIonizationCounts_[info->coneIdx].fetch_add(1, std::memory_order_relaxed);
            // logger_->panelIonizationCounts_[ci.panelIdx].fetch_add(1, std::memory_order_relaxed);
            // if (!logger_->IsInitialized()) 
            // {
            //     // Skip this step — logger is not ready yet!
            //     G4cout << "[SteppingAction] WARNING: DataLogger is not initialized, skipping ionization logging." << G4endl << std::flush;
            //     return;
            // }
            // {
            //     std::lock_guard<std::mutex> lock(logger_->counterMutex_);
            //     G4cout << "[SteppingAction] Incrementing ionization counts for cone " 
            //        << (copyNo + 1) << " and panel " << (ci.panelIdx + 1) << G4endl;
            //     logger_->coneIonizationCounts_[copyNo]++;
            //     logger_->panelIonizationCounts_[ci.panelIdx]++;
            // }
        }

        return;
    }

    /* -------- case 4 : never entered shell ------------------------------ */
    return;

}
// -----------------------------------------------------------------------------