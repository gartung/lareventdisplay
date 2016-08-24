// \file: BasicViewerFrame.cxx
// \brief: Implmentation of a TGMainFrame-derived frame to be handed by modules to EVE.  Contains one TEveViewer for basic event visulization.  
// \author: Andrew Olivier aolivier@ur.rochester.edu

//ART includes
#ifndef __CINT__
#include "fhiclcpp/ParameterSet.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#endif

//ROOT includes
#include "TGFrame.h"
#include "TEveViewer.h"
#include "TString.h" //For TEveElement::FindChild()
#include "TGLViewer.h"
#include "TEveScene.h"
#include "TEvePad.h"
#include "TGLViewer.h"
#include "TGLEmbeddedViewer.h"
#include "TGLayout.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h" 
#include "lareventdisplay/EveDisplay/Frames/BasicViewerFrame.h"
#include "lareventdisplay/EveDisplay/Config/CoordConfig.h"

ClassImp(eved::BasicViewerFrame)

namespace eved
{
  BasicViewerFrame::BasicViewerFrame(const TGWindow* parent): TGMainFrame(parent, art::ServiceHandle<eved::CoordConfig>()->GetFrameWidth(),art::ServiceHandle<eved::CoordConfig>()->GetFrameHeight()) //1000, 400)
  {
    auto glviewer = new TGLEmbeddedViewer(this, new TEvePad());
    AddFrame(glviewer->GetFrame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    fViewer = new TEveViewer("BasicViewerFrame", "BasicViewerFrame");
    fViewer->SetGLViewer(glviewer, glviewer->GetFrame());
    fViewer->IncDenyDestroy();
    art::ServiceHandle<eved::EveDisplay>()->getEve()->GetViewers()->AddElement(fViewer);

    glviewer->UseLightColorSet();
    glviewer->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
    glviewer->PreferLocalFrame();
    glviewer->ResetCameras();

    MapSubwindows();
    Resize();
    MapWindow();
  }

  void BasicViewerFrame::reconfigure(fhicl::ParameterSet const& p)
  {
    std::string viewerName = p.get<std::string>("ViewerName");
    fViewer->SetName(viewerName.c_str());
    
    fSceneNames = p.get<std::vector<std::string>>("SceneNames");
  }

  void BasicViewerFrame::RequestScenes()
  {
    auto scenes = art::ServiceHandle<eved::EveDisplay>()->getEve()->GetScenes();

    for(const auto& name: fSceneNames)
    {
      TEveElement* found = scenes->FindChild(TString(name.c_str()));
      if(found == nullptr) throw cet::exception("Scene Not Found") << "Failed to find TEveScene with name " << name << " for Viewer " << fViewer->GetName() 
                                                                   << " in BasicViewerFrame::RequestScenes().\n";
      
      else //My intention is that users might configure ART to catch the above exception.  So, we need to be prepared for this. 
      {
        fViewer->AddScene((TEveScene*)(found));
      } 
    } 
  }

  void BasicViewerFrame::DeleteAnnotations()
  {
    fViewer->GetGLViewer()->DeleteOverlayAnnotations();
  } 
}
