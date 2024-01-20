#include "SensitiveSIPM.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4RunManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include "PDEEventAction.hh"
#include "HelperFunctions.hh"
#include "myGlobals.hh"

#include <filesystem>

SensitiveSIPM::SensitiveSIPM(G4String name) : G4VSensitiveDetector(name )
{
  fEventAction = (PDEEventAction *) G4RunManager::GetRunManager()->GetUserEventAction();
  fhc = CLHEP::h_Planck * CLHEP::c_light;

  // adding PDE
  ovecPDE = new G4PhysicsFreeVector(false);
  
  G4double scaleX, scaleY;
  // using wavelengths, not energy, is okay. Do the efficiency call on my own
  OH::LoadTxt_OrderedVector("dataOptics/effective_internalPDE.txt", ovecPDE, MG::GetVerbosity(), nm, 1.);
  if(MG::GetVerbosity() > 1) ovecPDE->DumpValues();

  if(MG::GetPDETesting() == MG::NewWay){
    G4cout << "------------------------ SiPM Implementation: NEWWAY ------------------------\n"
    "using custom ProcessHits_photonAtBoundary, and defining efficiency for SiPM bulk."
    "Then no backing SiPM is required. Stepping action has to check for optical photon"
    "DETECTED status and manually call ProcessHits_photonAtBoundary.\n"
    "------------------------  ------------------------" << G4endl;
  } else if(MG::GetPDETesting() == MG::OldWay) {
    G4cout << 
    "------------------------ SiPM Implementation: OLDWAY ------------------------\n"
    "using normal ProcessHits in sensitive detector, which requires a second backing detector to be defined."
    "All of the photons need to be transmitted from bulk to backing (define 100% transmission layer) and then any photons"
    "that touch the backing detector call ProcessHits() which kills the track.\n"
    "------------------------  ------------------------" << G4endl;
  }
}

SensitiveSIPM::~SensitiveSIPM()
{}

G4bool SensitiveSIPM::ProcessHits(G4Step * step, G4TouchableHistory *ROhist){
  G4Track * track = step->GetTrack();

  // photon hits backing (OLDWAY)
  if(MG::GetPDETesting() == MG::OldWay){
    track->SetTrackStatus(fStopAndKill);
    this->CountPhotonIn(step);
    return true;
  }
  else if(MG::GetPDETesting() == MG::AbsWay){ // if the photon dies in the Si, count it

    // option 1: wait for photon to die
    // if (step->GetTrack()->GetTrackStatus() == fStopAndKill) {
    //     // G4cout << "killing with dead track status" << G4endl;
    //     this->CountPhotonIn(step);
    //     return true;
    // }

    // option 2: kill photon once inside Si (requires ABSLength short) and both pre and post steps are equal
    G4VPhysicalVolume* physPreVolume = step->GetPreStepPoint()->GetPhysicalVolume();
    G4VPhysicalVolume* physPostVolume = step->GetPostStepPoint()->GetPhysicalVolume();
    if(physPreVolume == physPostVolume){
      track->SetTrackStatus(fStopAndKill);
      this->CountPhotonIn(step);
      // G4cout << "killing with step volumes" << G4endl;
      return true;
    }
  }
  return false;
}

G4bool SensitiveSIPM::ProcessHits_photonAtBoundary(const G4Step * step){
  G4Track * track = step->GetTrack();
  if(MG::GetVerbosity() > 1)G4cout << "ProcessHits_photonAtBounday" << G4endl;
  if(track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false; // need to know if this is an optical photon
  this->CountPhotonIn(step);
  return true;
}

inline G4bool SensitiveSIPM::CountPhotonIn(const G4Step * step){

  G4StepPoint * preStepPoint = step->GetPreStepPoint();
  G4ThreeVector pPhoton = preStepPoint->GetMomentum();
  G4double wlen = fhc/pPhoton.mag(); //wavelength in nanometers 

  fEventAction->AddInBulk(); // photon touched
  if(MG::GetVerbosity() >= 2)G4cout << "MC truth Hit! wl = " << wlen/nm << " nm" << G4endl;

  // With Efficiencies
  if(G4UniformRand() < ovecPDE->Value(wlen)){
    if(MG::GetVerbosity() >= 2)G4cout << "PDE hit! wl = " << wlen/nm << " nm" <<G4endl;
    fEventAction->AddInBulkAfterPDE();
  }
  return true;
}