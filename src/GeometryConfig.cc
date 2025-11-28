// // // ----------------------------------------------------------------------------
// // // GeometryParameter.cc

// // #include "GeometryParameter.hh"
// // #include "G4Cons.hh"
// // #include "G4Tubs.hh"
// // #include "G4SubtractionSolid.hh"
// // #include "G4LogicalVolume.hh"
// // #include "G4Material.hh"
// // #include "G4SystemOfUnits.hh"
// // #include "G4UserLimits.hh"
// // #include "G4VisAttributes.hh"
// // #include "G4NistManager.hh"
// // #include "G4ThreeVector.hh"
// // #include "G4RotationMatrix.hh"
// // #include "G4PVPlacement.hh"

// // ////////////////////////////////////////////////////////////////////////////////
// // // Default constructor
// // ////////////////////////////////////////////////////////////////////////////////
// // GeometryParameter::GeometryParameter()
// //     : fRtip       (0.5  * nm)
// //     , fRbase      (10.0 * nm)
// //     , fHcone      (1000 * nm)
// //     , fGap        (50.0  * nm)
// //     , fPitchX     (150   * nm)
// //     , fPitchY     (150   * nm)
// //     , fNx         (15)  			// Number of cones in X direction
// //     , fNy         (5)   		// Number of cones in Y direction
// //     , fRMiddleOut (60    * nm)
// //     , fROuterOut  (75    * nm)
// // {
// // }

// // // -----------------------------------------------------------------------------
// // /**
// //  * @brief Parameterized constructor for custom geometry.
// //  *
// //  * @param r_tip Apex radius of the cone tip.
// //  * @param r_base Base radius of the cone.
// //  * @param h_cone Height of the cone.
// //  * @param gap Tip-to-electrode gap distance.
// //  * @param pitch_x Grid pitch in X direction.
// //  * @param pitch_y Grid pitch in Y direction.
// //  * @param N_x Number of cones in X direction.
// //  * @param N_y Number of cones in Y direction.
// //  * @param rMiddleOut Outer radius of the middle shell.
// //  * @param rOuterOut Outer radius of the outer shell.
// //  */
// // GeometryParameter::GeometryParameter(
// //     G4double r_tip,
// //     G4double r_base,
// //     G4double h_cone,
// //     G4double gap,
// //     G4double pitch_x,
// //     G4double pitch_y,
// //     G4int    N_x,
// //     G4int    N_y,
// //     G4double rMiddleOut,
// //     G4double rOuterOut
// // )
// //     : fRtip       (r_tip)
// //     , fRbase      (r_base)
// //     , fHcone      (h_cone)
// //     , fGap        (gap)
// //     , fPitchX     (pitch_x)
// //     , fPitchY     (pitch_y)
// //     , fNx         (N_x)
// //     , fNy         (N_y)
// //     , fRMiddleOut (rMiddleOut)
// //     , fROuterOut  (rOuterOut)
// // {
// // }

// // // -----------------------------------------------------------------------------
// // G4double GeometryParameter::rTip() const
// // {
// //     return fRtip;
// // }

// // // -----------------------------------------------------------------------------
// // G4double GeometryParameter::rBase() const
// // {
// //     return fRbase;
// // }

// // // -----------------------------------------------------------------------------
// // G4double GeometryParameter::hCone() const
// // {
// //     return fHcone;
// // }

// // // -----------------------------------------------------------------------------
// // G4double GeometryParameter::gap() const
// // {
// //     return fGap;
// // }

// // // -----------------------------------------------------------------------------
// // G4double GeometryParameter::pitchX() const
// // {
// //     return fPitchX;
// // }

// // // -----------------------------------------------------------------------------
// // G4double GeometryParameter::pitchY() const
// // {
// //     return fPitchY;
// // }

// // // -----------------------------------------------------------------------------
// // G4int GeometryParameter::nX() const
// // {
// //     return fNx;
// // }

