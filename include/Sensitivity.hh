/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

#ifndef Sensitivity_h
#define Sensitivity_h 1

#include "G4CMPElectrodeSensitivity.hh"
#include "G4Args.hh"

class Sensitivity final : public G4CMPElectrodeSensitivity {
public:
  Sensitivity(G4String name);
  virtual ~Sensitivity();
  // No copies
  Sensitivity(const Sensitivity&) = delete;
  Sensitivity& operator=(const Sensitivity&) = delete;
  /* Move is disabled for now because old versions of GCC can't move ofstream
  // Move OK
  Sensitivity(Sensitivity&&);
  Sensitivity& operator=(Sensitivity&&);
  */
  Sensitivity(Sensitivity&&) = delete;
  Sensitivity& operator=(Sensitivity&&) = delete;

  virtual void EndOfEvent(G4HCofThisEvent*);

  void SetHitOutputFile(const G4String& fn);
  void SetPrimaryOutputFile(const G4String& fn);

protected:
  virtual G4bool IsHit(const G4Step*, const G4TouchableHistory*) const;

private:
  std::ofstream primaryOutput;
  std::ofstream hitOutput;
  G4String primaryFileName;
  G4String hitFileName;

  // // ProcessHits method is called for each step in the detector
  // virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);

  // MyG4Args* PassArgs;
  
};

#endif
