#ifndef EVENT_HH
#define EVENT_HH

// Include necessary Geant4 headers for event actions and analysis
#include "G4UserEventAction.hh" // Base class for user event actions
#include "G4Event.hh" // Class representing an event in Geant4
#include "G4RunManager.hh"
#include "G4ToolsAnalysisManager.hh" // Class for managing analysis of simulation data
#include "G4UImanager.hh" // Class for user interface management
#include "G4SystemOfUnits.hh" // System of units for Geant4
#include "G4PhysicsOrderedFreeVector.hh" // Class for ordered free vector of physics processes

// Include custom headers for run and construction
#include "RunAction.hh" // Custom header for run-related actions
#include "DetectorConstruction.hh" // Custom header for construction of the simulation world
#include <string.h> // Functions for manipulating C-style strings
#include "G4Args.hh" // Custom header for argument handling

// Declare the MyEventAction class, inheriting from G4UserEventAction
// This class is used to define custom actions to be taken at the beginning and end of each event in a Geant4 simulation.
class EventAction : public G4UserEventAction
{
public:
    // Constructor takes pointers to MyRunAction and MyG4Args, presumably for configuration and passing arguments
    EventAction(RunAction*, MyG4Args*);
    // Destructor
    ~EventAction();

    // Virtual methods to define actions at the beginning and end of each event
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);


private:
   
    MyG4Args* PassArgs; // Pointer to MyG4Args for passing arguments
};

#endif
