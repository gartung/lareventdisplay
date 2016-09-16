// \file: RawConstructor_module.cc
// \brief: Instantiation of a module to create scenes of reconstructed 3D objects
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/TWQSceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/GenericViewerMaker.h"
#include "lareventdisplay/EveDisplay/Algs/SceneConstructor.h"
#include "lareventdisplay/EveDisplay/Algs/RawDigitMakerInt.h"
#include "lardataobj/RawData/RawDigit.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h" 

#ifndef MODULES_RECOCONSTRUCTOR_MODULE_CC
#define MODULES_RECOCONSTRUCTOR_MODULE_CC

namespace
{
  std::string rawL("Raw");
}

namespace eved
{
  typedef TWQSceneMaker<rawL, raw::RawDigit> RawScene;
}

DEFINE_ART_MODULE(eved::SceneConstructor<eved::RawScene>)

#endif
