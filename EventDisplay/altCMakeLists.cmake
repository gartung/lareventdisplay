include(CetRootCint)
include(CetParseArgs)
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )
 
set(HEADERS
     AnalysisBaseDrawer.h
     AnalysisDrawingOptions.h
     CalorPad.h
     CalorView.h
     ColorDrawingOptions.h
     Display3DPad.h
     Display3DView.h
     DrawingPad.h
     EvdLayoutOptions.h
     GeometryDrawer.h
     GraphClusterAlg.h
     HeaderDrawer.h
     HeaderPad.h
     HitSelector.h
     InfoTransfer.h
     MCBriefPad.h
     Ortho3DPad.h
     Ortho3DView.h
     OrthoProj.h
     RawDataDrawer.h
     RawDrawingOptions.h
     RecoBaseDrawer.h
     RecoDrawingOptions.h
     SimulationDrawer.h
     SimulationDrawingOptions.h
     Style.h
     TQPad.h
     TWQMultiTPCProjection.h
     TWQProjectionView.h
     TWireProjPad.h
     eventdisplay.h
     )

set(SOURCES 
     AnalysisBaseDrawer.cxx
     CalorPad.cxx
     CalorView.cxx
     Display3DPad.cxx
     Display3DView.cxx
     DrawingPad.cxx
     GeometryDrawer.cxx
     GraphClusterAlg.cxx
     HeaderDrawer.cxx
     HeaderPad.cxx
     HitSelector.cxx
     MCBriefPad.cxx
     Ortho3DPad.cxx
     Ortho3DView.cxx
     RawDataDrawer.cxx
     RecoBaseDrawer.cxx
     SimulationDrawer.cxx
     Style.cxx
     TQPad.cxx
     TWQMultiTPCProjection.cxx
     TWQProjectionView.cxx
     TWireProjPad.cxx
     )

set( PACKAGE EventDisplay )

FILE( GLOB src_files *.cxx )
 
cet_rootcint( ${PACKAGE} )
 
add_library(EventDisplay SHARED
	${HEADERS}
	${SOURCES} 
	${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}Cint.cc
	)

target_link_libraries(EventDisplay
     larsoft::Filters
     larsoft::Simulation 
     larsoft::RawData
     larsoft::RecoBase
     larsoft::RecoObjects
     larsoft::Utilities
     larsoft::AnalysisBase
     larsoft::AnalysisAlg
     ${SIMULATIONBASE}
     ${EVENTDISPLAYBASE}
     art::art_Framework_Core
     art::art_Framework_Principal
     art::art_Persistency_Provenance
     art::art_Utilities
     art::art_Framework_Services_Registry
     FNALCore::FNALCore
     ${ROOT_EVE_LIB_LIST}
     ${ROOT_X3d}
)

art_add_module(GraphCluster_module 
	GraphCluster_module.cc
	)


art_add_module(EVD_module 
	EVD_module.cc
	)


art_add_service(AnalysisDrawingOptions_service 
	AnalysisDrawingOptions_service.cc
	)
 
art_add_service(ColorDrawingOptions_service 
	ColorDrawingOptions_service.cc
	)

art_add_service(EvdLayoutOptions_service 
	EvdLayoutOptions_service.cc
	)


art_add_service(InfoTransfer_service 
	InfoTransfer_service.cc
	)


art_add_service(RawDrawingOptions_service 
	RawDrawingOptions_service.cc
	)


art_add_service(RecoDrawingOptions_service 
	RecoDrawingOptions_service.cc
	)

art_add_service(SimulationDrawingOptions_service 
	SimulationDrawingOptions_service.cc
	)


install(FILES ${HEADERS} DESTINATION
     ${CMAKE_INSTALL_INCLUDEDIR}/EventDisplay COMPONENT Development)

file(GLOB FHICL_FILES 
     [^.]*.fcl
)

install(FILES ${FHICL_FILES} DESTINATION job COMPONENT Runtime)



install(TARGETS
     EventDisplay
     GraphCluster_module
     EVD_module
     AnalysisDrawingOptions_service
     ColorDrawingOptions_service
     EvdLayoutOptions_service
     InfoTransfer_service
     RawDrawingOptions_service
     RecoDrawingOptions_service
     SimulationDrawingOptions_service
     EXPORT lareventdisplayLibraries
     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
     COMPONENT Runtime
     )
