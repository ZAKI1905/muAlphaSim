/**
 * @file      HistogramManager.hh
 * @brief     Histogram recording utility for muon-alpha ionization events.
 *
 * This class wraps G4AnalysisManager to provide modular, centralized control
 * over histogram creation and data recording throughout the simulation.
 *
 * Currently, it records a 2D histogram of ionization events as a function of
 * cylindrical radius \( \rho \) and axial height \( z \).
 *
 * Example Usage
 * -------------
 * @code
 *   HistogramManager::Initialize();
 *   HistogramManager::RecordIonization(rho, z);
 *   HistogramManager::Finalize();
 * @endcode
 *
 * Extend this class to include additional histograms for decay or capture statistics.
 *
 * @author   Mohammadreza Zakeri
 * @date     2025-05-27
 */

#pragma once

#include "G4AnalysisManager.hh"

/**
 * @class HistogramManager
 * @brief Centralized histogram manager for collecting muon-alpha interaction data.
 *
 * This singleton-style utility wraps around G4AnalysisManager, ensuring histograms
 * are created, filled, and written consistently.
 */
class HistogramManager {
  public:
    /**
     * @brief Initializes the histogram manager and declares all histograms.
     *
     * Must be called once at the beginning of the run. It sets up the histogram
     * IDs, titles, binning, and axes ranges. If already initialized, this function
     * is a no-op.
     */
    static void Initialize();  //!< Book histograms (per-thread), no file I/O here

    /**
     * @brief Records a single muon-alpha ionization event.
     *
     * This fills the 2D histogram with the given coordinates (\(\rho\), \(z\)).
     *
     * @param rho  Cylindrical radius at the point of ionization [m].
     * @param z    Height coordinate at the point of ionization [m].
     */
    static void RecordIonization(double rho, double z);

	/**
     * @brief Records a muon-α capture event into the capture histogram.
     *
     * This method fills the "Capture" histogram with the (rho, z) coordinates of
     * the mu-α atom at the point of capture.
     *
     * @param rho  Cylindrical radial position of the muon [m].
     * @param z    Axial (longitudinal) position of the muon [m].
     */
    static void RecordCapture(double rho, double z);

    /**
     * @brief Writes and finalizes all histogram output to file.
     *
     * Should be called at the end of the simulation. Closes the output file
     * and finalizes the ROOT data.
     */
    static void Finalize();

  private:

    /// Thread-local instance of the G4AnalysisManager
    // static G4ThreadLocal G4AnalysisManager* fManager;
    
    /// Thread-local initialization flag
    static G4ThreadLocal bool initialized;  //!< Initialization guard. 
};