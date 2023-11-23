/// \file PDESteppingAction.cc
/// \brief Implementation of the PDESteppingAction class

#include "PDESteppingAction.hh"
#include "PDEEventAction.hh"
#include "PDEDetectorConstruction.hh"
#include "PDETrackingAction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include "G4OpticalPhoton.hh"

#include "myGlobals.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDESteppingAction::PDESteppingAction(PDEEventAction* eventAction, PDETrackingAction * tracking, PDEDetectorConstruction* detCons)
: G4UserSteppingAction(), fScoringVolume(0){
    fEventAction = eventAction;
    fDetCons = detCons;
    fTrackingAction = tracking;
  }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDESteppingAction::~PDESteppingAction()
{}

// set scoring volumes from detector construction like below. but have multiple volumes

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDESteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume) { 
    const PDEDetectorConstruction* detectorConstruction
      = static_cast<const PDEDetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detectorConstruction->GetScoringVolume();   
  }
  G4double preZ, postZ;

  // get volume of the current step
  G4VPhysicalVolume* physPreVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  preZ = physPreVolume->GetTranslation().z();
  G4LogicalVolume* logVolume = physPreVolume->GetLogicalVolume();

  G4VPhysicalVolume* physPostVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
  // if(physPostVolume){
  //   postZ = physPostVolume->GetPosition().z();
  // }
  
  // G4LogicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  // G4LogicalVolume* postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  if(MG::GetVerbosity() > 1) G4cout << "stepping - preVolume: " << physPreVolume->GetName() << G4endl;
  
  if(physPostVolume && MG::GetVerbosity() > 1){
    G4cout << "postVolume: " << physPostVolume->GetName() << G4endl;
  }
  
  // getting the lowest point
  if(preZ < fTrackingAction->fLowestZPoint && logVolume != fDetCons->fWorld){
    fTrackingAction->fLowestZPoint = preZ;
    fTrackingAction->flowestVolume = logVolume;
  }


  // check if we are in scoring volume
  // if (volume != fScoringVolume) return;

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

  // if(step->GetTrack()->GetParticleDefinition() != opticalphoton){
  //   // ------------------------------------------
  //   // collect energy deposited in this step
  //   G4double edepStep = step->GetTotalEnergyDeposit();
    
    // fEventAction->AddEdep(edepStep);
  // }
  
 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

