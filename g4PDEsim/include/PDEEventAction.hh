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
  friend class PDETrackingAction; // to change reflection things
  friend class PDERunAction;
  friend class SensitiveSIPM;

  public:
    PDEEventAction(PDERunAction* runAction);
    virtual ~PDEEventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void PrintOutput(int verb = 0);

    void AddOnWindow(){ fNonWindow++; }
    void AddOnSurface(){ fNonSurface++; }
    void AddInSurface(){ fNonBulk++; }
    void AddInBulk(){ fNinBulk++;}         // hitting SensitiveSIPM
    void AddInBulkAfterPDE(){ fNinBulkAfterPDE++;} // after PDE
    

  protected:
    G4EventManager * evMan;
    
    PDERunAction* fRunAction;

    G4int       fNphotonFired;
    G4double    fPhotonWavelength; // what was shot
    G4double    fAngle; // angle from gun

    G4int       fNonWindow;   // mc number of photons hitting window (no efficiencies)
    G4int       fNonSurface;  // mc number of photons hitting detector (no efficiencies)
    G4int       fNonBulk;  // after efficiencies
    G4int       fNinBulk;     // after passing into bulk (hitting SiPM)
    G4int       fNinBulkAfterPDE; // after PDE
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
