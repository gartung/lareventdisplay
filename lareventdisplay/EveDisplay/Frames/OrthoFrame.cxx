// \file: OrthoFrame.cxx
// \brief: Mainframe for drawing two 2D orthonormal views of a specific TPC
// \author: Andrew Olivier aolivier@ur.rochester.edu

//Framework includes
#ifndef __CINT__
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib/exception.h"
#endif

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
#include "lareventdisplay/EveDisplay/Config/GeoConfig.h"
#include "larcore/SimpleTypesAndConstants/geo_types.h"
#include "lareventdisplay/EveDisplay/Frames/OrthoFrame.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/TPCGeo.h"
#include "lareventdisplay/EveDisplay/Config/CoordConfig.h"

//ROOT includes
#include "TEveViewer.h"
#include "TEveScene.h"
#include "TEveTrack.h"
#include "TEvePathMark.h"
#include "TVector3.h"
#include "TEveVector.h"
#include "TGLViewer.h"
#include "TGLEmbeddedViewer.h"
#include "TGClient.h"
#include "TGFrame.h"
#include "TEvePad.h"
#include "TVector3.h"
#include "TGSplitFrame.h"
#include "TGLayout.h"
#include "TGLBoundingBox.h"
#include "TGLOrthoCamera.h"
#include "TGLUtil.h"
#include "TGLCameraOverlay.h"
#include "TGLabel.h"

ClassImp(eved::OrthoFrame)

