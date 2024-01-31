#ifndef SensitiveSIPM_H
#define SensitiveSIPM_H

#include "G4VSensitiveDetector.hh"
#include "G4PhysicsFreeVector.hh"

class G4UniformRand;
class PDEEventAction;

class SensitiveSIPM : public G4VSensitiveDetector
{
  friend class PDESteppingAction;
public:
  SensitiveSIPM(G4String);
  ~SensitiveSIPM();  
  G4PhysicsFreeVector * ovecPDE;
  
  G4bool ProcessHits(G4Step * step, G4TouchableHistory *);
  G4bool ProcessHits_photonAtBoundary(const G4Step * step);
  void ResetForFirstEvent(){fLastTrackID = -1;}

  G4bool CountPhotonTouching(G4int trackID);
  G4bool CountPhotonIn(const G4Step * step);

private:
  PDEEventAction * fEventAction;
  G4int fLastTrackID; // use this to track photons from the same track (avoid double counting reflections)
  G4int fLastEventID; // use this for manual testing (new runs have same first event number)
  

  G4double fhc;

};

#endif