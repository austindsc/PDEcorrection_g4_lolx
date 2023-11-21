#ifndef SENSITIVESIPM_H
#define SENSITIVESIPM_H

#include "G4VSensitiveDetector.hh"
#include "G4PhysicsOrderedFreeVector.hh"

class G4UniformRand;
class PDEEventAction;

class sensitiveSipm : public G4VSensitiveDetector
{
public:
  sensitiveSipm(G4String);
  ~sensitiveSipm();  
  
private:
  PDEEventAction * fEventAction;

  virtual G4bool ProcessHits(G4Step * step, G4TouchableHistory *);
  
  G4PhysicsOrderedFreeVector * ovecPDE;

};

#endif