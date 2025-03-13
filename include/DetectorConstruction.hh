/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file exoticphysics/phonon/include/PhononDetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
// $Id: 4c06153e9ea08f2a90b22c53e5c39bde4b847c07 $
//

#ifndef DetectorConstruction_hh
#define DetectorConstruction_hh 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalSurface.hh"
#include "globals.hh"
#include "G4Args.hh"

class G4Material;
class G4VPhysicalVolume;
class G4CMPSurfaceProperty;
class G4CMPElectrodeSensitivity;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();
  
public:
  virtual G4VPhysicalVolume* Construct();

  G4LogicalVolume *GetScoringVolume() const { return fScoringVolume; }
  
private:
  void DefineMaterials();
  void SetupGeometry();
  void AttachPhononSensor(G4CMPSurfaceProperty * surfProp);

  
private:
  // G4Box *solidWorld, *solidRadiator, *solidDetector, *solidScintillator, *solidAir, *solidCu1, *solidCu2, *solidAl1, *solidAl2, *solid_strip, *solid_aSistrip;
  // G4LogicalVolume *logicWorld, *logicRadiator, *logicDetector, *logicScintillator, *logicAir[10], *logicCu1, *logicCu2, *logicAl1, *logicAl2, *logicsubstrate, *logicCompositeDetector, *logicComposite_aSiStrip;
  // G4VPhysicalVolume *physWorld, *physDetector, *physRadiator, *physScintillator, *physAir[10], *physCu1, *physCu2, *physAl1, *physAl2;

  G4Material* fSiO2;
  G4Material* fAir;
  G4Material* fCu;
  G4Material* fAl;
  G4Material* faSi;
  G4Material* fWSi;
  G4Material* fSi3N4;
  G4Material* fVacuum;
  
  G4VPhysicalVolume* fWorldPhys;

  G4LogicalVolume *fScoringVolume;

  G4OpticalSurface *mirrorSurface;

  // Substrate to copper housing interface
  G4CMPSurfaceProperty* fSi3N4CuInterface;
  // Substrate to SiO2 layer interface
  G4CMPSurfaceProperty* fSi3N4SiO2Interface;
  // SiO2 layer to Amorphous Silicon (non-superconducting top?) interface
  G4CMPSurfaceProperty* fSiO2aSiInterface;
  // SiO2 layer to Tungsten Silicide (superconducting wire?) interface    
  G4CMPSurfaceProperty* fSiO2WSiInterface;
  // aSi layer to WSi (interface between superconducting wire and non-superconducting gaps) interface   
  G4CMPSurfaceProperty* faSiWSiInterface;
  // WSi layer to vacuum interface 
  G4CMPSurfaceProperty* fWSiVacuumInterface;
  // aSi layer to vacuum interface 
  G4CMPSurfaceProperty* faSiVacuumInterface;
  
  G4CMPElectrodeSensitivity* fSuperconductorSensitivity;
  G4bool fConstructed;
  // MyG4Args* PassArgs;
  //G4bool fIfField;
  
  //public:
  //inline void Field(G4bool bl) { fIfField = bl; }
};

#endif

