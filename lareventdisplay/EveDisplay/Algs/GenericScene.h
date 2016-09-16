// \file: GenericScene.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief:  An algorithm to make a basic TEveScene from a list of data products

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//c++ includes
#include <memory>

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
  //*PRODS: LArSoft data product types to be drawn in this scene
  
  template <class ...PRODS>
  class GenericScene: public art::EDAnalyzer
  {
    public:
      // The destructor generated by the compiler is fine for classes
      // without bare pointers or other resource use.
  
      // Plugins should not be copied or assigned.
      GenericScene(GenericScene const &) = delete;
      GenericScene(GenericScene &&) = delete;
      GenericScene & operator = (GenericScene const &) = delete;
      GenericScene & operator = (GenericScene &&) = delete; 

      explicit GenericScene(fhicl::ParameterSet const& p);
    
      void beginJob() override;
      void reconfigure(const fhicl::ParameterSet& p) override;
      void analyze(const art::Event& e) override;

    private:
      //TEveScene* makeScene();

      std::map<std::string, std::string> fLabels; //map from typeid::name of data product to label
      TEveScene* fScene; //The TEveScene we will create and update

      /*template <class TYPE>
      void do_reconfigure(const fhicl::ParameterSet& p) 
      {
        art::ServiceHandle<eved::VisMakerInt<TYPE>> alg;
        return;
      }*/
      template <class TYPE>
      TEveElementList* do_analyze(const art::Event& e)
      {
        mf::LogWarning("GenericScene") << "Calling do_analyze in some version of GenericScene.\n";
        art::ServiceHandle<eved::VisMakerInt<TYPE>> alg;
        TEveElementList* list = new TEveElementList(alg->GetLabel().c_str(), alg->GetLabel().c_str());
        try
        {
          art::Handle<std::vector<TYPE>> prodHand;
          e.getByLabel(alg->GetLabel(), prodHand);
          mf::LogWarning("GenericScene") << "Got " << prodHand->size() << " data products for drawing in GenericScene using label " 
                                              << alg->GetLabel() << ".\n";
          for(auto& prod: (*prodHand))
          {
            if(alg->SelectDataProduct(prod))
            {
              auto el = alg->MakeVis(prod);
              if(el != nullptr)
              {
                el->VizDB_Insert(el->GetElementName(), kFALSE, kTRUE);
                list->AddElement(el);
                mf::LogWarning("GenericScene") << "Added element " << alg->MakeVis(prod)->GetElementName() << " to TEveElementList " << list->GetName() << ".\n";
              }
            }
          }
        }
        catch(cet::exception& exc)
        {
          //Following from the original LArSoft event display, catch exceptions when getting products since we will probably still want to draw all the other 
          //products.  I think there might be a better way to handle this an an ART setting, but someone will have to look into that later.  
          mf::LogWarning("GenericScene") << "Caught exception in do_analyze while making scene " << fScene->GetName() << ".\n";
        }
        return list;
      }

  };
  
  template <class ...PRODS>
  eved::GenericScene<PRODS...>::GenericScene(const fhicl::ParameterSet& p): EDAnalyzer(p)
  {
    reconfigure(p);
  }

  template <class ...PRODS>
  void eved::GenericScene<PRODS...>::reconfigure(const fhicl::ParameterSet& p)
  {
    //int null[] = {(do_reconfigure<PRODS>(pset), 0)...}; //This might not even need to be called here anymore
    //(void)null; //supposedly so that null appears to be used to the compiler without actually generating any code
  }

  //It turns out that we cannot specialize function templates for a number of parameters.  We could do this with some sort of functor class, or we 
  //might be able to use the braced-initialization-list-trick to do something for each template argument.  

  /*template <class ...PRODS>
  void eved::GenericScene<PRODS...>::beginJob()
  {
    //art::ServiceHandle<eved::EveDisplay>()->GetEve()->GetScenes()->AddElement(makeScene<PRODS>());
    this->makeScene<PRODS>(); //I think makeScene gets a new scene that TEveManager already knows about
  }*/

  template <class ...PRODS> 
  void eved::GenericScene<PRODS...>::analyze(const art::Event& e)
  {
    mf::LogWarning("GenericScene") << "In GenericScene::analyze, about to fill scene named " << fScene->GetName() << ".\n";
    DestroyRecursive(fScene);

    int null[] = {0, (fScene->AddElement(do_analyze<PRODS>(e)), 0)..., 0};
    (void)null; //supposedly so that null appears to be used to the compiler without actually generating any code
  }

  template <class ...PRODS>
  /*TEveScene*/ void eved::GenericScene<PRODS...>::beginJob() //::makeScene()
  {
    fScene = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene(currentContext()->moduleLabel()->c_str()); //Since we are an EDAnalyzer now, we might 
                                                                                                               //as well have our scene take our module 
                                                                                                               //label.  
    //NAME.c_str());
    //return fScene;
  }

}