// // // -----------------------------------------------------------------------------
// // G4int GeometryParameter::nY() const
// // {
// //     return fNy;
// // }

// // // -----------------------------------------------------------------------------
// // G4double GeometryParameter::rMiddleOuter() const
// // {
// //     return fRMiddleOut;
// // }

// // // -----------------------------------------------------------------------------
// // G4double GeometryParameter::rOuterOuter() const
// // {
// //     return fROuterOut;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setRTip(G4double v)
// // {
// //     fRtip = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setRBase(G4double v)
// // {
// //     fRbase = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setHCone(G4double v)
// // {
// //     fHcone = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setGap(G4double v)
// // {
// //     fGap = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setPitchX(G4double v)
// // {
// //     fPitchX = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setPitchY(G4double v)
// // {
// //     fPitchY = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setNX(G4int v)
// // {
// //     fNx = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setNY(G4int v)
// // {
// //     fNy = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setRMiddleOut(G4double v)
// // {
// //     fRMiddleOut = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setROuterOut(G4double v)
// // {
// //     fROuterOut = v;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setLogicWorld(G4LogicalVolume* LogicWorld)
// // {
// //     fLogicWorld = LogicWorld;
// // }

// // // -----------------------------------------------------------------------------
// // void GeometryParameter::setSolidCone(G4Cons* SolidCone) 
// // {
// // 	fSolidCone = SolidCone;
// // }

// // // -----------------------------------------------------------------------------
// // // Helper to build one shell and attach limits
// // // --------------------------------------------------------
// // G4LogicalVolume* GeometryParameter::makeHollowShell(const std::string& name,
// //                         G4double x, G4double y,
// //                         G4double rMin, G4double rMax,
// //                         G4double stepMax)
// // {
// // 	auto full = new G4Tubs(name+"_Full",
// // 							rMin, rMax,
// // 							(fHcone+fGap) / 2.0,  // half-length along z
// // 							0.0, 360.0*CLHEP::deg);

// // 	// Cut out the cone
// // 	auto* hollow = new G4SubtractionSolid(name,
// // 										full, fSolidCone,
// // 										nullptr,              // same rotation
// // 										G4ThreeVector(0,0,0)); // same center

// // 	auto logic = new G4LogicalVolume(hollow,
// // 					G4Material::GetMaterial("G4_Galactic"),
// // 					name+"Logic");

// // 	auto limits = new G4UserLimits();
// // 	limits->SetMaxAllowedStep(stepMax);
// // 	logic->SetUserLimits(limits);

// // 	// (optional) tighter secondary production cuts
// // 	// logic->SetRegion(someRegionWithCuts);

// // 	new G4PVPlacement(nullptr,               // rot
// // 						G4ThreeVector(x, y, (fHcone+fGap) / 2.0),       // at (0,0,(fHcone+gap)/2)
// // 						logic,
// // 						name,
// // 						fLogicWorld,
// // 						false, 0);
	
// // 	auto cyanAttr = new G4VisAttributes(G4Colour(0, 0.8, 0.8, 0.1)) ; // cyan translucent
// // 	cyanAttr->SetForceSolid(true);
// // 	logic->SetVisAttributes(cyanAttr );  
	
// // 	return logic;
// // };

