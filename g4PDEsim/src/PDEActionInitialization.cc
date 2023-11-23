/// \file PDEActionInitialization.cc
/// \brief Implementation of the PDEActionInitialization class

#include "PDEActionInitialization.hh"
#include "PDEPrimaryGeneratorAction.hh"
#include "PDERunAction.hh"
#include "PDEEventAction.hh"
#include "PDESteppingAction.hh"

#include "PDEDetectorConstruction.hh"
#include "PDETrackingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDEActionInitialization::PDEActionInitialization(PDEDetectorConstruction * detCons)
 : G4VUserActionInitialization(){

  fDetCons = detCons;

 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDEActionInitialization::~PDEActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDEActionInitialization::BuildForMaster() const
{
  // no multithreading

  G4cout << "In multithreading mode (BuildForMaster) " << "EXITING" << G4endl;
  exit(0);
  // PDEPrimaryGeneratorAction * genny = new PDEPrimaryGeneratorAction();
  // SetUserAction(genny);
  // PDERunAction* runAction = new PDERunAction(genny);
  // SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDEActionInitialization::Build() const
{
  PDEPrimaryGeneratorAction * genny = new PDEPrimaryGeneratorAction();
  SetUserAction(genny);

  PDERunAction* runAction = new PDERunAction(genny);
  SetUserAction(runAction);
  
  PDEEventAction* eventAction = new PDEEventAction(runAction);
  SetUserAction(eventAction);

  PDETrackingAction * trackAction = new PDETrackingAction(fDetCons, eventAction);
  SetUserAction(trackAction);
  
  PDESteppingAction* stepping = new PDESteppingAction(eventAction, trackAction, fDetCons);
  SetUserAction(stepping);
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
