#include "PDEDetectorMaterials.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4String.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4OpticalSurface.hh"

#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include <CLHEP/Units/SystemOfUnits.h>


#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

// #include <algorithm>

#include "HelperFunctions.hh"
#include "myGlobals.hh"

#include "PDEDetectorMaterials.hh"

// confused about this. but it is necessary.
// this is where we instantiate the globally used singleton pointer.
// future austin understands - it must be defined globally/permanently
PDEDetectorMaterials * PDEDetectorMaterials::singleton=nullptr;

PDEDetectorMaterials::PDEDetectorMaterials(){
  
  // default values here
  fDetectorMedium = &mLiquidCF4;     //        default is liquid 
  fmediumDensity = DEFAULT_VALUE; // g/cm3  flagged as not set
  fmediumTemp = DEFAULT_VALUE;    // kelvin      medium temperature  
  fmediumPressure = DEFAULT_VALUE;// bar

}

PDEDetectorMaterials *PDEDetectorMaterials::GetInstance(){
  if(singleton==nullptr){ singleton = new PDEDetectorMaterials(); }
  return singleton;
}

PDEDetectorMaterials::~PDEDetectorMaterials(){}

//////////// Done with namespace ////////////

void PDEDetectorMaterials::SetDetectionMedium(G4String med){
  if(med == "gCF4" || med == "GCF4"){
    fDetectorMedium = &mGasCF4;
  } else if (med == "lCF4" || med == "LCF4" || med == "lcf4"){
    fDetectorMedium = &mLiquidCF4;
  } else if (med == "gAr" || med == "GAr"){
    fDetectorMedium = &mGasAr;
  } else if (med == "air" || med == "Air" || med == "AIR"){
    fDetectorMedium = &mAir;
  } else if (med == "lxe" || med == "LXe" || med == "lXe"){
    fDetectorMedium = &mLXe;
  }
}

