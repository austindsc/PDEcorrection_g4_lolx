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
  fRunAction(runAction)
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
  G4double hc = CLHEP::h_Planck * CLHEP::c_light;
  G4GeneralParticleSource * tGun = fRunAction->fGenerator->fGun;

  // G4cout <<"xx resetting variables in BOEA" << G4endl;

  fNphotonFired = tGun->GetNumberOfParticles();
  fPhotonWavelength = hc/(tGun->GetCurrentSource()->GetParticleEnergy())/nm;
  fAngle = 180 - tGun->GetCurrentSource()->GetParticleMomentumDirection().theta()/deg;
  // no sure why I need -180 here. I guess Z is defined down for GPS but differently here

  fNonWindow = 0;
  fNonSurface = 0;
  fNonBulk = 0;
  fNinBulk = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDEEventAction::PrintOutput(int verb){
  if(MG::GetVerbosity() > 0 || verb > 0){
    G4cout << "wlen = " << fPhotonWavelength << " nm" << G4endl;
    G4cout << "fAngle = " << fAngle << " deg\n" << G4endl;
    G4cout << "Nphoton = " << fNphotonFired << "\n";
    G4cout << "Nwindow = " << fNonWindow << "\n";
    G4cout << "Nsurface = " << fNonSurface << "\n";
    G4cout << "NBulk = " << fNonBulk << "\n";
    G4cout << "Ninside = " << fNinBulk << "\n";
    G4cout << "fraction in = " << (float) fNinBulk/fNphotonFired << G4endl;
  }
}

void PDEEventAction::EndOfEventAction(const G4Event*)
{   

  // tracking some stuff for photon testing for many events
  fRunAction->ftotalHitPE += fNinBulk;
  fRunAction->ftotalFired += fNphotonFired;

  // G4cout <<"xx printing variables in BOEA" << G4endl;
  this->PrintOutput();

  // for(int i = 0; i < 10; i++){
  G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    if(fNphotonFired > 0 || MG::GetVerbosity() == -1 || MG::GetVerbosity() > 1){ // this is where we skip filling the ntuple
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->FillNtupleIColumn(0, fNphotonFired);
    man->FillNtupleDColumn(1, fPhotonWavelength);
    man->FillNtupleDColumn(2, fAngle);

    man->FillNtupleIColumn(3, fNonWindow);
    man->FillNtupleIColumn(4, fNonSurface);
    man->FillNtupleIColumn(5, fNonBulk);
    man->FillNtupleIColumn(6, fNinBulk);
    man->AddNtupleRow(0);
  }
  
  // if((MG::GetVerbosity() >= 1 || evMan->GetVerboseLevel() > 0) && (evt < 100 || evt % 1000 == 0)){
  //   G4cout << "\n---- My Event Info #" << evt << "  ----\n\n";

  //   // << "total MC PE = " << fmcNPE << "\n"
  //   // << "total PE = " << fNPE << G4endl;

  //   // G4cout << "PDEeventAction::EOEA fEdep = " << fEdep/keV << " keV" << G4endl;
  //   G4cout << "\n---- Event Complete ----\n" << G4endl;
  // }
}

//  void PDEEventAction::AddmcPE(){fmcNPE++;}
//  void PDEEventAction::AddPE(){fNPE++;}
