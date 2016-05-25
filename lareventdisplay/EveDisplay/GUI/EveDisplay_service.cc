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
//#include "EveDisplay/EvePSetGui.h"
#include "EventDisplayBase/RootEnv.h"
#include "lareventdisplay/EveDisplay/GUI/EveNavGui.h"

//ART includes
#include "art/Framework/IO/Root/RootInput.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/UserInteraction/UserInteraction.h"

namespace eved 
{

  static evdb::RootEnv gsRootEnv(0,0); //Taken from EventDisplayBase/EventDisplay_service.cc.  I want to find a way around this if I can.

  EveDisplay::EveDisplay(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg): ui::UserInteraction(reg)
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

  ui::UserInteraction::NextStep EveDisplay::nextAction()
  {

    //TApplication stuff to interrupt state machine taken from EventDisplayBase/EventDisplay_service.cc
    TApplication* app = gROOT->GetApplication();
    
    fEve->Redraw3D(kTRUE);
    //fEve->FullRedraw3D(kTRUE); //draw for the current event

    // Hold here for user input from the GUI...
    app->Run(kTRUE);

    int navState = evdb::NavState::Which();
    if(navState == evdb::kNEXT_EVENT) return  NextEvent;
    else if(navState == evdb::kPREV_EVENT) return RewindFile;
    else if(navState == evdb::kRELOAD_EVENT) return ReprocessEvent;
    else return Invalid;

    //if(navState == kGOTO_EVENT || navState == kSEQUENTIAL_ONLY) return Invalid;
  }
 
  /*void EveDisplay::postEvent(art::Event const & evt)
  {
    //TApplication stuff to interrupt state machine taken from EventDisplayBase/EventDisplay_service.cc
    TApplication* app = gROOT->GetApplication();

    // Hold here for user input from the GUI...
    app->Run(kTRUE);

    //Where do we go next?
    UserInteraction::postEvent(evt);
 
    //The following code was taken almost completely from evdb::EventDisplay_service.cc
    if(NextAction == Invalid)
    {
      if(evdb::NavState::Which() == kGOTO_EVENT)
      {
        art::EventID id(art::SubRunID::invalidSubRun(art::RunID(NavState::TargetRun())), NavState::TargetEvent());
        if(input_)
        {
          if (!input_->seekToEvent(id)) 
          { // Couldn't find event
            mf::LogWarning("EventDisplayBase") << "Unable to find "
                                               << id
                                               << " -- reloading current event.";
            // Reload current event.
            input_->seekToEvent(evt.id());
          }
        }
      }
    } //TODO: need to figure out what to do here
  }*/

  void EveDisplay::pickModule()
  {
    for(size_t it = 0; it < info_.size(); ++it)
    {
      callReconfigure(it, info_[it].pset);
    }
  }

  void EveDisplay::moduleList(std::vector<ui::UserInteraction::ModuleInfo> const & list)
  {
    info_ = list;
    //fPSet->Update(list);
  }
} //end namespace eved

DEFINE_ART_SERVICE(eved::EveDisplay)
