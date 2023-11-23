#ifndef SensitiveSIPM_H
#define SensitiveSIPM_H

#include "G4VSensitiveDetector.hh"
#include "G4PhysicsOrderedFreeVector.hh"

class G4UniformRand;
class PDEEventAction;

class SensitiveSIPM : public G4VSensitiveDetector
{
public:
  SensitiveSIPM(G4String);
  ~SensitiveSIPM();  
  
private:
  PDEEventAction * fEventAction;

  virtual G4bool ProcessHits(G4Step * step, G4TouchableHistory *);
  
  G4PhysicsOrderedFreeVector * ovecPDE;

};

#endif