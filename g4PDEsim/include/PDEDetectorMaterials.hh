#ifndef PDEDETECTORMATERIALS_H
#define PDEDETECTORMATERIALS_H 1

#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#ifndef DEFAULT_VALUE
#define DEFAULT_VALUE -69
#endif

class G4Material;
class G4String;
class G4OpticalSurface;

class PDEDetectorMaterials{
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
  PDEDetectorMaterials(PDEDetectorMaterials &other) = delete; // not cloneable
  void operator=(const PDEDetectorMaterials &) = delete;   // not assignable
  
  static PDEDetectorMaterials * GetInstance();
  
protected:
  PDEDetectorMaterials();
  static PDEDetectorMaterials* singleton;
  virtual ~PDEDetectorMaterials();

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

  G4OpticalSurface * opSurf_Air;

  G4OpticalSurface * opSurf_Quartz;       // normal quartz (with k index)
  // G4OpticalSurface * opSurf_surfaceQuartz;// if surface of sipm quartz is different
  G4OpticalSurface * testSurf;
  G4OpticalSurface * opSurf_silicon;      // silicon (bulk of SiPM)

  G4OpticalSurface * opSurf_perfectTrans; // full transmission, no refraction
  G4OpticalSurface * opSurf_perfectRef;   // full specular reflection

  G4OpticalSurface * opSurf_myCorrector1_diffuserIn;  // my own optical surface for doing the PDE correction. applies absorption and diffusion to scale the G4 reflectivity to match that measured in vacuum (and do the PDE correction)
  G4OpticalSurface * opSurf_myCorrector2_absorptionOut;
  

};

#endif