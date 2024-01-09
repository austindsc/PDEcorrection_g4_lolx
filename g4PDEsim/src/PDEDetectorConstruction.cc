#include "G4RunManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Polycone.hh"

#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"

#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4Element.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4UserLimits.hh"

#include "PDEDetectorConstruction.hh"
#include "PDEDetectorConstructionMessenger.hh"

#include "SensitiveSIPM.hh"
#include "myGlobals.hh"
#include "PDEDetectorMaterials.hh"
#include "MaterialsMessenger.hh"
#include "HelperFunctions.hh"

#include <cmath>

G4String PDEDetectorConstruction::fStrBulk = "logic_sipmBulk";
G4String PDEDetectorConstruction::fStrSurface = "logic_sipmSurface";
G4String PDEDetectorConstruction::fStrWindow = "logic_quartzWindow";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PDEDetectorConstruction::PDEDetectorConstruction()
: G4VUserDetectorConstruction(),
  fWorld(0)
{
  fDetMessenger = new PDEDetectorConstructionMessenger(this);
  
  // still control some things with this
  // detMat = new DetectorMaterials();
  detMat = PDEDetectorMaterials::GetInstance();
  MaterialsMessenger * dmatMessenger = new MaterialsMessenger(detMat);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PDEDetectorConstruction::ConstructSDandField(){
    
    SensitiveSIPM * ssipm = new SensitiveSIPM("SensitiveSIPM");
    fsipmDetectionBack->SetSensitiveDetector(ssipm);
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PDEDetectorConstruction::~PDEDetectorConstruction()
{
  delete fDetMessenger;
  delete detMat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* PDEDetectorConstruction::Construct()
{ 
  #include "colours.icc"
  detMat->BuildMaterials(); //PUT THIS HERE! (not in constructor)
  
  G4cout << "---------------------- Parameters in Construct() ----------------------\n" << G4endl;
  
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  //--------------------------- World ---------------------------//
  // (lab space where gamma sources live)
  G4double world_sizeXY = 20*cm;
  G4double world_sizeZ  = world_sizeXY;

  G4double maxTrackLength = 2*world_sizeXY;

  G4Box* solidWorld = new G4Box("World", 0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);
  G4LogicalVolume* logic_world = new G4LogicalVolume(solidWorld, detMat->GetDetectorMedium(), "World");   
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logic_world, "World", 0, false, 0, checkOverlaps);

  // G4cout << "Material used in logic_world: " << logic_world->GetMaterial()->GetName() << G4endl;

  G4UserLimits * myLimits = new G4UserLimits();
  myLimits->SetUserMaxTrackLength(maxTrackLength);
  logic_world->SetUserLimits(myLimits);
  // fWorld = logic_world;
  
  //--------------------------- SiPM ---------------------------//
  G4double sipmXY = 5 * cm; // in reality it is 5.95 x 5.85
  sipmXY = world_sizeXY;

  G4double sipmThick = 1*cm; // guess
  G4double sipmBackThick = 0.5*cm; // guess
  G4double sipmSurfaceThick = 1.*mm;

  G4double quartzWindowGap = 1*mm;
  G4double quartzWindowThick = 2.*mm;

  //-- silicon bulk --//
  G4Box * solid_sipmBulk = new G4Box("solid_sipmBulk", 0.5*sipmXY, 0.5*sipmXY, 0.5*sipmThick);
  G4LogicalVolume * logic_sipmBulk = new G4LogicalVolume(solid_sipmBulk, detMat->mSilicon, "logic_sipmBulk");
  logic_sipmBulk->SetVisAttributes(si_magenta);
  G4ThreeVector moveSipm = G4ThreeVector(0, 0, -solid_sipmBulk->GetZHalfLength());
  G4VPhysicalVolume* phys_sipmBulk = new G4PVPlacement(0, moveSipm, logic_sipmBulk, "phys_sipmBulk",
                    logic_world,         // mother volume
                    false, 0, checkOverlaps);
                  
  //-- silicon backing/avalanche region/detection region --//
  G4Box * solid_sipmBack = new G4Box("solid_sipmBack", 0.5*sipmXY, 0.5*sipmXY, 0.5*sipmBackThick);
  G4LogicalVolume * logic_sipmBack = new G4LogicalVolume(solid_sipmBack, detMat->mSilicon, "logic_sipmBack");
  logic_sipmBack->SetVisAttributes(filter_green);

  G4ThreeVector moveSipmBack = G4ThreeVector(0, 0, -2*solid_sipmBulk->GetZHalfLength()-solid_sipmBack->GetZHalfLength());
  // sipm wrt origin of solid_BoxPackage is y, z to rotate

  G4VPhysicalVolume* phys_sipmBack = new G4PVPlacement(0, moveSipmBack, logic_sipmBack, "phys_sipmBack",
                    logic_world,         // mother volume
                    false, 0, checkOverlaps);

  //-------- SETUP TOGGLING DONE HERE --------//
  bool buildWindow = true;
  bool buildSurface = true;

  //-- silicon surface (Quartz) --//
  G4VPhysicalVolume* phys_quartzWindow = NULL;
  G4VPhysicalVolume* phys_sipmSurface = NULL;

  G4Box * solid_sipmSurface = new G4Box("solid_sipmSurface", 0.5*sipmXY, 0.5*sipmXY, 0.5*sipmSurfaceThick);
  G4LogicalVolume * logic_sipmSurface = new G4LogicalVolume(solid_sipmSurface, detMat->mQuartz, "logic_sipmSurface");
  logic_sipmSurface->SetVisAttributes(filter_yellow);

  G4ThreeVector moveSurface = G4ThreeVector(0, 0, solid_sipmSurface->GetZHalfLength()); // sipm wrt origin of solid_BoxPackage is y, z to rotate

  if(buildSurface){ phys_sipmSurface = new G4PVPlacement(0, moveSurface, logic_sipmSurface, "phys_sipmSurface", logic_world, false, 0, checkOverlaps); }

  // ---- Top Quartz window ----//
  G4Box * solid_quartzWindow = new G4Box("solid_quartzWindow", 0.5*sipmXY, 0.5*sipmXY, 0.5*quartzWindowThick);
  G4LogicalVolume * logic_quartzWindow = new G4LogicalVolume(solid_quartzWindow, detMat->mQuartz, "logic_quartzWindow");
  logic_quartzWindow->SetVisAttributes(quartz_white);
  G4ThreeVector moveQuartzWindow = G4ThreeVector(0, 0, quartzWindowGap + solid_quartzWindow->GetZHalfLength() + 2*solid_sipmSurface->GetZHalfLength());

  if(buildWindow){ phys_quartzWindow = new G4PVPlacement(0, moveQuartzWindow, logic_quartzWindow, "phys_quartzWindow", logic_world, false, 0, checkOverlaps); }

  //------ Doing Optical Surfaces ------//
  /*
  skin around everything, override with border between SiO2 and top Si
  */

  new G4LogicalBorderSurface("opSurf_sibulk_backing", phys_sipmBulk, phys_sipmBack, (G4SurfaceProperty*) detMat->opSurf_perfectTrans); // bulk to detector (want perfect)
  new G4LogicalSkinSurface("opSurf_siBulk", logic_sipmBulk, (G4SurfaceProperty*) detMat->opSurf_silicon); // silicon bulk, use metal dielectric with Transmission = 100
  // this does no refraction into the silicon but thats okay
  
  /* all fake news below
  new G4LogicalBorderSurface("opSurf_passivation_air", phys_sipmSurface, physWorld, (G4SurfaceProperty*) detMat->opSurf_silicon); // bulk to detector (want perfect)
  new G4LogicalBorderSurface("opSurf_passivation_air2", physWorld, phys_sipmSurface, (G4SurfaceProperty*) detMat->opSurf_silicon); // bulk to detector (want perfect)
  new G4LogicalSkinSurface("opSurf_sio2", logic_sipmSurface, (G4SurfaceProperty*) detMat->opSurf_Quartz); // silicon passivation surface
  
  into passivation
  new G4LogicalBorderSurface("opSurf_passiv1", physWorld, phys_sipmSurface, (G4SurfaceProperty*) detMat->opSurf_Quartz); // bulk to detector (want perfect)
  new G4LogicalSkinSurface("opSurf_window", logic_quartzWindow, (G4SurfaceProperty*) detMat->opSurf_Quartz); // silicon passivation surface

  this doenst work with the any defined k values for the quartz surface. so doing it as here
  is the same as just defining RINDEX and doing 'normal' fresnel. its so annoying
  why can't I use delectric_metal with k
  */

  /*
  can't do n, k and refraction/reflection. don't want to hack it

  1. silicon is dielectric_metal
    also have to set transmission
    no refraction in (just counting pure 'in', no internal geometry of avalanche)

  2. quartz is just n, dielectric dielectric

  3. if i don't define the sipm optical surface (which should be polished dielectric by default)
    i get a very different transmission in.
  
  WE DON'T WANT SURFACES EVERYWHERE (or between two metals)
  */

  // setting items for usage later
  fsipmDetectionBack = logic_sipmBack; // very important!
  fBulkZ = phys_sipmBulk->GetTranslation().z();
  if(phys_sipmSurface) fSurfaceZ = phys_sipmSurface->GetTranslation().z();
  if(phys_quartzWindow) fWindowZ = phys_quartzWindow->GetTranslation().z();

  fLogBulk = logic_sipmBulk;
  fLogSurface = logic_sipmSurface;
  fLogWindow = logic_quartzWindow;
  fLogBacking = logic_sipmBack;
  fWorld = logic_world;

  // G4cout << fBulkZ/mm << " mm" << G4endl;
  // exit(0);

  G4cout << "\n---------------------- Done Construct() ----------------------" << G4endl;
  //always return the physical World
  return physWorld;
}