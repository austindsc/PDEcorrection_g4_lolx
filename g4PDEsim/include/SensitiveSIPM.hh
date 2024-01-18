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
  
private:
  PDEEventAction * fEventAction;

  G4bool ProcessHits(G4Step * step, G4TouchableHistory *);
  G4bool ProcessHits_photonAtBoundary(const G4Step * step);
  

  G4double fhc;

};

#endif