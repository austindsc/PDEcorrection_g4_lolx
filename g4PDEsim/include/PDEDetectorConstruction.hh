/// \file PDEDetectorConstruction.hh
/// \brief Definition of the PDEDetectorConstruction class

#ifndef PDEDetectorConstruction_h
#define PDEDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class PDEDetectorConstructionMessenger;
class DetectorMaterials;

/// Detector construction class to define materials and geometry.
class PDEDetectorConstruction : public G4VUserDetectorConstruction
{

  public:
    PDEDetectorConstruction();
    virtual ~PDEDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    
    // getters and setters
    G4LogicalVolume* GetScoringVolume() const { return fWorld; }

  protected:    
    
  private:
    virtual void ConstructSDandField();
    
    DetectorMaterials * detMat;
    PDEDetectorConstructionMessenger* fDetMessenger;
    G4LogicalVolume* fWorld;
    G4LogicalVolume* fsipmVolume;
  
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

