/// \file PDESteppingAction.cc
/// \brief Implementation of the PDESteppingAction class

#include "PDESteppingAction.hh"
#include "PDEEventAction.hh"
#include "PDEDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include "G4OpticalPhoton.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDESteppingAction::PDESteppingAction(PDEEventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDESteppingAction::~PDESteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDESteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume) { 
    const PDEDetectorConstruction* detectorConstruction
      = static_cast<const PDEDetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detectorConstruction->GetScoringVolume();   
  }

  // get volume of the current step
  G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();
      
  // check if we are in scoring volume
  if (volume != fScoringVolume) return;

  // ------------------------------------------
  static G4ParticleDefinition* opticalphoton = G4OpticalPhoton::OpticalPhotonDefinition();

  // G4AnalysisManager* analysisMan = G4AnalysisManager::Instance();
  // Run* run =
  // static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  // G4Track* track          = step->GetTrack();
  // G4StepPoint* endPoint   = step->GetPostStepPoint();
  // G4StepPoint* startPoint = step->GetPreStepPoint();

  // const G4DynamicParticle* theParticle = track->GetDynamicParticle();
  // const G4ParticleDefinition* particleDef = theParticle->GetParticleDefinition();

  // TrackInformation* trackInfo =
  // (TrackInformation*) (track->GetUserInformation());

  if(step->GetTrack()->GetParticleDefinition() != opticalphoton){
    // ------------------------------------------
    // collect energy deposited in this step
    G4double edepStep = step->GetTotalEnergyDeposit();
    
    // fEventAction->AddEdep(edepStep);
  }
  
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

