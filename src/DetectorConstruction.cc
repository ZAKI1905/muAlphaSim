// /**
//  * @file    DetectorConstruction.cc
//  * @brief   Concrete implementation of DetectorConstruction.
//  *
//  * Geometry constants are collected at the top for easy tweaking.
//  */

// #include "G4Box.hh"
// #include "G4Cons.hh"
// #include "G4Tubs.hh"
// #include "G4LogicalVolume.hh"
// #include "G4PVPlacement.hh"
// #include "G4SystemOfUnits.hh"
// #include "G4NistManager.hh"
// #include "G4VisAttributes.hh"
// #include "G4UserLimits.hh"

// #include "DetectorConstruction.hh"
// #include "GeometryParameter.hh"
// #include "RateTableSingleton.hh"

// /// ────────────────────────────────────────────────────────────────
// /// Geometry definition
// /// ────────────────────────────────────────────────────────────────
// G4VPhysicalVolume* DetectorConstruction::Construct()
// {
//     // --------------------------------------------------------------------
//     // 0. CONSTANTS  (expressed with Geant4 units)
//     // --------------------------------------------------------------------
//     GeometryParameter geo;                  //!< uses defaults
//     const G4double r_tip   = geo.rTip()   ;      //!< apex radius          (0.5 nm)
//     const G4double r_base  = geo.rBase()  ;      //!< base radius          (10 nm)
//     const G4double h_cone  = geo.hCone()  ;      //!< cone height          (1 µm)
//     const G4double gap     =  geo.gap()   ;     //!< tip-to-electrode gap (50 nm)
//     const G4double pitch_x = geo.pitchX() ;      //!< pitch in x-direction (150 nm)
//     const G4double pitch_y = geo.pitchY() ;      //!< pitch in y-direction (10=50 nm)
//     const G4int N_x = geo.nX() ;                   //!< number of copies in x-direction
//     const G4int N_y = geo.nY() ;                   //!< number of copies in y-direction
//     const G4double rMiddleOut  = geo.rMiddleOuter() ;       //< radius of the middle ring (60 nm)
//     const G4double rOuterOut   = geo.rOuterOuter()  ;       //< radius of the outer ring (75 nm)
//     // --------------------------------------------------------------------

//     // --------------------------------------------------------------------
//     // 1. MATERIALS
//     // --------------------------------------------------------------------
//     auto* nist   = G4NistManager::Instance();
//     auto* vacuum = nist->FindOrBuildMaterial("G4_Galactic");

//     // --------------------------------------------------------------------
//     // 2. WORLD  (simple 8 µm cube)
//     // --------------------------------------------------------------------
//     const G4double halfWorld = 4 * um;            //!< 8 µm full length
//     auto* solidWorld = new G4Box("WorldSolid",
//                                  halfWorld, halfWorld, halfWorld);

//     auto* logicWorld = new G4LogicalVolume(
//         solidWorld,         /* shape  */
//         vacuum,             /* material */
//         "WorldLV");         /* name     */

//     auto* physWorld = new G4PVPlacement(
//         nullptr,            /* no rotation                  */
//         {},                 /* at global origin             */
//         logicWorld,         /* LV                           */
//         "WorldPV",          /* name                         */
//         nullptr,            /* mother (null = top of world) */
//         false,              /* no boolean operation         */
//         0,                  /* copy number                  */
//         true);              /* check overlaps               */
//     // -----------------------------------------------------------
    

//     // // -----------------------------------------------------------
//     // //  3. Helper to build one shell and attach limits
//     // // -----------------------------------------------------------
//     // auto makeHollowShell = [&](const std::string& name,
//     //                     G4double x, G4double y,
//     //                     G4double rMin, G4double rMax,
//     //                     G4double stepMax)
//     // {
//     //     auto solid = new G4Tubs(name+"_Full",
//     //                             rMin, rMax,
//     //                             (h_cone+gap) / 2.0,  // half-length along z
//     //                             0.0, 360.0*CLHEP::deg);

//     //                                 // Cut out the cone
//     //     auto* hollow = new G4SubtractionSolid(name,
//     //                                         full, solidCone,
//     //                                         nullptr,              // same rotation
//     //                                         G4ThreeVector(0,0,0)); // same centre

