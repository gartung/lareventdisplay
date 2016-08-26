include(CetRootCint)
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

set( PACKAGE lareventdisplay_EveDisplay_Modules )
FILE( GLOB src_files *.cxx )

set( EVE_LIBS ${ART_FRAMEWORK_SERVICES_REGISTRY}
              ${ART_FRAMEWORK_CORE}
              ${ART_PERSISTENCY_COMMON}
              ${ART_FRAMEWORK_IO_SOURCES}
              ${ART_FRAMEWORK_IO_ROOT}
              ${ART_FRAMEWORK_IO_ROOT_ROOTINPUT_SOURCE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_FRAMEWORK_SERVICES_USERINTERACTION}
              ${ART_PERSISTENCY_PROVENANCE}
              ${ART_UTILITIES}
              ${MF_MESSAGELOGGER}
              ${MF_UTILITIES}
              ${FHICLCPP}
              ${CETLIB}
              ${EVENTDISPLAYBASE}
              ${Boost_SYSTEM_LIBRARY}
              ${SQLITE3}
              ${Boost_FILESYSTEM_LIBRARY}
              ${Boost_REGEX_LIBRARY}
              ${ROOT_EVE}
              ${ROOT_EG}
              ${ROOT_GX11}
              ${ROOT_TREEPLAYER}
              ${ROOT_GEOM}
              ${ROOT_GED}
              ${ROOT_RGL}
              ${ROOT_GUI}
              ${ROOT_CORE}
              ${ROOT_CINT}
              ${ROOT_RIO}
              ${ROOT_NET}
              ${ROOT_HIST}
              ${ROOT_GRAF}
              ${ROOT_GRAF3D}
              ${ROOT_GPAD}
              ${ROOT_TREE}
              ${ROOT_RINT}
              ${ROOT_POSTSCRIPT}
              ${ROOT_MATRIX}
              ${ROOT_PHYSICS}
              ${ROOT_MATHCORE}
              ${ROOT_THREAD} 
              lareventdisplay_EveDisplay_Algs
              lareventdisplay_EveDisplay_GUI
              lareventdisplay_EveDisplay_Util)

cet_rootcint( ${PACKAGE} )

art_make_library( LIBRARY_NAME ${PACKAGE}
                  SOURCE ${src_files} ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}Cint.cc
                  LIBRARIES SimulationBase ${EVE_LIBS} 
                  larcore_Geometry
                )

set( EVE_SERVICE_LIBS ${PACKAGE} ${EVE_LIBS} )
simple_plugin( DEdxViewer module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( TPC3DConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( RecoConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( TruthConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( GeoConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( TPCOrthoGeoConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( TPCOrthoConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( TPCTWQConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( RawConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( RecoTWQConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( TruthTWQConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( TWQGeoConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )
simple_plugin( TWQ2DRecoConstructor module ${EVE_SERVICE_LIBS} BASENAME_ONLY )

install_headers()
install_fhicl()
install_source()
