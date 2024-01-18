
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4RunManager.hh"

#include "PDEEventAction.hh"

#include "PDETrackingAction.hh"
#include "PDEDetectorConstruction.hh"

#include "myGlobals.hh"

// Implement the functions defined in the PDETrackingAction class here

PDETrackingAction::PDETrackingAction(PDEDetectorConstruction * detCons, PDEEventAction * eventAction){
  
  fDetCons = detCons;
  fEvAction = eventAction;

}

PDETrackingAction::~PDETrackingAction(){}

void PDETrackingAction::PreUserTrackingAction(const G4Track* track){

  // set Z to the top
  flowestVolume = fDetCons->fWorld;
  fLowestZPoint = fDetCons->GetTopOfWorld();

}

void PDETrackingAction::PostUserTrackingAction(const G4Track* track){

  if(MG::GetVerbosity() > 2)G4cout << "lowest volume: " << flowestVolume->GetName() << G4endl;

  G4int verbThreshold = 2;
  //-- three cases --//
  if(flowestVolume == fDetCons->fLogWindow){
    if(MG::GetVerbosity() > verbThreshold)G4cout << "++ window" << G4endl;
    fEvAction->AddOnWindow();
  }
  else if(flowestVolume == fDetCons->fLogSurface){
    if(MG::GetVerbosity() > verbThreshold)G4cout << "++ surface" << G4endl;
    fEvAction->AddOnSurface();
  }
  else if(flowestVolume == fDetCons->fLogBulk){
    if(MG::GetVerbosity() > verbThreshold)G4cout << "++ bulk" << G4endl;
    fEvAction->AddInSurface();
  }
  else if(flowestVolume == fDetCons->fLogBacking){
    if(MG::GetVerbosity() > verbThreshold)G4cout << "++ backing" << G4endl;
    
    // if(MG::GetTesting() == 0 && 1 == 2) fEvAction->AddInBulk(); // don't do this here. all done from processHits now
  }
  else{
    if(MG::GetVerbosity() > verbThreshold) G4cout << "ERROR: photon not detected" << G4endl;
  }
}