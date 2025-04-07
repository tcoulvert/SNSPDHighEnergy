#ifndef DETECTOR_HH
#define DETECTOR_HH

// Include necessary Geant4 headers for sensitive detectors and analysis
#include "G4VSensitiveDetector.hh" // Base class for sensitive detectors in Geant4
#include "G4RunManager.hh" // Class for managing the simulation run
#include "G4SystemOfUnits.hh" // System of units for Geant4
#include "G4PhysicsOrderedFreeVector.hh" // Class for ordered free vector of physics processes
#include "G4GenericMessenger.hh" // Class for handling command-line arguments
#include "G4Step.hh" // Class for storing step information
#include "G4TouchableHistory.hh" // Class for storing touchable history
#include "G4ParticleDefinition.hh" // For accessing particle types
#include "G4Track.hh" // Class for track information
#include "G4ThreeVector.hh" // Class for 3D vector operations
#include <unordered_map> // For storing energy deposited by each particle type
#include "G4Args.hh"

// Declare the MySensitiveDetector class, inheriting from G4VSensitiveDetector
class SensitiveDetector : public G4VSensitiveDetector
{
public:
    // Constructor takes a G4String for the detector name
    SensitiveDetector(G4String name, MyG4Args*);
    // Destructor
    ~SensitiveDetector();
    
  

    
private:
    // ProcessHits method is called for each step in the detector
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);

    MyG4Args* PassArgs;

};

#endif
