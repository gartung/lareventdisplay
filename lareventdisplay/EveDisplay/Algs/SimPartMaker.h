// \file: SimPartMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing simb::MCParticle in the EVE-based event display.  Implements SimPartMakerInt.h. 

//ART includes
#include "fhiclcpp/ParameterSet.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/SimPartMakerInt.h"
#include "SimulationBase/MCParticle.h"

//ROOT includes
#include "TEveTrack.h"
#include "TEveElement.h"

#ifndef EVED_SIMPARTMAKER_H
#define EVED_SIMPARTMAKER_H

namespace eved
{
  class SimPartMaker: public VisMakerInt<simb::MCParticle>
  {
    public:
      SimPartMaker(const fhicl::ParameterSet& p, art::ActivityRegistry &reg): VisMakerInt<simb::MCParticle>(p, reg), fMinE(-1) { reconfigure(p); }
      ~SimPartMaker() = default;
  
      TEveElement* MakeVis(const simb::MCParticle& track); //implementation of pure virtual function

      void reconfigure(fhicl::ParameterSet const & p) override;

      bool SelectDataProduct(const simb::MCParticle& track) override;
    private:
      double fMinE; //Minimum particle energy to be drawn in GeV
  };
}

DECLARE_ART_SERVICE_INTERFACE_IMPL(eved::SimPartMaker, eved::VisMakerInt<simb::MCParticle>, LEGACY)

#endif
