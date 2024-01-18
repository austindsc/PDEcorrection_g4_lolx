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
    fSingleSipm = ssipm; // for passing to stepping action later, to avoid lookup each time
    fsipmDetectionBack->SetSensitiveDetector(ssipm); // this depends on MG options (pointers are set at the bottom of Construct())
    
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
  G4double sipmSurfaceThick = 110*nm;
  if(DETECTORSETUP::GetVisibleStatus()) sipmSurfaceThick = 5*mm;

  G4double additionalBoxThick = 1. * um; // very thin box for doing extra optical layers in medium
  if(DETECTORSETUP::GetVisibleStatus()) additionalBoxThick = 5*mm;

  G4double quartzWindowGap = 0.7*mm;
  G4double quartzWindowThick = 0.5*mm;

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

  //-- silicon surface (Quartz) --//
  G4VPhysicalVolume* phys_quartzWindow = NULL;
  G4VPhysicalVolume* phys_sipmSurface = NULL;

  G4Box * solid_sipmSurface = new G4Box("solid_sipmSurface", 0.5*sipmXY, 0.5*sipmXY, 0.5*sipmSurfaceThick);
  G4LogicalVolume * logic_sipmSurface = new G4LogicalVolume(solid_sipmSurface, detMat->mQuartz, "logic_sipmSurface");
  logic_sipmSurface->SetVisAttributes(filter_yellow);

  G4ThreeVector moveSurface = G4ThreeVector(0, 0, solid_sipmSurface->GetZHalfLength()); // sipm wrt origin of solid_BoxPackage is y, z to rotate

  if(DETECTORSETUP::GetSurfaceStatus()){ phys_sipmSurface = new G4PVPlacement(0, moveSurface, logic_sipmSurface, "phys_sipmSurface", logic_world, false, 0, checkOverlaps); }

  // ---- Additional Box for correction----//
  G4Box* solid_additionalBox = new G4Box("solid_additionalBox", 0.5 * sipmXY, 0.5 * sipmXY, 0.5 * additionalBoxThick);
  G4LogicalVolume* logic_additionalBox = new G4LogicalVolume(solid_additionalBox, detMat->GetDetectorMedium(), "logic_additionalBox");
  logic_additionalBox->SetVisAttributes(extra_cyan);

  // G4double extraBoxBuffer = 0*mm;
  G4ThreeVector moveAdditionalBox = G4ThreeVector(0, 0, 2*solid_sipmSurface->GetZHalfLength() + solid_additionalBox->GetZHalfLength());
  G4VPhysicalVolume* phys_additionalBox = new G4PVPlacement(0, moveAdditionalBox, logic_additionalBox, "phys_additionalBox", logic_world, false, 0, checkOverlaps);

  // ---- Top Quartz window ----//
  G4Box * solid_quartzWindow = new G4Box("solid_quartzWindow", 0.5*sipmXY, 0.5*sipmXY, 0.5*quartzWindowThick);
  G4LogicalVolume * logic_quartzWindow = new G4LogicalVolume(solid_quartzWindow, detMat->mQuartz, "logic_quartzWindow");
  logic_quartzWindow->SetVisAttributes(quartz_white);
  G4ThreeVector moveQuartzWindow = G4ThreeVector(0, 0, quartzWindowGap + solid_quartzWindow->GetZHalfLength() + 2*solid_sipmSurface->GetZHalfLength());

  if(DETECTORSETUP::GetWindowStatus()){ phys_quartzWindow = new G4PVPlacement(0, moveQuartzWindow, logic_quartzWindow, "phys_quartzWindow", logic_world, false, 0, checkOverlaps); }

  //------ Doing Optical Surfaces ------//

  new G4LogicalBorderSurface("opSurf_sibulk_backing", phys_sipmBulk, phys_sipmBack, (G4SurfaceProperty*) detMat->opSurf_perfectTrans); // bulk to detector (want perfect)
  new G4LogicalSkinSurface("opSurf_siBulk", logic_sipmBulk, (G4SurfaceProperty*) detMat->opSurf_silicon); // silicon bulk, use metal dielectric with Transmission = 100
  // this does no refraction into the silicon bulk but thats okay

  // doing my PDE correction
  new G4LogicalBorderSurface("opSurf_myCorrector1_in", physWorld, phys_additionalBox, (G4SurfaceProperty*) detMat->opSurf_myCorrector1_diffuserIn);
  // because this is a border, photons going 'backwards' or exiting/reflected off the SiPM just go like normal (they don't see this border).
  new G4LogicalBorderSurface("opSurf_myCorrector2_out", phys_additionalBox, physWorld, (G4SurfaceProperty*) detMat->opSurf_myCorrector2_absorptionOut);

  /*
  ==== Final Implementation description ====
  We want the correct effective PDE, while also keeping realistic reflectivity, and not simulating any microphysics or pixel structure, etc

  issue: the downfall of this approach is that photons coming at a steep angles (off from normal) will enter Si bulk at an angle.
  For short MFP photons, PDE' used will be slightly too high because these photons may not make it so far into the depletion region
  for long MFP photons, PDE' may be too low because these photons have a higher chance of being absorbed in the depletion region.
  this is a challenge for a different study. call it 'good enough' here

  1. we want fresnel at SiO2/medium surface. So we don't define any optical surface between medium and SiO2(default
  behaviour is dielectric_dielectric boundary)

  2. from SiO2 into bulk Si, we use dielectric_metal with T = 100 (and crystalline SiO2 n, k data). This gets the correct specular R, T
  but doesn't do any refraction into the Si bulk. Thats okay because we aren't simulating the microphysics
  inside the junction, Pavalanche, etc.

    * we have tested (without any of the corrections) that the reflectivity has the correct shape
    when using pure fresnel@ air/SiO2 and dielectric_metal at SiO2/Si_bulk. So that approach is okay
    the absolute R value is too high, and we need to reduce it down, and add diffuse ref at the top
  
  3. add an additional box (material=medium) above the SiO2 so we can place an optical surface on top of it
  it can be as thin as we want.

  4. first we do diffuse reflection for photons incoming. This is done with a LogicalBorderSurface(medium/extra_box) for incoming photons at the top of the box
  groundfrontpainted, REFLECTIVITY = Rdiffuse, TRANSMISSION = 100. groundfrontpainted gives only diffuse reflection, no refraction,
  then the photons (1-Rdiffuse) making it in are again in a normal medium heading towards the SiO2 surface for normal dielectric_dielectric
  fresnel refraction

  5. G4 does all the physics listed in steps 1/2. We get too many reflected photons. So we add a second LogicalBorderSurface (extra_box/medium) for outgoing photons
  we set this to have REFLECTIVITY = 0  and TRANSMISSION = 1-ABSORPTION, where absorption is the scaling we need to get 
  R_effective = R_measured_in_vacuum = R_geant4pure x (1-Rdiffuse) x (1-ABSORPTION)

  */

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
  can't do n, k and refraction/reflection. don't want to hack it deeper in source code

  1. silicon is dielectric_metal
    also have to set transmission = 100
    no refraction in (just counting pure 'in', no internal geometry of avalanche)

  2. quartz is just n, dielectric dielectric

  3. if i don't define the sipm optical surface (which should be polished dielectric by default)
    i get a very different transmission in.
  
  WE DON'T WANT SURFACES EVERYWHERE (or between two metals)
  */

  //------ IMPORTANT ------//
  // setting items for usage elsewhere
  if(MG::GetTesting() == 1)fsipmDetectionBack = logic_sipmBulk; //xx detect on the front of the sipm bulk
  else{ fsipmDetectionBack = logic_sipmBack; }// very important!
  
  fBulkZ = phys_sipmBulk->GetTranslation().z();
  if(phys_sipmSurface) fSurfaceZ = phys_sipmSurface->GetTranslation().z();
  if(phys_quartzWindow) fWindowZ = phys_quartzWindow->GetTranslation().z();

  fLogBulk = logic_sipmBulk;
  fLogSurface = logic_sipmSurface;
  fLogWindow = logic_quartzWindow;
  fLogBacking = logic_sipmBack;
  fWorld = logic_world;


  G4cout << "\n---------------------- Done Construct() ----------------------" << G4endl;
  //always return the physical World
  return physWorld;
}