/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

// $Id$
// File:  ConfigManager.cc
//
// Description:	Singleton container class for user configuration of G4CMP
//		phonon example. Looks for environment variables	at
//		initialization to set default values; active values may be
//		changed via macro commands (see ConfigMessenger).
//
// 20170816  M. Kelsey -- Extract hit filename from G4CMPConfigManager.

#include "ConfigManager.hh"
#include "ConfigMessenger.hh"
#include "G4RunManager.hh"
#include <stdlib.h>


// Constructor and Singleton Initializer

ConfigManager* ConfigManager::theInstance = 0;

ConfigManager* ConfigManager::Instance() {
  if (!theInstance) theInstance = new ConfigManager;
  return theInstance;
}

ConfigManager::ConfigManager()
  : Hit_file(getenv("G4CMP_HIT_FILE")?getenv("G4CMP_HIT_FILE"):"_hits.txt"),
    Primary_file("_primary.txt"),
    messenger(new ConfigMessenger(this)) {;}

ConfigManager::~ConfigManager() {
  delete messenger; messenger=0;
}


// Trigger rebuild of geometry if parameters change

void ConfigManager::UpdateGeometry() {
  G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}
