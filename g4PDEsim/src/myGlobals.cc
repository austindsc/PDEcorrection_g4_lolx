
#include "myGlobals.hh"


namespace MG{
  static int atownsNumber = 69;
  static int myVerbose = 0;
  void SetVerbosity(int a){ myVerbose = a;}
  int GetVerbosity(){ return myVerbose; }

  // -- PDE toggling --//
  // default behaviour is absway
  static TestingMode testingPDE = AbsWay;

  TestingMode GetPDETesting(){ return testingPDE; }
  void SetPDETesting(TestingMode a) { testingPDE = a; }
}

namespace DETECTORSETUP{

  // default values in here
  static bool buildWindow = false;
  static bool buildSurface = true; // this also turns off correction borders
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