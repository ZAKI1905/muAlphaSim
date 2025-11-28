// ============================================================================
//  Project : muAlphaSim – Muon-Alpha State Propagation and Stripping
//  File    : main.cc
//  Purpose : Entry point; loads GeometryConfig (JSON or builtin),
//            instantiates detector / physics / actions and runs either
//            interactively (vis.mac) or in batch (/run/beamOn N).
//
//  Build-time deps  : nlohmann/json (header-only, bundled with Geant4 examples)
//  Run-time  flags  : --cfg=<geometry.json>   (optional)
//                     --nevents=<N>           (default 100)
//
//  Author  : Mohammadreza Zakeri (Zaki) — m.zakeri@eku.edu
//  Updated : 2025-06-01
// ============================================================================

#include <fstream>
#include <string>
#include <thread>

#include "DetectorConstruction.hh"
#include "G4MTRunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "GeometryConfig.hh"
#include "QGSP_BERT.hh"
// #include "GeometryConfigJSON.hh"        // <- JSON  struct helpers
#include "ActionInitialization.hh"
#include "PhysicsList.hh"

// ────────────────────────────────────────────────────────────────
//  Default (hard-wired) geometry – handy for “no-JSON” mode.
// ────────────────────────────────────────────────────────────────
static geom::GeometryConfig makeDefaultConfig() {
  using namespace geom;
  GeometryConfig cfg;

  PanelSpec p0;
  p0.nx = 2;
  p0.ny = 6;
  p0.pitch_nm = 150.0;
  p0.x0_nm = -1200;
  p0.offset_nm = {0.0, 0.0, -150};
  cfg.panels.push_back(p0);

  PanelSpec p1;
  p1.nx = 2;
  p1.ny = 6;
  p1.pitch_nm = 150.0;
  p1.x0_nm = -800;
  p1.offset_nm = {0.0, 0.0, -100};
  cfg.panels.push_back(p1);

  PanelSpec p2;
  p2.nx = 2;
  p2.ny = 6;
  p2.pitch_nm = 150.0;
  p2.x0_nm = -400;
  p2.offset_nm = {0.0, 0.0, -50};
  cfg.panels.push_back(p2);

  PanelSpec p3;
  p3.nx = 2;
  p3.ny = 6;
  p3.pitch_nm = 150.0;
  p3.x0_nm = 0.0;
  p3.offset_nm = {0.0, 0.0, 0};
  cfg.panels.push_back(p3);

  PanelSpec p4;
  p4.nx = 2;
  p4.ny = 6;
  p4.pitch_nm = 150.0;
  p4.x0_nm = 400.0;
  p4.offset_nm = {0.0, 0.0, 50};
  cfg.panels.push_back(p4);

  PanelSpec p5;
  p5.nx = 2;
  p5.ny = 6;
  p5.pitch_nm = 150.0;
  p5.x0_nm = 800.0;
  p5.offset_nm = {0.0, 0.0, 100};
  cfg.panels.push_back(p5);

  PanelSpec p6;
  p6.nx = 2;
  p6.ny = 6;
  p6.pitch_nm = 150.0;
  p6.x0_nm = 1200.0;
  p6.offset_nm = {0.0, 0.0, 150};
  cfg.panels.push_back(p6);

  return cfg;
}

// ────────────────────────────────────────────────────────────────
//  Ultra-light CLI parser for --cfg and --nevents
// ────────────────────────────────────────────────────────────────
struct Cli {
  std::string cfgPath;
  int nEvents = 100;
};

static Cli parse_cli(int argc, char** argv) {
  Cli out;
  for (int i = 1; i < argc; ++i) {
    std::string a(argv[i]);
    if (a.rfind("--cfg=", 0) == 0)
      out.cfgPath = a.substr(6);
    else if (a.rfind("--nevents=", 0) == 0)
      out.nEvents = std::stoi(a.substr(10));
  }
  return out;
}

// ────────────────────────────────────────────────────────────────
//  main()
// ────────────────────────────────────────────────────────────────
int main(int argc, char** argv) {
  // ------------ CLI --------------------------------------------------------
  Cli cli = parse_cli(argc, argv);

  const bool interactive = (argc == 1);
  auto* ui = interactive ? new G4UIExecutive(argc, argv) : nullptr;

  // ------------ Geometry ---------------------------------------------------
  geom::GeometryConfig cfg = makeDefaultConfig();
  if (!cli.cfgPath.empty()) {
    std::ifstream fp(cli.cfgPath);
    if (!fp)
      G4Exception("main", "BadCfg", FatalException,
                  ("Cannot open geometry file " + cli.cfgPath).c_str());

    nlohmann::json j;
    fp >> j;
    j.get_to(cfg);

    G4cout << "Loaded geometry from " << cli.cfgPath << G4endl;
  } else {
    G4cout << "Using built-in default geometry\n";
  }

  // ------------ Run manager & threading ------------------------------------
  auto* runManager = new G4MTRunManager;

  const unsigned hw = std::thread::hardware_concurrency();
  const unsigned nThreads = std::min<unsigned>(8, hw ? hw : 1);
  runManager->SetNumberOfThreads(nThreads);
  G4cout << "Running on " << nThreads << " threads (" << hw
         << " hardware threads available)\n";

  // ------------ Detector, physics, user actions ---------------------------
  auto* det = new DetectorConstruction(cfg);
  runManager->SetUserInitialization(det);
  runManager->SetUserInitialization(new PhysicsList);
  runManager->SetUserInitialization(new ActionInitialization(det, cfg));

  runManager->Initialize();

  // ------------ Visualisation ---------------------------------------------
  auto* visManager = new G4VisExecutive;
  visManager->Initialize();

  auto* UImanager = G4UImanager::GetUIpointer();

  // ------------ Interactive vs batch --------------------------------------
  if (interactive) {
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  } else {
    std::ostringstream cmd;
    cmd << "/run/beamOn " << cli.nEvents;
    UImanager->ApplyCommand(cmd.str());
  }

  // ------------ Cleanup ----------------------------------------------------
  delete visManager;
  delete runManager;
  return 0;
}