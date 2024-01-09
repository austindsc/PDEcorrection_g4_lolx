/// \file pdeCorrection.cc
/// \brief Main program of the PDE example

#include "PDEDetectorConstruction.hh"
#include "PDEActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4AnalysisManager.hh"

#include "G4UImanager.hh"

// physics lists
#include "PhysicsList.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4SystemOfUnits.hh"

//xx physics list testing
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalParameters.hh"
#include "G4OpticalPhysics.hh"
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  std::cout << "hello rooster" << std::endl;

  // Detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv);  }

  // getting the ntuple
  G4AnalysisManager * man = G4AnalysisManager::Instance();
  
  // Construct the default run manager
  G4RunManager * runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  runManager->SetVerboseLevel(0);

  // Detector construction
  PDEDetectorConstruction * detCons = new PDEDetectorConstruction();
  runManager->SetUserInitialization(detCons);

  // Physics list - from Optical/LXe example
  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  G4OpticalParameters * opticalParams = G4OpticalParameters::Instance();

  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new PDEActionInitialization(detCons));
  runManager->SetVerboseLevel(0);
  
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive();
  visManager->SetVerboseLevel(0);
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  UImanager->SetVerboseLevel(0);

  #if USE_METAL_COMPLEX
    std::cout << "using metal complex - Complex and Real Index" << std::endl;
  #else
    std::cout << "Using normal RINDEX" << std::endl;
  #endif

  // Process macro or start UI session
  if ( ! ui ) { 
    // batch mode
    G4cout << "\n\n---------------------- Script Settings ----------------------" << G4endl;
    // G4String command = "/control/execute /Users/austindestcroix/Desktop/school_holders/projects/sbc/geant4_cf4";
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode
    G4cout << "\n\n---------------------- Script Settings ----------------------\n" << G4endl;
    UImanager->ApplyCommand("/control/execute ../g4PDEsim/defaultRun.mac");

    ui->SessionStart();
    delete ui;
  }

 man->Write();
 man->CloseFile();

  delete visManager;
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
