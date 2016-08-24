#ifndef EVED_RECOSCENE_MODULE_CC
#define EVED_RECOSCENE_MODULE_CC

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lardata/RecoBase/Track.h"
#include "lareventdisplay/EveDisplay/Algs/GenericScene.h"

//ART includes
#include "art/Framework/Core/ModuleMacros.h"

namespace eved
{
  typedef eved::GenericScene<recob::Track> RecoScene;
}

DEFINE_ART_MODULE(eved::RecoScene)

#endif
