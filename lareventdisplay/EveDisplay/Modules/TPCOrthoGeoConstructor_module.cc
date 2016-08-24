// \file: TPCOrthoGeoConstructor_module.cc
// \brief: Instantiation of a module to create scenes of reconstructed 3D objects
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/GenericSceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TPCOnlyGeoMaker.h"
#include "lareventdisplay/EveDisplay/Algs/GenericViewerMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/SimPartMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/SceneConstructor.h"
#include "lardata/RecoBase/Track.h"
#include "SimulationBase/MCParticle.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h" 

#ifndef MODULES_TPCORTHOGEOCONSTRUCTOR_MODULE_CC
#define MODULES_TPCORTHOGEOCONSTRUCTOR_MODULE_CC

DEFINE_ART_MODULE(eved::SceneConstructor<eved::TPCOnlyGeoMaker>)

#endif
