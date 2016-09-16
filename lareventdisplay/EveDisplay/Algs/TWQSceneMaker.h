// \file: TWQSceneMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief:  An algorithm to make a TEveScene for a specific wire plane of a TPC

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
#include "larcore/Geometry/Geometry.h"
#include "lareventdisplay/EveDisplay/Config/GeoConfig.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/RecoBase/Cluster.h" //For GetPlane() specialization
#include "lareventdisplay/EveDisplay/Algs/DestroyRecursive.h"

//c++ includes
#include <vector>
#include <string>
#include <typeinfo>

//ROOT includes
#include "TEveScene.h"
#include "TEveViewer.h"
#include "TEveElement.h"

namespace 
{
  //I will write a function template that tries to call Channel() by default.  For data products that don't follow this rule like recob::Cluster, we have to 
  //write a specialization to avoid a compile error.  Include needed specializations below.  
  template <class PROD>
  geo::PlaneID GetPlane(const PROD& prod)
  {
    art::ServiceHandle<geo::Geometry> geom;
    
    auto channel = prod.Channel();
    auto wires = geom->ChannelToWire(channel);
    if(wires.size() != 1) throw cet::exception("Disambig") << "Got " << wires.size() << " wires mapped to channel " << channel << ".  You will need to handle "
                                                           << "disambiguation yourself in TWQSceneMaker.\n";

    return geo::PlaneID(wires[0].Cryostat, wires[0].TPC, wires[0].Plane);
  }

  //specialize for classes that require some conversion for this
  //specialization for recob::Cluster
  template <>
  geo::PlaneID GetPlane(const recob::Cluster& prod)
  {
    return prod.Plane();
  }
}

namespace eved {

  //Template arguments: 
  //*PRODS: LArSoft data product types to be drawn in this scene
  //*NAME: name of this TEveScene in the event display
  
  template <std::string& NAME, class ...PRODS>
  class TWQSceneMaker
  {
    public:
      TWQSceneMaker();
      ~TWQSceneMaker() = default;
    
      void initialize();
      void reconfigure(const fhicl::ParameterSet& p);
      void makeEvent(const art::Event& e);
      TEveScene* makeGlobal();

    private:
      std::map<geo::PlaneID, TEveScene*> fScenes; //The TEveScenes we will create and update.  One for each View in this TPC

      template <class TYPE>
      void do_reconfigure(const fhicl::ParameterSet& p) 
      {
        art::ServiceHandle<eved::VisMakerInt<TYPE>> alg;
        return;
      }
      template <class TYPE>
      std::map<geo::PlaneID, TEveElementList*> do_makeEvent(const art::Event& e)
      {
        //mf::LogWarning("TWQSceneMaker") << "Calling do_makeEvent in some version of TWQSceneMaker.\n";
        art::ServiceHandle<eved::VisMakerInt<TYPE>> alg;

        //A list of visualization elements for this module label
        //TEveElementList* list = new TEveElementList(alg->GetLabel().c_str(), alg->GetLabel().c_str());

        //For each view in the TPC, make a list of visulization elements in that view for this module label
        std::map<geo::PlaneID, TEveElementList*> lists;
        art::ServiceHandle<geo::Geometry> geom;
        art::ServiceHandle<eved::GeoConfig> config;
        auto tpcId = config->GetTpc();
        const auto& tpc = geom->TPC(tpcId);

        for(size_t plane = 0; plane < tpc.Nplanes(); ++plane)
        {
          geo::PlaneID id(tpcId, plane);
          lists.insert(std::make_pair(id, new TEveElementList(alg->GetLabel().c_str(), alg->GetLabel().c_str())));
          //list.AddElement(lists.insert(std::make_pair(view, new TEveElementList(name.c_str(), name.c_str())).second->second));
        }

        try
        {
          art::Handle<std::vector<TYPE>> prodHand;
          e.getByLabel(alg->GetLabel(), prodHand);
          //mf::LogWarning("TWQSceneMaker") << "Got " << prodHand->size() << " data products for drawing in TWQSceneMaker using label " 
          //                                    << alg->GetLabel() << ".\n";
          for(auto& prod: (*prodHand))
          {
            if(alg->SelectDataProduct(prod))
            {
              auto el = alg->MakeVis(prod);
              if(el != nullptr)
              {
                auto plane = GetPlane(prod);
                //mf::LogWarning("TWQSceneMaker") << "Got data product from plane " << plane << ".\n";
                if(plane.TPC == config->GetTpc().TPC)
                {
                  auto vList = lists.find(plane);
                  if(vList == lists.end()) throw cet::exception("BadView") << "Got a plane number " << GetPlane(prod).Plane << " that was supposedly not "
                                                                         << "registered with the geometry service.  Check out "
                                                                         << "lareventdisplay/EveDisplay/Algs/TWQSceneMaker.h "
                                                                         << "TWQSceneMaker::do_makeEvent for details.\n";     

                  el->VizDB_Insert(el->GetElementName(), kFALSE, kTRUE);
                  vList->second->AddElement(el);
                  //mf::LogWarning("TWQSceneMaker") << "Added element " << alg->MakeVis(prod)->GetElementName() << " to TEveElementList " 
                  //                                << vList->second->GetName() << ".\n";
                }
              }
            }
          }
        }
        catch(cet::exception& exc)
        {
          //Following from the original LArSoft event display, catch exceptions when getting products since we will probably still want to draw all the other 
          //products.  I think there might be a better way to handle this an an ART setting, but someone will have to look into that later.  
          mf::LogWarning("TWQSceneMaker") << "Caught exception in do_makeEvent while making scene " << NAME << ": \n" << exc << "\n";
        }
        return lists;
      }

  };
  
