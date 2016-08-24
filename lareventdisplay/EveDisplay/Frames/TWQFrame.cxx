// \file: TWQFrame.cxx
// \brief: Mainframe for drawing two 2D orthonormal views of a specific TPC
// \author: Andrew Olivier aolivier@ur.rochester.edu

//Framework includes
#ifndef __CINT__
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#endif

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
#include "larcore/SimpleTypesAndConstants/geo_types.h"
#include "lareventdisplay/EveDisplay/Frames/TWQFrame.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/TPCGeo.h"
#include "lareventdisplay/EveDisplay/Config/GeoConfig.h"
#include "lareventdisplay/EveDisplay/Config/CoordConfig.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"

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
#include "TEveTrans.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TGLClip.h"
#include "TGTab.h"

ClassImp(eved::OrthoViewerFrame)
ClassImp(eved::TWQFrame)

eved::OrthoViewerFrame::OrthoViewerFrame(TWQFrame* parent, unsigned int width, unsigned int height): 
                                         OrthoViewerFrame(parent, geo::PlaneID(0, 0, 0), width, height) {}

eved::OrthoViewerFrame::OrthoViewerFrame(TWQFrame* parent, geo::PlaneID plane, unsigned int width, unsigned int height): 
                                         TGCompositeFrame(parent, width, height)
{
  auto glviewer = new TGLEmbeddedViewer(this, new TEvePad());
  glviewer->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);

  //Setup maximum limits for TWQ viewer
  /*art::ServiceHandle<geo::Geometry> geom;
  detinfo::DetectorProperties const* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();
  const auto& planeGeo = geom->Plane(plane);

  TGLBoundingBox box(TGLVertex3(0, 0, 0), TGLVertex3(0, planeGeo->Nwires(), detprop->NumberTimeSamples()));

  glviewer->CurrentCamera().Setup(box);*/
  //glviewer->SetOrthoCamera(TGLViewer::kCameraOrthoZOY, 4, 0, centerArr, 0, 0); //We'll try this without zooming to see what happens
  //glviewer->CurrentCamera().SetFixDefCenterVec(centerArr[0], centerArr[1], centerArr[2]);
  AddFrame(glviewer->GetFrame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fViewer = new TEveViewer("OrthoViewerFrame", "OrthoViewerFrame");
  auto glvFrame = glviewer->GetFrame();
  fViewer->SetGLViewer(glviewer, glvFrame);
  fViewer->IncDenyDestroy();
  art::ServiceHandle<eved::EveDisplay>()->getEve()->GetViewers()->AddElement(fViewer);
                                                                                                                                                           
  glviewer->UseLightColorSet();
  glviewer->SetGuideState(TGLUtil::kAxesNone, kTRUE, kFALSE, 0);
  glviewer->PreferLocalFrame();
  glviewer->ResetCameras();
  SetPlane(plane);

  //Magic from ROOT's QT implementation!  When an object is clicked on in TGLViewer, TGLViewer emitts the signal "Clicked()".  The next line will make this 
  //TGLViewer call parent->RespondToClicked(TObject* obj) for the object that the user clicked on.
  glviewer->Connect("Clicked(TObject*)", "eved::TWQFrame", parent, "RespondToClicked(TObject*)");
                                        
  glvFrame->MapSubwindows();
  glvFrame->Resize();
  glvFrame->MapWindow();
                                                                                                                   
  MapSubwindows();
  Resize();
  MapWindow();
}

