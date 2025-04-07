/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file SNSPDHighEnergy/SNSPDHighEnergy.cc
/// \brief Main program of the SNSPDHighEnergy example (based on G4CMP's phonon example)
//
// $Id$
//
// 20140509  Add conditional code for Geant4 10.0 vs. earlier
// 20150112  Remove RM->Initialize() call to allow macro configuration
// 20160111  Remove Geant4 version check since we now hard depend on 10.2+
// 20170816  Add example-specific configuration manager
// 20220718  Remove obsolete pre-processor macros G4VIS_USE and G4UI_USE
// 20240521  Renamed for tutorial use

#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

#include "G4CMPPhysicsList.hh"
#include "G4CMPPhysics.hh"
#include "G4CMPConfigManager.hh"
#include "ActionInitialization.hh"
#include "ConfigManager.hh"
#include "DetectorConstruction.hh"
#include "DetectorParameters.hh"
#include "FTFP_BERT.hh"
#include "G4Args.hh"

using namespace DetectorParameters;

int main(int argc, char** argv)
{
  MyG4Args* myG4Args = new MyG4Args(argc, argv);
 // Construct the run manager
 //
 G4RunManager* runManager = new G4RunManager;

 // Set mandatory initialization classes
 //
 DetectorConstruction* detector = new DetectorConstruction(myG4Args);
 runManager->SetUserInitialization(detector);

 FTFP_BERT* physics = new FTFP_BERT;  
 physics->RegisterPhysics(new G4CMPPhysics);
 physics->SetCuts();
 runManager->SetUserInitialization(physics);
 
 // Set user action classes (different for Geant4 10.0)
 //
 runManager->SetUserInitialization(new ActionInitialization(myG4Args));

 // Create configuration managers to ensure macro commands exist
 G4CMPConfigManager::Instance();
 ConfigManager::Instance();

 if (argc==1)   // Define UI session for interactive mode
 {

  // Initialize the visualization manager
  G4VisManager *visManager = new G4VisExecutive();
  visManager->Initialize();

  // Get the UI manager and apply visualization commands
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  G4UIExecutive* ui = new G4UIExecutive(argc, argv);
  ui->SessionStart();
  delete ui;

  delete visManager;
  delete UImanager;

 } else if (myG4Args->GetRunevt() > 0) 
 {

  // Run the specified number of events
  G4int numberOfEvents = myG4Args->GetRunevt();
  G4cout << "### Running " << numberOfEvents << " events." << G4endl;
  runManager->BeamOn(numberOfEvents);

 } else           // Batch mode
 {

  // Initialize the visualization manager
  G4VisManager *visManager = new G4VisExecutive();
  visManager->Initialize();

  // Get the UI manager and apply visualization commands
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  G4String command = "/control/execute ";
  G4String fileName = argv[1];
  UImanager->ApplyCommand(command+fileName);

  delete visManager;
  delete UImanager;

 }

 delete runManager;

 return 0;
}


