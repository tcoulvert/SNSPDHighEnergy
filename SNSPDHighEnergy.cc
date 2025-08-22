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
#include "G4OpticalPhysics.hh" // Optical physics
#include "G4RadioactiveDecayPhysics.hh" // Radioactive decay physics
#include "G4StepLimiterPhysics.hh" // Step limiter physics
#include "G4EmLivermorePhysics.hh"

#include "G4Args.hh"

using namespace DetectorParameters;


void defaultUIManagerSettings(G4UImanager* UImanager) {

  // Open the visualization and set up the scene
  UImanager->ApplyCommand("/vis/open OGL");
  UImanager->ApplyCommand("/vis/viewer/set/viewpointVector 0 0 1");
  UImanager->ApplyCommand("/vis/viewer/zoom 1.4");
  UImanager->ApplyCommand("/vis/drawVolume");
  UImanager->ApplyCommand("/vis/viewer/set/autoRefresh true");
  // UImanager->ApplyCommand("/vis/scene/add/trajectories smooth");
  // UImanager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
  // UImanager->ApplyCommand("/vis/scene/add/axes");

  // Configure step point visualization
  UImanager->ApplyCommand("/tracking/verbose 2");
  UImanager->ApplyCommand("/tracking/storeTrajectory 1");
  UImanager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
  UImanager->ApplyCommand("/vis/scene/add/trajectories");

  UImanager->ApplyCommand("/vis/modeling/trajectories/create/drawByParticleID");
  UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set proton White");
  UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set e- Yellow");
  UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID/set G4CMPDriftElectron Violet");
  UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID/set G4CMPDriftHole Orange");
  UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID/set phononTS Red");
  UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID/set phononTF Green");
  UImanager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID/set phononL Blue");

  UImanager->ApplyCommand("/vis/viewer/set/style wireframe");
  UImanager->ApplyCommand("/vis/viewer/set/hiddenMarker true");
  UImanager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 0 90");
  UImanager->ApplyCommand("/vis/viewer/zoom 1.6");

  UImanager->ApplyCommand("/g4cmp/producePhonons 1");
  UImanager->ApplyCommand("/g4cmp/sampleLuke 1");
  UImanager->ApplyCommand("/g4cmp/produceCharges 0.00001");

}

int main(int argc, char** argv)
{
  MyG4Args* myG4Args = new MyG4Args(argc, argv);
 // Construct the run manager
 //
 G4RunManager* runManager = new G4RunManager;

 // Create configuration managers to ensure macro commands exist
 G4CMPConfigManager::Instance();
 ConfigManager::Instance();

 // Set mandatory initialization classes
 //
 DetectorConstruction* detector = new DetectorConstruction(myG4Args);
 runManager->SetUserInitialization(detector);

 G4cout<< " ### Starting Define Physics" <<G4endl;
 FTFP_BERT* physics = new FTFP_BERT(0);
 physics->RegisterPhysics(new G4CMPPhysics);

//  physics->RegisterPhysics(new G4OpticalPhysics);
//  physics->RegisterPhysics(new G4RadioactiveDecayPhysics); // For radioactive decay
//  physics->RegisterPhysics(new G4EmLivermorePhysics); // For low energy photons
 G4StepLimiterPhysics* stepLimitPhys = new G4StepLimiterPhysics();
//  stepLimitPhys->SetApplyToAll(true); // activates step limit for ALL particles
 physics->RegisterPhysics(stepLimitPhys);

 physics->SetCuts();
 G4cout<< " ### Finish Define Physics" <<G4endl;  

 runManager->SetUserInitialization(physics);
 
 // Set user action classes (different for Geant4 10.0)
 //
 runManager->SetUserInitialization(new ActionInitialization(myG4Args));

 if (argc==1)   // Define UI session for interactive mode
 {

  runManager->Initialize();
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);

  // Initialize the visualization manager
  G4VisManager *visManager = new G4VisExecutive();
  visManager->Initialize();

  // Get the UI manager and apply visualization commands
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  defaultUIManagerSettings(UImanager);
  
  ui->SessionStart();

  // delete ui;
  // delete visManager;
  // delete UImanager;

 } else if (myG4Args->GetRunevt() > 0) 
 {

  runManager->Initialize();

  if (myG4Args->GetRunevt() == 1) 
  {

    G4UIExecutive* ui = new G4UIExecutive(argc, argv);

    // Initialize the visualization manager
    G4VisManager *visManager = new G4VisExecutive();
    visManager->Initialize();

    // Get the UI manager and apply visualization commands
    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    defaultUIManagerSettings(UImanager);
    
    ui->SessionStart();

  }

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



