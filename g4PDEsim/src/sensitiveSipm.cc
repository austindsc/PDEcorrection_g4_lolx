#include "sensitiveSipm.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4RunManager.hh"

#include "PDEEventAction.hh"
#include "HelperFunctions.hh"
#include "myGlobals.hh"

#include <filesystem>

sensitiveSipm::sensitiveSipm(G4String name) : G4VSensitiveDetector(name )
{
  fEventAction = (PDEEventAction *) G4RunManager::GetRunManager()->GetUserEventAction();

  // adding PDE
  ovecPDE = new G4PhysicsFreeVector(true);
  
  OH::LoadTxt_OrderedVector("dataOptics/SiPMeff_Ham_5V.dat", ovecPDE, MG::GetVerbosity(), 1, 1./100);
  // ovecPDE->DumpValues();
}

sensitiveSipm::~sensitiveSipm()
{}

G4bool sensitiveSipm::ProcessHits(G4Step * step, G4TouchableHistory *ROhist){

  G4Track * track = step->GetTrack();
  track->SetTrackStatus(fStopAndKill);
  // exit(0);
  // G4LogicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  // G4LogicalVolume* postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  // G4cout << "preVolume: " << preVolume->GetName() << G4endl;
  // G4cout << "postVolume: " << postVolume->GetName() << G4endl;


  // if (step->GetPreStepPoint()->GetTouchableHandle()->GetVolume() ==preVolume) {
  //   track->SetTrackStatus(fStopAndKill);
  //   G4cout << "satisfied" << G4endl;
    // return;
    // }
  G4StepPoint * preStepPoint = step->GetPreStepPoint(); // entering detector
  G4StepPoint * postStepPoint = step->GetPostStepPoint(); // leaving detector?
  
  G4ThreeVector posPhoton = preStepPoint->GetPosition();
  G4ThreeVector pPhoton = preStepPoint->GetMomentum();
  G4double wlen = (1239.8*eV/pPhoton.mag()) * nm; //wavelength in nanometers 
  
  if(MG::GetVerbosity() >= 2)G4cout << "MC Hit! wl = " << wlen/nm << " nm" << G4endl;
  fEventAction->AddmcPE(); 

  // With Efficiencies
  if(G4UniformRand() < ovecPDE->Value(wlen)){
    if(MG::GetVerbosity() >= 2)G4cout << "Raw hit! wl = " << wlen/nm << " nm" <<G4endl;
    fEventAction->AddPE();
  }
  
  // G4cout << "photon hit!: " << posPhoton << G4endl;
  
  
  return true;
}