/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

#ifndef ConfigMessenger_hh
#define ConfigMessenger_hh 1

// $Id$
// File:  ConfigMessenger.hh
//
// Description:	Macro command defitions to set user configuration in
//		ConfigManager.
//
// 20170816  Michael Kelsey

#include "G4UImessenger.hh"

class ConfigManager;
class G4UIcmdWithAString;
class G4UIcommand;


class ConfigMessenger : public G4UImessenger {
public:
  ConfigMessenger(ConfigManager* theData);
  virtual ~ConfigMessenger();

  void SetNewValue(G4UIcommand* cmd, G4String value);

private:
  ConfigManager* theManager;
  G4UIcmdWithAString* hitsCmd;

private:
  ConfigMessenger(const ConfigMessenger&);	// Copying is forbidden
  ConfigMessenger& operator=(const ConfigMessenger&);
};

#endif /* ConfigMessenger_hh */
