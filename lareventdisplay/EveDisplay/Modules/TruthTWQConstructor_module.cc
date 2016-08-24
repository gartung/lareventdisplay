// \file: TruthConstructor_module.cc
// \brief: Instantiation of a module to create scenes of reconstructed 3D objects
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/ProjSceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TWQProjector.h"
#include "lareventdisplay/EveDisplay/Algs/SimpleGeoMaker.h"
#include "lareventdisplay/EveDisplay/Algs/GenericViewerMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/SimPartMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/SceneConstructor.h"
#include "SimulationBase/MCParticle.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h" 

#ifndef MODULES_TRUTHPROJCONSTRUCTOR_MODULE_CC
#define MODULES_TRUTHPROJCONSTRUCTOR_MODULE_CC

namespace
{
  std::string truth("Truth");
}

namespace eved
{
  typedef ProjSceneMaker<truth, eved::TWQProjector, simb::MCParticle> TruthProjScene;
}

DEFINE_ART_MODULE(eved::SceneConstructor<eved::TruthProjScene>)

#endif
