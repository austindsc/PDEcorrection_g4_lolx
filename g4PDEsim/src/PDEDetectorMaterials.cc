#include "DetectorMaterials.hh"

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

#include "DetectorMaterials.hh"

// confused about this. but it is necessary.
// this is where we instantiate the globally used singleton pointer.
DetectorMaterials * DetectorMaterials::singleton=nullptr;

DetectorMaterials::DetectorMaterials(){
  
  // default values here
  fDetectorMedium = &mLiquidCF4;     //        default is liquid 
  fmediumDensity = DEFAULT_VALUE; // g/cm3  flagged as not set
  fmediumTemp = DEFAULT_VALUE;    // kelvin      medium temperature  
  fmediumPressure = DEFAULT_VALUE;// bar

}

DetectorMaterials *DetectorMaterials::GetInstance(){
  if(singleton==nullptr){ singleton = new DetectorMaterials(); }
  return singleton;
}

DetectorMaterials::~DetectorMaterials(){}

//////////// Done with namespace ////////////

void DetectorMaterials::SetDetectionMedium(G4String med){
  if(med == "gCF4" || med == "GCF4"){
    fDetectorMedium = &mGasCF4;
  } else if (med == "lCF4" || med == "LCF4" || med == "lcf4"){
    fDetectorMedium = &mLiquidCF4;
  } else if (med == "gAr" || med == "GAr"){
    fDetectorMedium = &mGasAr;
  } else if (med == "air" || med == "Air" || med == "AIR"){
    fDetectorMedium = &mAir;
  }
}

