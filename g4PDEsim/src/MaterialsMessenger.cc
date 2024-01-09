
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "MaterialsMessenger.hh"
#include "PDEDetectorMaterials.hh"

MaterialsMessenger::MaterialsMessenger(PDEDetectorMaterials * dmat):fdmat(dmat)
{
  
  calledDensity = false;
  calledTempOrPressure = false;
  
  Dir = new G4UIdirectory("/setMaterial/");
  Dir->SetGuidance("Set detector medium, density and LY");
  
  SetDetectionMediumCmd = new G4UIcmdWithAString("/setMaterial/detectionMedium", this);
  SetDetectionMediumCmd->SetGuidance("Select detection medium. 'gCF4', 'lCF4', 'gAr', 'air', 'lXe'");
  SetDetectionMediumCmd->SetParameterName("medium", false, true);
  SetDetectionMediumCmd->SetDefaultValue("gCF4");
  SetDetectionMediumCmd->AvailableForStates(G4State_PreInit);
  
  SetMediumDensityCmd = new G4UIcmdWithADoubleAndUnit("/setMaterial/mediumDensity", this);
  SetMediumDensityCmd->SetGuidance("give medium density and associated units (probably use g/cm3)");
  SetMediumDensityCmd->SetParameterName("mediumDensity", false, true);
  SetMediumDensityCmd->SetDefaultValue(dmat->fmediumDensity);//default values in detectorconstruction constructor
  SetMediumDensityCmd->SetUnitCategory("Volumic Mass");
  SetMediumDensityCmd->AvailableForStates(G4State_PreInit);
  
  // environmental parameters
  SetMediumTempCmd = new G4UIcmdWithADoubleAndUnit("/setMaterial/mediumTemp", this);
  SetMediumTempCmd->SetGuidance("temp (if it matters), kelvin");
  SetMediumTempCmd->SetParameterName("mediumTemp", false, true);
  SetMediumTempCmd->SetDefaultValue(dmat->fmediumTemp);//default values in detectorconstruction constructor
  SetMediumTempCmd->SetUnitCandidates("kelvin"); 
  SetMediumTempCmd->AvailableForStates(G4State_PreInit);
  
  SetMediumPressureCmd = new G4UIcmdWithADoubleAndUnit("/setMaterial/mediumPressure", this);
  SetMediumPressureCmd->SetGuidance("pressure (if it matters), bar, atm");
  SetMediumPressureCmd->SetParameterName("mediumPressure", false, true);
  SetMediumPressureCmd->SetDefaultValue(dmat->fmediumPressure);//default values in detectorconstruction constructor
  SetMediumPressureCmd->SetUnitCandidates("bar atm"); 
  SetMediumPressureCmd->AvailableForStates(G4State_PreInit);
}

MaterialsMessenger::~MaterialsMessenger(){
  
  delete Dir;
  delete SetDetectionMediumCmd;
  delete SetMediumDensityCmd;
  delete SetMediumLYCmd;
  delete SetMediumVisibleFractionCmd;
  delete SetMediumLYCmd_UV;
  delete SetMediumLYCmd_Vis;
}

void MaterialsMessenger::SetNewValue(G4UIcommand *command, G4String newVal){

  if(command == SetDetectionMediumCmd){
    fdmat->SetDetectionMedium(newVal);
    G4cout << "Setting the medium to " << newVal << G4endl;
  }
  if (command == SetMediumDensityCmd){
    fdmat->fmediumDensity = SetMediumDensityCmd->GetNewDoubleValue(newVal);
    G4cout << "Changing medium Density to = " << SetMediumDensityCmd->GetNewDoubleValue(newVal)/(g/cm3) << " g/cm3" << G4endl;
    calledDensity = true;
  }
  if (command == SetMediumTempCmd){
    fdmat->fmediumTemp = SetMediumTempCmd->GetNewDoubleValue(newVal);
    G4cout << "Setting Temperature to = " << SetMediumTempCmd->GetNewDoubleValue(newVal)/kelvin << " K" << G4endl;
    calledTempOrPressure = true;
  }
  if (command == SetMediumPressureCmd){
    fdmat->fmediumPressure = SetMediumPressureCmd->GetNewDoubleValue(newVal);
    G4cout << "Setting Pressure to = " << SetMediumPressureCmd->GetNewDoubleValue(newVal)/bar << " bar" << G4endl;
    calledTempOrPressure = true;
  }
  
  if(calledDensity == true && calledTempOrPressure == true){
    G4ExceptionDescription msg;
    msg << "User set both Density and Pressure/Temperature. Using given density,\nNOT temperature calculated density)\n"; 
    G4Exception("MaterialsMessenger::SetNewValue", "Austin's warning message", JustWarning, msg);
  }
  return;
}








