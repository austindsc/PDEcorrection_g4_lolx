/// \file PDESteppingAction.hh
/// \brief Definition of the PDESteppingAction class

#ifndef PDESteppingAction_h
#define PDESteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class PDEEventAction;
class PDEDetectorConstruction;
class PDETrackingAction;

class G4LogicalVolume;

/// Stepping action class
/// 

class PDESteppingAction : public G4UserSteppingAction
{
  public:
    PDESteppingAction(PDEEventAction* eventAction, PDETrackingAction * tracking, PDEDetectorConstruction* detCons);
    virtual ~PDESteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

  private:

    PDEEventAction*  fEventAction;
    PDETrackingAction* fTrackingAction;
    PDEDetectorConstruction* fDetCons;
    G4LogicalVolume* fScoringVolume;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
