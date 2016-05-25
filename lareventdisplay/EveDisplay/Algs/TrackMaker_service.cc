// \file: TrackMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing recob::Track in the EVE-based event display.  Implements TrackMakerInt.h. 

//ART includes
#include "fhiclcpp/ParameterSet.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/TrackMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lardata/RecoBase/Track.h"

//ROOT includes
#include "TEveTrack.h"
#include "TEveElement.h"
#include "TEveTrackPropagator.h"
#include "TEveVSDStructs.h" //because I haven't yet found a way to set a track's vertex without this

#ifndef EVED_TRACKMAKER_SERVICE_CC
#define EVED_TRACKMAKER_SERVICE_CC

namespace eved
{

    void eved::TrackMaker::reconfigure(const fhicl::ParameterSet& p)
    {
    }

    TEveElement* eved::TrackMaker::MakeVis(const recob::Track& track) //implementation of pure virtual function
    {
      //TODO: Seg fault on event 7 of reconstructed pion files with only track drawing running.  No problems without recob::Track drawing, so something 
      //is wrong here. 
      double smallMom = 1e-3; //FIXME: Very small non-zero momentum value so the propagator doesn't freak out.  With 0 momentum, the propagator won't draw.  
      TEveRecTrackT<double> vtx;
      TEveVectorT<double> vtxPos, small3Mom;
      vtxPos.Set(track.LocationAtPoint(0));
      small3Mom.Set(smallMom, smallMom, smallMom);
      vtx.fV = vtxPos;
      vtx.fP = small3Mom;     
 
      auto prop = new TEveTrackPropagator(); //configure track propagator
      prop->SetMagField(0.); //Get this from LArSoft mag field service?  I don't know whether there is a default version.  If not, this line would 
                             //cause trouble for experiments without mag field service implementations.

      auto retVal = new TEveTrack(&vtx, prop); //This is the TEveTrack we will return

      size_t nTrajPt = track.NumberTrajectoryPoints();
      for(size_t posIt = 1; posIt < nTrajPt-1; ++posIt) //loop over track trajectory points; 0th point is the vertex included above
      {
        TEveVectorT<double> pos, mom;
        pos.Set(track.LocationAtPoint(posIt));
        mom.Set(track.DirectionAtPoint(posIt));
        retVal->AddPathMark(TEvePathMarkD(TEvePathMarkD::kDaughter, pos)); //we could also put a time value here if available
                                                                           //It would be nice if we could use something like kLineSegment here, but I haven't 
                                                                           //found a way to stop the track propagator from sending the track out to its 
                                                                           //maximum r/z value yet.  
      }

      //FIXME: Make sure TEveTrackPropagator does not propagate to end of bounds.  Using kDecay may not be physically correct in many cases, but it does what we
      //want for now.  
      TEveVectorT<double> pos, mom;
      pos.Set(track.LocationAtPoint(nTrajPt-1));
      mom.Set(track.DirectionAtPoint(nTrajPt-1));
      retVal->AddPathMark(TEvePathMarkD(TEvePathMarkD::kDecay, pos)); //we could also put a time value here if available
      
      retVal->MakeTrack();
      retVal->SetMainColor(track.ID()+1); //Impose some maximum number of colors here? Old event display used evdb::kNCOLS.  ROOT's TColor provides a function
                                         //for this.  
      retVal->SetMarkerStyle(1); //fcl parameters?
      retVal->SetMarkerColor(track.ID()+1);
      
      retVal->SetRnrPoints(kTRUE); //From CMS example with multiple points

      return (TEveElement*)retVal;
    } 
    
}

DEFINE_ART_SERVICE_INTERFACE_IMPL(eved::TrackMaker, eved::VisMakerInt<recob::Track>)

#endif
