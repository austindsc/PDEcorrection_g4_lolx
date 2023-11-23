/// \file PDERunAction.cc
/// \brief Implementation of the PDERunAction class

#include "PDERunAction.hh"
#include "PDEEventAction.hh"
#include "PDEPrimaryGeneratorAction.hh"
#include "PDEDetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4GeneralParticleSource.hh"

#include "G4AnalysisManager.hh"
// #include "G4AnalysisUtilities.hh"
#include "G4RootAnalysisManager.hh"

// #include "G4Random.hh"
#include "Randomize.hh"
#include "myGlobals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDERunAction::PDERunAction(PDEPrimaryGeneratorAction * genny)
: G4UserRunAction(){ 
  fGenerator = genny; // for photon testing if multiple photons are shot
  
  G4AnalysisManager * man = G4AnalysisManager::Instance();
  man->SetDefaultFileType("root");
  man->SetFileName("pde_outputStock");
  man->CreateNtuple("simOutput", "simOutput");
  // but if you don't do this, you can't name the file from the macro

  man->CreateNtupleIColumn("fNphoton");       //0 - how many photons shot
  man->CreateNtupleDColumn("fPhotonWln");     //1 - duh
  man->CreateNtupleDColumn("fAngle");         //2 - angle from gun

  man->CreateNtupleIColumn("fNonWindow");     //3 - hitting surface layer(no PDE)
  man->CreateNtupleIColumn("fNonSurface");    //4 - hitting surface layer(no PDE)
  man->CreateNtupleIColumn("fNonBulk");       //5 - inside surface layer
  man->CreateNtupleIColumn("fNinBulk");       //6 - passing into bulk (what we want)
  man->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDERunAction::~PDERunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDERunAction::BeginOfRunAction(const G4Run*)
{ 

  if(MG::GetVerbosity() > 0) G4cout << "called BeginOfRunAction" << G4endl;
  ftotalHitPE = 0;
  ftotalFired = 0;
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  
  G4int seed=time(0);
  G4Random::setTheSeed(seed);

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();
  
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man->OpenFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDERunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofPhotons = run->GetNumberOfEvent() * fGenerator->fGun->GetNumberOfParticles(); // same as ftotalFired
  if (nofPhotons == 0) return;

  PDEEventAction* eventAction = (PDEEventAction *) G4RunManager::GetRunManager()->GetUserEventAction();

  // getting the ntuple
  G4AnalysisManager * man = G4AnalysisManager::Instance();

  if(MG::GetVerbosity() > 0 || true) {
    // G4cout << nofPhotons << " photons fired, " << nofPhotons << " photons hit" << G4endl;
    G4cout << "acc check. should equal fraction in: " << (double) eventAction->fNinBulk/ftotalFired << G4endl;
  }
  
  // Print
  if (IsMaster()) {
    G4cout << "\nFinished run of " << nofPhotons << " photons. Sanity check" << bool(nofPhotons == ftotalFired) << G4endl;
    
    eventAction->PrintOutput(1);

    G4cout << "\n--------------------End of Global Run-----------------------" << G4endl;
    }
  else { 
    G4cout << "\nFinished run of " << nofPhotons << " photons. Sanity check" << bool(nofPhotons == ftotalFired) << G4endl;
    
    eventAction->PrintOutput(1);

    G4cout << "\n--------------------End of Local Run------------------------" << G4endl;
    }
  
    
   man->Write();
   man->CloseFile();

}
