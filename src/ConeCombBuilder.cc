/**
 * @file    ConeCombBuilder.cc
 * @brief   Implementation of the nano-cone comb geometry builder.
 *
 * The class translates a pure-data ::geom::GeometryConfig into actual
 * Geant4 volumes.  All heavy G4 headers live here, not in the header, so
 * that user code can include <ConeCombBuilder.hh> without dragging the
 * entire toolkit.
 *
 *  ────────────────────────────────────────────────────────────────────
 *  Dependencies
 *  ────────────────────────────────────────────────────────────────────
 *    • <G4…> headers – geometry primitives, materials, placement.
 *    • <GeometryConfig.hh> – data-only structs.
 *    • <iomanip>/<sstream> – debug-print helpers.
 *  ────────────────────────────────────────────────────────────────────
 *
 * @author  Mohammadreza Zakeri
 * @date    2025-06-01
 */

#include "ConeCombBuilder.hh"

// Geant4 geometry & units
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"

// Geant4 materials
#include "G4NistManager.hh"

// C++ standard
#include <iomanip>
#include <sstream>

using namespace geom;   // shorten type names locally

// -----------------------------------------------------------------------------
//  Ctor – store a read-only reference to the geometry config.
// -----------------------------------------------------------------------------
ConeCombBuilder::ConeCombBuilder(const GeometryConfig& cfg)
  : cfg_{cfg}
{
  /* nothing else – heavy objects are created lazily */
}

// -----------------------------------------------------------------------------
//  Build – public entry point called by DetectorConstruction.
// -----------------------------------------------------------------------------
void ConeCombBuilder::Build(G4LogicalVolume* mother)
{
  // Ensure the master cone LV exists before we start placing panels.
  createConeAndShellLVs();

  // Loop over every panel description and place it.
  std::size_t copyOffset = 0;
  for (std::size_t i = 0; i < cfg_.panels.size(); ++i)
  {
    copyOffset += placePanel(cfg_.panels[i],
                             static_cast<int>(i),
                             mother);
  }

  // Optional: print a summary for sanity during development.
#ifdef VERBOSE_GEOM
  std::ostringstream info;
  info << "[ConeCombBuilder] placed " << copyOffset
       << " cones in "       << cfg_.panels.size()
       << " panels\n";
  G4cout << info.str();
#endif
}

// =============================================================================
// createConeAndShellLVs  –  builds cone + two hollow shells once
// =============================================================================
void ConeCombBuilder::createConeAndShellLVs()
{
	if (logicCone_ != nullptr)
	{
		return;   // already initialised
	}

	const auto nm = CLHEP::nm;

	// ---------- radii & heights -------------------------------------------------
	const double rTip    = cfg_.cone.r_tip_nm    * nm;
	const double rBase   = cfg_.cone.r_base_nm   * nm;
	const double hCone   = cfg_.cone.h_cone_nm   * nm;
	const double gap     = cfg_.gap_nm          * nm;

	const double rMiddle = cfg_.r_middle_nm      * nm;
	const double rOuter  = cfg_.r_outer_nm       * nm;

	// ---------- materials -------------------------------------------------------
	auto* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

	// ---------- cone solid / LV -------------------------------------------------
	solidCone_ = new G4Cons("ConeSolid",
							0.0,         rBase,
							0.0,         rTip,
							0.5 * hCone,
							0.0, 360.0 * deg);

	logicCone_ = new G4LogicalVolume(solidCone_, vacuum, "ConeLogical");

	// Visualisation attributes for the cone
	// Note: this is a solid cone, not a hollow one.
	// The cone is bright red, so it stands out in the scene.
	// It is filled solid, not wireframe.
	auto* coneVis = new G4VisAttributes(G4Colour::Red());   // bright red
    coneVis->SetForceSolid(true);                           // filled solid
    logicCone_->SetVisAttributes(coneVis);                  // apply to cone LV

	// ---------- inner shell (hollow) ------------------------------------------
	auto* solidInShellFull_ = new G4Tubs("InShellSolid_full",
								0.0,
								rBase,
								0.5 * (hCone+2*gap),
								0.0, 360.0 * deg);
	// Cut out the cone
	solidInShell_ = new G4SubtractionSolid("InShellSolid",
										solidInShellFull_, solidCone_,
										nullptr,              // same rotation
										G4ThreeVector(0,0,0)); // same centre

	logicInShell_ = new G4LogicalVolume(solidInShell_, vacuum,
										"InShellLogical");

	//  Enforce finer step near the surface
	// const double inStep = rBase / 10.0;
	const double inStep = 1*nm ;
	logicInShell_->SetUserLimits(new G4UserLimits(inStep));

	// auto limits = new G4UserLimits();
	// limits->SetMaxAllowedStep(inStep);
	// logicInShell_->SetUserLimits(limits);

	// ---------- middle shell (hollow) ------------------------------------------
	solidMidShell_ = new G4Tubs("MidShellSolid",
								rBase,
								rMiddle,
								0.5 * (hCone+2*gap),
								0.0, 360.0 * deg);

	logicMidShell_ = new G4LogicalVolume(solidMidShell_, vacuum,
										"MidShellLogical");

	//  Enforce finer step near the surface
	// const double midStep = (rMiddle - rBase) / 10.0;
	const double midStep = 3*nm ;
	logicMidShell_->SetUserLimits(new G4UserLimits(midStep));

	// ---------- outer shell (hollow) -------------------------------------------
	solidOutShell_ = new G4Tubs("OutShellSolid",
								rMiddle,
								rOuter,
								0.5 * (hCone+2*gap),
								0.0, 360.0 * deg);

	logicOutShell_ = new G4LogicalVolume(solidOutShell_, vacuum,
										"OutShellLogical");

	// const double outStep = (rOuter - rMiddle) / 10.0;
	const double outStep = 5*nm ;

	logicOutShell_->SetUserLimits(new G4UserLimits(outStep));


	auto cyanAttr = new G4VisAttributes(G4Colour(0, 0.9, 0.9, 0.2)) ; // cyan translucent
	cyanAttr->SetForceSolid(true);
	logicOutShell_->SetVisAttributes(cyanAttr );  
	logicMidShell_->SetVisAttributes(cyanAttr );
	logicInShell_->SetVisAttributes(cyanAttr );
}

