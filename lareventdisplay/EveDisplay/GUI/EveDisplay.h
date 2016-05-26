//
// \file: EveDisplay.h
// \brief: Service to provide access to the EVE GUI from LArSoft and handle event navigation
// \author: aoliv23@lsu.edu
//

#ifndef EVED_EVEDISPLAY
#define EVED_EVEDISPLAY

//Art includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"

//ROOT includes
#include "TEveManager.h"

#ifndef __CINT__
#include "art/Framework/Services/Registry/ServiceMacros.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveNavGui.h"
#include "lareventdisplay/EveDisplay/GUI/GUINavigatorBase.h"
//#include "EveDisplay/EvePSetGui.h"

namespace fhicl { class ParameterSet; }
namespace art   { class ActivityRegistry; }

namespace eved
{

  class EveDisplay: public GUINavigatorBase 
  {
    public:
      EveDisplay(const fhicl::ParameterSet &pset, art::ActivityRegistry &reg);
      ~EveDisplay() {};

      //Event navigation methods

      void afterNewEvent(const art::Event& e) override; 

      TEveManager* getEve() { return fEve; } //How do I prevent users from keeping this pointer?  Check Geometry service provider

    private:
      TEveManager* fEve; //service provider; use this instead of gEve

      //GUI elements for event navigation and pset editing
      //These go in Eve's left browser area as additional tabs
      EveNavGui* fNav;
      //EvePSetGui* fPSet;
  };
 
} //close namespace eved 

DECLARE_ART_SERVICE(eved::EveDisplay, LEGACY)
#endif //CINT
#endif //EVED_EVEDISPLAY
