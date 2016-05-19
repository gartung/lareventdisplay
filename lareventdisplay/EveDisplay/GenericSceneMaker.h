// \file: GenericSceneMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief:  An algorithm to make a basic TEveScene from a list of data products

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//c++ includes
#include <vector>
#include <string>
#include <typeinfo>

//ROOT includes
#include "TEveScene.h"

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
      TEveScene* makeScene(const art::Event& e);

    private:
      std::map<std::string, std::string> fLabels; //map from typeid::name of data product to label
      template <class TYPE>
      void _do_reconfigure(const fhicl::ParameterSet& p) 
      {
        std::string name = typeid(TYPE()).name();
        fLabels[name] = p.get<std::string>(name);
        return;
      }
      template <class TYPE>
      void _do_makeScene(const art::Event& e, TEveScene* scene)
      {
        try
        {
          const auto found = fLabels.find(typeid(TYPE()).name());
          if(found != fLabels.end())
          {
            art::Handle<std::vector<TYPE>> prodHand;
            e.getByLabel(found->second, prodHand);
            for(auto& prod: (*prodHand))
            {
              scene->AddElement((TEveElement*)(prod.Clone()));
            }
          }
        }
        catch(cet::exception& exc)
        {
          //Following from the original LArSoft event display, catch exceptions when getting products since we will probably still want to draw all the other 
          //products.  I think there might be a better way to handle this an an ART setting, but someone will have to look into that later.  
          mf::LogWarning("GenericSceneMaker") << "Caught exception in _do_makeScene while make scene " << NAME << ": \n" << exc << "\n";
        }
        return;
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
    //_do_reconfigure<PRODS...>(pset);
    int null[] = {0, (_do_reconfigure<PRODS>(pset), 0)..., 0};
    (void)null; //supposedly so that null appears to be used to the compiler without actually generating any code
  }

  //It turns out that we cannot specialize function templates for a number of parameters.  We could do this with some sort of functor class, or we 
  //might be able to use the braced-initialization-list-trick to do something for each template argument.  

  //Base case: 1 TYPE
  /*template <std::string& NAME, class ...PRODS>
  template <class TYPE>
  void eved::GenericSceneMaker<NAME, PRODS...>::_do_reconfigure<TYPE>(const fhicl::ParameterSet& p)
  {
    std::string name = typeid(TYPE()).name();
    fLabels[name] = p.get<std::string>(name);
    return;
  }*/

  //General case: many TYPES
  /*template <std::string& NAME, class ...PRODS>
  template <class TYPE, class ...TYPES>
  void eved::GenericSceneMaker<NAME, PRODS...>::_do_reconfigure<TYPE, TYPES...>(const fhicl::ParameterSet& p)
  {
    std::string name = typeid(TYPE()).name();
    fLabels[name] = p.get<std::string>(name);
    _do_reconfigure<TYPES...>(p); //do this for each data product in PRODS
    return;
  }*/ //see comment above

  template <std::string& NAME, class ...PRODS>
  void eved::GenericSceneMaker<NAME, PRODS...>::initialize()
  {
  }

  //Base case: 1 TYPE
  /*template <std::string &NAME, class ...PRODS>
  template <class TYPE>
  void eved::GenericSceneMaker<NAME, PRODS...>::_do_makeScene<TYPE>(const art::Event& e, TEveScene* scene)
  {
    try
    {
      const auto found = fLabels.find(typeid(TYPE()).name());
      if(found != fLabels.end())
      {
        art::Handle<std::vector<TYPE>> prodHand;
        e.getByLabel(found.second, prodHand);
        for(auto& prod: (*prodHand))
        {
          scene->AddElement((TEveElement*)(prod.Clone()));
        }
      }
    }
    catch(cet::exception& exc)
    {
      //Following from the original LArSoft event display, catch exceptions when getting products since we will probably still want to draw all the other 
      //products.  I think there might be a better way to handle this an an ART setting, but someone will have to look into that later.  
      mf::LogWarning("GenericSceneMaker") << "Caught exception in _do_makeScene while make scene " << NAME << ": \n" << e << "\n";
    }
    return;
  }*/

  //General case: many TYPES
  /*template <std::string &NAME, class ...PRODS>
  template <class TYPE, class ...TYPES>
  void eved::GenericSceneMaker<NAME, PRODS...>::_do_makeScene<TYPE, TYPES...>(const art::Event& e, TEveScene* scene)
  {
    try
    {
      const auto found = fLabels.find(typeid(TYPE()).name());
      if(found != fLabels.end())
      {
        art::Handle<std::vector<TYPE>> prodHand;
        e.getByLabel(found.second, prodHand);
        for(auto& prod: (*prodHand))
        {
          scene->AddElement((TEveElement*)(prod.Clone()));
        }
      }
    }
    catch(cet::exception& exc)
    {
      //Following from the original LArSoft event display, catch exceptions when getting products since we will probably still want to draw all the other 
      //products.  I think there might be a better way to handle this an an ART setting, but someone will have to look into that later.  
      mf::LogWarning("GenericSceneMaker") << "Caught exception in _do_makeScene while make scene " << NAME << ": \n" << e << "\n";
    }
    _do_makeScene<TYPES...>(e, scene);
    return;
  }*/ //see comment near _do_reconfigure

  template <std::string &NAME, class ...PRODS> 
  TEveScene* eved::GenericSceneMaker<NAME, PRODS...>::makeScene(const art::Event& e)
  {
    TEveScene* retVal = new TEveScene(NAME.c_str());
    //_do_makeScene<NAME, PRODS...>(e, retVal);
    
    int null[] = {0, (_do_makeScene<PRODS>(e, retVal), 0)..., 0};
    (void)null; //supposedly so that null appears to be used to the compiler without actually generating any code

    return retVal;
  }

}