// // // -----------------------------------------------------------------------------
// // // helper that places an n_X × n_Y rectangular array of cones
// // void GeometryParameter::PlaceConePanel(G4LogicalVolume* mother,
// // 					G4LogicalVolume* logicCone,
// //                     int n_X, int n_Y,
// //                     G4double pitch,
// //                     G4double z0,
// //                     const G4ThreeVector& offsetXY = G4ThreeVector(),
// //                     const G4RotationMatrix* rot)
// // {
// //     for (int i_X=0; i_X<n_X; ++i_X) 
// // 	{
// //         for (int i_Y=0; i_Y<n_Y; ++i_Y) 
// // 		{
// //             G4double x = offsetXY.x() + (i_X - 0.5*(n_X-1))*pitch;
// //             G4double y = offsetXY.y() + (i_Y - 0.5*(n_Y-1))*pitch;
// //             G4double z = z0;
// //             new G4PVPlacement(const_cast<G4RotationMatrix*>(rot),
// //                               G4ThreeVector(x,y,z),
// //                               logicCone,      // assume you defined it earlier
// //                               "cone"+ std::to_string(i_X) 
// //                                 + "_" + std::to_string(i_Y) ,
// //                               mother,
// //                               false,
// //                               /*copyNo=*/ i_X*n_Y + i_Y,
// //                               /*checkOverlaps=*/ false);
// //         }
// //     }
// // }

// // // -----------------------------------------------------------------------------
// /**
//  * @file GeometryConfig.cc
//  * @brief Helper utilities for the pure-data geometry description.
//  *
//  * The header <GeometryConfig.hh> contains only POD structs, so a
//  * separate translation unit is not strictly required to compile a
//  * program.  Nevertheless, putting convenience functions here keeps
//  * DetectorConstruction uncluttered and centralises debug / I/O logic
//  * that may evolve.
//  *
//  *  Dependencies: 
//  *    - Standard C++ only – no Geant4 or CLHEP headers are included
//  *      so that unit tests can link this file without the full toolkit.
//  *
//  *
//  *  Content: 
//  *    1.  operator<< overload for easy printing of GeometryConfig  
//  *    2.  Convenience factory that appends a regular panel grid
//  */

// #include "GeometryConfig.hh"
// #include "GeometryConfigJSON.hh"  

// #include <iomanip>   // std::setw
// #include <iostream>  // std::ostream
// #include <sstream>   // std::ostringstream

// #include <nlohmann/json.hpp>

// namespace geom
// {

// // -----------------------------------------------------------------------------
// //  Pretty-printer – dumps the whole configuration in a human-readable format.
// // -----------------------------------------------------------------------------
// std::ostream& operator<<(std::ostream& os,
//                          const GeometryConfig& cfg)
// {
//   os << "GeometryConfig {\n"
//      << "  ConeSpec {\n"
//      << "    r_tip   = " << cfg.cone.r_tip_nm   << " nm\n"
//      << "    r_base  = " << cfg.cone.r_base_nm  << " nm\n"
//      << "    h_cone  = " << cfg.cone.h_cone_nm  << " nm\n"
//      << "  }\n"
//      << "  gap        = " << cfg.gap_nm        << " nm\n"
//      << "  r_middle   = " << cfg.r_middle_nm   << " nm\n"
//      << "  r_outer    = " << cfg.r_outer_nm    << " nm\n"
//      << "  panels     = " << cfg.panels.size() << '\n';

//   for (std::size_t i = 0; i < cfg.panels.size(); ++i)
//   {
//     const auto& p = cfg.panels[i];
//     os << "    Panel[" << i << "] {\n"
//        << "      nx       = " << p.nx          << '\n'
//        << "      ny       = " << p.ny          << '\n'
//        << "      pitch    = " << p.pitch_nm    << " nm\n"
//        << "      x0       = " << p.x0_nm       << " nm\n"
//        << "      offset   = (" << p.offset_nm.x_nm
//                                << ", " << p.offset_nm.y_nm
//                                << ", " << p.offset_nm.z_nm << ") nm\n"
//        << "    }\n";
//   }
//   os << "}\n";
//   return os;
// }

// // -----------------------------------------------------------------------------
// std::istream& operator>>(std::istream& is, GeometryConfig& cfg)
// {
//     nlohmann::json j;
//     is >> j;                  // read whole file/stream
//     j.get_to(cfg);            // relies on from_json()
//     return is;
// }