  template <std::string& NAME, class ...PRODS>
  eved::TWQSceneMaker<NAME, PRODS...>::TWQSceneMaker()
  {
  }

  template <std::string &NAME, class ...PRODS>
  void eved::TWQSceneMaker<NAME, PRODS...>::reconfigure(const fhicl::ParameterSet& p)
  {
    auto pset = p.get<fhicl::ParameterSet>(NAME); //the ParameterSet for this algorithm
    int null[] = {(do_reconfigure<PRODS>(pset), 0)...};
    (void)null; //supposedly so that null appears to be used to the compiler without actually generating any code
  }

  template <std::string& NAME, class ...PRODS>
  void eved::TWQSceneMaker<NAME, PRODS...>::initialize()
  {
  }

  template <std::string &NAME, class ...PRODS> 
  void eved::TWQSceneMaker<NAME, PRODS...>::makeEvent(const art::Event& e)
  {
    //mf::LogWarning("TWQSceneMaker") << "In TWQSceneMaker::makeEvent, about to fill scene named " << fScene->GetName() << ".\n";
    for(auto sceneP: fScenes)
    {
      DestroyRecursive(sceneP.second);
    }

    std::vector<std::map<geo::PlaneID, TEveElementList*>> listMapVect;
    //int null[] = {0, (fScene->AddElement(do_makeEvent<PRODS>(e)), 0)..., 0};
    int null[] = {0, (listMapVect.push_back(do_makeEvent<PRODS>(e)), 0)..., 0};
    (void)null; //supposedly so that null appears to be used to the compiler without actually generating any code

    //TODO: there has to be a better way to do this than triply nested containers!
    for(auto pair: fScenes)
    {
      auto scene = pair.second;
      auto view = pair.first;
      for(auto map: listMapVect)
      {
        auto found = map.find(view);
        if(found != map.end()) scene->AddElement(found->second); //else throw an exception?
      }
    }
  }

  template <std::string &NAME, class ...PRODS>
  TEveScene* eved::TWQSceneMaker<NAME, PRODS...>::makeGlobal()
  {
    //make a TEveScene for each view in this geometry
    art::ServiceHandle<geo::Geometry> geom;
    art::ServiceHandle<eved::GeoConfig> config;
    auto tpcId = config->GetTpc();
    const auto& tpc = geom->TPC(tpcId);
    auto eve = art::ServiceHandle<eved::EveDisplay>()->getEve(); 

    for(size_t plane = 0; plane < tpc.Nplanes(); ++plane)
    {
      geo::PlaneID id(tpcId, plane);
      fScenes.insert(std::make_pair(id, eve->SpawnNewScene((NAME+std::to_string(plane)).c_str())));
      //mf::LogWarning("TWQSceneMaker") << "Initializing TEveScene named " << NAME+std::to_string(plane) << " in TWQSceneMaker.\n";
    }

    //fScene = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene(NAME.c_str());
    //return fScene;
    //While we are working on rebuilding the EVE-based event display framework, I will return an empty TEveScene here.  In the future, I will probably 
    //make the return type of this function void since SceneConstructor doesn't use what we return here.  If we still want to use the "old" viewer makers, 
    //then we need to make some major modifications to all SceneMakers to return some kind of container here.  
    auto dummy = new TEveScene("dummy");
    dummy->IncDenyDestroy(); //so we don't segfault while trying to test this
    return dummy;
  }

}
