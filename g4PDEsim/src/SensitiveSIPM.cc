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

  if(MG::GetTesting() == 1){
    G4cout << "------------------------ SiPM Implementation ------------------------\n"
    "using custom ProcessHits_photonAtBoundary, and defining efficiency for SiPM bulk."
    "Then no backing SiPM is required. Stepping action has to check for optical photon"
    "DETECTED status and manually call ProcessHits_photonAtBoundary.\n"
    "------------------------  ------------------------" << G4endl;
  } else {
    G4cout << 
    "------------------------ SiPM Implementation ------------------------\n"
    "using normal ProcessHits in sensitive detector, which requires a second backing detector to be defined."
    "All of the photons need to be transmitted from bulk to backing (define 100% transmission layer) and then any photons"
    "that touch the backing detector call ProcessHits() which kills the track.\n"
    "------------------------  ------------------------" << G4endl;
  }
}

SensitiveSIPM::~SensitiveSIPM()
{}

G4bool SensitiveSIPM::ProcessHits(G4Step * step, G4TouchableHistory *ROhist){
  // add this call to avoid double counting or accidental counting, if ProcessHits gets called
  // by some other particle or action
  if(MG::GetTesting() == 0){
    this->ProcessHits_photonAtBoundary(step);
  }
  return false;
}

G4bool SensitiveSIPM::ProcessHits_photonAtBoundary(const G4Step * step){
  if(MG::GetVerbosity() > 1)G4cout << "ProcessHits_photonAtBounday" << G4endl;
  
  G4Track * track = step->GetTrack();
  if(track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false; // need to know if this is an optical photon
  track->SetTrackStatus(fStopAndKill); // this is for vanilla method (called by ProcessHits())
  
  fEventAction->AddInBulk(); // photon touched

  G4StepPoint * preStepPoint = step->GetPreStepPoint(); // entering detector
  G4StepPoint * postStepPoint = step->GetPostStepPoint(); // leaving detector?
  
  G4ThreeVector pPhoton = preStepPoint->GetMomentum();
  G4double wlen = fhc/pPhoton.mag(); //wavelength in nanometers 

  // G4cout << "PDE at this is " << ovecPDE->Value((wlen)) << G4endl;
  
  if(MG::GetVerbosity() >= 2)G4cout << "MC truth Hit! wl = " << wlen/nm << " nm" << G4endl;

  // With Efficiencies
  if(G4UniformRand() < ovecPDE->Value(wlen)){
    if(MG::GetVerbosity() >= 2)G4cout << "PDE hit! wl = " << wlen/nm << " nm" <<G4endl;
    fEventAction->AddInBulkAfterPDE();
  }
  
  // G4ThreeVector posPhoton = preStepPoint->GetPosition();
  // G4cout << "photon hit!: " << posPhoton << G4endl;
  
  
  return true;
}