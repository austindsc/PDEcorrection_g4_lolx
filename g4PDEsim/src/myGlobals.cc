
#include "myGlobals.hh"


namespace MG{
  static int atownsNumber = 69;
  static int myVerbose = 0;

  enum TestPDEMode {
    OldWay = 0,  // any photon touches photon backing calls ProcessHits
    NewWay = 1   // photons that hit the bulk and aren't reflected (EFFICIENCY = 100)
                  // call custom processHits method in sensitive SiPM
  };
  // static int myTesting = 0;  // 0 - do things the old way (any photon touches photon backing)
  static int myTesting = 0;     // 1 - do things the new way (photons that hit the bulk and aren't reflected (EFFICIENCY = 100)
                                // call custom processHits method in sensitive SiPM
  // a better way to do this toggling would be to define a preCompiler command,
  // or define some variables that are the different 'cases'
  
  void SetVerbosity(int a){ myVerbose = a;}
  int GetVerbosity(){ return myVerbose; }
  int GetTesting(){ return myTesting; }
}

namespace DETECTORSETUP{
  // default values in here
  static bool buildWindow = false;
  static bool buildSurface = true;
  static bool turnOnQuartzAbs = false;
  static bool makeVisible = true; // scale up the small things

  bool GetWindowStatus(){ return buildWindow; }
  void SetWindowStatus(bool a){ buildWindow = a; }

  bool GetSurfaceStatus(){ return buildSurface; }
  void SetSurfaceStatus(bool a){ buildSurface = a; }

  bool GetQuartzABSstatus(){ return turnOnQuartzAbs; }
  void SetQuartzABSstatus(bool a){ turnOnQuartzAbs = a; }

  bool GetVisibleStatus(){ return makeVisible; }
  void SetVisibleStatus(bool a){ makeVisible = a; }
}