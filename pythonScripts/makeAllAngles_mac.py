import os
import sys

import numpy as np
import scipy as sp

## Define the number of photons for each data point
Nphoton = 100000
zHeight = 0.5 # cm # if too far some photons will miss. should check manually at your max angle
myVerbosity = 0
pickMedium = "lxe"
mediumOptions = ["air", "lcf4", "lxe"] # need to compare to G4 code if you add stuff

cosineAngleSpacing = True # use non-linear spacing

for med in mediumOptions:
  pickMedium = med
  ## same input, output for one wavelength
  wlStart = 110
  wlEnd = 1050
  wlStep = 5

  ## same as above for fixed angle
  angleLow = 0
  angleHigh = 85
  angleStep = 15

  fixedPhi = -90 # leave this
  tRad = np.pi/180 # don't change this lol

  if pickMedium not in mediumOptions:
    exit("ERROR: pickMedium not in mediumOptions")

  nstring = f"{Nphoton:e}"; 
  nstring = nstring.replace("0", "").replace(".", "").replace("+", "")
  outputName = f"final_{pickMedium}_N{nstring}_{wlStart}-{wlEnd}nm_{angleLow}-{angleHigh}deg.mac"

  strVerbosities = """
  /vis/verbose 0
  /tracking/verbose 0
  /event/verbose 0
  /process/had/verbose 0
  /run/verbose 0
  /particle/verbose 0
  /tracking/verbose 0

  /process/em/verbose 0
  /process/optical/verbose 0
  #/process/optical/scintillation/verbose 1
  """

  strProcesses = """
  /process/optical/processActivation OpRayleigh false
  /process/optical/processActivation OpAbsorption false
  /process/optical/processActivation OpBoundary true
  /process/optical/processActivation OpMieHG false
  /process/optical/processActivation OpWLS false
  """

  # Check if the command line argument is provided
  if len(sys.argv) > 1:
    newMacName = sys.argv[1]
  else:
    newMacName = outputName
    
  summaryString = f"# medium = {pickMedium}\n"
  summaryString += "# Nphoton = {}, wlStart = {}, wlEnd = {}, wlStep = {}, \n#angleLow = {}, angleHigh = {}, angleStep = {}\n".format(Nphoton, wlStart, wlEnd, wlStep, angleLow, angleHigh, angleStep)

  # Define the range of wavelengths and angles
  wavelengths = np.arange(start = wlStart, stop = wlEnd + wlStep, step=wlStep)
  numAngles = int((angleHigh - angleLow)/angleStep)
  numAngles = 1 if numAngles == 0 else numAngles # if we want a single angle
  angles = (1/tRad) * np.arccos(np.linspace(np.cos(angleLow*tRad), np.cos(angleHigh*tRad), num = numAngles, endpoint= True)) # do spacing linear in cos(theta)
  if cosineAngleSpacing == False:
    angles = np.linspace(angleLow, angleHigh, num = numAngles+1, endpoint= True)

  # Get the directory of the current script
  script_path = os.path.abspath(__file__)
  script_dir = os.path.dirname(script_path)

  ## don't use this
  # skeleton_mac_path = os.path.join(script_dir, "macSkeleton.mac")

  # # Open the skeleton.mac file
  # with open(skeleton_mac_path, "r") as skeleton_mac_file:
  #   skeleton_mac_contents = skeleton_mac_file.read()

  # Print the contents of the skeleton.mac file
  # print(skeleton_mac_contents)


  output_file = os.path.join(script_dir, newMacName)
  # Open the file in write mode
  with open(output_file, "w") as file:
    
    ## summary string
    file.write(summaryString + "\n")
    file.write("# --- --- --- --- --- # \n")
    
    ## verbosities and processes
    file.write(strVerbosities)
    file.write(strProcesses)
    # file.write(skeleton_mac_contents + "\n")
    file.write("\n\n")
    file.write("# --- --- --- --- --- # \n\n")
    
    ## medium and my verbosity
    file.write(f"/setDetector/myVerbosity {myVerbosity}\n")
    file.write(f"/setMaterial/detectionMedium {pickMedium}\n")
    
    file.write("# --- --- --- --- --- # \n\n")
    
    file.write("/analysis/setFileName " + newMacName.rstrip(".mac") + " \n")    
    
    file.write("/run/initialize \n\n")
    
    file.write("# --- --- --- --- --- # \n\n")
    
    file.write("/gps/pos/type Point\n")
    file.write("/gps/particle opticalphoton\n\n")
    
    file.write(f"/gps/position 0 -9.5 {zHeight} cm\n")
    file.write(f"/gps/number {int(Nphoton)}\n")
    file.write(f"/gps/ang/minphi {fixedPhi} deg\n")
    file.write(f"/gps/ang/maxphi {fixedPhi} deg\n")
    
    file.write("# --- --- --- --- --- # \n\n")
    
    for wavelength in wavelengths:
      energy = 1240/wavelength
      file.write(f"/gps/ene/mono {energy:0.2f} eV\n")
      
      for angle in angles:
        # zComponent = np.cos(angle*tRad)
        # yComponent = np.sin(angle*tRad)
        # file.write(f"/gps/direction 0 {yComponent} {zComponent}\n")
        
        file.write(f"/gps/ang/mintheta {angle:.1f} deg \n")
        file.write(f"/gps/ang/maxtheta {angle:.1f} deg \n")
        
        file.write(f"/run/beamOn {1}\n\n")
        file.write("# --- --- --- # \n\n")

  #   # Close the file
    file.write("# END")