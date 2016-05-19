// \file: TrackMaker.cxx
// \brief: implementation of an algorithm to make visualization objects from LArSoft's recob::Track objects
// \author: Andrew Olivier, andrew@djoinc.com
// Date: 4/24/2016

//LArSoft includes
#include "EveDisplay/VisElemMaker.h"
#include "RecoBase/Track.h"
#include "Utilities/AssociationUtil.h"

//Art includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Persistency/Common/Ptr.h"

//ROOT includes
#include "TEveTrack.h"
#include "TEveTrack.h"
#include "TEvePathMark.h"
#include "TVector3.h"
#include "TEveVector.h"

//c++ includes
#include <memory>

#ifndef EVED_TRACKMAKER_MODULE_CC
#define EVED_TRACKMAKER_MODULE_CC

namespace eved {

  class TrackMaker
  {
    public:
      TrackMaker(fhicl::ParameterSet const& p) {} //nothing to do for the constructor yet
      ~TrackMaker() {}

      TEveTrack* MakeVis(const art::Ptr<recob::Track> &track); //make a TEveTrack for each recob::Track

      void Initialize() {} //do nothing at the beginning of each job

    //private:
  };
  

  TEveTrack* TrackMaker::MakeVis(const art::Ptr<recob::Track> &track) //make a TEveTrack for each recob::Track
  {
    TEveTrack* retVal = new TEveTrack(); //This is the TEveTrack we will return
    for(size_t posIt = 0; posIt < track->NumberTrajectoryPoints(); ++posIt) //loop over track trajectory points
    {
      TEveVectorT<double> evePos;
      evePos.Set(track->LocationAtPoint(posIt));
      retVal->AddPathMark(TEvePathMarkD(TEvePathMarkD::kLineSegment, evePos));
    }
    
    return retVal;
  }
    
  //Make the module that will produce these TEveTracks
  typedef VisElemMaker<eved::TrackMaker, recob::Track, TEveTrack> VisTrackMaker;
} //end of namespace eved

DEFINE_ART_MODULE(eved::VisTrackMaker)

#endif //EVED_TRACKMAKER_MODULE_CC
