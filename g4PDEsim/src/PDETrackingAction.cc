
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

  // G4cout << "\n xx my pre track is getting called\n" << G4endl;

  // set Z to the top
  flowestVolume = fDetCons->fWorld;
  fLowestZPoint = fDetCons->GetTopOfWorld();

}

void PDETrackingAction::PostUserTrackingAction(const G4Track* track){

  // G4cout << "\n xx my post track is getting called\n" << G4endl;

  if(MG::GetVerbosity() > 1)G4cout << "lowest volume: " << flowestVolume->GetName() << G4endl;

  //-- three cases --//
  if(flowestVolume == fDetCons->fLogWindow){
    if(MG::GetVerbosity() > 1)G4cout << "++ window" << G4endl;
    fEvAction->AddOnWindow();
  }
  else if(flowestVolume == fDetCons->fLogSurface){
    if(MG::GetVerbosity() > 1)G4cout << "++ surface" << G4endl;
    fEvAction->AddOnSurface();
  }
  else if(flowestVolume == fDetCons->fLogBulk){
    if(MG::GetVerbosity() > 1)G4cout << "++ bulk" << G4endl;
    fEvAction->AddInSurface();
  }
  else if(flowestVolume == fDetCons->fLogBacking){
    if(MG::GetVerbosity() > 1)G4cout << "++ backing" << G4endl;
    fEvAction->AddInBulk();
  }
  else{
    G4cout << "ERROR: photon not detected" << G4endl;
  }
}