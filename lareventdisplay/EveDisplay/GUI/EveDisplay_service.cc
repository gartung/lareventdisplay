// \brief: Implementation of EveDisplay.h
// \author: aoliv23@lsu.edu

//ROOT includes
#include "TEveManager.h"
#include "TROOT.h"
#include "TApplication.h"

//nutools includes
#include "EventDisplayBase/NavState.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
//#include "lareventdisplay/EveDisplay/GUI/EvePSetGui.h"
#include "EventDisplayBase/RootEnv.h"
#include "lareventdisplay/EveDisplay/GUI/EveNavGui.h"
#include "lareventdisplay/EveDisplay/GUI/GUINavigatorBase.h"

//ART includes
#include "art/Framework/IO/Root/RootInput.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"

namespace eved 
{

  EveDisplay::EveDisplay(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg): GUINavigatorBase(reg)
  {
    if(gEve != nullptr)
    {
      fEve = gEve;
    }
    else
    {
      fEve = TEveManager::Create(kTRUE, "FI"); //we will create viewers ourselves
    }

    fNav = new EveNavGui(fEve->GetBrowser());
    //fPSet = new EvePSetGui(fEve->GetBrowser(), info_);
  }

  void EveDisplay::afterNewEvent()
  {
    //GUINavigatorBase handles interrupting the state machine
    fEve->Redraw3D(kTRUE);
    //fEve->FullRedraw3D(kTRUE); //draw for the current event
  }
 
} //end namespace eved

DEFINE_ART_SERVICE(eved::EveDisplay)
