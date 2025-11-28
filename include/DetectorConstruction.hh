// /**
//  * @file    DetectorConstruction.hh
//  * @brief   Builds the world volume, the hollow capture cone, and an
//  *          (optional) surrounding cylindrical electrode for mu-α stripping.
//  *
//  * @author  Mohammadreza Zakeri
//  * @date    2025-05-26
//  *
//  * The class also exposes a pointer to the cone’s logical volume so that the
//  * SteppingAction can recognise when a mu-α track enters the cone.
//  */

// #ifndef DETECTOR_CONSTRUCTION_HH
// #define DETECTOR_CONSTRUCTION_HH

// #include "G4VUserDetectorConstruction.hh"
// #include "G4ThreeVector.hh"

// /// Forward declaration of Geant4 types
// class G4LogicalVolume;
// class G4VPhysicalVolume;

// /**
//  * @class DetectorConstruction
//  * @brief Implements Geant4 world and detector geometry.
//  *
//  * Layout (all dimensions in nanometres unless otherwise noted):
//  * ```
//  *   ┌────────────── World (vacuum, 5 µm box) ──────────────┐
//  *   │                                                      │
//  *   │   z ↑                                                │
//  *   │     │                                                │
//  *   │     │   tip                                          │
//  *   │     │   ● (0,0,h_cone)                               │
//  *   │     │  /|\                                           │
//  *   │     │   |   Capture cone (solid vacuum frustum)      │
//  *   │     │   |                                            │
//  *   │     │   |  base    radius r_base (10 nm)             │
//  *   │     └───┼────────── (0,0,0)  global origin           │
//  *   │                                                      │
//  *   │         (optional) cylindrical electrode             │
//  *   └──────────────────────────────────────────────────────┘
//  * ```
//  */
// class DetectorConstruction : public G4VUserDetectorConstruction
// {
//   public:
//     DetectorConstruction()  = default;          ///< default ctor
//     ~DetectorConstruction() override = default; ///< trivial dtor

//     /// Builds geometry; Geant4 calls it once at initialisation.
//     G4VPhysicalVolume* Construct() override;

//     /// Getter so SteppingAction can check “am I inside the cone?”
//     G4LogicalVolume* GetConeLogical() const { return fConeLogical; }

//     /// @brief Adds the center of a spike to the list of spike centers.
//     /// @param ctr Center of the spike to be added.
//     void   AddSpikeCenter(const G4ThreeVector& ctr) { fSpikeCenters.emplace_back(ctr); }
    
//     /// @brief Returns the list of spike centers.
//     /// @return A constant reference to the vector of spike centers.
//     /// This is used to access the spike centers for ionization calculations.
//     const std::vector<G4ThreeVector>& GetSpikeCenters() const { return fSpikeCenters; }

//   private:
//     G4LogicalVolume* fConeLogical = nullptr;    ///< capture-cone LV (cached)

//     /// @brief List of spike centers for the mu-α capture cone.
//     std::vector<G4ThreeVector> fSpikeCenters;
// };

// #endif // DETECTOR_CONSTRUCTION_HH

/**
 * @file    DetectorConstruction.hh
 * @brief   Builds the Geant4 world volume and instantiates the modular
 *          nano-cone panels that strip μHe⁺ atoms.
 *
 * The class wraps three distinct responsibilities:
 *   1) World creation – a vacuum box large enough to contain
 *      every panel specified by a ::geom::GeometryConfig.
 *   2) Delegation to ConeCombBuilder – converts pure-data panel
 *      specs into actual `G4LogicalVolume` instances and placements.
 *   3) Book-keeping hooks – exposes pointers and spike-centre
 *      coordinates so that physics actions (SteppingAction, etc.)
 *      can query “am I inside a cone?” or look up the nearest field
 *      table entry.
 *
 *
 * @author  Mohammadreza Zakeri
 * @date    2025-06-01
 */

#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

// Geant4 core
#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"

// standard C++
#include <memory>
#include <vector>

// project headers (pure data + builder logic)
#include "GeometryConfig.hh"
#include "ConeCombBuilder.hh"

/// Forward declarations to avoid heavy Geant4 headers in the header file
class G4LogicalVolume;
class G4VPhysicalVolume;

