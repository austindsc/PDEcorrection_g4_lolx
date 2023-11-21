/// \file PDEPrimaryGeneratorAction.cc
/// \brief Implementation of the PDEPrimaryGeneratorAction class

#include "PDEPrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4RotationMatrix.hh"

#include "G4Tubs.hh"
#include "G4GeneralParticleSource.hh"
#include "G4UImanager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SingleParticleSource.hh"

#include "G4RunManager.hh"
#include "PDEDetectorConstruction.hh"
#include "myGlobals.hh"

#include "Randomize.hh"
#include "G4SPSRandomGenerator.hh"

#include "G4IonTable.hh"

#include <stdlib.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDEPrimaryGeneratorAction::PDEPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(), fPDE_volume(0), fdetconstruction(0)
{
  G4int n_particle = 1;
  fGun = new G4GeneralParticleSource();
  fGun->SetNumberOfParticles(n_particle);

  G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton");
  fGun->SetParticleDefinition(particle); 

  // angular distribution
  fGun->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
  fGun->GetCurrentSource()->GetAngDist()->SetMaxTheta(90. * deg);
  
  // energy distribution
  fGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy(3.*eV);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDEPrimaryGeneratorAction::~PDEPrimaryGeneratorAction()
{
  delete fGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDEPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  
  if(!fdetconstruction){
    fdetconstruction = (PDEDetectorConstruction *)G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    if(!fdetconstruction){
      G4ExceptionDescription msg;
      msg << "Can't find PDEDetectorConstruction\n"; 
      G4Exception("PDEPrimaryGeneratorAction::GeneratePrimaries()", "Austin's warning message", FatalException, msg);
    }
  }
  
  // maybe do something to create all photons at an angle and track incident angle through data output
  
  fGun->GeneratePrimaryVertex(anEvent);
}

