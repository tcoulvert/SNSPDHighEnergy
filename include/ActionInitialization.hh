
#ifndef ActionInitialization_hh
#define ActionInitialization_hh 1

#include "G4VUserActionInitialization.hh"

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization() {;}
  virtual ~ActionInitialization() {;}
  virtual void Build() const;
};

#endif	/* ActionInitialization_hh */
