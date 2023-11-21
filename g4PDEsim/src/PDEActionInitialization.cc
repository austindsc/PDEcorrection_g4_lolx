/// \file PDEActionInitialization.cc
/// \brief Implementation of the PDEActionInitialization class

#include "PDEActionInitialization.hh"
#include "PDEPrimaryGeneratorAction.hh"
#include "PDERunAction.hh"
#include "PDEEventAction.hh"
#include "PDESteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDEActionInitialization::PDEActionInitialization()
 : G4VUserActionInitialization()
{}

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
  
  SetUserAction(new PDESteppingAction(eventAction));
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
