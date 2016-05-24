// \file: GenericSceneMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief:  An algorithm to make a basic TEveScene from a list of data products

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"

//c++ includes
#include <vector>
#include <string>
#include <typeinfo>

//ROOT includes
#include "TEveScene.h"
#include "TEveViewer.h"
#include "TEveElement.h"

namespace eved {

  //Template arguments: 
  //*PRODS: LArSoft data product types to be drawn in this scene
  //*NAME: name of this TEveScene in the event display
  
  template <std::string& NAME, class ...PRODS>
  class GenericSceneMaker
  {
    public:
      GenericSceneMaker();
      ~GenericSceneMaker() = default;
    
      void initialize();
      void reconfigure(const fhicl::ParameterSet& p);
      void makeEvent(const art::Event& e);
      TEveScene* makeGlobal();

    private:
      std::map<std::string, std::string> fLabels; //map from typeid::name of data product to label
      TEveScene* fScene; //The TEveScene we will create and update

      template <class TYPE>
      void _do_reconfigure(const fhicl::ParameterSet& p) 
      {
        //art::ServiceHandle<eved::VisMakerInt<TYPE>>()->reconfigure(p);
        return;
      }
      template <class TYPE>
      TEveElementList* _do_makeEvent(const art::Event& e)
      {
        mf::LogWarning("GenericSceneMaker") << "Calling _do_makeEvent in some version of GenericSceneMaker.\n";
        art::ServiceHandle<eved::VisMakerInt<TYPE>> alg;
        TEveElementList* list = new TEveElementList(alg->GetLabel().c_str(), alg->GetLabel().c_str());
        try
        {
          art::Handle<std::vector<TYPE>> prodHand;
          e.getByLabel(alg->GetLabel(), prodHand);
          mf::LogWarning("GenericSceneMaker") << "Got " << prodHand->size() << " data products for drawing in GenericSceneMaker using label " 
                                              << alg->GetLabel() << ".\n";
          for(auto& prod: (*prodHand))
          {
            auto el = alg->MakeVis(prod);
            el->VizDB_Insert(el->GetElementName(), kTRUE, kTRUE);
            list->AddElement(el);
            mf::LogWarning("GenericSceneMaker") << "Added element " << alg->MakeVis(prod)->GetElementName() << " to TEveElementList " << list->GetName() << ".\n";
          }
        }
        catch(cet::exception& exc)
        {
          //Following from the original LArSoft event display, catch exceptions when getting products since we will probably still want to draw all the other 
          //products.  I think there might be a better way to handle this an an ART setting, but someone will have to look into that later.  
          mf::LogWarning("GenericSceneMaker") << "Caught exception in _do_makeEvent while make scene " << NAME << ": \n" << exc << "\n";
        }
        return list;
      }

  };
  
  template <std::string& NAME, class ...PRODS>
  eved::GenericSceneMaker<NAME, PRODS...>::GenericSceneMaker()
  {
  }

  template <std::string &NAME, class ...PRODS>
  void eved::GenericSceneMaker<NAME, PRODS...>::reconfigure(const fhicl::ParameterSet& p)
  {
    auto pset = p.get<fhicl::ParameterSet>(NAME); //the ParameterSet for this algorithm
    int null[] = {(_do_reconfigure<PRODS>(pset), 0)...};
    (void)null; //supposedly so that null appears to be used to the compiler without actually generating any code
  }

  //It turns out that we cannot specialize function templates for a number of parameters.  We could do this with some sort of functor class, or we 
  //might be able to use the braced-initialization-list-trick to do something for each template argument.  

  template <std::string& NAME, class ...PRODS>
  void eved::GenericSceneMaker<NAME, PRODS...>::initialize()
  {
  }

  template <std::string &NAME, class ...PRODS> 
  void eved::GenericSceneMaker<NAME, PRODS...>::makeEvent(const art::Event& e)
  {
    mf::LogWarning("GenericSceneMaker") << "In GenericSceneMaker::makeEvent, about to fill scene named " << fScene->GetName() << ".\n";
    fScene->DestroyElements(); //get rid of last event's elements    

    int null[] = {0, (fScene->AddElement(_do_makeEvent<PRODS>(e)), 0)..., 0};
    (void)null; //supposedly so that null appears to be used to the compiler without actually generating any code
  }

  template <std::string &NAME, class ...PRODS>
  TEveScene* eved::GenericSceneMaker<NAME, PRODS...>::makeGlobal()
  {
    fScene = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene(NAME.c_str());
    return fScene;
  }

}
