/// \file PDERunAction.hh
/// \brief Definition of the PDERunAction class

#ifndef PDERunAction_h
#define PDERunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include "PDEPrimaryGeneratorAction.hh"

class G4Run;
// class PDEPrimaryGeneratorAction;

/// Run action class
///
/// In EndOfRunAction(), it calculates the dose in the selected volume 
/// from the energy deposit accumulated via stepping and event actions.
/// The computed dose is then printed on the screen.

class PDERunAction : public G4UserRunAction
{
  friend class PDEEventAction;
  friend class PDEPrimaryGeneratorAction;
  public:
    PDERunAction(PDEPrimaryGeneratorAction * genny);
    virtual ~PDERunAction();

    // virtual G4Run* GenerateRun();
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

  private:

    PDEPrimaryGeneratorAction * fGenerator;
    
    G4int    ftotalHitPE;
    G4int    ftotalFired;
};

#endif

