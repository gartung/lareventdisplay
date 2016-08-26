// \file: AssnSceneMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief:  An algorithm to make a TEveScene based on a list of related data products.  Products will be arranged in a list tree based on parentage. 

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Persistency/Common/Ptr.h"
#include "art/Framework/Core/FindManyP.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
//#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
#include "lareventdisplay/EveDisplay/Algs/DestroyRecursive.h"

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
  //*PRODS: LArSoft data product types IN ORDER OF PARENTAGE to be drawn in a list tree
  //*NAME: name of this TEveScene in the event display
  
  template <std::string& NAME, class ...PRODS>
  class AssnSceneMaker
  {
    public:
      AssnSceneMaker();
      ~AssnSceneMaker() = default;
    
      void initialize();
      void reconfigure(const fhicl::ParameterSet& p);
      void makeEvent(const art::Event& e);
      TEveScene* makeGlobal();

    private:
      TEveScene* fScene; //The TEveScene we will create and update

      template <class TYPE>
      void do_reconfigure(const fhicl::ParameterSet& p) 
      {
        art::ServiceHandle<eved::VisMakerInt<TYPE>> alg;
        return;
      }

      //Recursively build list tree based on FindManyP.  
      //TODO: It seems very suspicious to need to peel off three arguments at a time.  Is this really a good idea? 
      template <class U, class V, class W, class PRODS...>
      TEveElement* oneToMany(const art::Event& e, art::Ptr<U>& one, const std::vector<art::Ptr<V>>& many)
      {
        art::ServiceHandle<eved::VisMakerInt<U>> uAlg;
        art::ServiceHandle<eved::VisMakerInt<W>> wAlg;
        auto label = wAlg->GetLabel();

        auto el = uAlg->MakeVis(one.get());
        el->VizDB_Insert(el->GetElementName(), kFALSE, kTRUE);
        
        try
        {
          auto fmw = art::FindManyP<W>(many, e, label);
          for(size_t fmIt = 0; fmIt < fmw.size(); ++fmIt)
          {
            el->AddElement(oneToMany<V, W, PRODS>(e, many[fmIt], fmw[fmIt]));
          }
        }
        catch(cet::exception& e)
        {
          mf::LogWarning("AssnSceneMaker") << "Caught exception \n" << e << "\nin oneToMany for visualization object named " << el->GetName() << ".  We have "
                                           << "already made el, but its children will be skipped!\n";
        }
        return el;
      }
    
      //End of oneToMany recursion when we have only two types left.
      template <class U, class V, class PRODS...>
      TEveElement* oneToMany(const art::Event& e, const U& one, const std::vector<V>& many)
      {
        art::ServiceHandle<eved::VisMakerInt<U>> uAlg;
        art::ServiceHandle<eved::VisMakerInt<V>> vAlg;
       
        auto el = uAlg->MakeVis(one.get());
        el->VizDB_Insert(el->GetElementName(), kFALSE, kTRUE);
        for(auto& vPtr: many)
        {
          auto vVis = vAlg->MakeVis(vPtr.get());
          vVis->VizDB_Insert(vVis->GetElementName(), kFALSE, kTRUE);
          el->AddElement(vVis);
        } 
        return el;
      }

      //Entry point into recursion 
      template <class U, class V, class PRODS...>
      TEveElementList* do_makeEvent(const art::Event& e)
      {
        //First, get the Us we want to base all of this monstrosity on
        art::Handle<std::vector<U>> uHand;
        art::ServiceHandle<eved::VisMakerInt<U>> uAlg;
        auto uLabel = uAlg->GetLabel();
        e.getByLabel(uHand, uLabel);
       
        //Make a TEveElementList for the top-level U elements
        auto list = new TEveElementList(uLabel.c_str(), uLabel.c_str());       
 
        //Next, get associations of parent Us to vectors of child Vs
        try
        {
           auto fmp = art::FindManyP<V>(art::ServiceHandle<eved::VisMakerInt<V>>
          for(size_t fmIt = 0; fmIt < fmp.size(); ++fmIt)
          {
            const auto& u = (*uHand)[fmIt];
            if(uAlg->SelectDataProduct(u))
            {
              list->AddElement(oneToMany<U, V, PRODS>(e, u, fmp[fmIt]));
            }
          }
        }
        catch(cet::exception& e)
        {
          mf::LogWarning("AssnSceneMaker") << "Caught exception \n" << e << "\nin oneToMany for visualization object named " << el->GetName() << ".  We have "
                                           << "already made el, but its children will be skipped!\n";
        }
        return list;
      }
      
      //Default for the above in case there is only one type to draw.
      template <class TYPE>
      TEveElementList* do_makeEvent(const art::Event& e)
      {
        mf::LogWarning("AssnSceneMaker") << "Calling do_makeEvent in some version of AssnSceneMaker.\n";
        art::ServiceHandle<eved::VisMakerInt<TYPE>> alg;
        TEveElementList* list = new TEveElementList(alg->GetLabel().c_str(), alg->GetLabel().c_str());
        try
        {
          art::Handle<std::vector<TYPE>> prodHand;
          e.getByLabel(alg->GetLabel(), prodHand);
          //mf::LogWarning("AssnSceneMaker") << "Got " << prodHand->size() << " data products for drawing in AssnSceneMaker using label " 
          //                                    << alg->GetLabel() << ".\n";
          for(auto& prod: (*prodHand))
          {
            if(alg->SelectDataProduct(prod))
            {
              auto el = alg->MakeVis(prod);
              if(el != nullptr)
              {
                el->VizDB_Insert(el->GetElementName(), kFALSE, kTRUE);
                list->AddElement(el);
                //mf::LogWarning("AssnSceneMaker") << "Added element " << alg->MakeVis(prod)->GetElementName() << " to TEveElementList " << list->GetName() << ".\n";
              }
            }
          }
        }
        catch(cet::exception& exc)
        {
          //Following from the original LArSoft event display, catch exceptions when getting products since we will probably still want to draw all the other 
          //products.  I think there might be a better way to handle this an an ART setting, but someone will have to look into that later.  
          mf::LogWarning("AssnSceneMaker") << "Caught exception in do_makeEvent while making scene " << NAME << ": \n" << exc << "\n";
        }
        return list;
      }

  };
  
  template <std::string& NAME, class ...PRODS>
  eved::AssnSceneMaker<NAME, PRODS...>::AssnSceneMaker()
  {
  }

  template <std::string &NAME, class ...PRODS>
  void eved::AssnSceneMaker<NAME, PRODS...>::reconfigure(const fhicl::ParameterSet& p)
  {
    auto pset = p.get<fhicl::ParameterSet>(NAME); //the ParameterSet for this algorithm
    int null[] = {(do_reconfigure<PRODS>(pset), 0)...};
    (void)null; //supposedly so that null appears to be used to the compiler without actually generating any code
  }

  //It turns out that we cannot specialize function templates for a number of parameters.  We could do this with some sort of functor class, or we 
  //might be able to use the braced-initialization-list-trick to do something for each template argument.  

  template <std::string& NAME, class ...PRODS>
  void eved::AssnSceneMaker<NAME, PRODS...>::initialize()
  {
  }

  template <std::string &NAME, class ...PRODS> 
  void eved::AssnSceneMaker<NAME, PRODS...>::makeEvent(const art::Event& e)
  {
    mf::LogWarning("AssnSceneMaker") << "In AssnSceneMaker::makeEvent, about to fill scene named " << fScene->GetName() << ".\n";
    DestroyRecursive(fScene);

    do_makeEvent<PRODS>(e); //All of the hard work is done here.  
    //int null[] = {0, (fScene->AddElement(do_makeEvent<PRODS>(e)), 0)..., 0};
    //(void)null; //supposedly so that null appears to be used to the compiler without actually generating any code
  }

  template <std::string &NAME, class ...PRODS>
  TEveScene* eved::AssnSceneMaker<NAME, PRODS...>::makeGlobal()
  {
    fScene = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene(NAME.c_str());
    return fScene;
  }

}
