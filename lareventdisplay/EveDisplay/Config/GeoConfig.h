// \file: GeoConfig.h
// \brief: ART service for the EVE-based event display that keeps track of the current TPC to be drawn for ortho and TWQ views.  
// \author: Andrew Olivier aolivier@ur.rochester.edu

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

//LArSoft includes
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

#ifndef CONFIG_GEOCONFIG_H
#define CONFIG_GEOCONFIG_H

//I'd like to add a GUI interface to this later. 

namespace eved
{
  class GeoConfig
  {
    public:
      GeoConfig(fhicl::ParameterSet const& pset, art::ActivityRegistry &);
      ~GeoConfig() = default; //no bare pointers

      void reconfigure(fhicl::ParameterSet const& p);
      inline const geo::TPCID GetTpc() { return fTPC; }

    private:
      geo::TPCID fTPC; //The current TPC to be drawn in TWQ and ortho viewer modes
  };
}

DECLARE_ART_SERVICE(eved::GeoConfig, LEGACY)

#endif 
