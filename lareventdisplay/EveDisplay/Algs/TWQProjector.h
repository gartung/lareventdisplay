// \file: TWQProjector.h
// \brief: Algorithm to project specified TEveScenes into a TWQ Projection
// \author: Andrew Olivier aolivier@ur.rochester.edu

//Meant to be used as a template argument to SceneProjConstructor
//PROJ: An algorithm to project the elements made by the ALGS for each event. PROJ owns the projected scene(s).  
//A PROJ must implement:
//*PROJ::PROJ(TEveScene* scene)
//*void PROJ::reconfigure(fhicl::ParameterSet const& p)
//*void PROJ::Project()

//LArSoft includes
#include "lareventdisplay/EveDisplay/Util/TEveUserProjectionManager.h"
#include "lareventdisplay/EveDisplay/Util/TEveTWQProjection.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lareventdisplay/EveDisplay/Algs/DestroyRecursive.h"
#include "lareventdisplay/EveDisplay/Config/GeoConfig.h"
#include "larcore/Geometry/Geometry.h"

//ROOT includes
#include "TEveScene.h"

#ifndef ALGS_TWQPROJECTOR_H
#define ALGS_TWQPROJECTOR_H

namespace eved
{
  class TWQProjector
  {
    public:
      TWQProjector(TEveScene* scene);
      ~TWQProjector() = default;

      void reconfigure(fhicl::ParameterSet const& p) {}
      void Project();
    
    private:
      std::unique_ptr<vutl::TEveUserProjectionManager> fMan; //Does the projecting for us.  
      std::map<geo::PlaneID, TEveScene*> fScenes; //Projected scenes.  One for each view.  
      TEveScene* fScene; //The scene we will project
  };

  TWQProjector::TWQProjector(TEveScene* scene): fMan(new vutl::TEveUserProjectionManager()), fScenes(), fScene(scene)
  {
    auto tpcId = art::ServiceHandle<eved::GeoConfig>()->GetTpc();
    const auto& tpc = art::ServiceHandle<geo::Geometry>()->TPC(tpcId);
    auto eve = art::ServiceHandle<eved::EveDisplay>()->getEve();

    for(size_t plane = 0; plane < tpc.Nplanes(); ++plane)
    {
      geo::PlaneID id(tpcId, plane);
      fScenes.insert(std::make_pair(id, eve->SpawnNewScene((fScene->GetName()+std::to_string(plane)).c_str())));
    }
  }

  void TWQProjector::Project()
  {
    auto tpcId = art::ServiceHandle<eved::GeoConfig>()->GetTpc();
    const auto& tpc = art::ServiceHandle<geo::Geometry>()->TPC(tpcId);
    auto eve = art::ServiceHandle<eved::EveDisplay>()->getEve();

    //Should we be willing to create a new scene here, or should we throw on a PlaneID that does not match?  
    for(size_t plane = 0; plane < tpc.Nplanes(); ++plane)
    {
      geo::PlaneID id(tpcId, plane);
      auto found = fScenes.find(id);
      if(found == fScenes.end()) 
      {
        found = fScenes.insert(std::make_pair(id, eve->SpawnNewScene((fScene->GetName()+std::to_string(plane)).c_str()))).first;
      }
      auto pScene = found->second;
      DestroyRecursive(pScene);

      fMan->SetProjection(new vutl::TEveTWQProjection(id));
      fMan->ImportElements(fScene, pScene);
      //auto axes = new TEveProjectionAxes(new vutl::TEveUserProjectionManager(new vutl::TEveTWQProjection(id)));
      //axes->VizDB_Insert("Axes", kTRUE, kTRUE);
      //pScene->AddElement(axes);
      //If I call VizDB_Insert to be able to draw these axes, I get an exception from TClass::New.  This appears to be a conflict in the design of Eve 
      //itself, so discarding TEveProjectionAxes for now. 
    }
  }
}

#endif 
