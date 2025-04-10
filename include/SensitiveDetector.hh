#ifndef DETECTOR_HH
#define DETECTOR_HH

// Include necessary Geant4 headers for sensitive detectors and analysis
#include "G4CMPElectrodeSensitivity.hh"
#include "G4Args.hh"

// Declare the MySensitiveDetector class, inheriting from G4VSensitiveDetector
class SensitiveDetector final : public G4CMPElectrodeSensitivity
{
public:
    // Constructor takes a G4String for the detector name
    SensitiveDetector(G4String name, MyG4Args*);
    // Destructor
    ~SensitiveDetector();
    
protected:
    virtual G4bool IsHit(const G4Step*, const G4TouchableHistory*) const;
    
private:
    // ProcessHits method is called for each step in the detector
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);

    MyG4Args* PassArgs;
    std::ofstream primaryOutput;
    std::ofstream hitOutput;

};

#endif
