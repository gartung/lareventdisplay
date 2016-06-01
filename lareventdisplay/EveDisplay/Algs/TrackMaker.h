// \file: TrackMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing recob::Track in the EVE-based event display.  Implements TrackMakerInt.h. 

//ART includes
#include "fhiclcpp/ParameterSet.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lardata/RecoBase/Track.h"

//ROOT includes
#include "TEveTrack.h"
#include "TEveElement.h"

#ifndef EVED_TRACKMAKER_H
#define EVED_TRACKMAKER_H

namespace eved
{
  class TrackMaker: public VisMakerInt<recob::Track>
  {
    public:
      TrackMaker(const fhicl::ParameterSet& p, art::ActivityRegistry &reg): VisMakerInt<recob::Track>(p, reg) {}
      ~TrackMaker() = default;
  
      TEveElement* MakeVis(const recob::Track& track); //implementation of pure virtual function

      void reconfigure(fhicl::ParameterSet const & p);

  };
}

DECLARE_ART_SERVICE_INTERFACE_IMPL(eved::TrackMaker, eved::VisMakerInt<recob::Track>, LEGACY)

#endif