void DetectorMaterials::BuildMaterials(){
  G4cout << "\n-------- Build Materials() -----------\n" << G4endl;
  
  G4double hc = CLHEP::h_Planck * CLHEP::c_light;
  G4NistManager* nist = G4NistManager::Instance(); // Get nist material manager

  // -------------------- Misc Materials --------------------//
  // -- Vacuum -- //
  std::vector<G4double> photonEWorld = {hc/(1200*nm), hc/(100*nm)}; // set n=1 everywhere
  std::vector<G4double> nWorld = {1.00, 1.00};   
  G4MaterialPropertiesTable *mpt_vacuum = new G4MaterialPropertiesTable();
  mpt_vacuum->AddProperty("RINDEX", photonEWorld, nWorld); //2 is for length of arrays
  mVacuum = nist->FindOrBuildMaterial("G4_Galactic");
  mVacuum->SetMaterialPropertiesTable(mpt_vacuum);
  
  // -- air -- //
  mAir = nist->FindOrBuildMaterial("G4_AIR");
  G4MaterialPropertiesTable *mptAir = new G4MaterialPropertiesTable();
  mptAir->AddProperty("RINDEX", "Air");
  mAir->SetMaterialPropertiesTable(mptAir);
  // mptAir->DumpTable();

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

  mpt_liquidCF4->AddProperty("RINDEX", liq_cf4_photonE, liq_cf4_n);
  mLiquidCF4->SetMaterialPropertiesTable(mpt_liquidCF4);
  
  
  // -------------------- GAr --------------------//
  // 


  // -------------------- SiPM Stuff --------------------//
  // -- Quartz/SiO2 -- //
  mQuartz = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4MaterialPropertiesTable *mptQuartz = new G4MaterialPropertiesTable();
  
  // std::vector<G4double> photonE_Quartz, nQuartz, photonE_kQuartz, kQuartz;
  // OH::ReadAndFillArray("dataOptics/sio2_INDEX_fusedQuartz_uv_web.txt", photonE_Quartz, nQuartz, MG::GetVerbosity(), 1., 1., true); // no scaling, flip wavelengths to eV
  // std::reverse(photonE_Quartz.begin(), photonE_Quartz.end());
  // std::reverse(nQuartz.begin(), nQuartz.end());

  // OH::ReadAndFillArray("dataOptics/sio2_KINDEX_fusedQuartz_uv_web.txt", photonE_kQuartz, kQuartz, MG::GetVerbosity(), 1., 1., true); // no scaling, flip wavelengths to eV
  // std::reverse(photonE_kQuartz.begin(), photonE_kQuartz.end());
  // std::reverse(kQuartz.begin(), kQuartz.end());

  // mptQuartz->AddProperty("RINDEX", photonE_Quartz, nQuartz);

  // use built in RINDEX
  mptQuartz->AddProperty("RINDEX", "Fused Silica");
  mQuartz->SetMaterialPropertiesTable(mptQuartz);

  // mHeavyQuartz = nist->FindOrBuildMaterials("G4_MAGNESIUM_FLUORIDE");
  // mHeavyQuartz = nist->FindOrBuildMaterial("G4_BORON_OXIDE");

  // -- Silicon -- //
  mSilicon = nist->FindOrBuildMaterial("G4_Si");
  std::vector<G4double> photonE_Si, nSi;
  OH::ReadAndFillArray("dataOptics/siliconRINDEX_100nm_Pierce.txt", photonE_Si, nSi, MG::GetVerbosity(), 1000., 1., true); // no scaling, flip wavelengths to eV
  std::reverse(photonE_Si.begin(), photonE_Si.end());
  std::reverse(nSi.begin(), nSi.end());

  G4MaterialPropertiesTable *mptSi = new G4MaterialPropertiesTable();
  mptSi->AddProperty("RINDEX", photonE_Si, nSi);
  // mptSi->DumpTable();
  mSilicon->SetMaterialPropertiesTable(mptSi);

  /*
  // added by Ryan, SiPM optical properties////////////////////
  ///https://refractiveindex.info/?shelf=main&book=Si&page=Aspnes////

  const G4int SIPM_NUMENTRIES=46;
  G4double SIPM_PP[SIPM_NUMENTRIES]   =
  {1.499929801*eV,1.600002547*eV,1.700043842*eV,1.800002866*eV,1.900140956*eV,2.000067711*eV,2.100003343*eV,2.199861558*eV,2.299836716*eV,2.400003821*eV,2.500185469*eV,2.599794451*eV,2.700004298*eV,2.800004458*eV,2.900215144*eV,2.999859603*eV,3.10038003*eV,3.199592191*eV,3.300085105*eV,3.399621535*eV,3.50040083*eV,3.600005731*eV,3.699916365*eV,3.799699583*eV,3.900100578*eV,3.999490238*eV,4.100006527*eV,4.200006687*eV,4.300527138*eV,4.399723115*eV,4.500333843*eV,4.600526805*eV,4.699931667*eV,4.800007642*eV,4.900561162*eV,4.999362798*eV,5.100131526*eV,5.200679421*eV,5.300735245*eV,5.400008597*eV,5.500629875*eV,5.600008915*eV,5.700422868*eV,5.799073779*eV,5.901199304*eV,6.001171219*eV}; // lambda range 4 ri

  G4double SIPM_RIND[SIPM_NUMENTRIES] = {3.673,3.714,3.752,3.796,3.847,3.906,3.969,4.042,4.123,4.215,4.32,4.442,4.583,4.753,4.961,5.222,5.57,6.062,6.709,6.522,5.61,5.296,
  5.156,5.065,5.016,5.01,5.02,4.888,4.087,3.12,2.452,1.988,1.764,1.658,1.597,1.57,1.571,1.589,1.579,1.471,1.34,1.247,1.186,1.133,1.083,1.01};// ref index

  G4double SIPM_ABSL[SIPM_NUMENTRIES] = {13156.1356*nm,7708.300175*nm,5803.756167*nm,4216.506079*nm,3245.364476*nm,2242.342251*nm,1566.130829*nm,1401.599554*nm,893.7808372*nm,685.1822378*nm,540.5976989*nm,421.6846163*nm,281.1004052*nm,216.1837034*nm,167.5880415*nm,122.2689183*nm,82.23247918*nm,48.94790427*nm,22.65010442*nm,10.72930123*nm,9.352080867*nm,9.175523773*nm,8.72046998*nm,8.160557761*nm,7.56079627*nm,6.879460054*nm,6.047986211*nm,5.064014322*nm,4.252615698*nm,4.196405851*nm,4.314096815*nm,4.584600492*nm,4.907236123*nm,5.165988222*nm,5.370417203*nm,5.535987378*nm,5.641835489*nm,5.656478457*nm,5.551362828*nm,5.428260945*nm,5.432250485*nm,5.495624401*nm,5.547631582*nm,5.587574781*nm,5.606881247*nm,5.651835859*nm};// atten length

  G4MaterialPropertiesTable *SIPM_mt = new G4MaterialPropertiesTable();
  SIPM_mt->AddProperty("RINDEX", SIPM_PP, SIPM_RIND, SIPM_NUMENTRIES);
  SIPM_mt->AddProperty("ABSLENGTH", SIPM_PP, SIPM_ABSL, SIPM_NUMENTRIES);
  SIPM->SetMaterialPropertiesTable(SIPM_mt);
  */
  
  // -------------------- More Stuff --------------------//
  return;
}











// end