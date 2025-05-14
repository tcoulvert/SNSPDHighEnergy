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
#include "G4CMPUtils.hh"
#include "G4HCofThisEvent.hh"
#include "G4PhononLong.hh"
#include "G4PhononTransFast.hh"
#include "G4PhononTransSlow.hh"
#include "G4Proton.hh"


SensitiveDetector::SensitiveDetector(G4String name, MyG4Args* MainArgs): G4CMPElectrodeSensitivity(name)
{
    PassArgs = MainArgs;
    G4cout << "### Sensitive detector " << name << " is being created!" << G4endl;

}

SensitiveDetector::~SensitiveDetector()
{
}

G4bool SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
	
    
    // Only process hits where energy deposition is greater than 0
    if (IsHit(aStep, ROhist)) {

        G4double edep = aStep->GetTotalEnergyDeposit();
        
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

    G4bool isPhonon = G4CMP::IsPhonon(particle);
    G4bool deadAtBoundary = (
        step->GetTrack()->GetTrackStatus() == fStopAndKill &&
        postStepPoint->GetStepStatus() == fGeomBoundary
    );
    G4bool depositedNonzeroNonIonizingEnergy = step->GetNonIonizingEnergyDeposit() > 0.;
    G4bool depositedNonzeroEnergy = step->GetTotalEnergyDeposit() > 0.;

    G4cout<<"### Detected " << std::setprecision(8) << step->GetTotalEnergyDeposit() * 1e6 << " eV hit by "<< particle->GetParticleName() << " @ " << particle <<" of energy " << track->GetKineticEnergy() * 1e6 << " eV, will it be recorded? "<< ((!isPhonon && depositedNonzeroEnergy) | (isPhonon && deadAtBoundary && depositedNonzeroNonIonizingEnergy)) <<G4endl;

    if (!isPhonon) { return depositedNonzeroEnergy; }
    else { return deadAtBoundary && depositedNonzeroNonIonizingEnergy; }

    // //A phonon that is stopped and killed at a boundary with a
    // //nonzero energy deposition.
    // G4bool phononParticle = (
    //     particle == G4PhononLong::Definition() ||
    //     particle == G4PhononTransFast::Definition() ||
    //     particle == G4PhononTransSlow::Definition()
    // );
    // if (!phononParticle) { // always return true for non-phonons
    //     return true;
    // }

    // G4bool correctPhononStatus = (
    //     step->GetTrack()->GetTrackStatus() == fStopAndKill &&
    //     postStepPoint->GetStepStatus() == fGeomBoundary &&
    //     step->GetNonIonizingEnergyDeposit() > 0.
    // );

    // //A phonon that is stopped and killed at a boundary with a
    // //nonzero energy deposition.
    // return phononParticle && correctPhononStatus;


    // // Aphonon that satisfies all of the above things, but also landed in a specific
    // // volume name. Here, we're looking for a volume that contains the words "WSiWire", which
    // // in this geometry is the superconducting nanowire. (Can also just put this info in
    // // the output file and sort through this in analysis, but this helps us minimize output filesize.)
    // G4bool landedOnTargetSurface = (postStepPoint->GetPhysicalVolume()->GetName().find("WSiWire") != std::string::npos);
    // return phononParticle && correctPhononStatus && landedOnTargetSurface;

}
