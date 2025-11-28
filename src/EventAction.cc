/**
 * @file    EventAction.cc
 * @author  M. Zakeri (M.Zakeri@eku.edu)
 * @brief   Implementation of EventAction for muAlphaSim
 * @version 1.0
 * @date    2025-05-31
 */

// ======================================================================
//  Includes
// ======================================================================

#include "EventAction.hh"

#include "G4Event.hh"
#include "SteppingAction.hh"

// ======================================================================
//  EventAction implementation
// ======================================================================
// -----------------------------------------------------------------------------
EventAction::EventAction()
    : G4UserEventAction()
{
    // No special construction needed
}

// -----------------------------------------------------------------------------
EventAction::~EventAction()
{
    // No dynamic memory to free
}

// -----------------------------------------------------------------------------
void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    // Reset event-local flags
    SteppingAction::ResetEventFlags();
}

// -----------------------------------------------------------------------------
void EventAction::EndOfEventAction(const G4Event* event)
{
    // ------------------------------------------------------------------
    // Determine whether this event should be kept for visualization
    // ------------------------------------------------------------------
    const bool hadCapture    = SteppingAction::EventHadCapture();
    const bool hadIonization = SteppingAction::EventHadIonization();

    if (hadCapture || hadIonization)
    {
        const_cast<G4Event*>(event)->KeepTheEvent();
    }

    // Optional: you could also log which events were kept
    // G4cout << "[EventAction] Event " << event->GetEventID()
    //        << " kept? " << (nCaptures > 0 || nIonization > 0) << G4endl;
}
// -----------------------------------------------------------------------------