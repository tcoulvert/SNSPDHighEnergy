/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

#ifndef SteppingAction_hh
#define SteppingAction_hh 1

#include "G4UserSteppingAction.hh"
#include "G4Args.hh"

#include <fstream>

class G4Step;

class SteppingAction : public G4UserSteppingAction
{
public:

  SteppingAction(MyG4Args*);
  virtual ~SteppingAction();
  virtual void UserSteppingAction(const G4Step* step);
  void ExportStepInformation( const G4Step * step );
  
private:

  //Step info output file
  std::ofstream fOutputFile;
  
  MyG4Args* PassArgs;
  
  
};

#endif