// // -----------------------------------------------------------------------------
// //  Convenience factory – adds a rectangular panel series in a for-loop.
// //  This keeps DetectorConstruction macros concise.
// //
// //  @param cfg        Mutable reference – panels are pushed_back.
// //  @param nPanels    Number of panels to create.
// //  @param firstX_nm  x of the first panel base.
// //  @param dx_nm      Spacing between consecutive panels along +X.
// //  @param pitch_nm   Square lattice pitch (identical in X and Y).
// //  @param nx, ny     Grid dimensions inside each panel.
// // -----------------------------------------------------------------------------
// void appendRegularPanels(GeometryConfig& cfg,
//                          int             nPanels,
//                          double          firstX_nm,
//                          double          dx_nm,
//                          double          pitch_nm,
//                          int             nx,
//                          int             ny)
// {
//   for (int k = 0; k < nPanels; ++k)
//   {
//     PanelSpec p;
//     p.nx        = nx;
//     p.ny        = ny;
//     p.pitch_nm  = pitch_nm;
//     p.x0_nm     = firstX_nm + k * dx_nm;
//     cfg.panels.push_back(p);
//   }
// }

// } // namespace geom
/**
 * @file    GeometryConfig.cc
 * @author  Mohammadreza Zakeri
 * @date    2025-06-04
 *
 * @brief   Helper utilities for the *header-only* POD geometry structs.
 *
 * ────────────────────────────────────────────────────────────────────────────
 *  Why does this file exist?
 *  ────────────────────────────────────────────────────────────────────────────
 *  * The **structs and small helpers** live entirely in `GeometryConfig.hh`
 *    to stay header-only and Geant4-free.
 *  * This TU collects the *heavier* conveniences that would otherwise pull
 *    extra headers everywhere:  
 *      • pretty-printer (`operator<<`)  
 *      • JSON (de)serialisers (nlohmann::json)  
 *      • a “factory” that appends regular panels for quick prototyping.
 *
 *  No Geant4 or CLHEP includes appear below – unit tests can link this file
 *  without a full Geant4 build.
 */

#include "GeometryConfig.hh"
// #include "GeometryConfigJSON.hh"      // keep JSON helpers in one place

/*────────────────────────────── stdlib ───────────────────────────────────*/
#include <iomanip>
#include <iostream>
#include <sstream>

/*────────────────────────── 3rd-party (header-only) ──────────────────────*/
#include <nlohmann/json.hpp>