//     //     auto logic = new G4LogicalVolume(solid,
//     //                     G4Material::GetMaterial("G4_Galactic"),
//     //                     name+"Logic");

//     //     auto limits = new G4UserLimits();
//     //     limits->SetMaxAllowedStep(stepMax);
//     //     logic->SetUserLimits(limits);

//     //     // (optional) tighter secondary production cuts
//     //     // logic->SetRegion(someRegionWithCuts);

//     //     new G4PVPlacement(nullptr,               // rot
//     //                         G4ThreeVector(x, y, (h_cone+gap) / 2.0),       // at (0,0,(h_cone+gap)/2)
//     //                         logic,
//     //                         name,
//     //                         logicWorld,
//     //                         false, 0);
        
//     //     auto cyanAttr = new G4VisAttributes(G4Colour(0, 0.8, 0.8, 0.3)) ; // cyan translucent
//     //     cyanAttr->SetForceSolid(true);
//     //     logic->SetVisAttributes(cyanAttr );  
        
//     //     return logic;
//     // };


//     // --------------------------------------------------------------------
//     // 4. CAPTURE CONE  (frustum, axis = +z)
//     //     Geant4’s G4Cons uses *half-length* → 0.5 h_cone.
//     //     The solid is centred on z = +0.5 h_cone, which places:
//     //       – its base disk at z = 0
//     //       – its tip       at z = +h_cone
//     // --------------------------------------------------------------------
//     auto* solidCone = new G4Cons(
//         "CaptureConeSolid",
//         /* rInner at -dz */ 0.0 * nm,
//         /* rOuter at -dz */ r_base,
//         /* rInner at +dz */ 0.0 * nm,
//         /* rOuter at +dz */ r_tip,
//         /* half-length   */ 0.5 * h_cone,
//         /* φ-segment     */ 0. * deg,
//                             360.* deg);

//     fConeLogical = new G4LogicalVolume(
//         solidCone,
//         vacuum,             /* same material (vacuum) */
//         "CaptureConeLV");

//     // --------------------------------------------------------------------
//     // 4a.  VISUAL ATTRIBUTES FOR THE CAPTURE CONE  ────────────────────
//     // --------------------------------------------------------------------
//     auto* coneVis = new G4VisAttributes(G4Colour::Red());   // bright red
//     coneVis->SetForceSolid(true);                           // filled solid
//     fConeLogical->SetVisAttributes(coneVis);                //
    
//     geo.setLogicWorld(logicWorld);  // store world LV for later use
//     geo.setSolidCone(solidCone);    // store the cone solid volume

//     // --------------------------------------------------------------------
//     // 4b.  PLACEMENT OF THE CAPTURE CONE
//     //     Place the cone copies in a grid pattern around the origin.
//     //     The copies are arranged in a grid with N_x copies in x-direction
//     //     and N_y copies in y-direction, with a pitch of pitch_x and pitch_y.
//     // --------------------------------------------------------------------
//     // Place the cone copies in a grid pattern
//     for (G4int i_x = 0; i_x < N_x; i_x++)
//     {
//         for (G4int i_y = 0; i_y < N_y; i_y++)
//         {
//             // Compute translation vector for each copy
//             G4double x_offset = (i_x - (N_x - 1) / 2.0) * pitch_x;
//             G4double y_offset = (i_y - (N_y - 1) / 2.0) * pitch_y;

//             // Place the cone copies
//             new G4PVPlacement(
//                 nullptr,                                /* no rotation          */
//                 G4ThreeVector(x_offset, y_offset, 0.5 * h_cone), /* translation          */
//                 fConeLogical,                           /* LV                   */
//                 "CaptureConePV_" + std::to_string(i_x) 
//                     + "_" + std::to_string(i_y)  ,      /* name                 */
//                 logicWorld,                             /* mother LV            */
//                 false, i_x * N_y + i_y, true);          /* copy-no, overlap chk */
            
//             // Add the center of this cone to the list of spike centers
//             // (the base center is at z = 0, so we use y_offset and x_offset)
//             G4ThreeVector baseCenter{ x_offset, y_offset, 0.0 };  
//             fSpikeCenters.push_back(baseCenter);