// =============================================================================
// placePanel – now drops FOUR physical volumes per cone (cone + three shells)
// =============================================================================
std::size_t ConeCombBuilder::placePanel(const PanelSpec& ps,
                                        int              panelIdx,
                                        G4LogicalVolume* mother)
{
	const auto nm = CLHEP::nm;

	// The unit of the following variables is millimeter.
	const double pitch = ps.pitch_nm * nm;
	const double x0    = ps.x0_nm    * nm;
	const double hCone = cfg_.cone.h_cone_nm * nm;

	const double y0 = ps.offset_nm.y_nm * nm;
	const double z0 = ps.offset_nm.z_nm * nm;

	std::size_t physCount = 0;

	for (int ix = 0; ix < ps.nx; ++ix)
	{
		for (int iy = 0; iy < ps.ny; ++iy)
		{
			const double x = x0 + (ix - 0.5 * (ps.nx - 1)) * pitch;
			const double y = y0 + (iy - 0.5 * (ps.ny - 1)) * pitch;
			const double z = z0 + 0.5*hCone;

			const int baseCopy = panelIdx * ps.nx * ps.ny + ix * ps.ny + iy;

#define VERBOSE_GEOM = 1
			// Debug print for cone placement
#ifdef VERBOSE_GEOM
			std::ostringstream oss;
			oss << "[ConeCombBuilder] (" << ix <<  ", " << iy << "), placing cone at ("
				<< std::fixed << std::setprecision(3)
				<< x / nm << " nm, "
				<< y / nm << " nm, "
				<< z / nm << " nm) in panel "
				<< panelIdx << ", copy #" << baseCopy;
			G4cout << oss.str() << G4endl;
#endif

			// -- cone itself --------------------------------------------------------
			new G4PVPlacement(nullptr,
							G4ThreeVector(x, y, z),
							logicCone_,
							"cone" + std::to_string(ix)+ "_" + std::to_string(iy),
							mother,
							false,
							baseCopy,
							false);
			
			// Convert the distances from mm to meters for the ConeInfo
			// and store the cone information.
			gConesInfo.emplace_back( geom::ConeInfo{ {1e-3*x, 1e-3*y, 1e-3*z0}, panelIdx, ix, iy } );

			// -- inner shell (hollow) ----------------------------------------------
			new G4PVPlacement(nullptr,
							G4ThreeVector(x, y, z),
							logicInShell_,
							"inShell",
							mother,
							false,
							baseCopy,
							false);

			// -- middle shell -------------------------------------------------------
			new G4PVPlacement(nullptr,
							G4ThreeVector(x, y, z),
							logicMidShell_,
							"midShell",
							mother,
							false,
							baseCopy,
							false);

			// -- outer shell --------------------------------------------------------
			new G4PVPlacement(nullptr,
							G4ThreeVector(x, y, z),
							logicOutShell_,
							"outShell",
							mother,
							false,
							baseCopy,
							false);

			// Base [z=0] of the cone (needed for ionization tables)
			// spikeCenters_.emplace_back(x, y, z0);

			++physCount;
		}
	}

	return physCount;
}
// =============================================================================