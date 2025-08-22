/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

// $Id: 539f524339ae53ad098a07cfa3bebd07784d23dd $

#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "G4CMPStackingAction.hh"
#include "EventAction.hh"
#include "RunAction.hh"

ActionInitialization::ActionInitialization(MyG4Args *MainArgs) {
  PassArgs = MainArgs;
}
ActionInitialization::~ActionInitialization() {
}

void ActionInitialization::Build() const {
  SetUserAction(new PrimaryGeneratorAction(PassArgs));
  SetUserAction(new G4CMPStackingAction);
  // SetUserAction(new SteppingAction(PassArgs));
  
  RunAction* runAction = new RunAction(PassArgs);
  SetUserAction(new EventAction(runAction, PassArgs));
  SetUserAction(runAction);
} 
