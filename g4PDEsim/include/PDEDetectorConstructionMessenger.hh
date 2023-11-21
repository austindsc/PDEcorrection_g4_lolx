#ifndef PDEDetectorConstructionMessenger_h
#define PDEDetectorConstructionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4String.hh"

class PDEDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class PDEDetectorConstructionMessenger : public G4UImessenger{
public:
  PDEDetectorConstructionMessenger(PDEDetectorConstruction *);
  ~PDEDetectorConstructionMessenger();
  
  void SetNewValue(G4UIcommand *, G4String );
  
private:
  
  PDEDetectorConstruction * Detector;
  G4UIdirectory           * Dir;
  
  G4UIcmdWithAnInteger    * SetMyVerbosityCmd;
  // 0 - none of my outputs
  // 1 - event level outputs
  
};

#endif