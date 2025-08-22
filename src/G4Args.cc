#include "G4Args.hh"
#include <cstring>  // For strcmp
#include <iostream> // For G4cout
#include <unistd.h> // For exit()
#include <regex>
#include "CLHEP/Units/SystemOfUnits.h"

// Constructor: Process command-line arguments
MyG4Args::MyG4Args(int mainargc, char** mainargv) {
    G4cout << "### Processing Command Line Arguments for the Simulation: " << G4endl;

    for (int j = 1; j < mainargc; ++j) {
        G4cout << mainargv[j] << G4endl;

        // Check for "-o" argument to set the output name
        if (strcmp(mainargv[j], "-o") == 0) {

            if (j + 1 < mainargc) { // Ensure there's a next argument
                OutName = mainargv[j + 1];
                G4cout << "### Changed Output Name to: " << OutName << G4endl;
                ++j; // Skip the next argument since it's the value
            } else {
                G4cerr << "### Error: Missing output name after '-o'" << G4endl;
                exit(EXIT_FAILURE);
            }

        }
	    // Check for "-random" argument to activate random particle generator location
		if (strcmp(mainargv[j], "-rndgun") == 0) {

			randomGunLocation = true;
			G4cout << "### Random particle location activated." << G4endl;

		}else if(strcmp(mainargv[j],"-runevt")==0)
        {   
            runevt = atoi(mainargv[j+1]); j=j+1;
            G4cout<< " ### Run "<< runevt <<" evts" <<G4endl;     
                
        }else if(strcmp(mainargv[j],"-Allrecord")==0)
        {   
            Allrecord = true;
            G4cout<< " ### Allrecord true, storing all impacts even in Edep==0 (needed for storing impact initial point)" <<G4endl;     
                
        }else if(strcmp(mainargv[j], "-PosResScan") == 0) 
        {  // do pos res scan (across wire) by default
            posResScan = true;
        }else if(strcmp(mainargv[j], "-particlePos") == 0) 
        {

            std::string particleArg = mainargv[j+1]; j=j+1;
            std::regex pattern(",");
            if (std::regex_search(particleArg, pattern)) {
                std::vector<double> particlexyz;
                size_t pos = 0;
                std::string token;
                while ((pos = particleArg.find(",")) != std::string::npos) {
                    token = particleArg.substr(0, pos);
                    particlexyz.push_back(atof(token.c_str()));
                    particleArg.erase(0, pos + 1);
                }
                particlexyz.push_back(atof(particleArg.c_str()));

                particlePos = ConvertToPos(particlexyz);
            } else {
                particlePos = ConvertToPos(particleArg);
            }
            
            G4cout<< " ### Generate particle at "<< particlePos <<G4endl; 

        }else if(strcmp(mainargv[j], "-particleMom") == 0) 
        {

            particleMom = atof(mainargv[j+1]); j=j+1;
            G4cout<< " ### Generate particle with momentum "<< particleMom << " MeV" <<G4endl; 

        }else if(strcmp(mainargv[j], "-particleMomDir") == 0) 
        {

            std::string particleArg = mainargv[j+1]; j=j+1;
            std::regex pattern(",");
            std::vector<double> particlexyz;
            size_t pos = 0;
            std::string token;
            while ((pos = particleArg.find(",")) != std::string::npos) {
                token = particleArg.substr(0, pos);
                particlexyz.push_back(atof(token.c_str()));
                particleArg.erase(0, pos + 1);
            }
            particlexyz.push_back(atof(particleArg.c_str()));

            particleMomDir = ConvertToPos(particlexyz);

            G4cout<< " ### Generate particle with momentum direction "<< particleMomDir <<G4endl; 

        }else if(strcmp(mainargv[j],"-particleName")==0)
        {   

            particleName = mainargv[j+1]; j=j+1;
            G4cout<< " ### Generate "<< particleName <<G4endl;   
                
        }else if (strcmp(mainargv[j],"-nParticles")==0)
        {   

            nParticles = atoi(mainargv[j+1]); j=j+1;
            G4cout<< " ### Generate "<< nParticles << " particles per events" <<G4endl;   
                
        }
    }

    if (randomGunLocation && posResScan) {
        G4cerr << "### Error: both 'rndgun' and 'PosResScan' were activated, however both can't be run." << G4endl;
        exit(EXIT_FAILURE);
    }else if (randomGunLocation){
        for (int i=0; i<runevt; i++) {

            // Generate random positions within a 10-micron width in X and Y (range: -5 microns to +5 microns)
            G4double randomX = -25.0 + (static_cast<double>(rand()) / RAND_MAX) * 50.0;  // Random value between -50 and +50 microns
            G4double randomY = -25.0 + (static_cast<double>(rand()) / RAND_MAX) * 50.0;  // Random value between -50 and +50 microns
            // Set position with random X and Y, fixed Z at -1.525230 mm (within SiO2 substrate)
            gunpositions.push_back(G4ThreeVector(randomX / 1000 * CLHEP::mm, randomY / 1000 * CLHEP::mm, -1.525230 * CLHEP::mm));

        }
    }else if (posResScan){

        G4ThreeVector currentPos = G4ThreeVector(0. * CLHEP::mm, 0. * CLHEP::mm, 0 * CLHEP::mm);
        double totalDistance = 0.0015 + 0.00225;
        double pointSpacing = totalDistance / runevt;
        for (int i=0; i<runevt; i++) {

            currentPos += G4ThreeVector(0. * CLHEP::mm, (i * pointSpacing) * CLHEP::mm, 0 * CLHEP::mm);
            gunpositions.push_back(currentPos);

        }

    }

}

// Destructor
MyG4Args::~MyG4Args() {
    // No dynamic memory to clean up
}

// Add energy deposition to the total for the given particle type and event number
void MyG4Args::AddToEnergyByParticleAndEvent(const G4String& particleType, G4double energyDeposit, G4int eventNumber) {
    // Store energy deposit for the specific event number
    totalEnergyByParticleAndEvent[eventNumber][particleType] += energyDeposit;
}

// Create a new hit record and append it to the hitRecords vector
void MyG4Args::AddHitRecord(const G4double energyDeposit, const G4ThreeVector& position, 
                            const G4double time, const G4String& particleType) {
    HitData newHit = {energyDeposit, position, time, particleType};
    hitRecords.push_back(newHit);
}


void MyG4Args::ResetTotalEnergyByParticleAndEvent() {
    // Clear the entire map, removing all its contents
    totalEnergyByParticleAndEvent.clear();

    G4cout << "Total energy by particle and event has been reinitialized (cleared)." << G4endl;
}

// Implementation of StorePosition
void MyG4Args::StorePosition(const G4ThreeVector& position) {
    // Add the position to the hitRecords with default or placeholder values for other fields
    gunpositions.push_back(position);
}

// Implementation of GetPosition
G4ThreeVector MyG4Args::GetPosition(int i) {
    // Add the position to the hitRecords with default or placeholder values for other fields
    return gunpositions[i];
}
