# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(EoP_framework)


# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/interface)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc )
file(GLOB headers ${PROJECT_SOURCE_DIR}/interface/*.h )

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(BuildEopEta main/BuildEopEta.cpp ${sources} ${headers})
target_link_libraries(BuildEopEta ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS BuildEopEta DESTINATION bin)


# If necessary, use the RELATIVE flag, otherwise each source file may be listed 
# with full pathname. RELATIVE may makes it easier to extract an executable name
# automatically.
# file( GLOB APP_SOURCES RELATIVE app/*.cxx )
#file(GLOB APP_SOURCES ${PROJECT_SOURCE_DIR}/main/*.cpp )
#foreach( testsourcefile ${APP_SOURCES} )
    # I used a simple string replace, to cut off .cpp.
#    string( REPLACE ".cpp" "" testname ${testsourcefile} )
#    add_executable( ${testname} ${testsourcefile} ${sources} ${headers})
    # Make sure YourLib is linked to each app
#    target_link_libraries( ${testname} ${ROOT_LIBRARIES} )
#    install(TARGETS ${testname} DESTINATION bin)
#endforeach( testsourcefile ${APP_SOURCES} )


