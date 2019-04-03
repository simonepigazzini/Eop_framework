# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Create a library called "Eoplib" which includes the source files in sources and headers.
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc )
file(GLOB headers ${PROJECT_SOURCE_DIR}/interface/*.h )
add_library (Eoplib ${sources} ${headers})

# Make sure the compiler can find include files for our Eoplib library
# when other libraries or executables link to Eoplib
target_include_directories (Eoplib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
