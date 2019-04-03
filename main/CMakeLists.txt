# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

set(CMAKE_EXECUTABLE_SUFFIX ".exe")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Add executable called "helloDemo" that is built from the source files
#add_executable(BuildEopEta_EB BuildEopEta_EB.cpp)

# Link the executable to the Eoplib library. Since the Eoplib library has
# public include directories we will use those link directories when building
# helloDemo
#target_link_libraries (BuildEopEta_EB LINK_PUBLIC Eoplib ${ROOT_LIBRARIES})

# If necessary, use the RELATIVE flag, otherwise each source file may be listed 
# with full pathname. RELATIVE may makes it easier to extract an executable name
# automatically.
#file( GLOB APP_SOURCES RELATIVE app/*.cxx )
file(GLOB APP_SOURCES *.cpp )
foreach( sourcefile ${APP_SOURCES} )
   # I used a simple string replace, to cut off .cpp.
   string( REPLACE ${PROJECT_SOURCE_DIR}/main/ "" relsourcefile ${sourcefile} )
   string( REPLACE ".cpp" "" exename ${relsourcefile} )
   add_executable( ${exename} ${relsourcefile} )
   # Make sure Eoplib is linked to each app
   target_link_libraries(${exename} LINK_PUBLIC Eoplib ${ROOT_LIBRARIES} )
#    install(TARGETS ${exename} DESTINATION bin)
endforeach( sourcefile ${APP_SOURCES} )


