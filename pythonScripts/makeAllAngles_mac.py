import os
import sys

import numpy as np
import scipy as sp

strVerbosities = """
/vis/verbose 0
/tracking/verbose 0
/event/verbose 0
/process/had/verbose 0
/run/verbose 0
/particle/verbose 0
/tracking/verbose 0

/process/em/verbose 0
/process/optical/verbose 1
#/process/optical/scintillation/verbose 1
"""

strProcesses = """
/process/optical/processActivation OpRayleigh false
/process/optical/processActivation OpAbsorption false
/process/optical/processActivation OpBoundary true
/process/optical/processActivation OpMieHG false
/process/optical/processActivation OpWLS false
"""

# Define the number of photons
Nphoton = 100
zHeight = 3 # cm

newMacName = "testBlah.mac"

pickMedium = "air"
mediumOptions = ["air", "blood"]
if pickMedium not in mediumOptions:
  exit("ERROR: pickMedium not in mediumOptions")

wlStart = 100
wlEnd = 400
wlStep = 50

angleLow = 0
angleHigh = 20
angleStep = 5
tRad = np.pi/180

summaryString = f"# medium = {pickMedium}\n"
summaryString += "# Nphoton = {}, wlStart = {}, wlEnd = {}, wlStep = {}, \n#angleLow = {}, angleHigh = {}, angleStep = {}\n".format(Nphoton, wlStart, wlEnd, wlStep, angleLow, angleHigh, angleStep)

# Define the range of wavelengths and angles
wavelengths = np.arange(start = wlStart, stop = wlEnd, step=wlStep)
numAngles = int((angleHigh - angleLow)/angleStep)
numAngles = 1 if numAngles == 0 else numAngles # if we want a single angle
angles = (1/tRad) * np.arccos(np.linspace(np.cos(angleLow*tRad), np.cos(angleHigh*tRad), num = numAngles, endpoint= True))

# Get the directory of the current script
script_path = os.path.abspath(__file__)
script_dir = os.path.dirname(script_path)
skeleton_mac_path = os.path.join(script_dir, "macSkeleton.mac")

# Open the skeleton.mac file
with open(skeleton_mac_path, "r") as skeleton_mac_file:
  skeleton_mac_contents = skeleton_mac_file.read()

# Print the contents of the skeleton.mac file
# print(skeleton_mac_contents)


output_file = os.path.join(script_dir, newMacName)
# Open the file in write mode
with open(output_file, "w") as file:
  file.write(summaryString + "\n")
  file.write(strVerbosities)
  file.write(strProcesses)
  # file.write(skeleton_mac_contents + "\n")
  
  file.write("\n\n")
  
  file.write("/analysis/setFileName " + newMacName + " \n")    
  
  file.write("/run/initialize + \n\n")
  
  file.write("/gps/pos/type Point\n")
  file.write("/gps/particle opticalphoton\n\n")
  
  file.write(f"/gps/position 0 0 {zHeight} cm\n")
  file.write(f"gps/number {Nphoton}\n")
  
  for wavelength in wavelengths:
    energy = 1240/wavelength
    file.write(f"/gps/energy {energy} eV\n")
    
    for angle in angles:
      zComponent = np.cos(angle*tRad)
      yComponent = np.sin(angle*tRad)
      # Write the necessary commands to generate the photons with the desired angles and wavelengths
      
      file.write(f"/gps/direction 0 {yComponent} {zComponent}\n")
      file.write(f"/run/beamOn {1}\n")

#   # Close the file
  file.write("# END")