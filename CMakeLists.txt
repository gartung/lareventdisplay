# ======================================================================
#  lareventdisplay main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/lareventdisplay/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# use cmake 3.12 or later
cmake_minimum_required (VERSION 3.12)

project(lareventdisplay VERSION 08.12.03 LANGUAGES CXX)

set(fcl_dir job)
find_package( cetmodules REQUIRED )
include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_package( ROOT REQUIRED COMPONENTS GenVector Gui Geom Minuit Minuit2 XMLIO Gdml TMVA EG X3d Eve)
set(HAVE_ROOT6 BOOL:True)
set(ROOTCLING ROOT::rootcling)
find_package( larreco REQUIRED )
# use the system zlib
find_package( ZLIB REQUIRED )
include_directories( ZLIB_INCLUDE_DIRS )

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(lareventdisplay)
add_subdirectory(cmakeConfig)
# ups - table and config files
#add_subdirectory(ups)

# packaging utility
#include(UseCPack)
