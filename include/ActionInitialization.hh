/**
 * @file    ActionInitialization.hh
 * @brief   Configure thread-local user actions for an MT run.
 *
 *  * One *shared* util::DataLogger is created in the ctor (master thread).  
 *  * The master RunAction uses it to write JSON/TSV.  
 *  * Worker RunActions merely hold the pointer (they never call it).
 */

#ifndef ACTION_INITIALIZATION_HH
#define ACTION_INITIALIZATION_HH

/*─────────────────────────── Geant4 core ─────────────────────────────*/
#include "G4VUserActionInitialization.hh"

/*──────────────────────────── std / proj ─────────────────────────────*/
#include "GeometryConfig.hh"

namespace util { class DataLogger; }
class DetectorConstruction;

/*====================================================================*/
class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(DetectorConstruction*       det,
                         const geom::GeometryConfig& cfg);
    ~ActionInitialization() override;

    /* master vs. worker hooks */
    void BuildForMaster() const override;
    void Build()          const override;

  private:
    const DetectorConstruction* fDet_;   ///< geometry handle
    const geom::GeometryConfig cfg_;     ///< deep copy for threads
    util::DataLogger*          logger_;  ///< ONE instance, owned here
};

#endif /* ACTION_INITIALIZATION_HH */