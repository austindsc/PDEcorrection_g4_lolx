
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "PDEDetectorConstruction.hh"
#include "PDEDetectorConstructionMessenger.hh"

#include "myGlobals.hh"


PDEDetectorConstructionMessenger::PDEDetectorConstructionMessenger(PDEDetectorConstruction * det)
:Detector(det)
{
  
  Dir = new G4UIdirectory("/setDetector/");
  Dir->SetGuidance("Set run to run detector parameters (source height, cf4 density, etc)");
  
  SetMyVerbosityCmd = new G4UIcmdWithAnInteger("/setDetector/myVerbosity", this);
  SetMyVerbosityCmd->SetDefaultValue(MG::GetVerbosity()); //default values in detectorconstruction constructor
  SetMyVerbosityCmd->SetParameterName("myVerbosity", false, true);
  SetMyVerbosityCmd->AvailableForStates(G4State_PreInit);

}

PDEDetectorConstructionMessenger::~PDEDetectorConstructionMessenger(){
  
  delete Dir;
  delete SetMyVerbosityCmd;
}

void PDEDetectorConstructionMessenger::SetNewValue(G4UIcommand *command, G4String newVal){
  
  // do something
  if(command == SetMyVerbosityCmd){
    MG::SetVerbosity((int) SetMyVerbosityCmd->GetNewIntValue(newVal));
    G4cout << "Setting MyVerbosity to " << SetMyVerbosityCmd->GetNewIntValue(newVal) << G4endl;
  }

}








