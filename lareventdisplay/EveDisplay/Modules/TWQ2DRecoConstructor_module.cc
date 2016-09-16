// \file: TWQ2DRecoConstructor_module.cc
// \brief: Instantiation of a module to create scenes of reconstructed 2D objects in TWQ view
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/TWQSceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/GenericViewerMaker.h"
#include "lareventdisplay/EveDisplay/Algs/SceneConstructor.h"
#include "lareventdisplay/EveDisplay/Algs/WireMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/HitMakerInt.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardataobj/RecoBase/Hit.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h" 

#ifndef MODULES_TWQ2DRECOCONSTRUCTOR_MODULE_CC
#define MODULES_TWQ2DRECOCONSTRUCTOR_MODULE_CC

namespace
{
  std::string reco2D("Reco2D");
}

namespace eved
{
  typedef TWQSceneMaker<reco2D, recob::Wire, recob::Hit> Reco2DScene;
}

DEFINE_ART_MODULE(eved::SceneConstructor<eved::Reco2DScene>)

#endif
