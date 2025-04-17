/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file exoticphysics/phonon/src/PhononDetectorConstruction.cc \brief
/// Implementation of the PhononDetectorConstruction class
//
// $Id: a2016d29cc7d1e75482bfc623a533d20b60390da $
//
// 20140321  Drop passing placement transform to G4LatticePhysical
// 20211207  Replace G4Logical*Surface with G4CMP-specific versions.
// 20220809  [ For M. Hui ] -- Add frequency dependent surface properties.

#include "DetectorConstruction.hh"
#include "DetectorParameters.hh"
#include "SensitiveDetector.hh"
#include "G4CMPPhononElectrode.hh"
#include "G4CMPElectrodeSensitivity.hh"
#include "G4CMPLogicalBorderSurface.hh"
#include "G4CMPSurfaceProperty.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4GeometryManager.hh"
#include "G4LatticeLogical.hh"
#include "G4LatticeManager.hh"
#include "G4LatticePhysical.hh"
#include "G4CMPLogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4Sphere.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4Tubs.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4MultiUnion.hh"

using namespace DetectorParameters;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

DetectorConstruction::DetectorConstruction(MyG4Args* MainArgs){
  PassArgs = MainArgs;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

DetectorConstruction::~DetectorConstruction() {;}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  if (fConstructed) {
    if (!G4RunManager::IfGeometryHasBeenDestroyed()) {
      // Run manager hasn't cleaned volume stores. This code shouldn't execute
      G4GeometryManager::GetInstance()->OpenGeometry();
      G4PhysicalVolumeStore::GetInstance()->Clean();
      G4LogicalVolumeStore::GetInstance()->Clean();
      G4SolidStore::GetInstance()->Clean();
    }
    // Have to completely remove all lattices to avoid warning on reconstruction
    G4LatticeManager::GetLatticeManager()->Reset();
    // Clear all LogicalSurfaces
    // NOTE: No need to redefine the G4CMPSurfaceProperties
    G4CMPLogicalBorderSurface::CleanSurfaceTable();
  }

  DefineMaterials();
  SetupGeometry();
  fConstructed = true;

  return fWorldPhys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void DetectorConstruction::DefineMaterials()
{ 
  G4NistManager* nistManager = G4NistManager::Instance();

  G4cout<< " ### - Define SiO2" <<G4endl;    
  fSiO2 = new G4Material("SiO2", 2.201*g/cm3, 2);
    fSiO2->AddElement(nistManager->FindOrBuildElement("Si"), 1);
    fSiO2->AddElement(nistManager->FindOrBuildElement("O"), 2);
    G4MaterialPropertiesTable *mptSiO2 = new G4MaterialPropertiesTable();
    G4double energySiO2[2] = {1.378*eV, 6.199*eV};
    G4double rindexSiO2[2] = {1.4585, 1.53};
    G4double ABSSiO2[2] = {0.01*mm, 0.01*mm};
    mptSiO2->AddProperty("RINDEX", energySiO2, rindexSiO2, 2);
    //mptSiO2->AddProperty("ABSLENGTH", energySiO2, ABSSiO2, 2);
    fSiO2->SetMaterialPropertiesTable(mptSiO2);

  G4cout<< " ### - Define Air" <<G4endl;    
  fAir = nistManager->FindOrBuildMaterial("G4_AIR");
    G4double energyWorld[2] = {1.378*eV, 6.199*eV};
    G4double rindexWorld[2] = {1.0, 1.0};
    G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
    mptWorld->AddProperty("RINDEX", energyWorld, rindexWorld, 2);
    fAir->SetMaterialPropertiesTable(mptWorld);
    
	G4cout << " ### - Define Vacuum" << G4endl;    
  fVacuum = nistManager->FindOrBuildMaterial("G4_Galactic");
    G4double energyVacuum[2] = {1.378*eV, 6.199*eV}; 
    G4double rindexVacuum[2] = {1.0, 1.0};  // Refractive index for vacuum is 1
    G4MaterialPropertiesTable* mptVacuum = new G4MaterialPropertiesTable();
    mptVacuum->AddProperty("RINDEX", energyVacuum, rindexVacuum, 2);
    fVacuum->SetMaterialPropertiesTable(mptVacuum);

  G4cout<< " ### - Define Cu" <<G4endl;    
  fCu = nistManager->FindOrBuildMaterial("G4_Cu");
    G4MaterialPropertiesTable *mptCu = new G4MaterialPropertiesTable();
    G4double energyCu[2] = {2*eV, 6*eV};
    G4double rindexCu[2] = {0.7, 1};
    G4double ABSCu[2] = {1.68e-9*m, 1.68e-9*m};
    mptCu->AddProperty("RINDEX", energyCu, ABSCu, 2);
    mptCu->AddProperty("ABSLENGTH", energyCu, ABSCu, 2);
    fCu->SetMaterialPropertiesTable(mptCu);
  
  G4cout<< " ### - Define Al" <<G4endl;    
  fAl = nistManager->FindOrBuildMaterial("G4_Al");
    G4MaterialPropertiesTable *mptAl = new G4MaterialPropertiesTable();
    G4double energyAl[2] = {400*eV, 1000*eV};
    G4double rindexAl[2] = {0.99, 0.99};
    G4double ABSAl[2] = {6.6e-9*m, 6.63e-9*m};
    mptAl->AddProperty("RINDEX", energyAl, ABSAl, 2);
    mptAl->AddProperty("ABSLENGTH", energyAl, ABSAl, 2);
    fAl->SetMaterialPropertiesTable(mptAl);
    
  // https://refractiveindex.info/?shelf=main&book=Si3N4&page=Kischkat
  // https://refractiveindex.info/?shelf=main&book=Si3N4&page=Luke
  // G4cout << "### - Define Silicon Nitride (Si3N4)" << G4endl;
  // // Define the material using the NIST database or creating a custom material
  // fSi = new G4Material("Silicon", 3.44*g/cm3, 2);
  //   fSi3N4->AddElement(nistManager->FindOrBuildElement("Si"), 3);
  //   fSi3N4->AddElement(nistManager->FindOrBuildElement("N"), 4);
  //   // Define optical properties
  //   G4MaterialPropertiesTable* mptSi3N4 = new G4MaterialPropertiesTable();
  //   G4double energySi3N4[2] = {400*eV, 1000*eV}; // Define photon energy range
  //   //G4double rindexSi3N4[2] = {2.05, 2.05};      // Example refractive index values
  //   G4double absLengthSi3N4[2] = {4.08*mm, 4.08*mm}; // Absorption length in mm
  //   // Define photon energies in eV
  //   const G4int numEntries = 10;
  //   G4double photonEnergy[numEntries] = {
  //     4.465*eV, 3.79*eV, 3.115*eV, 2.647*eV, 2.18*eV,
  //     1.712*eV, 1.245*eV, 0.8294*eV, 0.5178*eV, 0.3619*eV};
  //   // Corresponding refractive indices
  //   G4double refractiveIndex[numEntries] = {
  //     1.8721213170359, 1.9110346794217, 1.943231277958, 1.9619062604624, 1.9778699738446,
  //     1.9917473810267, 2.0050993473208, 2.0222778887066, 2.0590472271639, 2.1244526847835};
  //   // Add properties to the material properties table
  //   mptSi3N4->AddProperty("RINDEX", photonEnergy, refractiveIndex, numEntries);
  //   mptSi3N4->AddProperty("ABSLENGTH", energySi3N4, absLengthSi3N4, 2);
  //   // Assign the properties table to the material
  //   fSi3N4->SetMaterialPropertiesTable(mptSi3N4);
  // G4cout << "### - Silicon Nitride (Si3N4) defined" << G4endl;
  fSi = nistManager->FindOrBuildMaterial("G4_Si");

    
  G4cout<< " ### - Define a-Si" <<G4endl;   // Amorphous Silion
  G4double density_aSi = 2.32 * g/cm3;  // Typical density for Amorphous Silicon
  faSi = new G4Material("AmorphousSi", density_aSi, 1);
    G4Element* Si = nistManager->FindOrBuildElement("Si");  // Silicon element
    faSi->AddElement(Si, 1);  // 1 Silicon atom
    G4MaterialPropertiesTable *mptSi = new G4MaterialPropertiesTable();
    G4double energySi[2] = {1.378*eV, 6.199*eV};
    G4double rindexSi[2] = {4, 4};
    G4double ABSSi[2] = {0.01*mm, 0.01*mm};
    mptSi->AddProperty("RINDEX", energySi, rindexSi, 2);
    mptSi->AddProperty("ABSLENGTH", energySi, ABSSi, 2);
    faSi->SetMaterialPropertiesTable(mptSi);

  G4cout<< " ### - Define WSi" <<G4endl;   // Tungsten Silicide (WSi)
  G4double density_WSi = 9.3 * g/cm3;  // Approximate density of WSi
  fWSi = new G4Material("WSi", density_WSi, 2);  // 2 elements in WSi
    G4Element* W = nistManager->FindOrBuildElement("W");  // Tungsten element  
    fWSi->AddElement(W, 1);  // 1 Tungsten atom
    fWSi->AddElement(Si, 2); // 2 Silicon atoms
    
    G4MaterialPropertiesTable *mptWSi = new G4MaterialPropertiesTable();
    G4double energyWSi[2] = {1.5*eV, 3*eV};
    G4double rindexWSi[2] = {4, 4};
    mptWSi->AddProperty("RINDEX", energyWSi, rindexWSi, 2);
    fWSi->SetMaterialPropertiesTable(mptWSi);
    
    G4cout<< " ### Finished Material Definition " <<G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void DetectorConstruction::SetupGeometry()
{



  //---------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------
  // First, define border surface properties that can be referenced later
  const G4double GHz = 1e9 * hertz; 

  //the following coefficients and cutoff values are not well-motivated
  //the code below is used only to demonstrate how to set these values.
  const std::vector<G4double> anhCoeffs = {0,0,0,0,0,0};//Turn this off temporarily
  const std::vector<G4double> diffCoeffs = {1,0,0,0,0,0};//Explicitly make this 1 for now
  const std::vector<G4double> specCoeffs = {0,0,0,0,0,0};//Turn this off temporarily
  const G4double anhCutoff = 520., reflCutoff = 350.;   // Units external
    
  
  //These are just the definitions of the interface TYPES, not the interfaces themselves. These must be called in a set of loops
  //below, and invoke these surface definitions.
  if( !fConstructed ){
    // Substrate to copper housing interface
    fSiCuInterface = new G4CMPSurfaceProperty("SiCuInterface",
        1.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 0.0 );
    // Si substrate to SiO2 layer interface
    fSiSiO2Interface = new G4CMPSurfaceProperty("SiSiO2Interface",
        1.0, 0.0, 0.0, 0.0,
        0.5, 0.5, 0.0, 0.0);
    // SiO2 ubstrate to SiO2 top layer interface
    fSiO2SiO2Interface = new G4CMPSurfaceProperty("SiO2SiO2Interface",
        1.0, 0.0, 0.0, 0.0,
        0.05, 0.05, 0.0, 0.0);
    // SiO2 layer to Amorphous Silicon (non-superconducting cap) interface
    fSiO2aSiInterface = new G4CMPSurfaceProperty("SiO2aSiInterface",
        0.5, 0.5, 0.0, 0.0,
        0.5, 0.5, 0.0, 0.0 );
    // SiO2 layer to Tungsten Silicide (superconducting wire) interface
    fSiO2WSiInterface = new G4CMPSurfaceProperty("SiO2WSiInterface",
        1.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 0.0 );
    // aSi layer to WSi (interface between superconducting wire and non-superconducting cap) interface   
    faSiWSiInterface = new G4CMPSurfaceProperty("aSiWSiInterface",
        1.0, 0.5, 0.0, 0.0,
        1.0, 0.5, 0.0, 0.0 );
    // SiO2 layer to vacuum interface 
    fSiO2VacuumInterface = new G4CMPSurfaceProperty("SiO2VacuumInterface",
        0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0 );


    fSiCuInterface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
        diffCoeffs, specCoeffs, GHz, GHz, GHz);
    fSiSiO2Interface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
        diffCoeffs, specCoeffs, GHz, GHz, GHz);
    fSiO2SiO2Interface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
        diffCoeffs, specCoeffs, GHz, GHz, GHz);
    fSiO2aSiInterface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
        diffCoeffs, specCoeffs, GHz, GHz, GHz);
    fSiO2WSiInterface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
        diffCoeffs, specCoeffs, GHz, GHz, GHz);
    faSiWSiInterface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
        diffCoeffs, specCoeffs, GHz, GHz, GHz);  
    fSiO2VacuumInterface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
        diffCoeffs, specCoeffs, GHz, GHz, GHz);


    //Add a phonon sensor to the interface properties here.
    AttachPhononSensor(fSiO2WSiInterface);
    AttachPhononSensor(faSiWSiInterface);
  }
















  //---------------------------------------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------------------------------------
  // Now we start constructing the various components and their interfaces  
  //     
  // World
  //
  G4Box *solidWorld = new G4Box("solidWorld", dp_worldSize/2, dp_worldSize/2, dp_worldSize/2);
  G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, fVacuum, "logicWorld");
  fWorldPhys = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);
  bool checkOverlaps = true;






  







  //---------------------------------------------------------------------------------------------------------------------
  // First, set up the Aluminum absorption fridge
  G4Tubs *solidRadiator = new G4Tubs("solidRadiator", dp_fridgeInnerRadius, dp_fridgeOuterRadius, dp_fridgeHeight, 0.*CLHEP::deg, 360.*CLHEP::deg);
  G4Tubs *solidRadiatorShield2 = new G4Tubs("solidRadiator2", dp_fridgeInnerRadiusShield, dp_fridgeOuterRadiusShield, dp_fridgeHeight, 0.*CLHEP::deg, 360.*CLHEP::deg);

  G4LogicalVolume *logicRadiator = new G4LogicalVolume(solidRadiator, fAl, "logicalRadiator");
  G4LogicalVolume *logicRadiatorShield2 = new G4LogicalVolume(solidRadiatorShield2, fAl, "logicalRadiator2");

  // Create a rotation matrix to rotate 90 degrees around the Z-axis
  G4RotationMatrix *rotation = new G4RotationMatrix();
  rotation->rotateX(90 * CLHEP::deg); // Rotate 90 degrees around the Y-axis
  // Place the cylinder with the rotation
  G4VPhysicalVolume *physRadiator = new G4PVPlacement(rotation, G4ThreeVector(0., 0., 0. * m), logicRadiator, "physRadiator", logicWorld, false, 0, true);
  G4VPhysicalVolume *physRadiator2 = new G4PVPlacement(rotation, G4ThreeVector(0., 0., 0. * m), logicRadiatorShield2, "physRadiator2", logicWorld, false, 0, true);






















  //---------------------------------------------------------------------------------------------------------------------
  // Next, set up the Copper Housing
  G4Box* solidCu1 = new G4Box("solidCu1", dp_housing1DimX/2, dp_housing1DimY/2, dp_housing1DimZ/2);
	G4LogicalVolume* logicCu1 = new G4LogicalVolume(solidCu1, fCu, "logicCu1");
	G4VPhysicalVolume* physCu1 = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicCu1,"physCu1",logicWorld,false,0,true);
	G4Box* solidCu2 = new G4Box("solidCu2", dp_housing2DimX/2, dp_housing2DimY/2, dp_housing2DimZ/2);
	G4LogicalVolume* logicCu2 = new G4LogicalVolume(solidCu2, fCu, "logicCu2");
	G4VPhysicalVolume* physCu2 = new G4PVPlacement(0,G4ThreeVector(0,3.,2),logicCu2,"physCu2",logicWorld,false,0,true);

  G4VisAttributes* Cu1VisAtt= new G4VisAttributes(G4Colour(1.0,0.647,0.0,0.9));
  Cu1VisAtt->SetVisibility(true);
  logicCu1->SetVisAttributes(Cu1VisAtt);

  G4VisAttributes* Cu2VisAtt= new G4VisAttributes(G4Colour(0.7,0.647,0.0,0.9));
  Cu2VisAtt->SetVisibility(true);
  logicCu2->SetVisAttributes(Cu2VisAtt);




 












  //----------------------------------------------------------------
  //Max allowed step-size in substrate
  G4UserLimits* substrateUserLimits = new G4UserLimits();
  substrateUserLimits->SetMaxAllowedStep(1. * CLHEP::nm);
  G4UserLimits* wireUserLimits = new G4UserLimits();
	wireUserLimits->SetMaxAllowedStep(0.05 * CLHEP::nm);













  //-------------------------------------------------------------------------------------------------------------------
  //Then, set up the Si substrate.
  G4Box* solid_Sisubstrate = new G4Box("solid_Sisubstrate", dp_SisubstrateDimX/2, dp_SisubstrateDimY/2, dp_SisubstrateDimZ/2);
  G4LogicalVolume* logic_Sisubstrate = new G4LogicalVolume(solid_Sisubstrate, fSi, "logic_Sisubstrate");
	logic_Sisubstrate->SetUserLimits(substrateUserLimits);
	G4VPhysicalVolume* phys_Sisubstrate = new G4PVPlacement(
		0,
		G4ThreeVector(0., 0., dp_sensorDimZ-dp_SisubstrateDimZ/2),
		logic_Sisubstrate,
		"phys_Sisubstrate",
		logicWorld,
		false,
		0,
		true
	);

  //Set up the G4CMP silicon lattice information using the G4LatticeManager
  // G4LatticeManager gives physics processes access to lattices by volume
  G4LatticeManager* LM = G4LatticeManager::GetLatticeManager();
  G4LatticeLogical* logic_SiLattice = LM->LoadLattice(fSi, "Si");
  // G4LatticePhysical assigns G4LatticeLogical a physical orientation
  G4LatticePhysical* phys_SiLattice = new G4LatticePhysical(logic_SiLattice);
  phys_SiLattice->SetMillerOrientation(1,0,0); 
  LM->RegisterLattice(phys_Sisubstrate, phys_SiLattice);
  
  G4VisAttributes* SiVisAtt= new G4VisAttributes(G4Colour(0.4,0.4,0.4));
  SiVisAtt->SetVisibility(true);
  logic_Sisubstrate->SetVisAttributes(SiVisAtt);

  //Set up border surfaces
  G4CMPLogicalBorderSurface* border_Si_Cu = new G4CMPLogicalBorderSurface("border_Si_Cu", phys_Sisubstrate, physCu2, fSiCuInterface);

















  //-------------------------------------------------------------------------------------------------------------------
  //Next, setup the SiO2 substrate
  G4Box* solid_SiO2substrate = new G4Box("solid_SiO2substrate", dp_SiO2substrateDimX/2, dp_SiO2substrateDimY/2, dp_SiO2substrateDimZ/2);
  G4LogicalVolume* logic_SiO2substrate = new G4LogicalVolume(solid_SiO2substrate, fSiO2, "logic_SiO2substrate");
	logic_SiO2substrate->SetUserLimits(substrateUserLimits);
	G4VPhysicalVolume* phys_SiO2substrate = new G4PVPlacement(
		0,
		G4ThreeVector(0., 0., dp_sensorDimZ-dp_SisubstrateDimZ-dp_SiO2substrateDimZ/2),
		logic_SiO2substrate,
		"phys_SiO2substrate",
		logicWorld,
		false,
		0,
		true
	);
  
  // G4LatticeLogical* logic_SiO2Lattice = LM->LoadLattice(fSiO2, "SiO2");
  G4LatticeLogical* logic_SiO2Lattice = LM->LoadLattice(fSiO2, "Si");
  G4LatticePhysical* phys_SiO2Lattice = new G4LatticePhysical(logic_SiO2Lattice);
  phys_SiO2Lattice->SetMillerOrientation(1,0,0); 
  LM->RegisterLattice(phys_SiO2substrate, phys_SiO2Lattice);

  G4VisAttributes* SiO2substrateVisAtt= new G4VisAttributes(G4Colour(0.6,0.6,0.6));
  SiO2substrateVisAtt->SetVisibility(true);
  logic_SiO2substrate->SetVisAttributes(SiO2substrateVisAtt);

  //Set up border surfaces
  G4CMPLogicalBorderSurface* border_Si_SiO2 = new G4CMPLogicalBorderSurface("border_Si_SiO2", phys_Sisubstrate, phys_SiO2substrate, fSiSiO2Interface);

















