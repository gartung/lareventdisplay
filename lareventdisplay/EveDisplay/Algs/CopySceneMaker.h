// \file: CopySceneMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief:  An algorithm to grab an existing TEveScene from the TEveManager.  Throws if unsucessful.

//Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
//#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"

//c++ includes
#include <vector>
#include <string>
#include <typeinfo>

//ROOT includes
#include "TEveScene.h"
#include "TEveViewer.h"
#include "TEveElement.h"

/*namespace 
{
  void DestroyRecursive(TEveElement* el)
  {
    for(auto child = el->BeginChildren(); child != el->EndChildren(); ++child)
    {
      DestroyRecursive(*child);
    }
    el->DestroyElements();
    return;
  }
}*/

namespace eved {

  template<std::string& NAME>
  class CopySceneMaker
  {
    public:
      CopySceneMaker();
      ~CopySceneMaker() = default;
    
      void initialize();
      void reconfigure(const fhicl::ParameterSet& p);
      void makeEvent(const art::Event& e);
      TEveScene* makeGlobal();

    private:
      std::string fSceneName; //Name of the scene to copy
      TEveScene* fScene; //The TEveScene we will create and update
  };
  
  template<std::string& NAME>
  eved::CopySceneMaker<NAME>::CopySceneMaker(): fScene(nullptr)
  {
  }

  template<std::string& NAME>
  void eved::CopySceneMaker<NAME>::reconfigure(const fhicl::ParameterSet& p)
  {
    auto pset = p.get<fhicl::ParameterSet>(NAME); //the ParameterSet for this algorithm
    fSceneName = pset.get<std::string>("SceneName");
  }

  template<std::string& NAME>
  void eved::CopySceneMaker<NAME>::initialize()
  {
  }

  template<std::string& NAME>
  void eved::CopySceneMaker<NAME>::makeEvent(const art::Event& /*e*/)
  {
    //Do nothing here because someone else is presumably updating this scene
  }

  template<std::string& NAME>
  TEveScene* eved::CopySceneMaker<NAME>::makeGlobal()
  {
    fScene = dynamic_cast<TEveScene*>(art::ServiceHandle<eved::EveDisplay>()->getEve()->GetScenes()->FindChild(TString(fSceneName.c_str())));
    if(fScene == nullptr) //If we failed to find the scene we wanted
    {
      throw cet::exception("BadSceneName") << "Could not find scene with name " << fSceneName << " in CopySceneMaker::makeGlobal().\n";
    }
    //fScene->IncDenyDestroy();
    return fScene;
  }

}
