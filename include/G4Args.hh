#ifndef MY_G4_ARGS_HH
#define MY_G4_ARGS_HH

#include <string>
#include <unordered_map>
#include <vector>
#include "G4String.hh"
#include "G4ThreeVector.hh" // For G4ThreeVector

class MyG4Args 
{
public:
    // Constructor and Destructor
    MyG4Args(int, char**);
    ~MyG4Args();
    
    struct G4StringHasher {
    std::size_t operator()(const G4String& key) const {
        return std::hash<std::string>()(key);
    }
};


    // Struct to store hit data
    struct HitData {
        G4double energyDeposit;
        G4ThreeVector position;
        G4double time;
        G4String particleType;
    };

    // Getter for the output name
    const G4String& GetOutName() const { return OutName; }

    // Function to add energy deposition to a particle type in the map
    void AddToEnergyByParticleAndEvent(const G4String& particleType, G4double energyDeposit, G4int eventNumber);

    // Function to add a hit record to the vector
    void AddHitRecord(const G4double energyDeposit, const G4ThreeVector& position, 
                      const G4double time, const G4String& particleType);

	// Function to store a G4ThreeVector position
	void StorePosition(const G4ThreeVector& position);
	// Function to get a G4ThreeVector position from the gunpositions vector
    G4ThreeVector GetPosition(int i);

    // Getter for hit records
    const std::vector<HitData>& GetHitRecords() const { return hitRecords; }

    // Getter for total energy by particle type for a specific event
    const std::unordered_map<G4String, G4double, G4StringHasher>& GetTotalEnergyByParticleAndEvent(G4int eventNumber) const {
        return totalEnergyByParticleAndEvent.at(eventNumber);  // Access energy by event number
    }
    
	const std::vector<G4ThreeVector>& GetGunPositions() const {
		return gunpositions;
	}
    
	// Getter for total energy by particle type and event number
	const std::unordered_map<G4int, std::unordered_map<G4String, G4double, G4StringHasher>>& GetTotalEnergyByParticleAndEventAll() const {
		return totalEnergyByParticleAndEvent;  // Return the entire map
	}

	// Getter for the randomGunLocation flag
	bool GetRandomGunLocation() const { return randomGunLocation; }
    bool GetPosResScan() const { return posResScan; }
	bool GetAllrecord() const { return Allrecord; }
	G4int GetRunevt() const {return runevt;}

    void ResetTotalEnergyByParticleAndEvent();
	G4double CurrentEvtEdep = 0.0;  // Ensure this is initialized
    void ResetCurrentEvtEdep() {
        CurrentEvtEdep = 0.0;
    }

	void AddCurrentEvtEdep(const G4double energyDeposit) {
		// Store energy deposit for the specific event number
		CurrentEvtEdep += energyDeposit;
	}

	const G4double GetCurrentEvtEdep() const {
		return CurrentEvtEdep;
	}

    G4ThreeVector GetParticlePos() {
        return particlePos;
    }
    G4double GetParticleMom() {
        return particleMom;
    }
    G4ThreeVector GetParticleMomDir() {
        return particleMomDir;
    }
    G4String GetParticleName() {
        return particleName;
    }

    
private:
    // Member variables
    G4String OutName = "sim_output";
    // G4String BasicOutName= "basic_output";
    G4String MacName;    
    G4int runevt = 0;
    bool Allrecord = false;
	bool randomGunLocation = false;  // Flag to determine if random location is activated
    bool posResScan = false;  // Flag to determine if position resolution scan is activated
    G4ThreeVector particlePos = ConvertToPos(); // Location where particle is generated, default is outside cryostat
    G4double particleMom = 1.;  // Default is 1 GeV
    G4ThreeVector particleMomDir = G4ThreeVector(0, 0, 1);  // Default is +z direction
    G4String particleName = "proton";
	//G4double CurrentEvtEdep = 0;
	
    std::vector<G4ThreeVector> gunpositions; // Vector to store position data

    std::unordered_map<G4String, G4double, G4StringHasher> totalEnergyByParticle; // Total energy by particle type
    std::unordered_map<G4int, std::unordered_map<G4String, G4double, G4StringHasher>> totalEnergyByParticleAndEvent; // Energy by event and particle type
    std::vector<HitData> hitRecords; // Vector to store hit data

    G4ThreeVector ConvertToPos(std::string posName="outsideCryostat") {  // By default in CLHEP lengths are in mm and energy is in MeV
        if (posName == "insideCryostat") {
            return G4ThreeVector(0., 0., -100.);
        }else if (posName == "SiO2TopLayer") {
            return G4ThreeVector(0., 0., -1.525260);
        }else if (posName == "aSi") {
            return G4ThreeVector(0., 0., -1.5252445);
        }else if (posName == "WSi") {
            return G4ThreeVector(0., 0., -1.5252415);
        }else if (posName == "SiO2Substrate") {
            return G4ThreeVector(0., 0., -1.525239);
        }else {
            return G4ThreeVector(0., 0., -130.);
        }
    }
    G4ThreeVector ConvertToPos(std::vector<double> particlexyz) {
        return G4ThreeVector(particlexyz[0], particlexyz[1], particlexyz[2]);
    }
};
#endif
