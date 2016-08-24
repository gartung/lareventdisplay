// \file: CoordConfig.h
// \brief: Service to keep track of screen-geometry related coordinates
// \author: Andrew Olivier aolivier@ur.rocheseter.edu

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

#ifndef CONFIG_COORDCONFIG_H
#define CONFIG_COORDCONFIG_H

namespace eved 
{
  class CoordConfig
  {
    public:
      CoordConfig(fhicl::ParameterSet const& p, art::ActivityRegistry &) { reconfigure(p); }
      ~CoordConfig() = default;
  
      void reconfigure(fhicl::ParameterSet const& p);
  
      inline unsigned int GetFrameWidth() const { return fFrameWidth; }
      inline unsigned int GetFrameHeight() const { return fFrameHeight; }
  
    private:
      unsigned int fFrameWidth; //width of frames that fit in new EVE tabs
      unsigned int fFrameHeight; //height of frames that fit in new EVE tabs
  };
}

DECLARE_ART_SERVICE(eved::CoordConfig, LEGACY)

#endif
