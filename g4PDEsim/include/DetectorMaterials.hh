#ifndef DETECTORMATERIALS_H
#define DETECTORMATERIALS_H 1

#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#ifndef DEFAULT_VALUE
#define DEFAULT_VALUE -69
#endif

class G4Material;
class G4String;
class G4OpticalSurface;

class DetectorMaterials{
  friend class PDEDetectorConstruction;
  friend class MaterialsMessenger;
  
public:
  void BuildMaterials(); //call this after setting variables with messenger

  void SetDetectionMedium(G4String med);
  G4String GetDetectorMediumString(){return fmedString; }
  G4Material * GetDetectorMedium(){return *fDetectorMedium; }
  
  void SetMediumDensity(G4double r){ fmediumDensity = r; }
  G4double GetMediumDensity(){ return fmediumDensity; }
  
  void SetMediumTemp(G4double temp){ fmediumTemp = temp; }
  G4double GetMediumTemp(){ return fmediumTemp; }
    
  // singleton Stuff
  DetectorMaterials(DetectorMaterials &other) = delete; // not cloneable
  void operator=(const DetectorMaterials &) = delete;   // not assignable
  
  static DetectorMaterials *GetInstance();
  
protected:
  DetectorMaterials();
  static DetectorMaterials* singleton;
  virtual ~DetectorMaterials();

  // for materials messenger and setting medium
  G4String fmedString;
  G4Material ** fDetectorMedium;// what is currently in active volume
  
  // maybe use these if code gets more complex
  G4double fmediumDensity;
  G4double fmediumTemp;
  G4double fmediumPressure;
  
  // some normal materials where PDE is measured
  G4Material * mVacuum;
  G4Material * mAir;

  //--- different detection mediums
  G4Material * mGasCF4;
  G4Material * mLiquidCF4;
  G4Material * mGasAr;
  G4Material * mLXe;
  
  // SiPM material
  // G4Material *
  G4Material * mSilicon;
  // G4Material * Si02;      //silicon dioxide (for SiPM surface?)
  G4Material * mQuartz;
  G4Material * mHeavyQuartz; // top of optical filter

  ////// --- Optical Surfaces ---//////
  // optical property tables (for surfaces and skins, excluding for the LP filter)
  G4OpticalSurface * opSteel_rodSurface;
  G4OpticalSurface * opAl_surface;
  

};

#endif