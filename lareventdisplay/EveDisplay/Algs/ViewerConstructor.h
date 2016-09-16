////////////////////////////////////////////////////////////////////////
// Class:       ViewerConstructor
// Module Type: analyzer
// File:        ViewerConstructor.h
// Brief:       Constructs a window to be given to TEveManager and gets scenes for that window's viewers 
//
// Generated at Sun Jul 24 11:58:58 2016 by Andrew Olivier using artmod
// from cetpkgsupport v1_10_01.
////////////////////////////////////////////////////////////////////////

//Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"

//ROOT includes
#include "TEveScene.h"
#include "TEveTrack.h"
#include "TEvePathMark.h"
#include "TVector3.h"
#include "TEveVector.h"
#include "TGTab.h"

//c++ includes
#include <memory>

//The following flow of function calls seems like it could ensure scene creation in time for viewer creation:
//*Scenes are REQURESTED from TEveManager at module construction
//*Windows are created and given to TEveManager in beginJob
//  *Viewers are requested from TEveManager as needed by windows
//  *Scenes are added to viewers.  We will throw if we don't find a scene we need.  It is up to the user-controllable ART configuration of exception handling 
//   to decide whether to proceed without the needed scene.
//*Scenes are reset and filled in analyze()

//Takes one template parameter: FRAME
//*FRAME shall be a class that inherits from ROOT's TGMainFrame
//*FRAME shall define the following functions:
//  a.) FRAME::FRAME(const TGWindow* parent) //constructs a FRAME object
//  b.) FRAME::reconfigure(fhicl::ParameterSet const&) //read new parameters from a parameter set and reconfigure as needed
//  c.) FRAME::RequestScenes() //Any TEveViewers used by FRAME request needed TEveScenes.  This will be called in beginJob() so that TEveScenes have 
//                             //already been created 
//  d.) FRAME::DeleteAnnotations() //Call TEveViewer::DeleteAnnotations for any TEveViewers
//*This module shall construct a FRAME object and embed it in a new window requested from TEVEManager
//*FRAME objects could also encapsulate TQObject signal-slot mechanisms for things like a histogram of a wire view

namespace eved
{
  
  template <class FRAME>
  class ViewerConstructor : public art::EDAnalyzer {
  public:
    explicit ViewerConstructor(fhicl::ParameterSet const & p);
    // The destructor generated by the compiler is fine for classes
    // without bare pointers or other resource use.
  
    // Plugins should not be copied or assigned.
    ViewerConstructor(ViewerConstructor const &) = delete;
    ViewerConstructor(ViewerConstructor &&) = delete;
    ViewerConstructor & operator = (ViewerConstructor const &) = delete;
    ViewerConstructor & operator = (ViewerConstructor &&) = delete;
  
    // Required functions.
    void analyze(art::Event const & e) override;
  
    // Selected optional functions.
    void beginJob() override;
    void reconfigure(fhicl::ParameterSet const & p) override;
  
  private:
  
    // Declare member data here.
    FRAME* fFrame; //Owned by one of the EVE classes, so this is just an observer
    std::string fName; //Name of this viewer to be shown in tab widget
  
  };
  
  template <class FRAME> 
  eved::ViewerConstructor<FRAME>::ViewerConstructor(fhicl::ParameterSet const & p)
    :
    EDAnalyzer(p)
   // More initializers here.
  {
    //create and embed the FRAME object we are responsible for
    auto eve = art::ServiceHandle<eved::EveDisplay>()->getEve();
  
    auto browse = eve->GetBrowser();

    //New method for creating a window in a tab.  I can control the tabs' window sizes directly this way so that frames with multiple viewers (like the 
    //TWQ and orthonormal frames in Frames) are shown completely and do not "overflow" their "parents".
    auto tab = browse->GetTabRight();
    fFrame = new FRAME(tab);
    reconfigure(p);
    tab->AddTab(fName.c_str(), fFrame); 

    //Does this prevent the weird problem with needing to resize the Eve window?  
    auto container = tab->GetCurrentContainer(); 
    container->MapSubwindows();
    container->Resize();
    container->MapWindow();

    tab->MapSubwindows();
    tab->Resize();
    tab->MapWindow();

    //fFrame = new FRAME(parent);
    //parent->AddFrame(fFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY)); 
  
    /*browse->ShowCloseTab(kFALSE);
    browse->StartEmbedding(TRootBrowser::kRight);
    auto parent = gClient->GetRoot();
  
    fFrame = new FRAME(parent); //add our FRAME in a new tab in the EVE browser
  
    browse->StopEmbedding(); //We don't seem to be able to get a module's module label from its parameter set 
    browse->ShowCloseTab(kTRUE);*/
  
    browse->SanitizeTabCounts(); 
  }
  
  template <class FRAME>
  void eved::ViewerConstructor<FRAME>::analyze(art::Event const & e)
  {
    // Implementation of required member function here.
    fFrame->DeleteAnnotations(); //Be very careful of working with scenes here.  The order in which SceneConstructor fills scenes and this function is called 
                                 //is left ambiguous.  
  }
  
  template <class FRAME>
  void eved::ViewerConstructor<FRAME>::beginJob()
  {
    // Implementation of optional member function here.
    fFrame->RequestScenes(); //TEveViewers, if any, get the scenes they need.  TEveScenes were created in module constructors.  
  }
  
  template <class FRAME>
  void eved::ViewerConstructor<FRAME>::reconfigure(fhicl::ParameterSet const & p)
  {
    // Implementation of optional member function here.
    fFrame->reconfigure(p.get<fhicl::ParameterSet>("Viewer"));
    fName = p.get<std::string>("Name");
  }
}

//You need to declare a particular implementation of this class to use it as a module.  Your header for a specific ViewerConstructor implementation 
//might look like this: 
//DEFINE_ART_MODULE(eved::ViewerConstructor<FRAME>) //we don't need to use typedef here since we only have one template parameter
