// \file: CoordConfig_service.cc
// \brief: Implementation of access to frame size-related coordinate information
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Config/CoordConfig.h"

namespace eved
{
  void CoordConfig::reconfigure(fhicl::ParameterSet const& p)
  {
    fFrameWidth = p.get<unsigned int>("Width");
    fFrameHeight = p.get<unsigned int>("Height");
  }
}

DEFINE_ART_SERVICE(eved::CoordConfig)

