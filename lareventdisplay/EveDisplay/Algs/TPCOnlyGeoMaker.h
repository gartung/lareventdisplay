// \file: TPCOnlyGeoMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief:  An algorithm to visualize a simplified version of the geometry

#ifndef __CINT__

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"

//LArSoft includes
#include "larcore/Geometry/Geometry.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

//c++ includes
#include <vector>
#include <string>

//ROOT includes
#include "TEveScene.h"

namespace eved {

  class TPCOnlyGeoMaker
  {
    public:
      TPCOnlyGeoMaker();
      ~TPCOnlyGeoMaker() = default;
    
      void initialize();
      void reconfigure(const fhicl::ParameterSet& p);
      void makeEvent(const art::Event& e);
      TEveScene* makeGlobal();

    private:
      double fGeoTrans; //transparency to apply to all geometry objects
      short fGeoColor; //color to apply to all geometry objects
      geo::TPCID fTPCID; //The only TPC we will draw
  };
  
}

#endif //__CINT__
