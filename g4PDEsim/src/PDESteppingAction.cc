/// \file PDESteppingAction.cc
/// \brief Implementation of the PDESteppingAction class

#include "PDESteppingAction.hh"
#include "PDEEventAction.hh"
#include "PDEDetectorConstruction.hh"
#include "PDETrackingAction.hh"
#include "SensitiveSIPM.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include "G4OpticalPhoton.hh"

#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4SteppingManager.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"

#include "myGlobals.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDESteppingAction::PDESteppingAction(PDEEventAction* eventAction, PDETrackingAction * tracking, PDEDetectorConstruction* detCons)
: G4UserSteppingAction(), fScoringVolume(0){
    fEventAction = eventAction;
    fDetCons = detCons;
    fTrackingAction = tracking;
    fScoringVolume = detCons->GetScoringVolume();
  }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDESteppingAction::~PDESteppingAction()
{}

// set scoring volumes from detector construction like below. but have multiple volumes

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDESteppingAction::UserSteppingAction(const G4Step* step)
{
  // if (!fScoringVolume) { 
  //   const PDEDetectorConstruction* detectorConstruction = static_cast<const PDEDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  //   fScoringVolume = detectorConstruction->GetScoringVolume();   
  // }
  G4double preZ, postZ;

  // get volume of the current step
  G4Track* track = step->GetTrack();
  G4VPhysicalVolume* physPreVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  G4VPhysicalVolume* physPostVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
  G4StepPoint* thePostPoint = step->GetPostStepPoint();

  // for my sorting
  preZ = physPreVolume->GetTranslation().z();
  G4LogicalVolume* logVolume = physPreVolume->GetLogicalVolume();

  // if(physPostVolume){
  //   postZ = physPostVolume->GetPosition().z();
  // }
  
  G4LogicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4LogicalVolume* postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  if(MG::GetVerbosity() > 2) G4cout << "stepping\npreVolume: " << physPreVolume->GetName() << G4endl;
  
  if(physPostVolume && MG::GetVerbosity() > 2){
    G4cout << "postVolume: " << physPostVolume->GetName() << "\n\n" << G4endl;
  }
  
  // getting the lowest point
  if(preZ < fTrackingAction->fLowestZPoint && logVolume != fDetCons->fWorld){
    fTrackingAction->fLowestZPoint = preZ;
    fTrackingAction->flowestVolume = logVolume;
  }


  // check if we are in scoring volume
  // if (volume != fScoringVolume) return;

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
    // ------------------------------------------
  static G4ParticleDefinition* opticalphoton = G4OpticalPhoton::OpticalPhotonDefinition();

  G4OpBoundaryProcessStatus boundaryStatus           = Undefined;
  static G4ThreadLocal G4OpBoundaryProcess* boundary = nullptr;
  // find the boundary process only once
  if(!boundary){
    G4ProcessManager* pm = track->GetDefinition()->GetProcessManager();
    G4int nprocesses    = pm->GetProcessListLength();
    G4ProcessVector* pv = pm->GetProcessList();
    for(G4int i = 0; i < nprocesses; ++i){
      if((*pv)[i]->GetProcessName() == "OpBoundary"){
        boundary = (G4OpBoundaryProcess*) (*pv)[i];
        break;
      }
    }
  }

 // xx adding
  if(track->GetDefinition() == opticalphoton && MG::GetTesting() == 1){ // Optical photon only

    // Was the photon absorbed by the absorption process
    // if(thePostPoint->GetProcessDefinedStep()->GetProcessName() ==
    //    "OpAbsorption")
    // {
    //   fEventAction->IncAbsorption();
    //   trackInformation->AddTrackStatusFlag(absorbed);
    // }
    // return;
    boundaryStatus = boundary->GetStatus();

    if(MG::GetVerbosity() > 1) G4cout << "boundaryStatus: " << boundaryStatus << G4endl;
    // return;

    if(thePostPoint->GetStepStatus() == fGeomBoundary){
      // Check to see if the particle was actually at a boundary
      // Otherwise the boundary status may not be valid
      // if(fExpectedNextStatus == StepTooSmall)
      // {
      //   if(boundaryStatus != StepTooSmall)
      //   {
      //     G4ExceptionDescription ed;
      //     ed << "LXeSteppingAction::UserSteppingAction(): "
      //        << "No reallocation step after reflection!" << G4endl;
      //     G4Exception("LXeSteppingAction::UserSteppingAction()", "LXeExpl01",
      //                 FatalException, ed,
      //                 "Something is wrong with the surface normal or geometry");
      //   }
      // }
      // fExpectedNextStatus = Undefined;
      switch(boundaryStatus){
        case Absorption:
        {
          if(MG::GetVerbosity()> 1) G4cout << "inside - ABS" << G4endl;
          // trackInformation->AddTrackStatusFlag(boundaryAbsorbed);
          // fEventAction->IncBoundaryAbsorption();
          break;
        }
        case Detection:  // Note, this assumes that the volume causing detection
                         // is the photocathode because it is the only one with
                         // non-zero efficiency
        {
          if(MG::GetVerbosity() > 1)G4cout << "inside - DET" << G4endl;
          // Trigger sensitive detector manually since photon is
          // absorbed but status was Detection
          // G4SDManager* SDman = G4SDManager::GetSDMpointer();
          // // G4String sdName    = "/LXeDet/pmtSD";
          // SensitiveSIPM* sipmSD    = (SensitiveSIPM*) SDman->FindSensitiveDetector("SensitiveSIPM");
          // if(pmtSD)
          //   pmtSD->ProcessHits_boundary(theStep, nullptr);
          // trackInformation->AddTrackStatusFlag(hitPMT);
          (fDetCons->fSingleSipm)->ProcessHits_photonAtBoundary(step);
          break;
        }
        // case FresnelReflection:
        // case TotalInternalReflection:
        // case LambertianReflection:
        // case LobeReflection:
        // case SpikeReflection:
        // case BackScattering:
        //   trackInformation->IncReflections();
        //   fExpectedNextStatus = StepTooSmall;
        //   break;
        // default:
        //   break;
      }
    }
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

