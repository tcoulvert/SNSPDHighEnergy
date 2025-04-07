
#ifndef ActionInitialization_hh
#define ActionInitialization_hh 1

#include "G4VUserActionInitialization.hh"
#include "G4Args.hh" // Custom header for argument handling

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization(MyG4Args* MainArgs);
  virtual ~ActionInitialization();
  virtual void Build() const;

private:
  MyG4Args* PassArgs;
};

#endif	/* ActionInitialization_hh */