void eved::OrthoViewerFrame::SetPlane(geo::PlaneID plane)
{
  art::ServiceHandle<geo::Geometry> geom;
  detinfo::DetectorProperties const* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();
  //const auto& planeGeo = geom->Plane(plane);
  //const auto& center = geom->GetTPCFrontFaceCenter(plane.TPC, plane.Cryostat);  
  /*const auto& tpcGeo = geom->TPC(plane);

  art::ServiceHandle<eved::EveDisplay> eved;
  auto twqWindow = eved->getEve()->GetBrowser()->GetTabRight()->GetContainer();
  auto width = twqWindow->GetWidth();
  auto height = twqWindow->GetHeight();*/

  double conversion = detprop->NumberTimeSamples()*GetWidth()/GetHeight();
  TGLBoundingBox box(TGLVertex3(0, 0, 0), TGLVertex3(0, detprop->NumberTimeSamples(), conversion*1.05));
  mf::LogWarning("OrthoViewerFrame") << "Set viewing box between (0, 0, 0) and (0, " << detprop->NumberTimeSamples() << ", " << conversion*1.05 << ")\n";   
  mf::LogWarning("OrthoViewerFrame") << "Width is " << GetWidth() << ".  Height is " << GetHeight() << ".\n";

  //Don't draw anything outside of the TPC's TWQ projection
  /*TGLClipBox* clip = new TGLClipBox();
  clip->Setup(box);
  clip->SetMode(TGLClip::kOutside);*/
  
  fViewer->GetGLViewer()->CurrentCamera().Setup(box);

  //double clipParams[6] = {center.X(), detprop->NumberTimeSamples()/2., conversion/2., 2.*tpcGeo.HalfWidth(), 1.*detprop->NumberTimeSamples(), conversion}; 
  //See TGLClipSet::SetClipState
  /*fViewer->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipBox);
  fViewer->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipBox, clipParams); //From the CMS stereo example, 0=no clip, 1=plane clip, 2=box clip
  fViewer->GetGLViewer()->GetClipSet()->GetCurrentClip()->SetMode(TGLClip::kOutside);*/
  //fViewer->GetGLViewer()->SetClip(clip);
  //fViewer->GetGLViewer()->CurrentCamera().RefModelViewMatrix().Scale(TGLVector3(1., 1., (GetHeight()*detprop->NumberTimeSamples())/(GetWidth()*planeGeo.Nwires()))); 
  //fViewer->GetGLViewer()->CurrentCamera().SetViewport(TGLRect(0, 0, GetWidth(), GetHeight()));
}
                                                                                                                                                           
void eved::OrthoViewerFrame::reconfigure(fhicl::ParameterSet const& p)
{
  std::string viewerName = p.get<std::string>("ViewerName");
  fViewer->SetName(viewerName.c_str());
  
  fSceneNames = p.get<std::vector<std::string>>("SceneNames");
}
                                                                                                                                                           
void eved::OrthoViewerFrame::RequestScenes()
{
  auto scenes = art::ServiceHandle<eved::EveDisplay>()->getEve()->GetScenes();
                                                                                                                                                           
  for(const auto& name: fSceneNames)
  {
    TEveElement* found = scenes->FindChild(TString(name.c_str()));
    if(found == nullptr) throw cet::exception("Scene Not Found") << "Failed to find TEveScene with name " << name << " for Viewer " << fViewer->GetName() 
                                                                 << " in OrthoViewerFrame::RequestScenes().\n";
    
    else //My intention is that users might configure ART to catch the above exception.  So, we need to be prepared for this. 
    {
      fViewer->AddScene((TEveScene*)(found));
    } 
  } 
}
                                                                                                                                                           
void eved::OrthoViewerFrame::DeleteAnnotations()
{
  fViewer->GetGLViewer()->DeleteOverlayAnnotations();
  //auto overlay = fViewer->GetGLViewer()->GetCameraOverlay();
  //overlay->SetShowOrthographic(kTRUE);
  //overlay->SetOrthographicMode(TGLCameraOverlay::kGridBack); //Add grid behind scene objects to help with debugging
}

eved::TWQFrame::TWQFrame(const TGWindow* parent): TGCompositeFrame(parent, art::ServiceHandle<eved::CoordConfig>()->GetFrameWidth(), art::ServiceHandle<eved::CoordConfig>()->GetFrameHeight()), fWireHisto(nullptr), fWireCan(nullptr)
{
  //auto eve = art::ServiceHandle<eved::EveDisplay>()->getEve();

  art::ServiceHandle<eved::GeoConfig> geoConfig;
  auto tpcId = geoConfig->GetTpc();
 
  //The following code was taken from my Algs/TPCTWQViewer.h.  I think there are still a lot of experimental formatting options here to get only the correct 
  //TPC drawn.   
  art::ServiceHandle<geo::Geometry> geom;
  auto frontFaceCenter = geom->GetTPCFrontFaceCenter(tpcId); //+TVector3(0, 0, geom->TPC(tpcId).Length()/2.);
  const geo::TPCGeo& tpc = geom->TPC(tpcId);

  //For each view in this TPC, make a separate TEveViewer in orthographic mode
  art::ServiceHandle<eved::CoordConfig> config;
  for(size_t plane = 0; plane < tpc.Nplanes(); ++plane)
  {
    auto frame = new OrthoViewerFrame(this, geo::PlaneID(tpcId, plane), config->GetFrameWidth(), 0.5*config->GetFrameHeight()/(tpc.Nplanes()+0.5));
    AddFrame(frame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY));
    fViewerFrames.push_back(frame);
  }

  fWireCan = new TRootEmbeddedCanvas("Selected Wire", this, config->GetFrameWidth(), 0.5*config->GetFrameHeight()/(tpc.Nplanes()+0.5));
  fWireCan->GetCanvas()->cd();

  detinfo::DetectorProperties const* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();
  auto nSamples = detprop->NumberTimeSamples();
  fWireHisto = new TH1D("Selected Wire Waveform", ";Time[TDC Ticks];Charge[ADC];;Time Versus Wire Waveform for the Selected Wire;", nSamples, 0, nSamples); 
  fWireHisto->Draw();
  AddFrame(fWireCan, new TGLayoutHints(kLHintsTop | kLHintsExpandX)); //| kLHintsExpandY));
  
  //Do the usual MainFrame stuff
  MapSubwindows();
  Resize();
  MapWindow();
}

