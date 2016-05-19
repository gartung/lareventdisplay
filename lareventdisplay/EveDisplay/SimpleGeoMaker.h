// \file: SimpleGeoMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief:  An algorithm to visualize a simplified version of the geometry

#ifndef __CINT__

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"

//LArSoft includes
#include "larcore/Geometry/Geometry.h"

//c++ includes
#include <vector>
#include <string>

//ROOT includes
#include "TEveScene.h"

namespace eved {

  class SimpleGeoMaker
  {
    public:
      SimpleGeoMaker();
      ~SimpleGeoMaker() = default;
    
      void initialize();
      void reconfigure(const fhicl::ParameterSet& p);
      TEveScene* makeScene(const art::Event& e);

    //private:
  };
  
}

#endif //__CINT__
