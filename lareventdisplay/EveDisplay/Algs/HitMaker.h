// \file: HitMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing recob::Hit in the EVE-based event display.  Implements HitMakerInt.h. 

//ART includes
#include "fhiclcpp/ParameterSet.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/HitMakerInt.h"
#include "lardata/RecoBase/Hit.h"

//ROOT includes
#include "TEveElement.h"
#include "TEveFrameBox.h"

#ifndef EVED_WIREMAKER_H
#define EVED_WIREMAKER_H

namespace eved
{
  class HitMaker: public VisMakerInt<recob::Hit>
  {
    public:
      HitMaker(const fhicl::ParameterSet& p, art::ActivityRegistry &reg): VisMakerInt<recob::Hit>(p, reg) { reconfigure(p); }
      ~HitMaker() = default;
  
      TEveElement* MakeVis(const recob::Hit& digit); //implementation of pure virtual function

      void reconfigure(fhicl::ParameterSet const & p) override;
    private:
      short fSigMin; //minimum signal value read from fcl
      short fSigMax; //maximum signal value read from fcl
      short fSigCut; //do not draw time-wire points with signals below this ADC value
      double fTransp; //Transparency of objects drawn
  };
}

DECLARE_ART_SERVICE_INTERFACE_IMPL(eved::HitMaker, eved::VisMakerInt<recob::Hit>, LEGACY)

#endif
