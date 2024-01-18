/// \file PDEDetectorConstruction.hh
/// \brief Definition of the PDEDetectorConstruction class

#ifndef PDEDetectorConstruction_h
#define PDEDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"

class G4VPhysicalVolume;
class PDEDetectorConstructionMessenger;
class PDEDetectorMaterials;
class SensitiveSIPM;

/// Detector construction class to define materials and geometry.
class PDEDetectorConstruction : public G4VUserDetectorConstruction
{
  friend class PDESteppingAction;
  friend class PDETrackingAction;
  public:
    PDEDetectorConstruction();
    virtual ~PDEDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    
    // getters and setters
    G4LogicalVolume* GetScoringVolume() const { return fWorld; }

    G4double GetTopOfWorld() { return ((G4Box*)fWorld->GetSolid())->GetZHalfLength(); }

  protected:  

    G4double fBulkZ;
    G4LogicalVolume* fLogBulk;
    static G4String fStrBulk;

    G4double fSurfaceZ;
    G4LogicalVolume* fLogSurface;
    static G4String fStrSurface;

    G4double fWindowZ;
    G4LogicalVolume* fLogWindow;
    static G4String fStrWindow;

    G4double fBackingZ;
    G4LogicalVolume* fLogBacking;
    static G4String fStrBacking;
    
    G4LogicalVolume* fWorld;
    
  // private:
    virtual void ConstructSDandField();
    
    PDEDetectorMaterials * detMat;
    PDEDetectorConstructionMessenger* fDetMessenger;

    G4LogicalVolume* fsipmDetectionBack;
    SensitiveSIPM* fSingleSipm;
    // G4LogicalSkinSurface * f
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

