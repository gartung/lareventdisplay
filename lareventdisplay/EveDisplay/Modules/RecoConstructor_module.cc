// \file: RecoConstructor_module.cc
// \brief: Instantiation of a module to create scenes of reconstructed 3D objects
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/GenericSceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/SimpleGeoMaker.h"
#include "lareventdisplay/EveDisplay/Algs/GenericViewerMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/SceneConstructor.h"
#include "lardataobj/RecoBase/Track.h"
#include "nusimdata/SimulationBase/MCParticle.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h" 

#ifndef MODULES_RECOCONSTRUCTOR_MODULE_CC
#define MODULES_RECOCONSTRUCTOR_MODULE_CC

namespace
{
  std::string reco("Reco");
}

namespace eved
{
  typedef GenericSceneMaker<reco, recob::Track> RecoScene;
}

DEFINE_ART_MODULE(eved::SceneConstructor<eved::RecoScene>)

#endif
