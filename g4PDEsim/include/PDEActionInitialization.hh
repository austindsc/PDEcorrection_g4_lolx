/// \file PDEActionInitialization.hh
/// \brief Definition of the PDEActionInitialization class

#ifndef PDEActionInitialization_h
#define PDEActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "PDERunAction.hh"

/// Action initialization class.

class PDEDetectorConstruction;

class PDEActionInitialization : public G4VUserActionInitialization
{
  public:
    PDEActionInitialization(PDEDetectorConstruction * detCons);
    virtual ~PDEActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

    PDEDetectorConstruction * fDetCons;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
