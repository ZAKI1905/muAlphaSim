/**
 * @file    DataLogger.hh
 * @author  Mohammadreza Zakeri
 * @date    2025-06-04
 *
 * @brief   Lightweight *master-thread–only* run-summary writer.
 *
 * ### High-level workflow 
 * | Stage | Thread(s) | Who calls it | What happens |
 * |-------|-----------|--------------|---------------|
 * | RunAction::BeginOfRunAction | master only | `RunAction` | `DataLogger::InitOutputFiles()` creates `results/<timestamp>/` and writes the TSV + JSON headers. |
 * | Event loop | worker threads | `SteppingAction` | Per-cone / per-panel counters are incremented via `G4Accumulables` ( *no* DataLogger involvement — zero locks ). |
 * | RunAction::EndOfRunAction | master only | `RunAction` | Geant4 merges all accumulables → master now has global tallies → `DataLogger::DumpRunSummary()` writes the final JSON and closes the TSV. |
 *
 * ### Thread-safety 
 * * Only the master owns and touches a `DataLogger` instance.  
 * * Worker threads never call any `DataLogger` method → no mutex / no races.  
 *
 * ### Files produced (per run) 
 * ```
 * results/YYYYMMDDTHHMMSS/
 * ├── events.tsv        (optional per-event rows → header written here, footer closed in DumpRunSummary)
 * └── run.json          (flat one-object summary for jq / pandas)
 * ```
 */

#pragma once

/*──────────────────────────────── C++ stdlib ───────────────────────────────*/
#include <string>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <mutex>      // still needed if we later add a TSV row from workers
/*───────────────────────────────── Geant4 ──────────────────────────────────*/
#include "G4ios.hh"   // G4cout / G4cerr

/*──────────────────────────── project headers ──────────────────────────────*/
#include "GeometryConfig.hh"

namespace util {

/**
 * @class DataLogger
 * @brief Collect-all-results-and-write-once helper (master thread only).
 *
 * The class is deliberately *stateless* with respect to event counters; those
 * are provided to `DumpRunSummary()` by the master‐thread `RunAction` after
 * Geant4 has merged the per-thread `G4Accumulables`.
 */
class DataLogger
{
  public:
    //───────────────────────────────────────────────────────────── Ctors / Dtor
    /**
     * @param[outDir] Directory where the per-run sub-directories will live
     *                (default = `"results"`).
     */
    explicit DataLogger(const std::string& outDir = "results");
    ~DataLogger();

    DataLogger(const DataLogger&)            = delete;
    DataLogger& operator=(const DataLogger&) = delete;

    //──────────────────────────────────────────────────────────────────────────
    /// @name Life-cycle hooks  (called from RunAction / master thread only)
    /// @{
    /**
     * @brief Create `results/<timestamp>/`, open the TSV, write header lines
     *        (cone dictionary, column names, etc.).
     *
     * Safe to call exactly once per run (asserts if you call twice).
     */
    void InitOutputFiles(const geom::GeometryConfig& cfg);

    /**
     * @brief Finish the TSV footer and emit a flat JSON run summary.
     *
     * @param[in] cfg            GeometryConfig (pretty-printed into JSON)
     * @param[in] nEvents        Number of events processed in the run
     * @param[in] nIon           Total ionisation events (merged)
     * @param[in] nCap           Total capture events    (merged)
     * @param[in] coneIon        Per-cone ionisation tallies   (size = #cones)
     * @param[in] coneCap        Per-cone capture tallies      (size = #cones)
     * @param[in] panelIon       Per-panel ionisation tallies  (size = #panels)
     * @param[in] panelCap       Per-panel capture tallies     (size = #panels)
     */
    void DumpRunSummary(const geom::GeometryConfig& cfg,
                        unsigned long               nEvents,
                        unsigned long               nIon,
                        unsigned long               nCap,
                        const std::vector<unsigned>& coneIon,
                        const std::vector<unsigned>& coneCap,
                        const std::vector<unsigned>& panelIon,
                        const std::vector<unsigned>& panelCap);

    /// @return `true` once `InitOutputFiles()` has completed.
    [[nodiscard]] bool IsInitialized() const noexcept { return isInitialized_; }
    /// @}

  private:
    //───────────────────────────────────────────────────────────── Data members
    bool        isInitialized_ {false};   ///< Set by `InitOutputFiles()`.

    std::string outDir_;      ///< Top-level directory (`results` by default).
    std::string subDir_;      ///< `results/YYYYMMDDTHHMMSS`
    std::string tsvPath_;     ///< `…/events.tsv`
    std::string jsonPath_;    ///< `…/run.json`

    std::ofstream tsv_;       ///< Kept open between Init… and Dump… (header→footer).

    /* If we later decide to let *workers* write per-event rows, protect tsv_
       with this mutex.  For Design B (no worker I/O) it stays unused.       */
    std::mutex tsvMutex_;
};

} // namespace util