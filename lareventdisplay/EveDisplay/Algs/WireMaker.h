// \file: WireMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing recob::Wire in the EVE-based event display.  Implements WireMakerInt.h. 

//ART includes
#include "fhiclcpp/ParameterSet.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/WireMakerInt.h"
#include "lardataobj/RecoBase/Wire.h"

//ROOT includes
#include "TEveElement.h"
#include "TEveFrameBox.h"

#ifndef EVED_WIREMAKER_H
#define EVED_WIREMAKER_H

namespace eved
{
  class WireMaker: public VisMakerInt<recob::Wire>
  {
    public:
      WireMaker(const fhicl::ParameterSet& p, art::ActivityRegistry &reg): VisMakerInt<recob::Wire>(p, reg) { reconfigure(p); }
      ~WireMaker() = default;
  
      TEveElement* MakeVis(const recob::Wire& digit); //implementation of pure virtual function

      void reconfigure(fhicl::ParameterSet const & p) override;
    private:
      short fSigMin; //minimum signal value read from fcl
      short fSigMax; //maximum signal value read from fcl
      short fSigCut; //do not draw time-wire points with signals below this ADC value
      double fTransp; //Transparency of objects drawn
  };
}

DECLARE_ART_SERVICE_INTERFACE_IMPL(eved::WireMaker, eved::VisMakerInt<recob::Wire>, LEGACY)

#endif
