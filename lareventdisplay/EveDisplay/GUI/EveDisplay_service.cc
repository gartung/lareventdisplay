// \brief: Implementation of EveDisplay.h
// \author: aoliv23@lsu.edu

//ROOT includes
#include "TEveManager.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TGTextEdit.h"
#include "TGFrame.h"
#include "TEveWindow.h"

//nutools includes
#include "EventDisplayBase/NavState.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
//#include "lareventdisplay/EveDisplay/GUI/EvePSetGui.h"
#include "EventDisplayBase/RootEnv.h"
#include "lareventdisplay/EveDisplay/GUI/EveNavGui.h"
#include "lareventdisplay/EveDisplay/GUI/GUINavigatorBase.h"

//ART includes
#include "art/Framework/IO/Root/RootInput.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Principal/Event.h"

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
      fEve = TEveManager::Create(kTRUE, "FI"); //we will create viewers ourselves
    }

    fNav = new EveNavGui(fEve->GetBrowser());

    //fFclEdit = new TGTextEdit(TEveWindow::CreateWindowInTab(fEve->GetBrowser()->GetTabRight())->GetGUIFrame()); //can add text line-by-line to this widget
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
    //fEve->FullRedraw3D(kTRUE); //draw for the current event
  }
 
} //end namespace eved

DEFINE_ART_SERVICE(eved::EveDisplay)
