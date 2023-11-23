#ifndef PDETRACKINGACTION_HPP
#define PDETRACKINGACTION_HPP

#include "globals.hh"
#include "G4UserTrackingAction.hh"

class PDEDetectorConstruction;
class PDEEventAction;

class PDETrackingAction : public G4UserTrackingAction {

friend class PDEDetectorConstruction;
friend class PDESteppingAction;

public:
  PDETrackingAction(PDEDetectorConstruction * detCons, PDEEventAction * eventAction);
  ~PDETrackingAction();

  void PreUserTrackingAction(const G4Track*);   
  void PostUserTrackingAction(const G4Track*);

protected:
  PDEDetectorConstruction * fDetCons;
  PDEEventAction * fEvAction;

  G4LogicalVolume * flowestVolume;
  G4double          fLowestZPoint;
};



#endif // PDETRACKINGACTION_HPP
