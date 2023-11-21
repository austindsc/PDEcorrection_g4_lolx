/// \file PDEEventAction.hh
/// \brief Definition of the PDEEventAction class

#ifndef PDEEventAction_h
#define PDEEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class PDERunAction;
class G4EventManager;

/// Event action class
///

class PDEEventAction : public G4UserEventAction
{
  public:
    PDEEventAction(PDERunAction* runAction);
    virtual ~PDEEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void AddmcPE();
    void AddPE();
    

  private:
    G4EventManager * evMan;
    
    PDERunAction* fRunAction;

    G4double     fPhotonWavelength; // what was shot
    G4int        fNphotonFired;
    G4int        fmcNPE; // mc number of photons hitting detector (no efficiencies)
    G4int        fNPE;  // after efficiencies
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
