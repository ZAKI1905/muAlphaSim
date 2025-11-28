/**
 * @file      HistogramManager.cc
 * @brief     Source file for HistogramManager.
 * 
 * This manager handles all histogram creation, filling, and finalization.
 * It uses per-thread instances of G4AnalysisManager to ensure thread safety.
 * Histograms are merged automatically at the end of the run.
 */

#include "G4RunManager.hh"


#include "HistogramManager.hh"

/// Thread-local instance of G4AnalysisManager
// G4ThreadLocal G4AnalysisManager* HistogramManager::fManager = nullptr;

/// Flag to indicate whether histograms have been initialized for this thread
G4ThreadLocal bool HistogramManager::initialized = false; 

// -----------------------------------------------------------------------------
// Initialize the histogram manager and declare all histograms for this thread
// -----------------------------------------------------------------------------
/**
 * @brief Initialize the histogram manager.
 * 
 * This function creates all histograms and enables merging.
 * It is safe to call multiple times; it will only initialize once per thread.
 */
void HistogramManager::Initialize()
{
    if (initialized) return;

    // Obtain the per-thread analysis manager instance
    auto* fManager = G4AnalysisManager::Instance();
    fManager->SetVerboseLevel(1);
    fManager->SetFirstHistoId(1);

    // Enable automatic merging of histograms across threads
    fManager->SetNtupleMerging(true);  // CHANGE: ensure ROOT ntuple merging

    // Declare histograms
    fManager->CreateH1("IonizationRho", "Ionization Rho [m]", 100, 0, 100e-9);
    fManager->CreateH1("IonizationZ",   "Ionization Z [m]",   100, 950e-9, 1050e-9);
    fManager->CreateH1("CaptureRho", "Capture Rho [m]", 100, 0, 10e-9);
    fManager->CreateH1("CaptureZ",   "Capture Z [m]",   100, 0, 1000e-9);
	// fH2_Capture = fManager->CreateH2(
	// 	"Capture",
	// 	"mu-α Capture (ρ,z)",
	// 	100, 0.0, 1e-6,
	// 	100, 0.0, 2e-6
	// );

    initialized = true;
    G4cout << "[HistogramManager] Initialized histograms on thread "
           << G4Threading::G4GetThreadId() << G4endl;
}

//----------------------------------------------------------------------------//
// Record a muon-alpha ionization event to the histogram
//----------------------------------------------------------------------------//
/**
 * @brief Record a muon-alpha ionization event.
 * 
 * @param rho Radial coordinate of the event [m]
 * @param z   Z coordinate of the event [m]
 */
void HistogramManager::RecordIonization(double rho, double z)
{
    // if (!initialized) Initialize();

    auto* fManager = G4AnalysisManager::Instance();
    fManager->FillH1(1, rho);
    fManager->FillH1(2, z);

    // G4cout << "[HistogramManager] Filled Ionization H1 (rho=" << rho << ", z=" << z << ") on thread "
    //        << G4Threading::G4GetThreadId() << G4endl;
}

// -----------------------------------------------------------------------------
// Finalize the histogram manager and write all histograms to file
// -----------------------------------------------------------------------------
/**
 * @brief Finalize the histogram manager.
 * 
 * This function writes all histograms to file. It should only be called by the master thread.
 */
void HistogramManager::Finalize() 
{
    // Only master thread should write the output
    // if (G4RunManager::GetRunManager()->GetRunManagerType() == G4RunManager::masterRM)
    // {
    //     if (fManager)
    //     {
    //         fManager->Write();
    //         fManager->CloseFile();
    //     }
    // }

    if (G4RunManager::GetRunManager()->GetRunManagerType() == G4RunManager::masterRM)
    {
        auto* mgr = G4AnalysisManager::Instance();
        G4cout << "[HistogramManager] Finalizing on master. About to write histograms." << G4endl;
        G4bool success = mgr->Write();
        G4cout << "[HistogramManager] Write() returned: " << success << G4endl;
        mgr->CloseFile();
    }
}

// -----------------------------------------------------------------------------
// Record a muon-α capture event to the histogram
// -----------------------------------------------------------------------------
/**
 * @brief Record a muon-alpha capture event.
 * 
 * @param rho Radial coordinate of the event [m]
 * @param z   Z coordinate of the event [m]
 */
void HistogramManager::RecordCapture(double rho, double z)
{
	// if (!initialized) Initialize();

    auto* fManager = G4AnalysisManager::Instance();

    // if (fH2_Capture >= 0)
    // {
    //     fManager->FillH2(fH2_Capture, rho, z);
    // }
    fManager->FillH1(3, rho);
    fManager->FillH1(4, z);

    // G4cout << "[HistogramManager] Filled Capture H1 (rho=" << rho << ", z=" << z << ") on thread "
    //        << G4Threading::G4GetThreadId() << G4endl;
}
// -----------------------------------------------------------------------------