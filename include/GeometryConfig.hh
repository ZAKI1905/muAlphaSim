/**
 * @file    GeometryConfig.hh
 * @author  Mohammadreza Zakeri
 * @date    2025-06-04
 *
 * @brief   Pure-data description of the nano-comb geometry (no Geant4).
 *
 * ────────────────────────────────────────────────────────────────────────────
 *  Why keep this header Geant4-free?
 *  ────────────────────────────────────────────────────────────────────────────
 *  * Unit tests, JSON/YAML parsers, CLI tools, Jupyter notebooks, … can all
 *    manipulate the geometry without dragging in a heavy Geant4 tool-chain.
 *  * DetectorConstruction only reads this struct; any field added here
 *    automatically propagates everywhere with zero changes elsewhere.
 *
 * ────────────────────────────────────────────────────────────────────────────
 *  Units convention
 *  ────────────────────────────────────────────────────────────────────────────
 *  * All distances are stored in nanometres  [nm].
 *  * The builder (or DetectorConstruction) is responsible for converting to
 *    Geant4 internal units (`CLHEP::nm`, `CLHEP::mm`, …).
 *
 * ────────────────────────────────────────────────────────────────────────────
 *  Helper functions (NEW in the “accumulable” redesign)
 *  ────────────────────────────────────────────────────────────────────────────
 *  To avoid casting `DetectorConstruction` just to discover simple counts,
 *  we now provide lightweight helpers that:
 *
 *  * return the total number of cones,  
 *  * return the total number of panels,  
 *  * map a flattened cone index → global center position (nm).
 *
 *  These helpers are constexpr / header-only and add zero runtime overhead.
 */

#ifndef GEOMETRY_CONFIG_HH
#define GEOMETRY_CONFIG_HH

/*────────────────────────────── stdlib ───────────────────────────────────*/
#include <cstddef>          // std::size_t
#include <ostream>
#include <vector>

/*──────────────────────────── third-party ───────────────────────────────*/
#include <nlohmann/json.hpp> // only for (de)serialisers – header-only

namespace geom {

/*======================================================================*/
/* 1.  Lightweight math helpers – no external deps                      */
/*======================================================================*/

/**
 * @struct Vec3
 * @brief Minimal 3-vector (nm).  _No operators_ – keep it POD-simple.
 */
struct Vec3 {
    double x_nm {0.0};   ///< X-coordinate [nm]
    double y_nm {0.0};   ///< Y-coordinate [nm]
    double z_nm {0.0};   ///< Z-coordinate [nm]
};

/*======================================================================*/
/* 2.  Dimensional specs                                                */
/*======================================================================*/

/**
 * @struct ConeSpec
 * @brief Dimensions of a single nano-cone.
 */
struct ConeSpec {
    double r_tip_nm   { 0.5 };   ///< Apex (tip) radius [nm]
    double r_base_nm  {10.0 };   ///< Base radius         [nm]
    double h_cone_nm  {1000.0};  ///< Cone height         [nm]
};

/**
 * @struct PanelSpec
 * @brief Rectangular grid of cones.
 *
 * The panel is rigid; the builder will replicate one `ConeSpec`
 * on an `nx × ny` lattice with `pitch_nm` spacing.
 *
 * The optional `offset_nm` allows a whole-panel transverse shift so that
 * multiple panels can be slightly staggered.
 */
struct PanelSpec {
    int    nx {5};               ///< Cones along local +X
    int    ny {5};               ///< Cones along local +Y
    double pitch_nm {150.0};     ///< center-to-center pitch [nm]
    double x0_nm    {0.0};       ///< Global X position of the panel’s *first* cone [nm]
    Vec3   offset_nm{};          ///< Extra global (x,y,z) shift [nm]

    /** @return total number of cones in this panel (`nx × ny`). */
    constexpr std::size_t nCones() const noexcept { return nx * ny; }
};

/*======================================================================*/
/* 3.  Top-level geometry container                                     */
/*======================================================================*/

/**
 * @struct GeometryConfig
 * @brief Everything the builder needs, nothing more, nothing less.
 */
struct GeometryConfig
{
    /*──── Cone + shells ─────────────────────────────────────────────*/
    ConeSpec cone;                 ///< Master cone dimensions (all cones identical)
    double   gap_nm      { 50.0 }; ///< Tip-to-electrode gap [nm]
    double   r_middle_nm { 60.0 }; ///< Middle shell outer radius [nm]
    double   r_outer_nm  { 75.0 }; ///< Outer  shell outer radius [nm]

    /*──── Panels (ordered) ─────────────────────────────────────────*/
    std::vector<PanelSpec> panels; ///< Panels 0…N-1 in *insertion* order

    /*------------------------------------------------------------------*/
    /** @name  Tiny convenience helpers (constexpr / header-only)        */
    /** @{ */
    /** @return number of panels in the comb. */
    constexpr std::size_t nPanels() const noexcept { return panels.size(); }

    /** @return total number of cones (sum over every panel). */
    constexpr std::size_t nCones()  const noexcept
    {
        std::size_t total = 0;
        for (const auto& p : panels) total += p.nCones();
        return total;
    }

    /**
     * @brief  Compute the *global* center of cone idx (flattened index).
     *
     * The flattening order is: panel0 row0…row(ny-1) then panel1 … etc.
     *
     * @param idx 0-based flattened cone index (must be `< nCones()`).
     * @return    center position in nanometres.  *z* is the panel’s `offset_nm.z_nm`.
     */
    Vec3 ConeGlobalCenter(std::size_t idx) const noexcept
    {
        Vec3 c;
        std::size_t running = 0;
        for (const auto& p : panels)
        {
            const std::size_t pn = p.nCones();
            if (idx < running + pn)
            {
                /* Cone lies in this panel.  Compute local (ix,iy). */
                const std::size_t local = idx - running;
                const std::size_t ix = local % p.nx;
                const std::size_t iy = local / p.nx;

                c.x_nm = p.x0_nm + p.offset_nm.x_nm;          // panel shift + x0
                c.y_nm = (iy - (p.ny-1)/2.0) * p.pitch_nm     // center the grid
                       + p.offset_nm.y_nm;
                c.z_nm = p.offset_nm.z_nm;                    // typically 0
                return c;
            }
            running += pn;
        }
        return c;  // idx out of range → returns (0,0,0) – caller should guard
    }
    /** @} */
};

/*======================================================================*/
/* 4.  Stream operators (human-readable dump & JSON ↔ struct)           */
/*======================================================================*/

/**
 * @brief Pretty-print entire `GeometryConfig` (nm) to an ostream.
 *
 * Intended for debugging and for the `geometry` block inside `run.json`.
 */
std::ostream& operator<<(std::ostream& os, const GeometryConfig& cfg);

/**
 * @brief Parse from a human-readable text block (mainly unit-test helper).
 */
std::istream& operator>>(std::istream& is, GeometryConfig& cfg);

/*──────────── nlohmann_json (de)serialisers – one-liner versions ───────*/
void to_json(nlohmann::json& j, const Vec3& v);
void from_json(const nlohmann::json& j, Vec3& v);

void to_json(nlohmann::json& j, const ConeSpec& c);
void from_json(const nlohmann::json& j, ConeSpec& c);

void to_json(nlohmann::json& j, const PanelSpec& p);
void from_json(const nlohmann::json& j, PanelSpec& p);

void to_json(nlohmann::json& j, const GeometryConfig& g);
void from_json(const nlohmann::json& j, GeometryConfig& g);

} // namespace geom
#endif /* GEOMETRY_CONFIG_HH */