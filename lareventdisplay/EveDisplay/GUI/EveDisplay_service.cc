// \brief: Implementation of EveDisplay.h
// \author: aoliv23@lsu.edu

//ROOT includes
#include "TEveManager.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TGTextEdit.h"
#include "TGFrame.h"
#include "TEveWindow.h"
#include "TGWindow.h"
#include "TGFileBrowser.h"
#include "TGListTree.h"
#include "TFile.h"
#include "TGTab.h"

//nutools includes
#include "nutools/EventDisplayBase/NavState.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
//#include "lareventdisplay/EveDisplay/GUI/EvePSetGui.h"
#include "nutools/EventDisplayBase/RootEnv.h"
#include "lareventdisplay/EveDisplay/GUI/EveNavGui.h"
#include "lareventdisplay/EveDisplay/GUI/GUINavigatorBase.h"

//ART includes
#include "art/Framework/IO/Root/RootInput.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "fhiclcpp/ParameterSetRegistry.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Worker.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "canvas/Persistency/Provenance/ModuleDescription.h"

//TODO: Either remove all event dipslay modules when the EVE window is closed (ideal) or force the ART job to finish.  

namespace eved 
{

  EveDisplay::EveDisplay(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg): GUINavigatorBase(reg)
  {
    if(gEve != nullptr)
    {
      fEve = gEve;
    }
    else
    {
      fEve = TEveManager::Create(kTRUE, "I"); //we will create viewers and file browser ourselves
      //In light of the code below, note that we get an Eve list tree widget for "free"
    }

    //fNav = new EveNavGui(fEve->GetBrowser());
    //Control GUI
    auto eBrowse = fEve->GetBrowser();
    auto leftTab = eBrowse->GetTabLeft();
    fNav = new EveNavGui(leftTab);
    leftTab->AddTab("Control", fNav);
    leftTab->MapSubwindows(); //TODO: Do we really need to do this for each new tab we add?  
    leftTab->Resize();
    leftTab->MapWindow();
    
    //Make our own "custom" file browser that browses the TFileSiervice's histograms.  
    eBrowse->StartEmbedding(TRootBrowser::kLeft);
    auto fileBrowse = fEve->GetBrowser()->MakeFileBrowser(kTRUE);
    eBrowse->StopEmbedding("TFileService");
    auto file = &(art::ServiceHandle<art::TFileService>()->file());
    fileBrowse->BrowseObj(file);

    //ParameterSet editor
    //eBrowse->StartEmbedding(TRootBrowser::kLeft);
    fPSetEdit = new PSetLTFrame(leftTab); 
    leftTab->AddTab("PSets", fPSetEdit);
    leftTab->MapSubwindows();
    leftTab->Resize();
    leftTab->MapWindow();
    //eBrowse->StopEmbedding("ParameterSets");
    
    //Cosmetic changes to default display
    leftTab->SetTab("Control", kTRUE); //Set the default tab users will see when they open the Eve display to the event control tab.  
    //auto bottomTab = eBrowse->GetTabBottom();
    //bottomTab->Resize(bottomTab->GetWidth(), 1); //Set bottom tab to minimum size to maximize detail in the TWQ viewer
    ((TGWindow*)(eBrowse->GetTabBottom()->GetParent()))->Resize(0, 1);
    eBrowse->Layout();

    fEve->GetBrowser()->SanitizeTabCounts();

    fEve->GetBrowser()->GetMainFrame()->Connect("CloseWindow()", "TApplication", gROOT->GetApplication(), "Terminate()"); //end the event display application
                                                                                                                          //when EVE is asked to close its 
                                                                                                                          //main window. 

    //fEve->GetBrowser()->RemoveTab(TRootBrowser::kBottom, 0);

    //fPSet = new EvePSetGui(fEve->GetBrowser(), info_);
  }

  void EveDisplay::afterNewEvent(const art::Event& e)
  {
    fNav->SetRunSubRunEvent(e.id().run(), e.id().subRun(), e.id().event());

    //GUINavigatorBase handles interrupting the state machine
    fEve->Redraw3D(kFALSE);
  }

  void EveDisplay::afterBeginJob() //Happens after GUINavigatorBase fills fWorkers
  {
    fEve->Redraw3D(kFALSE);
    auto window = fEve->GetMainWindow();
    auto client = window->GetClient();
    fEve->GetMainWindow()->Resize(client->GetDisplayWidth(), client->GetDisplayHeight());

    //Get service configurations for ParameterSet editor.  Learned to do this from EventDisplayBase/ServiceTable.h
    auto& reg = art::ServiceRegistry::instance();
    std::vector<fhicl::ParameterSet> psets;
    reg.presentToken().getParameterSets(psets);
    for(const auto& pset: psets) //TODO: Understand why I get 2 or 4 of some services here!  Also, understand what the "." parameter set means.
    {
      fPSetEdit->AddPSet(pset);
      /*std::string name;
      if(pset.get_if_present("service_type", name))
      {
        mf::LogWarning("EveDisplay_service") << "Adding a service parameter set named " << name << " to editor\n";
      }*/
    }

    //Get worker configurations for ParameterSet editor.  Learned how to do this from art/Framework/Services/UserInteraction/UserInteraction.cc
    for(const auto worker: fWorkers)
    {
      fhicl::ParameterSet pset;
      fhicl::ParameterSetRegistry::get(worker->description().parameterSetID(), pset);
      fPSetEdit->AddPSet(pset);
    }

  }

  void EveDisplay::afterGUIEvent() //Happens just after we get input from the GUI
  {
    //Update service and module configurations
    auto& reg = art::ServiceRegistry::instance();
    std::vector<fhicl::ParameterSet> psets;
    auto token = reg.presentToken();
    token.getParameterSets(psets);
    for(auto& pset: psets)
    {
      fPSetEdit->UpdatePSet(pset);
    }
    token.putParameterSets(psets);

    for(auto worker: fWorkers)
    {
      fhicl::ParameterSet pset;
      fhicl::ParameterSetRegistry::get(worker->description().parameterSetID(), pset);
      if(fPSetEdit->UpdatePSet(pset)) worker->reconfigure(pset);
    }
  }
 
} //end namespace eved

DEFINE_ART_SERVICE(eved::EveDisplay)