eved::OrthoFrame::OrthoFrame(const TGWindow* parent): TGMainFrame(parent, art::ServiceHandle<eved::CoordConfig>()->GetFrameWidth(),art::ServiceHandle<eved::CoordConfig>()->GetFrameHeight()) /*1000, 400)*/, fXZViewer(nullptr), fYZViewer(nullptr)//, fTPCID(0, 0)
{
  auto eve = art::ServiceHandle<eved::EveDisplay>()->getEve();
 
  //The following code was taken from my Algs/TPCOrthoViewer.h.  I think there are still a lot of experimental formatting options here to get only the correct 
  //TPC drawn.   
  art::ServiceHandle<geo::Geometry> geom;
  art::ServiceHandle<eved::GeoConfig> config;
  const auto tpcId = config->GetTpc();
  auto frontFaceCenter = geom->GetTPCFrontFaceCenter(tpcId); //+TVector3(0, 0, geom->TPC(fTPCID).Length()/2.);
  const geo::TPCGeo& tpc = geom->TPC(tpcId);
  TGLVertex3 low(tpc.MinX(), tpc.MinY(), tpc.MinZ()), high(tpc.MaxX(), tpc.MaxY(), tpc.MaxZ());
  /*double tpcBoxLim[8][3] = {{tpc.MinX(), tpc.MinY(), tpc.MinZ()}, {tpc.MaxX(), tpc.MaxY(), tpc.MaxZ()}, {tpc.MinX(), tpc.MinY(), tpc.MaxZ()},
                        {tpc.MaxX(), tpc.MaxY(), tpc.MaxZ()}, {tpc.MinX(), tpc.MaxY(), tpc.MaxZ()}, {tpc.MaxX(), tpc.MinY(), tpc.MinZ()},
                        {tpc.MaxX(),tpc.MinY(), tpc.MaxZ()}, {tpc.MinX(), tpc.MaxY(), tpc.MinZ()}};*/
  
  //TGLBoundingBox tpcBox(tpcBoxLim);
  TGLBoundingBox tpcBox(low, high);
  //double centerArr[3] = {frontFaceCenter.X(), frontFaceCenter.Y(), frontFaceCenter.Z()+geom->TPC(tpcId).Length()/2.};

  fSplit = new TGSplitFrame(this, GetWidth(), GetHeight());
  AddFrame(fSplit, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY)); 
  fSplit->VSplit(200); //now we have a left frame, fSplit->GetFirst(), and a right frame, fSplit->GetSecond(). HSplit(0) splits the frame in half.  
  fPad = new TEvePad();

  auto left = fSplit->GetFirst();
  auto right = fSplit->GetSecond();

  fXLabel = new TGLabel(left, new TGHotString("z[cm]"));
  left->AddFrame(fXLabel, new TGLayoutHints(kLHintsLeft));

  auto glxzviewer = new TGLEmbeddedViewer(left, fPad);
  glxzviewer->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
  //glxzviewer->CurrentCamera().Setup(tpcBox);
  //glxzviewer->SetOrthoCamera(TGLViewer::kCameraOrthoXOZ, 4, 0, centerArr, 0, 0);
  //glxzviewer->CurrentCamera().SetFixDefCenterVec(centerArr[0], centerArr[1], centerArr[2]);
  left->AddFrame(glxzviewer->GetFrame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fXZViewer = new TEveViewer("OrthoXZViewer", ("Orthographic XZ Viewer for TPC "+std::to_string(tpcId.TPC)+" Cryostat "+std::to_string(tpcId.Cryostat)).c_str() );
  fXZViewer->SetGLViewer(glxzviewer, glxzviewer->GetFrame());
  fXZViewer->IncDenyDestroy();
  eve->GetViewers()->AddElement(fXZViewer);
  glxzviewer->CurrentCamera().Setup(tpcBox);
  
  fXZLabel = new TGLabel(left, new TGHotString("x[cm]"));
  left->AddFrame(fXZLabel, new TGLayoutHints(kLHintsBottom | kLHintsCenterX));

  fYLabel = new TGLabel(right, new TGHotString("y[cm]"));
  right->AddFrame(fYLabel, new TGLayoutHints(kLHintsLeft));
  
  auto glyzviewer = new TGLEmbeddedViewer(right, fPad);
  //glyzviewer->SetOrthoCamera(TGLViewer::kCameraOrthoZOY, 2.5, 0, centerArr, 0, 0);
  glyzviewer->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
  //glyzviewer->CurrentCamera().SetFixDefCenterVec(centerArr[0], centerArr[1], centerArr[2]);
  right->AddFrame(glyzviewer->GetFrame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fYZViewer = new TEveViewer("OrthoYZViewer", ("Orthographic YZ Viewer for TPC "+std::to_string(tpcId.TPC)+" Cryostat "+std::to_string(tpcId.Cryostat)).c_str());
  fYZViewer->SetGLViewer(glyzviewer, glyzviewer->GetFrame());
  fYZViewer->IncDenyDestroy();
  eve->GetViewers()->AddElement(fYZViewer);
  glyzviewer->CurrentCamera().Setup(tpcBox);

  fYZLabel = new TGLabel(right, new TGHotString("z[cm]"));
  right->AddFrame(fYZLabel, new TGLayoutHints(kLHintsBottom | kLHintsCenterX));

  //Do the usual MainFrame stuff
  MapSubwindows();
  Resize();
  MapWindow();
}

void eved::OrthoFrame::reconfigure(fhicl::ParameterSet const& p)
{
  //size_t TPCNum = p.get<size_t>("TPCNum");
  //size_t CryoNum = p.get<size_t>("CryoNum");
  //fTPCID = geo::TPCID(TPCNum, CryoNum);
  //if(!art::ServiceHandle<geo::Geometry>()->HasTPC(fTPCID)) throw cet::exception("BadTPCID") << "Got bad TPCID for TPC " << TPCNum << " and cryostat "
  //                                                                                          << CryoNum << " in TPCOrthoViewerMaker.\n";

  std::string viewerName = p.get<std::string>("ViewerName");
  fXZViewer->SetName((viewerName+"XZ").c_str());
  fYZViewer->SetName((viewerName+"YZ").c_str());

  fSceneNames = p.get<std::vector<std::string>>("SceneNames");
}

void eved::OrthoFrame::RequestScenes()
{
  auto scenes = art::ServiceHandle<eved::EveDisplay>()->getEve()->GetScenes();

  for(const auto& name: fSceneNames)
  {
    TEveElement* found = scenes->FindChild(TString(name.c_str()));
    if(found == nullptr) throw cet::exception("Scene Not Found") << "Failed to find TEveScene with name " << name << " for Viewers " << fXZViewer->GetName()
                                                                 << "and " << fYZViewer->GetName() << " in OrthoFrame::RequestScenes().\n";

    else //My intention is that users might configure ART to catch the above exception.  So, we need to be prepared for this. 
    {
      fXZViewer->AddScene((TEveScene*)(found));
      fYZViewer->AddScene((TEveScene*)(found));
    }
  }
}

void eved::OrthoFrame::DeleteAnnotations()
{
  fXZViewer->GetGLViewer()->DeleteOverlayAnnotations();
  fYZViewer->GetGLViewer()->DeleteOverlayAnnotations();

  auto xzoverlay = fXZViewer->GetGLViewer()->GetCameraOverlay();
  xzoverlay->SetShowOrthographic(kTRUE);
  xzoverlay->SetOrthographicMode(TGLCameraOverlay::kGridBack);

  auto yzoverlay = fYZViewer->GetGLViewer()->GetCameraOverlay();
  yzoverlay->SetShowOrthographic(kTRUE);
  yzoverlay->SetOrthographicMode(TGLCameraOverlay::kGridBack);
}