void PDEDetectorMaterials::BuildMaterials(){
  int verb;
  bool flipWL;
  G4double scaleX, scaleY;

  G4cout << "\n-------- Build Materials() -----------\n" << G4endl;
  
  G4double hc = CLHEP::h_Planck * CLHEP::c_light;
  G4NistManager* nist = G4NistManager::Instance(); // Get nist material manager

  // -------------------- Misc Materials --------------------//
  // -- Vacuum -- //
  std::vector<G4double> photonE_world = {hc/(1200*nm), hc/(100*nm)}; // set n=1 everywhere
  std::vector<G4double> nWorld = {1.00, 1.00};   
  std::vector<G4double> kWorld = {0.00, 0.00};   
  G4MaterialPropertiesTable *mpt_vacuum = new G4MaterialPropertiesTable();
  mVacuum = nist->FindOrBuildMaterial("G4_Galactic");
  mpt_vacuum->AddProperty("RINDEX", photonE_world, nWorld);
  mVacuum->SetMaterialPropertiesTable(mpt_vacuum);
  
  // -- air -- //
  mAir = nist->FindOrBuildMaterial("G4_AIR");
  G4MaterialPropertiesTable *mptAir = new G4MaterialPropertiesTable();
  mptAir->AddProperty("RINDEX", photonE_world, nWorld);
  mAir->SetMaterialPropertiesTable(mptAir);

  opSurf_Air = new G4OpticalSurface("air_opSurf", unified, polished, dielectric_dielectric);
  opSurf_Air->SetSigmaAlpha(0.);
  opSurf_Air->SetMaterialPropertiesTable(mptAir);

  // -------------------- General Surfaces --------------------//
  //-- perfect transmission --//
  opSurf_perfectTrans = new G4OpticalSurface("perfectTrans_opSurf");
  opSurf_perfectTrans->SetModel(unified);
  opSurf_perfectTrans->SetType(dielectric_metal);
  opSurf_perfectTrans->SetFinish(polished); //could also use dielectric_dielectric -> polished front painted
  opSurf_perfectTrans->SetSigmaAlpha(0.); // perfectly smooth
  
  G4double t = 1.0;
  std::vector<G4double> photonE_perfectTrans = {1.0*eV, 12.4*eV};
  std::vector<G4double> transPerfect = {t, t};
  std::vector<G4double> noRef = {1.0-t, 1.0-t};

  G4MaterialPropertiesTable* mpt_perfectTrans = new G4MaterialPropertiesTable();
  mpt_perfectTrans->AddProperty("TRANSMITTANCE", photonE_perfectTrans, transPerfect);
  mpt_perfectTrans->AddProperty("REFLECTIVITY", photonE_perfectTrans, noRef);
  opSurf_perfectTrans->SetMaterialPropertiesTable(mpt_perfectTrans);

  //-- perfect reflection --//
  opSurf_perfectRef = new G4OpticalSurface("perfectTrans_opSurf", unified, polished, dielectric_metal);
  opSurf_perfectRef->SetSigmaAlpha(0.); // perfectly smooth.  //could also use dielectric_dielectric -> polishedfrontpainted
  G4MaterialPropertiesTable* mpt_perfectRef = new G4MaterialPropertiesTable();
  mpt_perfectRef->AddProperty("TRANSMITTANCE", photonE_perfectTrans, noRef);
  mpt_perfectRef->AddProperty("REFLECTIVITY", photonE_perfectTrans, transPerfect);
  opSurf_perfectRef->SetMaterialPropertiesTable(mpt_perfectRef);

  // -------------------- Liquid Xenon --------------------//
  mLXe = new G4Material("LXe", 54., 131.29 * g / mole, 3.020 * g / cm3);
  G4MaterialPropertiesTable * mptLXe = new G4MaterialPropertiesTable();
  std::vector<G4double> photonE_lxe, n_lxe;
  OH::ReadAndFillArray("dataOptics/lxe_n_merged_wl.txt", photonE_lxe, n_lxe, verb = MG::GetVerbosity(), scaleX = nm, 1., flipWL = true); // scale um to nm, flip wavelengths to eV
  mptLXe->AddProperty("RINDEX", photonE_lxe, n_lxe);
  mLXe->SetMaterialPropertiesTable(mptLXe);

  // mptLXe->DumpTable();
  // exit(0);

  // -------------------- Generic CF4 stuff --------------------//
  // -- Elements in CF4 -- //
  G4Element * elCarbon = new G4Element("Carbon", "C", 6, 12.01 * g/mole);
  G4Element * elFluorine = new G4Element("Fluorine", "F", 9,  19. * g/mole);

  // -------------------- Gas CF4 --------------------//
  // default parameters baked in
  G4double density_cf4Gas = 0.0022*g/cm3; // default value? need to make P and T function to get density
  G4double cf4Gas_temp = 160*kelvin;
  G4double cf4Gas_P = 0.5*bar;

  // include elements
  mGasCF4 = new G4Material("cf4Gas", density_cf4Gas, 2, kStateGas, cf4Gas_temp, cf4Gas_P);
  mGasCF4->AddElement(elCarbon, 1);
  mGasCF4->AddElement(elFluorine, 4);
  #include "Optics/GasCF4.icc" // all the physics is in here

  // -------------------- Liquid CF4 --------------------//

  // default parameters baked in
  G4double density_cf4Liq = 1.6*g/cm3;  // default value? unknown if correct
  G4double cf4Liq_temp = 160*kelvin;
  G4double cf4Liq_P = 0.5*bar;

  mLiquidCF4 = new G4Material("cf4Liq", density_cf4Liq, 2, kStateLiquid, cf4Liq_temp, cf4Liq_P);
  mLiquidCF4->AddElement(elCarbon, 1);
  mLiquidCF4->AddElement(elFluorine, 4);
  // #include "Optics/LiquidCF4.icc" // all the physics is in here!

  G4MaterialPropertiesTable *mpt_liquidCF4 = new G4MaterialPropertiesTable();
  //------- Refractive Indexes ------- //
  // taken from literature with many different densities
  // french paper (7, 5eV), other two from second paper
  std::vector<G4double> liq_cf4_photonE = {7*eV, 5*eV, 2*eV, 1*eV}; 
  std::vector<G4double> liq_cf4_n = {1.24, 1.225, 1.21, 1.2039};

  std::reverse(liq_cf4_photonE.begin(), liq_cf4_photonE.end());
  std::reverse(liq_cf4_n.begin(), liq_cf4_n.end());

  // #if not USE_METAL_COMPLEX
    mpt_liquidCF4->AddProperty("RINDEX", liq_cf4_photonE, liq_cf4_n);
  // #else
  //  mpt_liquidCF4->AddProperty("REALRINDEX", liq_cf4_photonE, liq_cf4_n);
  // #endif
  mLiquidCF4->SetMaterialPropertiesTable(mpt_liquidCF4);
  
  
  // -------------------- GAr --------------------//
  // 

  // -------------------- Quartz/SiO2 --------------------//
  // -- Quartz/SiO2 Bulk -- //
  mQuartz = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  
  std::vector<G4double> photonE_nQuartz, nQuartz, photonE_kQuartz, kQuartz;
  OH::ReadAndFillArray("dataOptics/sio2_n_wl.txt", photonE_nQuartz, nQuartz, verb = MG::GetVerbosity(), scaleX = nm, 1., flipWL = true); // no scaling, flip wavelengths to eV
  OH::ReadAndFillArray("dataOptics/sio2_k_wl.txt", photonE_kQuartz, kQuartz, verb = MG::GetVerbosity(), scaleX = nm, 1., flipWL = true); // no scaling, flip wavelengths to eV

  std::vector<G4double> photonE_absLQuartz, absLQuartz;
  OH::ReadAndFillArray("dataOptics/quartz_absLength.txt", photonE_absLQuartz, absLQuartz, verb = MG::GetVerbosity(), scaleX = nm, scaleY = um, flipWL = true); // no scaling, flip wavelengths to eV

  // to absorb all photons in bulk
  // std::vector<G4double> kAbsorb = {100.00, 100.00};  
  // std::vector<G4double> quartzAbs_eV, quartzAbsLength;

  G4MaterialPropertiesTable *mptQuartz = new G4MaterialPropertiesTable();
  if(DETECTORSETUP::GetQuartzABSstatus() && DETECTORSETUP::GetVisibleStatus()){
    mptQuartz->AddProperty("ABSLENGTH", photonE_absLQuartz, absLQuartz);
  }

  mptQuartz->AddProperty("RINDEX", photonE_nQuartz, nQuartz);
  // mptQuartz->AddProperty("RINDEX", photonE_world, {100, 100}); // testing
  mQuartz->SetMaterialPropertiesTable(mptQuartz);

  // -- Quartz/SiO2 Surface -- //
  opSurf_Quartz = new G4OpticalSurface("quartzWindow_opSurf", unified, polished, dielectric_dielectric);
  opSurf_Quartz->SetSigmaAlpha(0.);

  G4MaterialPropertiesTable *mpt_quartzSurf = new G4MaterialPropertiesTable();
  // mpt_quartzSurf->AddProperty("REALRINDEX", photonE_nQuartz, nQuartz); // not dielectric
  // mpt_quartzSurf->AddProperty("IMAGINARYRINDEX", photonE_kQuartz, kQuartz);
  mpt_quartzSurf->AddProperty("RINDEX", photonE_nQuartz, nQuartz);
  // mpt_quartzSurf->AddProperty("RINDEX", photonE_world, {100, 100});
  // mpt_quartzSurf->AddProperty("IMAGINARYRINDEX", photonE_world, {4, 4});

  opSurf_Quartz->SetMaterialPropertiesTable(mpt_quartzSurf);

  // mpt_quartzSurf->DumpTable();

  //-- SiPM surface quartz --//
  // opSurf_surfaceQuartz = new G4OpticalSurface("quartzSiPM_opSurf");
  // opSurf_surfaceQuartz->SetModel(unified);
  // opSurf_surfaceQuartz->SetType(dielectric_metal);
  // opSurf_surfaceQuartz->SetFinish(polished);
  // opSurf_surfaceQuartz->SetSigmaAlpha(0.);

  // G4MaterialPropertiesTable *mpt_quartzSiliconSurf = new G4MaterialPropertiesTable();
  // // mpt_quartzSiliconSurf->AddProperty("RINDEX", photonE_nQuartz, nQuartz);
  // mpt_quartzSiliconSurf->AddProperty("REALRINDEX", photonE_nQuartz, nQuartz);
  // mpt_quartzSiliconSurf->AddProperty("IMAGINARYRINDEX", photonE_kQuartz, kQuartz);
  // opSurf_surfaceQuartz->SetMaterialPropertiesTable(mpt_quartzSiliconSurf);
  
  // opSurf_surfaceQuartz = opSurf_Quartz; // use this if you want both quartz to be the same

  // no heavy quartz inside detector
  // mHeavyQuartz = nist->FindOrBuildMaterials("G4_MAGNESIUM_FLUORIDE");
  // mHeavyQuartz = nist->FindOrBuildMaterial("G4_BORON_OXIDE");

  // -------------------- Silicon -------------------- //
  // -- Silicon Bulk -- //
  mSilicon = nist->FindOrBuildMaterial("G4_Si");
  std::vector<G4double> photonE_nSilicon, nSi, photonE_kSilicon, kSilicon;

  std::string silicon_nFile = "dataOptics/silicon_merged_n_wl.txt";
  std::string silicon_kFile = "dataOptics/silicon_merged_k_wl.txt";
  // using amorphous
  // silicon_nFile = "dataOptics/silicon_n_amorphous_wl.txt";
  // silicon_kFile = "dataOptics/silicon_k_amorphous_wl.txt";

  OH::ReadAndFillArray(silicon_nFile, photonE_nSilicon, nSi, verb = MG::GetVerbosity(), scaleX = nm, 1., flipWL = true); // scale um to nm, flip wavelengths to eV
  OH::ReadAndFillArray(silicon_kFile, photonE_kSilicon, kSilicon, verb = MG::GetVerbosity() , scaleX = nm, 1., flipWL = true); // scale um to nm, flip wavelengths to eV

  G4MaterialPropertiesTable *mptSi = new G4MaterialPropertiesTable();
  mptSi->AddProperty("RINDEX", photonE_nSilicon, nSi);

  // cheating to fix group velocities, because n<1 is not allowed by G4
  std::vector<G4double> gv = {CLHEP::c_light/2, CLHEP::c_light/2};
  mptSi->AddProperty("GROUPVEL", photonE_world, gv);
  mSilicon->SetMaterialPropertiesTable(mptSi);

  // -- silicon surface --//
  opSurf_silicon = new G4OpticalSurface("silicon_opSurf", unified, polished, dielectric_metal);
  opSurf_silicon->SetSigmaAlpha(0.);

  G4MaterialPropertiesTable *mpt_siliconSurf = new G4MaterialPropertiesTable();
  mpt_siliconSurf->AddProperty("REALRINDEX", photonE_nSilicon, nSi);
  mpt_siliconSurf->AddProperty("IMAGINARYRINDEX", photonE_kSilicon, kSilicon);
  // mpt_siliconSurf->AddProperty("REFLECTIVITY", photonE_world, {0, 0}); // you can't do the absorptive scaling here. it won't calculate with index values
  // mpt_siliconSurf->AddProperty("REALRINDEX", photonE_world, {1000.0, 1000.0});
  // mpt_siliconSurf->AddProperty("IMAGINARYRINDEX", photonE_world, {1000.0, 1000.0});
  
  if(MG::GetTesting() == 1){
    mpt_siliconSurf->AddProperty("EFFICIENCY", photonE_world, {1.0, 1.0}); // xx perfect efficiency (for non reflected photons)
  } else {
    mpt_siliconSurf->AddProperty("TRANSMITTANCE", photonE_perfectTrans, transPerfect); // needed to get the nonReflected photons inside the bulk to the backing (no abs)
  }

  opSurf_silicon->SetMaterialPropertiesTable(mpt_siliconSurf);

  // -- correction surfaces -- //
  G4double Rdiffuse = 0.17; // should be 0.17, from nEXO vacuum paper  
  opSurf_myCorrector1_diffuserIn = new G4OpticalSurface("opSurf_myCorrector1_diffuserIn", unified, groundfrontpainted, dielectric_dielectric);
  // groundfrontpainted only diffuse reflection. set by REFLECTIVITY
  // absorption then is 1 - TRANSMITTANCE, default is T = 0

  G4MaterialPropertiesTable *mpt_myCorrector1_diffuseIn = new G4MaterialPropertiesTable();  
  mpt_myCorrector1_diffuseIn->AddProperty("TRANSMITTANCE", photonE_world, {1.0, 1.0}); // get everything in (after doing diffuse reflection), ABS = 1 - TRANSMITTANCE = 0
  mpt_myCorrector1_diffuseIn->AddProperty("REFLECTIVITY", photonE_world, {Rdiffuse, Rdiffuse}); // how much diffusion do we want
  opSurf_myCorrector1_diffuserIn->SetMaterialPropertiesTable(mpt_myCorrector1_diffuseIn);


  opSurf_myCorrector2_absorptionOut = new G4OpticalSurface("opSurf_myCorrector2_absorptionOut", unified, polishedfrontpainted, dielectric_dielectric);
  // std::vector<G4double> correctRwithAbs_transmission = {0., 0.}; // load in these values from my calculations
  std::vector<G4double> xphotonE_TransCorrection, y_TransCorrection;
  OH::ReadAndFillArray("dataOptics/effective_correction_transmission.txt", xphotonE_TransCorrection, y_TransCorrection, verb = MG::GetVerbosity() , scaleX = nm, scaleY = 1., flipWL = true); // scale um to nm, flip wavelengths to eV

  G4MaterialPropertiesTable *mpt_myCorrector2_absorptionOut = new G4MaterialPropertiesTable();
  mpt_myCorrector2_absorptionOut->AddProperty("TRANSMITTANCE", xphotonE_TransCorrection, y_TransCorrection); // absorb certain amount of SiPM reflections
  mpt_myCorrector2_absorptionOut->AddProperty("REFLECTIVITY", photonE_world, {0., 0.}); // no reflections
  opSurf_myCorrector2_absorptionOut->SetMaterialPropertiesTable(mpt_myCorrector2_absorptionOut);

  // mpt_myCorrector2_absorptionOut->DumpTable();
  // exit(0);

  return;
}











// end