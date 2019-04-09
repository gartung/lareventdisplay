cet_find_library( SQLITE3  NAMES sqlite3 )
cet_find_library( ZLIB  NAMES z )
include(${Geant4_USE_FILE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Geant4_CXX_FLAGS} -Wno-shadow -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-overloaded-virtual")
include_directories ( ${Geant4_INCLUDE_DIRS} )
art_make( 
         MODULE_LIBRARIES 
            larcore_Geometry_Geometry_service
            lareventdisplay_Landed_LandedSocket_service
            ${ART_ROOT_IO_TFILESERVICE_SERVICE}
            ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
            larcorealg_Geometry
            larsim_Simulation nutools_ParticleNavigation lardataobj_Simulation
            lardata_Utilities
            larevt_Filters
            lardataobj_RawData
            nutools_NuReweight_art
            nusimdata_SimulationBase
            ${ART_FRAMEWORK_CORE}
            ${ART_FRAMEWORK_PRINCIPAL}
            ${ART_FRAMEWORK_SERVICES_REGISTRY}
            ${ART_ROOT_IO_TFILE_SUPPORT}
            art_Persistency_Common
            art_Persistency_Provenance
            art_Utilities
            canvas
            ${MF_MESSAGELOGGER}
            ${FHICLCPP}
            cetlib cetlib_except
            ${CLHEP_LIBRARIES}
            ${ROOT_LIBRARIES}
            ${Geant4_LIBRARIES}
            ${SQLITE3}
            ${ZLIB_LIBRARIES}
            nutools_G4Base
      SERVICE_LIBRARIES
            ${ART_FRAMEWORK_CORE}
            art_Utilities canvas
            ${ART_ROOT_IO_ROOTINPUT_SOURCE}
            ${ART_ROOT_IO}
            art_Framework_Principal
            art_Persistency_Provenance
            canvas
            ${MF_MESSAGELOGGER}
            cetlib cetlib_except
            pthread
            ${ROOT_LIBRARIES}
          )

install_headers()
install_fhicl()
install_source()