//-------------------------------------------------------------------------------------------------------------------
  //Finally, setup the SiO2 top layer
  G4Box* solid_SiO2toplayer = new G4Box("solid_SiO2toplayer", dp_SiO2toplayerDimX/2, dp_SiO2toplayerDimY/2, dp_SiO2toplayerDimZ/2);
  G4LogicalVolume* logic_SiO2toplayer = new G4LogicalVolume(solid_SiO2toplayer, fSiO2, "logic_SiO2toplayer");
	logic_SiO2toplayer->SetUserLimits(substrateUserLimits);
	G4VPhysicalVolume* phys_SiO2toplayer = new G4PVPlacement(
		0,
		G4ThreeVector(0., 0., dp_sensorDimZ-dp_SisubstrateDimZ-dp_SiO2substrateDimZ-dp_SiO2toplayerDimZ/2),
		logic_SiO2toplayer,
		"phys_SiO2toplayer",
		logicWorld,
		false,
		0,
		true
	);

  LM->RegisterLattice(phys_SiO2toplayer, phys_SiO2Lattice);

  G4VisAttributes* SiO2toplayerVisAtt= new G4VisAttributes(G4Colour(0.8,0.8,0.8));
  SiO2toplayerVisAtt->SetVisibility(true);
  logic_SiO2toplayer->SetVisAttributes(SiO2toplayerVisAtt);

  //Set up border surfaces
  G4CMPLogicalBorderSurface* border_SiO2_SiO2 = new G4CMPLogicalBorderSurface("border_SiO2_SiO2", phys_SiO2substrate, phys_SiO2toplayer, fSiO2SiO2Interface);

















  //-------------------------------------------------------------------------------------------------------------------
  //Finally, setup the nanowire strips and establish a sensitivity object

  G4MultiUnion* solid_WSiWire = new G4MultiUnion("solid_WSiWire");
  G4MultiUnion* solid_aSiWire = new G4MultiUnion("solid_aSiWire");
	
	for (G4int i = 0; i < dp_numStrips; i++) {
		// Position along y for each strip, considering the thickness and the spacing
		G4double strip_y_pos = -(dp_stripDimY / 2) + i * (dp_stripThickness + dp_stripSpacing);
    G4double wrap_y_pos = strip_y_pos + (dp_stripThickness / 2) + (dp_stripSpacing / 2);

    G4double startAngle = 0.*CLHEP::deg;
    G4double endAngle = 180.*CLHEP::deg;
    G4double xRot, yRot, zRot, wrapPosX;
    if (i % 2 == 0){
      xRot = 0.*CLHEP::deg;
      yRot = 0.*CLHEP::deg;
      zRot = 90.*CLHEP::deg;
      wrapPosX = dp_stripDimX / 2;
    }
    else{
      xRot = 0.*CLHEP::deg;
      yRot = 0.*CLHEP::deg;
      zRot = 270.*CLHEP::deg;
      wrapPosX = -dp_stripDimX / 2;
    }

    G4bool last_wire = false;
    if (i + 1 == dp_numStrips){
      last_wire = true;
    }

		// Create a strip solid with specified dimensions
		G4Box* solid_WSiStrip = new G4Box("solid_strip_" + std::to_string(i), dp_stripDimX / 2, dp_stripThickness / 2, dp_stripDimZ / 2);
    G4Transform3D tr_WSiStrip = G4Transform3D(G4RotationMatrix(0, 0, 0), G4ThreeVector(0, strip_y_pos, +dp_SiO2toplayerDimZ/2 - dp_stripDimZ/2));
    solid_WSiWire->AddNode(*solid_WSiStrip, tr_WSiStrip);

		G4Box* solid_aSiStrip = new G4Box("solid_strip_" + std::to_string(i), dp_stripDimX / 2, dp_stripThickness / 2, dp_stripDimZ / 2);
    G4Transform3D tr_aSiStrip = G4Transform3D(G4RotationMatrix(0, 0, 0), G4ThreeVector(0, strip_y_pos, +dp_SiO2toplayerDimZ/2 - dp_stripDimZ - dp_stripDimZ/2));
    solid_aSiWire->AddNode(*solid_aSiStrip, tr_aSiStrip);

    if (!last_wire){
      G4Tubs* solid_WSiWrap = new G4Tubs("solid_strip_" + std::to_string(i), dp_stripWrapInnerRadius, dp_stripWrapOuterRadius, dp_stripDimZ / 2, startAngle, endAngle);
      G4Transform3D tr_WSiWrap = G4Transform3D(G4RotationMatrix(xRot, yRot, zRot), G4ThreeVector(wrapPosX, wrap_y_pos, +dp_SiO2toplayerDimZ/2 - dp_stripDimZ/2));
      solid_WSiWire->AddNode(*solid_WSiWrap, tr_WSiWrap);

      G4Tubs* solid_aSiWrap = new G4Tubs("solid_strip_" + std::to_string(i), dp_stripWrapInnerRadius, dp_stripWrapOuterRadius, dp_stripDimZ / 2, startAngle, endAngle);
      G4Transform3D tr_aSiWrap = G4Transform3D(G4RotationMatrix(xRot, yRot, zRot), G4ThreeVector(wrapPosX, wrap_y_pos, +dp_SiO2toplayerDimZ/2 - dp_stripDimZ - dp_stripDimZ/2));
      solid_aSiWire->AddNode(*solid_aSiWrap, tr_aSiWrap);
	  }
  }

  solid_WSiWire->Voxelize();
  solid_aSiWire->Voxelize();

  G4LogicalVolume* logic_WSiWire = new G4LogicalVolume(solid_WSiWire, fWSi, "logic_WSiWire");
  G4LogicalVolume* logic_aSiWire = new G4LogicalVolume(solid_aSiWire, faSi, "logic_aSiWire");

  logic_WSiWire->SetUserLimits(wireUserLimits);
  logic_aSiWire->SetUserLimits(wireUserLimits);

  G4VisAttributes* WSiVisAtt= new G4VisAttributes(G4Colour(0.0,1.0,1.0,0.5));
  WSiVisAtt->SetVisibility(true);
  logic_WSiWire->SetVisAttributes(WSiVisAtt);

  G4VisAttributes* aSiVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,1.0,0.3));
  aSiVisAtt->SetVisibility(true);
  logic_aSiWire->SetVisAttributes(aSiVisAtt);

  G4VPhysicalVolume* phys_WSiWire = new G4PVPlacement(
		0,
		G4ThreeVector(0., 0., 0.),
		logic_WSiWire,
		"phys_WSiWire",
		logic_SiO2toplayer,
		false,
		0,
		true
	);

  G4VPhysicalVolume* phys_aSiWire = new G4PVPlacement(
		0,
		G4ThreeVector(0., 0., 0.),
		logic_aSiWire,
		"phys_aSiWire",
		logic_SiO2toplayer,
		false,
		0,
		true
	);

  //Set up border surfaces
  G4CMPLogicalBorderSurface* border_aSiWire_WSiWire = new G4CMPLogicalBorderSurface("border_aSiStrip_WSiStrip", phys_aSiWire, phys_WSiWire, faSiWSiInterface);
  G4CMPLogicalBorderSurface* border_SiO2_aSiWire = new G4CMPLogicalBorderSurface("border_SiO2_aSiStrip", phys_SiO2toplayer, phys_aSiWire, fSiO2aSiInterface);
  G4CMPLogicalBorderSurface* border_SiO2_WSiWire = new G4CMPLogicalBorderSurface("border_SiO2_WSiStrip", phys_SiO2toplayer, phys_WSiWire, fSiO2WSiInterface);
  G4CMPLogicalBorderSurface* border_SiO2substrate_WSiWire = new G4CMPLogicalBorderSurface("border_SiO2substrate_WSiStrip", phys_SiO2substrate, phys_WSiWire, fSiO2WSiInterface);

  //Set up sensitive detector
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  fSuperconductorSensitivity = new SensitiveDetector("SensitiveDetector", PassArgs);
  SDman->AddNewDetector(fSuperconductorSensitivity);
  logic_WSiWire->SetSensitiveDetector(fSuperconductorSensitivity);

  // /control/execute ../SNSPDHighEnergy/G4Macros/vis.mac
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
// Set up a phonon sensor for this surface property object. I'm pretty sure that this
// phonon sensor doesn't get stapled to individual geometrical objects, but rather gets
// stapled to a surface property, but I'm not sure... have to ask mKelsey
void DetectorConstruction::AttachPhononSensor(G4CMPSurfaceProperty * surfProp)
{
  //If no surface, don't do anything
  if(!surfProp) return;

  //Specify properties of the niobium sensors
  auto sensorProp = surfProp->GetPhononMaterialPropertiesTablePointer();
  sensorProp->AddConstProperty("filmAbsorption", 0.0);              //NOT WELL MOTIVATED - probably parametrize and put on slider?
  sensorProp->AddConstProperty("filmThickness", 90.*CLHEP::nm);     //Accurate for our thin film.
  sensorProp->AddConstProperty("gapEnergy", 1.6e-3*CLHEP::eV);      //Reasonably motivated. Actually, looks like Novotny and Meincke are quoting 2Delta, and this is delta. Nuss and Goossen mention that Nb has a delta value closer to this.
  sensorProp->AddConstProperty("lowQPLimit", 3.);                   //NOT WELL MOTIVATED YET -- Dunno how to inform this...
  sensorProp->AddConstProperty("phononLifetime", 4.17*CLHEP::ps);   //Kaplan paper says 242ps for Al, same table says 4.17ps for characteristic time for Nb.
  sensorProp->AddConstProperty("phononLifetimeSlope", 0.29);        //Based on guessing from Kaplan paper, I think this is material-agnostic?
  sensorProp->AddConstProperty("vSound", 3.480*CLHEP::km/CLHEP::s); //True for room temperature, probably good to 10%ish - should follow up
  sensorProp->AddConstProperty("subgapAbsorption", 0.0);            //Assuming that since we're mostly sensitive to quasiparticle density, phonon "heat" here isn't something that we're sensitive to? Unsure how to select this.

  //  sensorProp->AddConstProperty("gapEnergy",3.0e-3*CLHEP::eV);      //Reasonably motivated. Novotny and Meincke, 1975 (2.8-3.14 meV)
  //  sensorProp->AddConstProperty("phononLifetime",242.*ps);      //Kaplan paper says 242ps for Al, same table says 4.17ps for characteristic time for Nb.
  
  surfProp->SetPhononElectrode(new G4CMPPhononElectrode);
  
}
