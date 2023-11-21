/// \file PDEPrimaryGeneratorAction.hh
/// \brief Definition of the PDEPrimaryGeneratorAction class

#ifndef PDEPrimaryGeneratorAction_h
#define PDEPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4Event;
class G4Tubs;
class G4VPhysicalVolume;
class PDEDetectorConstruction;
class G4SingleParticleSource;

/// The primary generator action class with particle gun.
///
/// The default kinematic is a 6 MeV gamma, randomly distribued 
/// in front of the phantom across 80% of the (X,Y) phantom size.

class PDEPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  friend class PDERunAction;
  friend class PDEEventAction;
  public:
    PDEPrimaryGeneratorAction();    
    virtual ~PDEPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);
  
    // method to access particle gun
    // const G4GeneralParticleSource* GetParticleSource() const { return fGunGamma1; }
  
  protected:
  
  private:
    G4GeneralParticleSource* fGun;
    G4VPhysicalVolume * fPDE_volume;
    PDEDetectorConstruction * fdetconstruction;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
