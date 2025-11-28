/**
 * @file    ConeCombBuilder.hh
 * @brief   Converts a ::geom::GeometryConfig into Geant4 volumes,
 * 			including the middle/outer hollow shells used for step control.
 *
 * This helper class owns all Geant-specific code required to
 *   > build the master cone solid / logical volume,
 *   > place one or more rectangular “panels” of cones,
 *   > collect spike-base coordinates for ionisation look-ups,
 *   > expose a pointer to the shared cone LV so that physics
 *     actions can identify cone crossings with a simple pointer test.
 *
 * The header keeps Geant4 dependencies local – DetectorConstruction
 * only sees the forward declarations, not the heavy includes.
 *
 *
 * @author  Mohammadreza Zakeri
 * @date    2025-06-01
 */

#ifndef CONE_COMB_BUILDER_HH
#define CONE_COMB_BUILDER_HH

// ───────────────────────────────────────────────────────────────────────────
//  Geant4 forward declarations  – avoids pulling <G4…> into every TU.
// ───────────────────────────────────────────────────────────────────────────
class G4LogicalVolume;
// class G4RotationMatrix;
class G4VSolid;
class G4VPhysicalVolume;
class G4UserLimits;
// class G4ThreeVector;

// ───────────────────────────────────────────────────────────────────────────
//  Geant4 *minimal* includes we actually need in the header
// ───────────────────────────────────────────────────────────────────────────
// #include "G4ThreeVector.hh"      // required: used by SpikeCenters()
#include "G4RotationMatrix.hh"   // required: unique_ptr owns the object

// ───────────────────────────────────────────────────────────────────────────
//  Standard C++
// ───────────────────────────────────────────────────────────────────────────
#include <cstddef>
#include <memory>
#include <vector>

// ───────────────────────────────────────────────────────────────────────────
//  Project headers (pure data only – no G4 types)
// ───────────────────────────────────────────────────────────────────────────
#include "GeometryConfig.hh"

namespace geom    
{ 
  struct GeometryConfig; 

 /**
  * @brief Holds the cone information including the base centre and panel index.
  * 
  */
  struct ConeInfo 
  {
    G4ThreeVector baseCentre;   // (x0,y0,0) in metres
    int panelIdx, ix, iy;
  };

}

/**
 * @class ConeCombBuilder
 * @brief Utility that owns the "geometry construction" details of the nano-comb.
 *
 * Lifetime expectations:
 *   > Created by DetectorConstruction after it has parsed a GeometryConfig.  
 *   > Destroyed automatically when the run manager cleans up user initialisers.  
 *   > Holds raw pointers to Geant4 logical volumes (Geant4 manages their
 *     deletion; builder does not delete them).
 */
class ConeCombBuilder
{
  public:
    /**
     * @brief Constructor – stores a const reference to the geometry data.
     * @param cfg Pure-data geometry description.
     */
    explicit ConeCombBuilder(const geom::GeometryConfig& cfg);

    // -----------------------------------------------------------------------
    /** @brief Builds cone LV and places every panel into `mother`. */
    void Build(G4LogicalVolume* mother);

    // -----------------------------------------------------------------------
    /** @return Pointer to the shared cone logical volume. */
    G4LogicalVolume* ConeLogical() const
    {
      return logicCone_;
    }

    // -----------------------------------------------------------------------
    /** @return Pointer to the inner shell logical volume. */
    G4LogicalVolume* InShellLogical() const
    {
      return logicInShell_;
    }

    // -----------------------------------------------------------------------
    /** @return Pointer to the middle shell logical volume. */
    G4LogicalVolume* MidShellLogical() const
    {
      return logicMidShell_;
    }

    // -----------------------------------------------------------------------
    /** @return Pointer to the outer shell logical volume. */
    G4LogicalVolume* OutShellLogical() const
    {
      return logicOutShell_;
    }

    // -----------------------------------------------------------------------
    /** @return Reference to the ConeInfo list (populated by Build). */
    const std::vector<geom::ConeInfo>& GetConesInfo() const
    {
      return gConesInfo;
    }

  private:
    // -----------------------------------------------------------------------
    /// Helper that lazily creates `logicCone_` on first call.
    void createConeAndShellLVs();

    // -----------------------------------------------------------------------
    /** Places one rectangular panel; returns number of cones instantiated. */
    std::size_t placePanel(const geom::PanelSpec& spec,
                           int                   panelIndex,
                           G4LogicalVolume*      mother);

    // ──────────────────────────────────────────────────────────────────────
    //  Data members
    // ──────────────────────────────────────────────────────────────────────
    const geom::GeometryConfig& cfg_;          		///< immutable geometry spec
    G4LogicalVolume* 	logicCone_{nullptr}; 		///< shared cone LV
    G4LogicalVolume* 	logicInShell_{nullptr};	 	///< The inner shell LV
    G4LogicalVolume* 	logicMidShell_{nullptr};	///< The middle shell LV
    G4LogicalVolume* 	logicOutShell_{nullptr};	///< The outer shell LV

    G4VSolid*        solidCone_{nullptr}; 		 ///< shared cone solid
    G4VSolid* 		 solidInShell_{nullptr};	 ///< The inner shell solid
    G4VSolid*        solidMidShell_{nullptr};	 ///< The middle shell solid
    G4VSolid*        solidOutShell_{nullptr};	 ///< The outer shell solid

    std::unique_ptr<G4RotationMatrix> rotY_;   ///< optional rotation holder
    // std::vector<G4ThreeVector> spikeCenters_;  ///< base positions (z=0) (mm)
    std::vector<geom::ConeInfo> gConesInfo;    // global, size = total #cones
    
};

#endif  // CONE_COMB_BUILDER_HH