//             // build shells: they nest automatically because radii are monotonic
//             auto innerLogic  = geo.makeHollowShell("InnerShell", x_offset, y_offset, 0.0*nm, r_base,     1*nm);
//             auto midLogic    = geo.makeHollowShell("MidShell", x_offset, y_offset,  r_base, rMiddleOut, 3.0*nm);
//             auto outerLogic  = geo.makeHollowShell("OuterShell", x_offset, y_offset, rMiddleOut, rOuterOut, 5.0*nm);
//         }
//     }


//     // --------------------------------------------------------------------
//     // 5. BOUNDING REGION  (cylindrical volume around the cone)
//     //     This is a cylindrical volume that contains the entire cone.
//     //     It is used to define the region where the rate table applies.
//     // --------------------------------------------------------------------
//     // // Get bounding box
//     // double minRho, maxRho, minZ, maxZ;
//     // RateTable().GetBoundingBox(minRho, maxRho, minZ, maxZ);
    
//     // // G4cout << "[DEBUG] Rate table bounding box: "
//     // //        << "Rho [" << minRho << ", " << maxRho << "] m, "
//     // //        << "Z [" << minZ << ", " << maxZ << "] m" << G4endl;

//     // // Compute parameters
//     // double Rmax = maxRho;                      // in meters
//     // double halfLength = 0.5 * (maxZ - minZ);   // in meters
//     // double zCenter = 0.5 * (minZ + maxZ);      // in meters

//     // // Convert to GEANT4 units (mm)
//     // Rmax *= CLHEP::m;
//     // halfLength *= CLHEP::m;
//     // zCenter *= CLHEP::m;

//     // // Create cylindrical volume
//     // auto solidBoundingRegion = new G4Tubs("RateBoundingRegion",
//     //                                     0.0, Rmax,
//     //                                     halfLength,
//     //                                     0.0, 360.0 * CLHEP::deg);

//     // auto logicBoundingRegion = new G4LogicalVolume(
//     // solidBoundingRegion,
//     // G4Material::GetMaterial("G4_Galactic"),  // use vacuum or air
//     // "RateBoundingRegionLogical");

//     // new G4PVPlacement(
//     // nullptr,
//     // G4ThreeVector(0, 0, zCenter),
//     // logicBoundingRegion,
//     // "RateBoundingRegion",
//     // logicWorld,  // or wherever appropriate
//     // false, 0);

//     // auto userLimits = new G4UserLimits();
//     // userLimits->SetMaxAllowedStep(2e-6);  // set tight limit

//     // logicBoundingRegion->SetUserLimits(userLimits);

//     // logicBoundingRegion->SetVisAttributes(
//     // new G4VisAttributes(G4Colour(0.0, 0.8, 0.8)));  // cyan translucent

//     // --------------------------------------------------------------------
//     // 6. OPTIONAL CYLINDRICAL ELECTRODE (visual sanity check)
//     //     Outer surface that starts just above the tip.
//     // --------------------------------------------------------------------
//     // auto* solidElec = new G4Tubs(
//     //     "ElectrodeSolid",
//     //     0.0,                /* rMin                        */
//     //     r_elec,             /* rMax                        */
//     //     0.5*(h_cone+gap),   /* half-length                 */
//     //     0.*deg, 360.*deg);

//     // auto* logicElec = new G4LogicalVolume(
//     //     solidElec, vacuum, "ElectrodeLV");

//     // new G4PVPlacement(
//     //     nullptr,
//     //     /* place so its bottom face touches cone tip */
//     //     G4ThreeVector(0, 0, 0.5*(h_cone+gap)),
//     //     logicElec,
//     //     "ElectrodePV",
//     //     logicWorld,
//     //     false, 0, true);

//     // --------------------------------------------------------------------
//     // 6. VISUAL HELPER – a 20-nm axis cross at the origin              //
//     // --------------------------------------------------------------------
//     // auto* axes = new G4UserVisAction;          // helper object
//     // G4VisManager::GetInstance()->Register(axes); // GEANT4 will draw axes at run-time
//     // axes->AddAxes(G4ThreeVector(), 20*nm, "OriginAxes");     // NEW helper
    
