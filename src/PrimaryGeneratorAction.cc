/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file exoticphysics/phonon/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//
// $Id: e75f788b103aef810361fad30f75077829192c13 $
//
// 20140519  Allow the user to specify phonon type by name in macro; if
//	     "geantino" is set, use random generator to select.

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4Geantino.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4RandomDirection.hh"
#include "G4PhononTransFast.hh"
#include "G4PhononTransSlow.hh"
#include "G4PhononLong.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"

using namespace std;

PrimaryGeneratorAction::PrimaryGeneratorAction(MyG4Args* MainArgs) { 
  PassArgs = MainArgs;
  fParticleGun = new G4ParticleGun(1);

  // default particle kinematics ("geantino" triggers random phonon choice)
  //  fParticleGun->SetParticleDefinition(G4Geantino::Definition());
  //  fParticleGun->SetParticleMomentumDirection(G4RandomDirection());
  //  fParticleGun->SetParticlePosition(G4ThreeVector(0.0,0.0,0.4998095*CLHEP::cm));
  //  fParticleGun->SetParticleEnergy(0.0075*eV);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

 
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

  G4cout<< " ### Starting Generator  " <<G4endl;
  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  // Define the proton particle
  G4String particleName = "proton";
  // G4String particleName = "phononL";
  G4ParticleDefinition* particle_p = particleTable->FindParticle(particleName);
  
  // Set particle properties for 120 GeV proton
  fParticleGun->SetParticleDefinition(particle_p);
  fParticleGun->SetParticleMomentum(120. * GeV); // Set momentum to 120 GeV 
  if (particleName == "phononL") {
    fParticleGun->SetParticleMomentum(0.03 * eV); // Set momentum to 0.03 eV
  }

  // Set particle direction to +z
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
    
	// Declare pos outside the if-else blocks
	G4ThreeVector pos;

	// Check if randomGunLocation is true or false
	if (PassArgs->GetRandomGunLocation() || PassArgs->GetPosResScan()) {
		pos = PassArgs->GetPosition(anEvent->GetEventID());
	} else {
		// Set position to -200 mm in Z with X and Y as 0 if randomGunLocation and posResScan are false
    pos = G4ThreeVector(0. * CLHEP::mm, 0. * CLHEP::mm, -200. * CLHEP::mm);
    if (particleName == "phononL") {
		  pos = G4ThreeVector(0. * CLHEP::mm, 0. * CLHEP::mm, -1.525260 * CLHEP::mm); // in the middle of the SiO2 toplayer
    }
	  PassArgs->StorePosition(pos);
	}

	// Set the particle gun position
	fParticleGun->SetParticlePosition(pos);

  G4cout<< " ### Finshing Generator  " <<G4endl;    
  
  fParticleGun->GeneratePrimaryVertex(anEvent);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