/**
 * @class DetectorConstruction
 * @brief High-level driver that translates a GeometryConfig into
 *        concrete Geant4 volumes.
 *
 * Usage pattern (simplified):
 * @code
 *   geom::GeometryConfig cfg = loadFromJson("geom.json");
 *   auto* det = new DetectorConstruction(cfg);
 *   runManager->SetUserInitialization(det);
 * @endcode
 *
 * The constructor makes a deep copy of the GeometryConfig so that
 * the user can discard the original after passing it in.
 */
class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    /**
     * @brief Constructor.
     * @param cfg  Pure-data description of the nano-comb geometry.
     */
    explicit DetectorConstruction(const geom::GeometryConfig& cfg);

    // -------------------------------------------------------------------------
    /// Trivial virtual destructor – nothing to clean up manually
    ~DetectorConstruction() override = default;

    // -------------------------------------------------------------------------
    /**
     * @brief Mandatory Geant4 entry point – builds the geometry tree.
     * @return Pointer to the world’s physical volume.
     *
     * Internally this method:
     *   1. Creates the world logical & physical volumes.
     *   2. Instantiates ::ConeCombBuilder with `cfg_`.
     *   3. Delegates placement of every cone panel to the builder.
     */
    G4VPhysicalVolume* Construct() override;

    // -------------------------------------------------------------------------
    /**
     * @brief Pointer to the master cone logical volume.
     *
     * Every cone panel re-uses one cone LV; SteppingAction can
     * therefore test `preStep->GetTouchable()->GetVolume()->GetLogicalVolume()`
     * against this pointer to decide whether the current step is
     * “inside a cone”.
     */
    G4LogicalVolume* GetConeLogical() const
    {
      return fConeLogical_;
    }

    /**
     * @brief Access to the inner, middle, and outer shell logical volumes.
     *
     * These are used to enforce a finer step size near the cone surface.
     * The SteppingAction can use these pointers to check if a step is
     * inside any of the shells.
     */
    G4LogicalVolume* GetInShellLogical() const
    {
      return fInShellLogical_;
    }

    /**
     * @brief Access to the middle shell logical volume.
     *
     * The middle shell is used to enforce a finer step size near the
     * cone surface. The SteppingAction can use this pointer to check if
     * a step is inside the middle shell.
     */
    G4LogicalVolume* GetMidShellLogical() const
    {
      return fMidShellLogical_;
    }

    /**
     * @brief Access to the outer shell logical volume.
     *
     * The outer shell is used to enforce a finer step size near the
     * cone surface. The SteppingAction can use this pointer to check if
     * a step is inside the outer shell.
     */
    G4LogicalVolume* GetOutShellLogical() const
    {
      return fOutShellLogical_;
    }
    
    // -------------------------------------------------------------------------
    /**
     * @brief Access to cones information.
     *
     * Filled by ConeCombBuilder during placement.  Provides the
     * (x,y,z) list required by the ionization-probability lookup.
     */
    const std::vector<geom::ConeInfo>& GetConesInfo() const
    {
      return fConesInfo_;
    }

    /**
     * @brief Get the Geometry Config object
     * 
     * @return geom::GeometryConfig 
     */
    geom::GeometryConfig GetGeometryConfig() const
    {
      return cfg_;
    }

  private:
    // -------------------------------------------------------------------------
    /// Pure-data copy of the geometry description
    const geom::GeometryConfig cfg_;

    // -------------------------------------------------------------------------
    /// Helper that creates cone solids / LV and places every panel
    std::unique_ptr<ConeCombBuilder> builder_;

    // -------------------------------------------------------------------------
    /// Cached pointer to the world logical volume
    G4LogicalVolume* fWorldLogical_{nullptr};

    // -------------------------------------------------------------------------
    /// Cached pointer to the shared cone logical volume
    G4LogicalVolume* fConeLogical_{nullptr};
    G4LogicalVolume* fInShellLogical_{nullptr};  ///< The inner shell LV
    G4LogicalVolume* fMidShellLogical_{nullptr}; ///< The middle shell LV
    G4LogicalVolume* fOutShellLogical_{nullptr}; ///< The outer shell LV

    // -------------------------------------------------------------------------
    /// List of cone information (one entry per physical cone)
    std::vector<geom::ConeInfo>  fConesInfo_;

    
};

#endif  // DETECTOR_CONSTRUCTION_HH