//     return physWorld;
// }
/**
 * @file    DetectorConstruction.cc
 * @brief   Implementation of the high-level geometry driver.
 *
 *  Responsibilities
 *  ────────────────────────────────────────────────────────────────
 *    1.  Build an axis-aligned vacuum “world” box that comfortably
 *        encloses *all* cone panels requested in ::geom::GeometryConfig.
 *    2.  Delegate creation & placement of the nano-cone comb to
 *        ::ConeCombBuilder.
 *    3.  Cache pointers / spike centres so that physics actions can
 *        query them at run-time.
 *
 *  Hollow-shell voxelisation note
 *  ────────────────────────────────────────────────────────────────
 *    The thin middle/outer shells that are introduced to
 *    enforce a finer step-size near the cone surface are still
 *    produced – but their construction logic now lives inside
 *    `ConeCombBuilder::createConeLV()`.  Moving the code there keeps
 *    every volume that is "glued" to the master cone in one place,
 *    making future pitch / radius tweaks local to the builder class.
 *
 *    DetectorConstruction therefore remains agnostic: it just asks
 *    the builder for "one comb, please," and gets back the shared
 *    cone LV plus the placement of every physical copy (which
 *    includes the co-centred shells).
 *
 *
 * @author  Mohammadreza Zakeri
 * @date    2025-06-01
 */

#include "DetectorConstruction.hh"

// Geant4 geometry core
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

// C++ std
#include <algorithm>  // std::max_element
#include <numeric>    // std::accumulate

// -----------------------------------------------------------------------------
//  Constructor – make a deep copy of the data-only config
//  and create the builder helper.
// -----------------------------------------------------------------------------
DetectorConstruction::DetectorConstruction(const geom::GeometryConfig& cfg)
  : cfg_{cfg}
  , builder_{std::make_unique<ConeCombBuilder>(cfg_)}
{
  /* nothing else – heavy G4 objects created later in Construct() */
}

// -----------------------------------------------------------------------------
//  Construct – Geant4 entry point.  Builds world, then delegates to builder.
// -----------------------------------------------------------------------------
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // ────────────────────────────────────────────────────────────────
  // 1)  Compute world half-extents large enough to contain every panel.
  //     We assume the panels are all at positive z for simplicity.
  // ────────────────────────────────────────────────────────────────
  double maxX_nm = 0.0;
  double maxY_nm = 0.0;
  double maxZ_nm = 0.0;

  for (const auto& p : cfg_.panels)
  {
    const double halfSpanX = 0.5 * (p.nx - 1) * p.pitch_nm + cfg_.cone.r_base_nm;
    const double halfSpanY = 0.5 * (p.ny - 1) * p.pitch_nm + cfg_.cone.r_base_nm;

    maxX_nm = std::max(maxX_nm, std::abs(p.offset_nm.x_nm) + halfSpanX);
    maxY_nm = std::max(maxY_nm, std::abs(p.offset_nm.y_nm) + halfSpanY);
    maxZ_nm = std::max(maxZ_nm, cfg_.cone.h_cone_nm + cfg_.gap_nm);
  }

  // Pad by an additional cone height for safety
  const double xWorld = (maxX_nm + 2 * cfg_.cone.h_cone_nm) * nm;
  const double yWorld = (maxY_nm + 2 * cfg_.cone.h_cone_nm) * nm;
  const double zWorld = (maxZ_nm +  cfg_.cone.h_cone_nm) * nm;

  auto* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

  auto* solidWorld = new G4Box("WorldSolid",
                               xWorld,
                               yWorld,
                               zWorld);

  fWorldLogical_ = new G4LogicalVolume(solidWorld,
                                       vacuum,
                                       "WorldLogical");

  auto* physWorld = new G4PVPlacement(nullptr,
                                      G4ThreeVector(),
                                      fWorldLogical_,
                                      "World",
                                      nullptr,
                                      false,
                                      0,
                                      false);

  // ────────────────────────────────────────────────────────────────
  // 2)  Ask the builder to place every panel into the world.
  // ────────────────────────────────────────────────────────────────
  builder_->Build(fWorldLogical_);

  // Cache shared cone LV & base list for later retrieval
  fConeLogical_  = builder_->ConeLogical();
  fInShellLogical_ = builder_->InShellLogical();
  fMidShellLogical_ = builder_->MidShellLogical();
  fOutShellLogical_ = builder_->OutShellLogical();
  fConesInfo_ = builder_->GetConesInfo();

#ifdef VERBOSE_GEOM
  G4cout << "[DetectorConstruction] geometry built with "
         << fSpikeCenters_.size() << " spike centres\n";
#endif

  return physWorld;
}