#ifndef LAREVENTDISPLAY_EVEDISPLAY_GUI_GUINAVIGATORBASE_H
#define LAREVENTDISPLAY_EVEDISPLAY_GUI_GUINAVIGATORBASE_H

//\file: GUINavigatorBase.h
//\author: Andrew Olivier aoliv23@lsu.edu
//\brief: Base class for art services that use a GUI to navigate between events
//Based very heavily on ART's UserInteraction.h and inspired by nutools/EventDisplayBase/EventDisplay.h

#ifndef __CINT__

//ART includes
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"

//c++ includes
#include <vector>
#include <string>

namespace art {
  class InputSource;
}

namespace eved {

  class GUINavigatorBase {
  public:

    explicit GUINavigatorBase(art::ActivityRegistry &);
    virtual ~GUINavigatorBase() = default;

    // must be supplied by user, called when we want to tell the derived
    // service to update its GUI after processing a new event.  
    virtual void afterNewEvent(art::Event const & ev) = 0;

  private:
    void preEvent(art::Event const & ev);
    void postEvent(art::Event const & ev);
    void postBeginJobWorkers(art::InputSource * is, std::vector<art::Worker *> const &);

    art::InputSource * input_;

  };
}

#endif //__CINT__

#endif /* LAREVENTDISPLAY_EVEDISPLAY_GUI_GUINAVIGATORBASE_H */
