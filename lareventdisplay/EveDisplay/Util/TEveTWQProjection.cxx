// \file: TEveTWQProjection.cxx
// \brief: Implementation of a projection to time versus wire versus charge view for a specified plane
// \author: Andrew Olivier aolivier@ur.rochester.edu

//FIXME: Framework include for debugging
#include "messagefacility/MessageLogger/MessageLogger.h"

//ROOT includes
#include "TEveProjections.h" 
#include "TVector3.h"
#include "TGTab.h"

//LArSoft includes
#include "larcore/SimpleTypesAndConstants/geo_types.h" //for geo::PlaneID
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "larcore/Geometry/Geometry.h"

//ART includes
#include "cetlib/exception.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

//EVE Display includes
#include "lareventdisplay/EveDisplay/Util/TEveTWQProjection.h"
#include "lareventdisplay/EveDisplay/Config/CoordConfig.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"

ClassImp(vutl::TEveTWQProjection)

namespace vutl
{
  void TEveTWQProjection::ProjectPoint(Float_t& x, Float_t& y, Float_t& z, Float_t d, TEveProjection::EPProc_e proc = TEveProjection::kPP_Full)
  {
    if(fPlane.Plane == geo::PlaneID::InvalidID) throw cet::exception("TEveTWQProjection") << "Tried to project point into invalid plane " << fPlane.Plane 
                                                                                          << ".\n"; 

    if(fDisplaceOrigin)
    {
      x -= fCenter.fX;
      y -= fCenter.fY;
      z -= fCenter.fZ;
    }
    
    if(proc != TEveProjection::kPP_Plane)
    {
      art::ServiceHandle<geo::Geometry> geom;
      detinfo::DetectorProperties const* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

      auto cryo = fPlane.Cryostat;
      auto tpc = fPlane.TPC;
      auto plane = fPlane.Plane;

      double pos[3] = {x, y, z};

      double local[3] = {0}, world[3] = {0};
      geom->Plane(fPlane).LocalToWorld(local, world);
      pos[0] = world[0];

      //Code taken from LArSoft Event Display's RecoBaseDrawer
      double tickHit = detprop->ConvertXToTicks(x, plane, tpc, cryo);
      /*if(tickHit > detprop->NumberTimeSamples()) tickHit = detprop->NumberTimeSamples(); //Handled elsewhere by a TGLClipBox
      if(tickHit < 0) tickHit = 0;*/

      double wireHit = 0.;
      try{
          wireHit = 1.*geom->NearestWire(pos, plane, tpc, cryo);
      }
      catch(cet::exception &e){
          wireHit = 1.*atoi(e.explain_self().substr(e.explain_self().find("#")+1,5).c_str());
      }

      const auto& planeGeo = geom->Plane(fPlane);
      /*if(wireHit > planeGeo.Nwires()) wireHit = planeGeo.Nwires();
      if(wireHit < 0) wireHit = 0;*/

      //Convert wire number such that the size of a standard frame divided by the number of planes fits both all time ticks and all wires
      art::ServiceHandle<eved::CoordConfig> coordConfig;
      const auto& tpcGeo = geom->TPC(fPlane.Cryostat, fPlane.TPC);

      art::ServiceHandle<eved::EveDisplay> eved;
      auto twqWindow = eved->getEve()->GetBrowser()->GetTabRight()->GetContainer();
      auto width = twqWindow->GetWidth();
      auto height = twqWindow->GetHeight();
      mf::LogWarning("TEveTWQProjection") << "In TEveTWQProjection, width=" << width << " and height=" << height << ".\n";

      const auto conversion = detprop->NumberTimeSamples()*width/height*(tpcGeo.Nplanes()+0.5)/planeGeo.Nwires(); 
      mf::LogWarning("TEveTWQProjection") << "In TEveTWQProjection, conversion*nWires is " << conversion*planeGeo.Nwires() << ".\n";

      wireHit *= conversion; 
      
      //Return positions by reference
      z = wireHit;
      y = tickHit;
      //We don't need anymore since this is a 2D view 
    }
  }
}
