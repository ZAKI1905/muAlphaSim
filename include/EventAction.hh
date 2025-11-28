/**
 * @file    EventAction.hh
 * @author  M. Zakeri (M.Zakeri@eku.edu)
 * @brief   EventAction class for muAlphaSim project
 * @version 1.0
 * @date    2025-05-31
 *
 * @copyright Copyright (c) 2025
 *
 * EventAction decides at the end of each event whether the event
 * should be kept for visualization. This allows us to only display
 * interesting events such as muon capture or ionization.
 */

#ifndef EVENTACTION_HH
#define EVENTACTION_HH

// ======================================================================
//  Includes
// ======================================================================

#include "G4UserEventAction.hh"
#include "globals.hh"

// ======================================================================
//  EventAction class declaration
// ======================================================================

/**
 * @class EventAction
 * @brief End-of-event action for controlling visualization
 *
 * This class examines counters (from SteppingAction) at the end of
 * each event. If the event contains one or more captures or ionizations,
 * it marks the event to be kept. Otherwise, the event will be skipped
 * in visualization if /vis/drawOnlyToBeKeptEvents is true.
 */
class EventAction : public G4UserEventAction
{
public:

    /**
     * @brief Constructor
     */
    EventAction();

    /**
     * @brief Destructor
     */
    virtual ~EventAction();

    /**
     * @brief End-of-event hook
     * @param event Pointer to the current G4Event
     *
     * Called once per event after stepping is complete.
     * Marks interesting events for visualization.
     */
    virtual void EndOfEventAction(const G4Event* event) override;

	virtual void BeginOfEventAction(const G4Event*) override ;

};  // class EventAction

#endif  // EVENTACTION_HH