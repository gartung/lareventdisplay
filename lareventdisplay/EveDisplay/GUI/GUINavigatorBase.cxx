// \file: GUINavigatorBase.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Implementation of functions for services that use a GUI to navigate events.  

// system include files
#include <iostream>
#include <cerrno>
#include <cstdlib>

// LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/GUINavigatorBase.h"
#include "lareventdisplay/EveDisplay/GUI/SRNavState.h"
#include "nutools/EventDisplayBase/RootEnv.h"

//ART includes
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "canvas/Persistency/Provenance/Timestamp.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "art/Framework/Core/InputSource.h"
#include "fhiclcpp/ParameterSetRegistry.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/IO/Root/RootInput.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"

//ROOT includes
#include "TROOT.h"
#include "TApplication.h"

namespace eved {

  static evdb::RootEnv gsRootEnv(0,0); //Taken from EventDisplayBase/EventDisplay_service.cc.  I want to find a way around this if I can.

  GUINavigatorBase::GUINavigatorBase(art::ActivityRegistry & iReg)
  {
    iReg.sPostBeginJobWorkers.watch(this, &GUINavigatorBase::postBeginJobWorkers);
    iReg.sPreProcessEvent.watch(this, &GUINavigatorBase::preEvent);
    iReg.sPostProcessEvent.watch(this, &GUINavigatorBase::postEvent);
  }

  void GUINavigatorBase::postBeginJobWorkers(art::InputSource * is,
                                            std::vector<art::Worker *> const & workers)
  {
    input_ = is;
    fWorkers = workers;
    afterBeginJob();
  }

  void GUINavigatorBase::preEvent(art::Event const &)
  {
  }

  void GUINavigatorBase::postEvent(art::Event const &e)
  {
    //Much of this function taken from EventDisplayBase/EventDisplay_service.cc
    TApplication* app = gROOT->GetApplication();    

    afterNewEvent(e); //For an event display, you will probably want to call some kind of draw function here

    // Hold here for user input from the GUI...
    app->Run(kTRUE);

    afterGUIEvent();

    art::RootInput* rootInput = dynamic_cast<art::RootInput*>(input_);

    if(!rootInput && SRNavState::Which() != sr_nav_states::kSEQUENTIAL_ONLY){ //copied almost entirely from EventDisplayBase/EventDisplay_service.cc
      SRNavState::Set(sr_nav_states::kSEQUENTIAL_ONLY);
      mf::LogWarning("GUINavigatorBase")
      << "Random access for the EventDisplay requires a rootInput source for proper operation.\n"
      << "You do not have a rootInput source so only sequential access works.\n";
    }

    sr_nav_states navState = SRNavState::Which(); 
    if(navState == sr_nav_states::kNEXT_EVENT) 
    {
      if(rootInput!= nullptr) rootInput->seekToEvent(0); //see EventDisplayBase/EventDisplay_service.cc for an explanation of this
    }
    else if(navState == sr_nav_states::kPREV_EVENT) 
    {
      if(rootInput!= nullptr) rootInput->seekToEvent(-2); //ART's version of this interface uses a RewindFile method.  
    }
    else if(navState == sr_nav_states::kRELOAD_EVENT)
    {
      if(rootInput!= nullptr) rootInput->seekToEvent(-1);
    } 
    else if(navState == sr_nav_states::kGOTO_EVENT)
    {
      if(rootInput!= nullptr)
      {
        art::EventID id(SRNavState::TargetRun(), SRNavState::TargetSubRun(), SRNavState::TargetEvent());
        if (!rootInput->seekToEvent(id)) 
        { // Couldn't find event
          mf::LogWarning("GUINavigatorBase") << "Unable to find "
                                             << id
                                             << " -- reloading current event.";
          // Reload current event.
          rootInput->seekToEvent(e.id());
        }
      }
    }
    else if(navState == sr_nav_states::kSEQUENTIAL_ONLY)
    { //don't do anything! 
    }
    else 
      throw art::Exception(art::errors::LogicError)
      << "EvengtDisplay in unhandled state "
      << ".\n"; 
  }

}

