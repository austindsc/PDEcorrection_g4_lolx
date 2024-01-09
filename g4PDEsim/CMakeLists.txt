# mkdir buildName
# cd buildName
# cmake ../myPDE_or_whatever_new_projectNameIs

# see refpropNotes.txt for install instructions relevant to working with this project
# some env variables, installs and data locations are required


#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(projPDE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "-std=c++17 -lstdc++fs") 

# compiler argument for how do you want to do the optics
option(USE_METAL_COMPLEX "Use both indexes instead of RINDEX, define as metals" ON)
if(USE_METAL_COMPLEX)
  add_definitions(-DUSE_METAL_COMPLEX=1)
else()
  add_definitions(-DUSE_METAL_COMPLEX=0)
endif()

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED qt ui_all vis_all)
else()
  find_package(Geant4 REQUIRED qt)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

## using ROOT ##
find_package(ROOT REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIRS}
                  )


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
# add_library(OpticalHelper )
# instead this is just a header/source file with a namespace

add_executable(pde pdeCorrection.cc ${sources} ${headers})
target_link_libraries(pde ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build PDE. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
# set(EXAMPLEPDE_SCRIPTS
#   examplePDE.in
#   examplePDE.out
#   init_vis.mac
#   run1.mac
#   run2.mac
#   AtownRun.mac
#   myVis.mac
#   vis.mac
#   )

# foreach(_script ${EXAMPLEPDE_SCRIPTS})
#   configure_file(
#     ${PROJECT_SOURCE_DIR}/${_script}
#     ${PROJECT_BINARY_DIR}/${_script}
#     COPYONLY
#     )
# endforeach()

file(GLOB DATA_FOLDER "data")
file(COPY ${DATA_FOLDER} DESTINATION ${PROJECT_BINARY_DIR})
message(STATUS "Copied ${DATA_FOLDER} to build directory")


file(GLOB DATA_FOLDER "dataOptics")
file(COPY ${DATA_FOLDER} DESTINATION ${PROJECT_BINARY_DIR})
message(STATUS "Copied ${DATA_FOLDER} to build directory")
#message(STATUS "project binary dir =" ${PROJECT_BINARY_DIR})

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(projPDE DEPENDS pde)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS pde DESTINATION bin)
