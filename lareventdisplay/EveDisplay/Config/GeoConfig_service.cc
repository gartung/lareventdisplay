// \file: GeoConfig_service.cc
// \brief: Implementation of a service to provide the current TPC being drawn in ortho and TWQ views of the EVE-based event display
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArsoft includes
#include "lareventdisplay/EveDisplay/Config/GeoConfig.h"
#include "larcore/Geometry/Geometry.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

//Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "cetlib/exception.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

eved::GeoConfig::GeoConfig(fhicl::ParameterSet const& pset, art::ActivityRegistry&)
{
  reconfigure(pset);
}

void eved::GeoConfig::reconfigure(fhicl::ParameterSet const& p)
{
  auto tpc = p.get<unsigned int>("TPC");
  auto cryo = p.get<unsigned int>("Cryo");

  fTPC = geo::TPCID(cryo, tpc);

  //Check that the TPCID we got from a fcl file is valid
  if(!art::ServiceHandle<geo::Geometry>()->HasTPC(fTPC)) throw cet::exception("BadTPCID") << "Got bad TPCID for TPC " << tpc << " and cryostat "
                                                                                            << cryo << " in TPCOrthoViewerMaker.\n";
}

DEFINE_ART_SERVICE(eved::GeoConfig)