namespace geom {

/*══════════════════════════════════════════════════════════════════════════*/
/* 1.  Human-readable pretty-printer                                        */
/*══════════════════════════════════════════════════════════════════════════*/
std::ostream& operator<<(std::ostream& os, const GeometryConfig& cfg)
{
    os << "GeometryConfig {\n"
       << "  ConeSpec {\n"
       << "    r_tip   = " << cfg.cone.r_tip_nm   << " nm\n"
       << "    r_base  = " << cfg.cone.r_base_nm  << " nm\n"
       << "    h_cone  = " << cfg.cone.h_cone_nm  << " nm\n"
       << "  }\n"
       << "  gap_nm      = " << cfg.gap_nm      << " nm\n"
       << "  r_middle_nm = " << cfg.r_middle_nm << " nm\n"
       << "  r_outer_nm  = " << cfg.r_outer_nm  << " nm\n"
       << "  panels      = " << cfg.nPanels()
       << "  (total cones = " << cfg.nCones() << ")\n";

    for (std::size_t i = 0; i < cfg.panels.size(); ++i)
    {
        const PanelSpec& p = cfg.panels[i];
        os << "    Panel[" << i << "] {\n"
           << "      nx,ny      = " << p.nx << ", " << p.ny << '\n'
           << "      pitch_nm   = " << p.pitch_nm   << '\n'
           << "      x0_nm      = " << p.x0_nm      << '\n'
           << "      offset_nm  = (" << p.offset_nm.x_nm << ", "
                                      << p.offset_nm.y_nm << ", "
                                      << p.offset_nm.z_nm << ")\n"
           << "    }\n";
    }
    os << "}\n";
    return os;
}

/*══════════════════════════════════════════════════════════════════════════*/
/* 2.  operator>>  – read JSON from any istream                             */
/*══════════════════════════════════════════════════════════════════════════*/
std::istream& operator>>(std::istream& is, GeometryConfig& cfg)
{
    nlohmann::json j;
    is >> j;           // read entire stream
    j.get_to(cfg);     // relies on from_json overloads below
    return is;
}

/*══════════════════════════════════════════════════════════════════════════*/
/* 3.  nlohmann::json (de)serialisers                                       */
/*══════════════════════════════════════════════════════════════════════════*/
using nlohmann::json;

/*── Vec3 ────────────────────────────────────────────────────────────────*/
void to_json(json& j, const Vec3& v)
{
    j = json{{"x_nm", v.x_nm}, {"y_nm", v.y_nm}, {"z_nm", v.z_nm}};
}
void from_json(const json& j, Vec3& v)
{
    j.at("x_nm").get_to(v.x_nm);
    j.at("y_nm").get_to(v.y_nm);
    j.at("z_nm").get_to(v.z_nm);
}

/*── ConeSpec ────────────────────────────────────────────────────────────*/
void to_json(json& j, const ConeSpec& c)
{
    j = json{{"r_tip_nm",  c.r_tip_nm},
             {"r_base_nm", c.r_base_nm},
             {"h_cone_nm", c.h_cone_nm}};
}
void from_json(const json& j, ConeSpec& c)
{
    j.at("r_tip_nm"). get_to(c.r_tip_nm);
    j.at("r_base_nm").get_to(c.r_base_nm);
    j.at("h_cone_nm"). get_to(c.h_cone_nm);
}

/*── PanelSpec ───────────────────────────────────────────────────────────*/
void to_json(json& j, const PanelSpec& p)
{
    j = json{
        {"nx",        p.nx},
        {"ny",        p.ny},
        {"pitch_nm",  p.pitch_nm},
        {"x0_nm",     p.x0_nm},
        {"offset_nm", p.offset_nm}
    };
}
void from_json(const json& j, PanelSpec& p)
{
    j.at("nx").       get_to(p.nx);
    j.at("ny").       get_to(p.ny);
    j.at("pitch_nm"). get_to(p.pitch_nm);
    j.at("x0_nm").    get_to(p.x0_nm);
    j.at("offset_nm").get_to(p.offset_nm);
}

/*── GeometryConfig ──────────────────────────────────────────────────────*/
void to_json(json& j, const GeometryConfig& g)
{
    j = json{
        {"cone",        g.cone},
        {"gap_nm",      g.gap_nm},
        {"r_middle_nm", g.r_middle_nm},
        {"r_outer_nm",  g.r_outer_nm},
        {"panels",      g.panels}
    };
}
void from_json(const json& j, GeometryConfig& g)
{
    j.at("cone").       get_to(g.cone);
    j.at("gap_nm").     get_to(g.gap_nm);
    j.at("r_middle_nm").get_to(g.r_middle_nm);
    j.at("r_outer_nm"). get_to(g.r_outer_nm);
    j.at("panels").     get_to(g.panels);
}

/*══════════════════════════════════════════════════════════════════════════*/
/* 4.  Convenience panel factory (unchanged)                                */
/*══════════════════════════════════════════════════════════════════════════*/
void appendRegularPanels(GeometryConfig& cfg,
                         int             nPanels,
                         double          firstX_nm,
                         double          dx_nm,
                         double          pitch_nm,
                         int             nx,
                         int             ny)
{
    for (int k = 0; k < nPanels; ++k)
    {
        PanelSpec p;
        p.nx        = nx;
        p.ny        = ny;
        p.pitch_nm  = pitch_nm;
        p.x0_nm     = firstX_nm + k * dx_nm;
        cfg.panels.push_back(p);
    }
}

} // namespace geom