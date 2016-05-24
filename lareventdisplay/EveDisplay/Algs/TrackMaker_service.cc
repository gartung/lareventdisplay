// \file: TrackMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing recob::Track in the EVE-based event display.  Implements TrackMakerInt.h. 

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/TrackMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lardata/RecoBase/Track.h"

//ROOT includes
#include "TEveTrack.h"
#include "TEveElement.h"
#include "TEveTrackPropagator.h"

#ifndef EVED_TRACKMAKER_SERVICE_CC
#define EVED_TRACKMAKER_SERVICE_CC

namespace eved
{

    TEveElement* eved::TrackMaker::MakeVis(const recob::Track& track) //implementation of pure virtual function
    {
      TEveTrack* retVal = new TEveTrack(); //This is the TEveTrack we will return
      retVal->SetPropagator(new TEveTrackPropagator());
      for(size_t posIt = 0; posIt < track.NumberTrajectoryPoints(); ++posIt) //loop over track trajectory points
      {
        TEveVectorT<double> evePos;
        evePos.Set(track.LocationAtPoint(posIt));
        retVal->AddPathMark(TEvePathMarkD(TEvePathMarkD::kLineSegment, evePos)); 
      }

      retVal->MakeTrack();

      return (TEveElement*)retVal;
    } 
    
}

DEFINE_ART_SERVICE_INTERFACE_IMPL(eved::TrackMaker, eved::VisMakerInt<recob::Track>)

#endif
