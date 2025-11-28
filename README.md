# muAlphaSim – Muon-Alpha State Propagation and Stripping

`muAlphaSim` is a Geant4-based simulation for propagating muonic helium (`μHe⁺`, “mu–alpha”) states through a structured target and estimating **field-induced ionization / stripping** and **capture** probabilities.

The current implementation focuses on a **nanocone / nanocomb–like geometry** and a DC-field configuration, scanning over panel layouts to quantify how often bound μ–α states are ionized or captured as they traverse the structured region.

> ⚠️ **Status:** research code, under active development. Interfaces and file formats may change.

---

## Features

- C++17 / Geant4 simulation with:

  - configurable nanocone-panel geometry via JSON (`geometry.json`) or built-in defaults,
  - multithreaded event processing via `G4MTRunManager`,
  - custom geometry builder (`ConeCombBuilder`),
  - custom physics list including a μ–α step limiter and ionization bookkeeping,
  - ROOT-based histogramming of ionization / capture events.

- Python tools to:

  - generate / sweep geometries (`grid_sweep/`, `sweep_results/`),
  - analyze ionization and capture statistics,
  - post-process event TSVs and ROOT files into figures.

- A small Python toolkit for **muonic bound states** (`muonic_atom/`) with
  Dirac hydrogenic solvers used to derive input parameters for the Geant4 model.

---

## Repository Layout

```text
CMakeLists.txt          # Build config for Geant4
src/                    # C++ sources (Geant4 application)
include/                # C++ headers (Detector, Physics, Geometry, ...)
muonic_atom/            # Muonic atom tools (Dirac solver)
grid_sweep/             # Geometry parameter scans
sweep_results/          # Summaries from smaller sweeps
output/                 # ROOT files, TSVs, plots (ignored by .gitignore)
results/                # Run summaries
vis.mac                 # Visualization macro
```

Key C++ components:

- `src/main.cc` – entry point, CLI, geometry loading, interactive/batch modes
- `DetectorConstruction` – world volume and nanocone panels
- `PhysicsList` – EM physics + μ–α step-limiter
- User actions (`ActionInitialization`, `RunAction`, `EventAction`, `SteppingAction`)
- `GeometryConfig` – JSON-serializable geometry description (panel layouts)

Python:

- `grid_sweep.py` – orchestrates large parameter sweeps
- `grid_sweep/analyze_grid.py` – aggregates run results into CSVs and plots
- `ionization_histograms.py` – histogram utilities
- `muonic_atom/*` – Dirac solvers + radial distribution tools

---

## Dependencies

### C++ / Geant4

- C++17 compiler (Clang or GCC)
- Geant4 (multithreading enabled; visualization optional)
- CMake ≥ 3.10
- `nlohmann::json` (bundled via `include/nlohmann/json.hpp`)

### Python

For the analysis scripts:

```
numpy
matplotlib
pandas
scipy (optional)
```

---

## Building

From the repository root:

```bash
mkdir -p build
cd build

cmake -DGeant4_DIR=/path/to/geant4/lib/Geant4-XX.YY.ZZ ..
cmake --build . -- -j$(nproc)
```

If your Geant4 install provides `geant4.sh`, source it first:

```bash
source /path/to/geant4-install/bin/geant4.sh
```

---

## Running

### Interactive mode:

```bash
./main
```

This:

- uses built-in geometry,
- launches a UI session,
- loads `vis.mac`,
- displays panels + μ–α trajectories.

### Batch mode:

```bash
./main --nevents=100000
```

Using an explicit geometry file:

```bash
./main --cfg=geometry.json --nevents=500000
```

Outputs are stored in:

- `output/root/*.root`
- `output/events/*.tsv`
- `results/YYYYMMDDThhmmss/` (run summaries)

---

## Parameter Sweeps

`grid_sweep/` holds large scans over panel configurations.

Typical workflow:

```bash
python grid_sweep.py           # run all configurations
python grid_sweep/analyze_grid.py
```

This generates:

- ionization fractions
- capture fractions
- ranked configuration tables
- plots under `grid_sweep/`

---

## Muonic Atom Tools

Located in `muonic_atom/`, these scripts include:

- Dirac hydrogenic solver for μHe⁺,
- bound-state distributions,
- diagnostic plots of radial probability.

Used to validate and provide parameters for the Geant4 model.

---

## Figures

A representative screenshot of the μ–α simulation is shown below:

![Muon-alpha trajectories through nanocone panels](docs/figures/nanocone_hit_pattern.png)

Example output summary from a 1000-event run:

    ============ Global Event Summary ============
         Number of events in this run : 1000
         Total Captures               : 9
         Total Ionizations            : 930
         Ionization Fraction (%)      : 93  ± 0.806846
         Capture    Fraction (%)      : 0.9  ± 0.298647
         Ionization / Capture Ratio   : 103.333
        ==============================================


---

## Reproducibility Notes

- Add a custom seed in `vis.mac` (if desired):
  ```
  /random/setSeeds N1 N2
  ```
- JSON geometries, event TSVs, and ROOT files are typically not tracked in Git due to size.
- Results can be regenerated using the sweep and analysis scripts.

---

## License and Publication Use

This code is provided for research transparency, but **it is not licensed for
reuse in scientific publications, preprints, theses, or commercial work without
explicit written permission from the author**.

To request permission for any publication-related use, please contact:

**Mohammadreza Zakeri (Zaki)**  
**Email:** m.zakeri@eku.edu

Unauthorized scientific use, including derivative work used in papers or reports,
is strictly prohibited.

A formal publication describing the methods is planned; a citation entry will be
provided once it becomes available.

---

## Contact

**Author:** Mohammadreza Zakeri (Zaki)  
**Email:** m.zakeri@eku.edu

Issues and contributions welcome.
