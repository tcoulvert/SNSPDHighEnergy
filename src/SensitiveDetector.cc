#include "SensitiveDetector.hh"

#include "G4RunManager.hh" // Class for managing the simulation run
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "ConfigManager.hh"

#include "G4SystemOfUnits.hh" // System of units for Geant4
#include "G4PhysicsOrderedFreeVector.hh" // Class for ordered free vector of physics processes

#include "G4Event.hh"
#include "G4Track.hh" // Class for track information
#include "G4Step.hh" // Class for storing step information

#include "G4GenericMessenger.hh" // Class for handling command-line arguments
#include "G4TouchableHistory.hh" // Class for storing touchable history
#include "G4ParticleDefinition.hh" // For accessing particle types
#include "G4ThreeVector.hh" // Class for 3D vector operations
#include <unordered_map> // For storing energy deposited by each particle type

#include "G4CMPElectrodeHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4PhononLong.hh"
#include "G4PhononTransFast.hh"
#include "G4PhononTransSlow.hh"
#include "G4Proton.hh"


SensitiveDetector::SensitiveDetector(G4String name, MyG4Args* MainArgs): G4CMPElectrodeSensitivity(name)
{
    PassArgs = MainArgs;
    G4cout << "### Sensitive detector '" << name << "' is being created!" << G4endl;

}

SensitiveDetector::~SensitiveDetector()
{
}

G4bool SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
	

    // Accessing step information
    G4double edep = aStep->GetTotalEnergyDeposit();
    
    // Only process hits where energy deposition is greater than 0
    if (edep > 0) {
        
        // Get position of the hit
        G4ThreeVector position = aStep->GetPostStepPoint()->GetPosition();
        
        // Get the time of the hit (time at the post step point)
        G4double time = aStep->GetPostStepPoint()->GetGlobalTime();
        
        // Get particle type (the track associated with the step)
        G4Track *track = aStep->GetTrack();
        G4ParticleDefinition *particle = track->GetDefinition();
        G4String particleType = particle->GetParticleName();
        
        // Store the hit data
		PassArgs->AddHitRecord(edep, position, time, particleType);

		// Get the event number
        G4int eventNumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
        
		PassArgs->AddToEnergyByParticleAndEvent(particleType, edep, eventNumber);
		PassArgs->AddCurrentEvtEdep(edep);
		
    }

    
    return true;
}

G4bool SensitiveDetector::IsHit(const G4Step* step,
    const G4TouchableHistory*) const
{

    //Establish track/step information
    const G4Track* track = step->GetTrack();
    const G4StepPoint* postStepPoint = step->GetPostStepPoint();
    const G4ParticleDefinition* particle = track->GetDefinition();

    //-------------------------------------------------------------------
    //Set criteion for what counts as a "hit" that should be recorded.
    bool selectTargetVolumes = true;

    //Option one: a phonon that is stopped and killed at a boundary with a
    //nonzero energy deposition.
    G4bool correctParticle = (
        particle == G4PhononLong::Definition() ||
        particle == G4PhononTransFast::Definition() ||
        particle == G4PhononTransSlow::Definition()
    );
    if (!correctParticle) { // always return true for non-phonons
        return true;
    }

    G4bool correctStatus = (
        step->GetTrack()->GetTrackStatus() == fStopAndKill &&
        postStepPoint->GetStepStatus() == fGeomBoundary &&
        step->GetNonIonizingEnergyDeposit() > 0.
    );

    G4bool landedOnTargetSurface = (postStepPoint->GetPhysicalVolume()->GetName().find("WSiWire") != std::string::npos);

    //Now select which critera matter:
    //Option one: a phonon that is stopped and killed at a boundary with a
    //nonzero energy deposition.  
    if( !selectTargetVolumes ){ return correctParticle && correctStatus; }

    //Option two: a phonon that satisfies all of the above things, but also landed in a specific
    //volume name. Here, we're looking for a volume that contains the words "shuntConductor", which
    //in this tutorial's geometry is one of the qubit crosses. (Can also just put this info in
    //the output file and sort through this in analysis, but this helps us minimize output filesize.)
    else{ return correctParticle && correctStatus && landedOnTargetSurface; }
}
