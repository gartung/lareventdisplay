// \file: RawDigitMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing raw::RawDigit in the EVE-based event display.  Implements RawDigitMakerInt.h. 

//ART includes
#include "fhiclcpp/ParameterSet.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/RawDigitMakerInt.h"
#include "lardata/RawData/RawDigit.h"
#include "lareventdisplay/EveDisplay/VisElem/TEveWvFrmDigit.h"

//ROOT includes
#include "TEveElement.h"
#include "TEveFrameBox.h"

#ifndef EVED_RAWDIGITMAKER_H
#define EVED_RAWDIGITMAKER_H

namespace eved
{
  class RawDigitMaker: public VisMakerInt<raw::RawDigit>
  {
    public:
      RawDigitMaker(const fhicl::ParameterSet& p, art::ActivityRegistry &reg): VisMakerInt<raw::RawDigit>(p, reg) { reconfigure(p); }
      ~RawDigitMaker() = default;
  
      TEveElement* MakeVis(const raw::RawDigit& digit); //implementation of pure virtual function

      void reconfigure(fhicl::ParameterSet const & p) override;
    private:
      short fSigMin; //minimum signal value read from fcl
      short fSigMax; //maximum signal value read from fcl
      short fSigCut; //do not draw time-wire points with signals below this ADC value
      double fTransp; //Transparency of objects drawn
  };
}

DECLARE_ART_SERVICE_INTERFACE_IMPL(eved::RawDigitMaker, eved::VisMakerInt<raw::RawDigit>, LEGACY)

#endif
