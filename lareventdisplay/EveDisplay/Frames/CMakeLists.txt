include(CetRootCint)
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

set( PACKAGE lareventdisplay_EveDisplay_Frames )
FILE( GLOB src_files *.cxx )

set( EVE_LIBS ${ART_FRAMEWORK_SERVICES_REGISTRY}
              ${ART_FRAMEWORK_CORE}
              art_Persistency_Common canvas_Persistency_Common
              ${ART_FRAMEWORK_IO_SOURCES}
              ${ART_FRAMEWORK_IO_ROOT}
              ${ART_FRAMEWORK_IO_ROOT_ROOTINPUT_SOURCE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_FRAMEWORK_SERVICES_USERINTERACTION}
              art_Persistency_Provenance canvas_Persistency_Provenance
              art_Utilities canvas_Utilities
              ${MF_MESSAGELOGGER}
              ${MF_UTILITIES}
              ${FHICLCPP}
              ${CETLIB}
              nutools_EventDisplayBase
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
              lareventdisplay_EveDisplay_GUI 
              lareventdisplay_EveDisplay_Config)

cet_rootcint( ${PACKAGE} )

art_make_library( LIBRARY_NAME ${PACKAGE}
                  SOURCE ${src_files} ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}Cint.cc
                  LIBRARIES nusimdata_SimulationBase ${EVE_LIBS} 
                  larcore_Geometry
                  EveDisplay_service
                  GeoConfig_service
                )

install_headers()
install_fhicl()
install_source()