void eved::TWQFrame::RespondToClicked(TObject* obj) //What we do when a TGLViewer tells us that the user has clicked on a TObject
{
  if(obj != nullptr)
  {
    mf::LogWarning("OrthoViewerFrame") << "Responding to click on TObject named " << obj->GetName() << ".\n";
  }
  auto boxSet = dynamic_cast<TEveBoxSet*>(obj); //Maybe we click on the frame instead of the box
  if(boxSet != nullptr)
  {
    FillWireHisto(boxSet);
  }
}

void eved::TWQFrame::FillWireHisto(TEveBoxSet* boxSet)
{
  mf::LogWarning("OrthoViewerFrame") << "Filling wire histogram for TEveBoxSet named " << boxSet->GetName() << ".\n";
  auto can = fWireCan->GetCanvas();
  can->Clear();
  can->cd();

  fWireHisto->Reset("ICEM");
  auto plex = boxSet->GetPlex();
  int nBoxes = plex->Size();

  for(int bIt = 0; bIt < nBoxes; ++bIt)
  {
    fWireHisto->Fill(bIt, boxSet->GetDigit(bIt)->fValue); //Should be tdc tick and adc value, but I have been skipping boxes below threshold lately.  So, 
                                                          //this won't make much sense until I encode the entire waveform in each TEveBoxSet or set the box 
                                                          //set's user data to the waveform vector.  I believe a wrapper class may be in order.  
  }

  fWireHisto->Draw();
  can->Update();
}

void eved::TWQFrame::reconfigure(fhicl::ParameterSet const& p)
{
  //size_t TPCNum = p.get<size_t>("TPCNum");
  //size_t CryoNum = p.get<size_t>("CryoNum");
  //fTPCID = geo::TPCID(TPCNum, CryoNum);

  art::ServiceHandle<geo::Geometry> geom;
  //if(!geom->HasTPC(fTPCID)) throw cet::exception("BadTPCID") << "Got bad TPCID for TPC " << TPCNum << " and cryostat "
  //                                                                                          << CryoNum << " in TWQViewerMaker.\n";

  art::ServiceHandle<eved::GeoConfig> config;
  auto tpcId = config->GetTpc();

  const geo::TPCGeo& tpc = geom->TPC(tpcId);
  for(size_t plane = 0; plane < tpc.Nplanes(); ++plane)
  {
    auto const& pset = p.get<fhicl::ParameterSet>("Viewer"+std::to_string(plane));
    fViewerFrames.at(plane)->reconfigure(pset);
  }

  /*std::string viewerName = p.get<std::string>("ViewerName");
  fXZViewer->SetName((viewerName+"XZ").c_str());
  fYZViewer->SetName((viewerName+"YZ").c_str());

  fSceneNames = p.get<std::vector<std::string>>("SceneNames");*/
}

void eved::TWQFrame::RequestScenes()
{
  for(auto& frame: fViewerFrames)
  {
    frame->RequestScenes();
  }
}

void eved::TWQFrame::DeleteAnnotations()
{
  for(auto& frame: fViewerFrames)
  {
    frame->DeleteAnnotations();
  }

  art::ServiceHandle<geo::Geometry> geom;
  art::ServiceHandle<eved::GeoConfig> config;
  
  auto tpc = config->GetTpc();
  const auto& tpcGeo = geom->TPC(tpc);

  for(size_t plane = 0; plane < tpcGeo.Nplanes(); ++plane)
  {
    //Add new frames as needed for the current TPC
    if(plane >= fViewerFrames.size()) 
    {
      auto frame = new OrthoViewerFrame(this, geo::PlaneID(tpc, plane), GetWidth(), GetHeight()/(tpcGeo.Nplanes()+0.5));
      frame->RequestScenes();
      AddFrame(frame, new TGLayoutHints(kLHintsBottom | kLHintsExpandX));
      fViewerFrames.push_back(frame);
    }
    fViewerFrames.at(plane)->SetPlane(geo::PlaneID(tpc, plane));
  }
}
