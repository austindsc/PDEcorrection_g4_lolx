/// \file PDERunAction.cc
/// \brief Implementation of the PDERunAction class

#include "PDERunAction.hh"
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
  ftotalMCPE = 0;
  ftotalFired = 0;
  
  G4AnalysisManager * man = G4AnalysisManager::Instance();
  man->SetDefaultFileType("root");
  man->SetFileName("pde_outputStock");
  man->CreateNtuple("simOutput", "simOutput");
  // but if you don't do this, you can't name the file from the macro
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDERunAction::~PDERunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDERunAction::BeginOfRunAction(const G4Run*)
{ 
  ftotalMCPE = 0;
  ftotalFired = 0;
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  
  G4int seed=time(0);
  G4Random::setTheSeed(seed);

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();
  
  G4AnalysisManager *man = G4AnalysisManager::Instance();
  
  man->CreateNtupleIColumn("fNphoton");       //0 - how many photons shot
  man->CreateNtupleDColumn("fPhotonWln");     //1 - duh
  man->CreateNtupleDColumn("fAngle");         //2 - angle from gun
  man->CreateNtupleIColumn("fOnSurface");     //3 - hitting surface - MC truth value on bulk(no PDE)
  man->CreateNtupleIColumn("fInSurface");     //4 - in surface
  man->CreateNtupleIColumn("fInBulk");        //3 - passing into bulk (what we want)
  man->FinishNtuple(0);

  man->OpenFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDERunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;
        
  // Print
  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------";
  }
  
  G4cout << "\nFinished run of " << nofEvents << " events" << G4endl;

  // getting the ntuple
  G4AnalysisManager * man = G4AnalysisManager::Instance();

  if(MG::GetVerbosity() > 0) {
    
    G4cout << ftotalFired << " particles fired, " << ftotalMCPE << " photons hit" << G4endl;
    G4cout << (double) ftotalMCPE/ftotalFired << G4endl;

  }
    

  
   man->Write();
   man->CloseFile();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// void PDERunAction::AddEdep(G4double edep)
// {
//   fEdep  += edep;
//   fEdep2 += edep*edep;
// }


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

