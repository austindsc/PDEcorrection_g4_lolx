/// \file PDEEventAction.cc
/// \brief Implementation of the PDEEventAction class

#include "PDEEventAction.hh"
#include "PDERunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4RootAnalysisManager.hh"

#include "G4GeneralParticleSource.hh"

#include <stdlib.h>

#include "myGlobals.hh"
#include <TRandom3.h>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDEEventAction::PDEEventAction(PDERunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction),
  fmcNPE(0)
{
  evMan = G4EventManager::GetEventManager();
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDEEventAction::~PDEEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDEEventAction::BeginOfEventAction(const G4Event*)
{    
  G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  if((MG::GetVerbosity() == 1 || evMan->GetVerboseLevel() > 0) && (evt < 100 || evt % 1000 == 0)){
    G4cout << "\n-------- Start Event --------" << G4endl;
  }

  fPhotonWavelength = 420*nm; // not used now
  fNphotonFired = 1;
  fmcNPE = 0;
  fNPE = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDEEventAction::EndOfEventAction(const G4Event*)
{   
  // skip filling ntuple if zero energy deposited

  // tracking some stuff for photon testing
  fRunAction->ftotalMCPE += fmcNPE;
  fRunAction->ftotalFired += fRunAction->fGenerator->fGun->GetNumberOfParticles();
  
  G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    if(fNphotonFired > 0 || MG::GetVerbosity() == -1 || MG::GetVerbosity() > 1){ // this is where we skip filling the ntuple
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    // man->FillNtupleIColumn(0, evt);
    // // man->FillNtupleDColumn(1, fNphotonFired/keV);
    // man->FillNtupleIColumn(2, fmcNPE);
    // man->FillNtupleIColumn(3, fNPE);
    // man->AddNtupleRow(0);
  }
  
  if((MG::GetVerbosity() >= 1 || evMan->GetVerboseLevel() > 0) && (evt < 100 || evt % 1000 == 0)){
    G4cout << "\n---- My Event Info #" << evt << "  ----\n\n" 
    << "total MC PE = " << fmcNPE << "\n"
    << "total PE = " << fNPE << G4endl;
    // G4cout << "PDEeventAction::EOEA fEdep = " << fEdep/keV << " keV" << G4endl;
    G4cout << "\n---- Event Complete ----\n" << G4endl;
  }
}

 void PDEEventAction::AddmcPE(){fmcNPE++;}
 void PDEEventAction::AddPE(){fNPE++;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
