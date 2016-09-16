////////////////////////////////////////////////////////////////////////
// Class:       TPCOrthoViewerMaker
// Module Type: analyzer
// File:        TPCOrthoViewerMaker_module.cc
// Brief:       Module template to make a viewer that shows XZ and YZ orthographic views of a specific TPC for the Eve-based event display
//
// Generated at Tue May 17 13:33:55 2016 by Andrew Olivier using artmod
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
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib/exception.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
#include "lareventdisplay/EveDisplay/Algs/AlgHolder.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/TPCGeo.h"

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

//c++ includes
#include <memory>

namespace eved {
  template <std::string& NAME, class ...ALGS>
  class TPCOrthoViewerMaker;
}

//Template Parameters
//ALGS: A list of algorithms that construct TEveScene objects
//An ALG must have the following methods: 
//*ALG::ALG()
//*void ALG::reconfigure(const fhicl::ParameterSet& p)
//*void ALG::makeEvent(const art::Event& evt) 
//*TEveScene* ALG::makeGlobal() 
//*void ALG::initialize()

//NAME: the name of this TEveViewer in the event display

template <std::string& NAME, class ...ALGS>

class eved::TPCOrthoViewerMaker : public art::EDAnalyzer, virtual SingleAlgHolder<ALGS>... {
public:
  explicit TPCOrthoViewerMaker(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TPCOrthoViewerMaker(TPCOrthoViewerMaker const &) = delete;
  TPCOrthoViewerMaker(TPCOrthoViewerMaker &&) = delete;
  TPCOrthoViewerMaker & operator = (TPCOrthoViewerMaker const &) = delete;
  TPCOrthoViewerMaker & operator = (TPCOrthoViewerMaker &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

  // Selected optional functions.
  void beginJob() override;
  void beginRun(art::Run const & r) override;
  void beginSubRun(art::SubRun const & sr) override;
  void endJob() override;
  void endRun(art::Run const & r) override;
  void endSubRun(art::SubRun const & sr) override;
  void reconfigure(fhicl::ParameterSet const & p) override;
  void respondToCloseInputFile(art::FileBlock const & fb) override;
  void respondToCloseOutputFiles(art::FileBlock const & fb) override;
  void respondToOpenInputFile(art::FileBlock const & fb) override;
  void respondToOpenOutputFiles(art::FileBlock const & fb) override;

private:

  // Declare member data here.
  // These are ROOT GUI classes, so we don't own any of them! So, do not delete them in this class.  
  TEveViewer* fXZViewer;
  TEveViewer* fYZViewer;
  TGMainFrame* fMain; 
  TGSplitFrame* fSplit; 
  TGLabel* fXLabel;
  TGLabel* fXZLabel;
  TGLabel* fYLabel; 
  TGLabel* fYZLabel;
  TEvePad* fPad;
  geo::TPCID fTPCID; //Number of the TPC and cryostat we will draw.  Checked for validity against the geometry service

};

template <std::string &NAME, class ...ALGS>
eved::TPCOrthoViewerMaker<NAME, ALGS...>::TPCOrthoViewerMaker(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p), fXZViewer(nullptr), fYZViewer(nullptr)
  //SingleAlgHolder<ALGS>()...
  // More initializers here.
{
  //fXZViewer = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewViewer((NAME+"XZ").c_str());
  //fYZViewer = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewViewer((NAME+"YZ").c_str());

  reconfigure(p);

  auto eve = art::ServiceHandle<eved::EveDisplay>()->getEve();

  auto browse = eve->GetBrowser();

  browse->ShowCloseTab(kFALSE);
  browse->StartEmbedding(TRootBrowser::kRight);
  //My best guess is that starting embedding makes gClient->GetRoot() point to a new tab in the EVE browser.  If you know a better way to do this, please 
  //improve it!

  art::ServiceHandle<geo::Geometry> geom;
  auto frontFaceCenter = geom->GetTPCFrontFaceCenter(fTPCID); //+TVector3(0, 0, geom->TPC(fTPCID).Length()/2.);
  const geo::TPCGeo& tpc = geom->TPC(fTPCID);
  //TGLVertex3 low(tpc.MinX(), tpc.MinY(), tpc.MinZ()), high(tpc.MaxX(), tpc.MaxY(), tpc.MaxZ());
  double tpcBoxLim[8][3] = {{tpc.MinX(), tpc.MinY(), tpc.MinZ()}, {tpc.MaxX(), tpc.MaxY(), tpc.MaxZ()}, {tpc.MinX(), tpc.MinY(), tpc.MaxZ()},
                        {tpc.MaxX(), tpc.MaxY(), tpc.MaxZ()}, {tpc.MinX(), tpc.MaxY(), tpc.MaxZ()}, {tpc.MaxX(), tpc.MinY(), tpc.MinZ()},
                        {tpc.MaxX(),tpc.MinY(), tpc.MaxZ()}, {tpc.MinX(), tpc.MaxY(), tpc.MinZ()}};
  std::string reco("OrthoReco");
  std::string truth("OrthoTruth");
  
  TGLBoundingBox tpcBox(tpcBoxLim);
  double centerArr[3] = {frontFaceCenter.X(), frontFaceCenter.Y(), frontFaceCenter.Z()+geom->TPC(fTPCID).Length()/2.};

  auto parent = gClient->GetRoot();
  fMain = new TGMainFrame(parent, 1000, 400);
  fSplit = new TGSplitFrame(fMain, fMain->GetWidth(), fMain->GetHeight());
  fMain->AddFrame(fSplit, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY)); 
  fSplit->VSplit(200); //now we have a left frame, fSplit->GetFirst(), and a right frame, fSplit->GetSecond(). HSplit(0) splits the frame in half.  
  fPad = new TEvePad();

  auto left = fSplit->GetFirst();
  auto right = fSplit->GetSecond();

  fXLabel = new TGLabel(left, new TGHotString("z[cm]"));
  left->AddFrame(fXLabel, new TGLayoutHints(kLHintsLeft));

  auto glxzviewer = new TGLEmbeddedViewer(left, fPad);
  glxzviewer->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
  glxzviewer->CurrentCamera().Setup(tpcBox, kFALSE);
  glxzviewer->SetOrthoCamera(TGLViewer::kCameraOrthoXOZ, 4, 0, centerArr, 0, 0);
  glxzviewer->CurrentCamera().SetFixDefCenterVec(centerArr[0], centerArr[1], centerArr[2]);
  left->AddFrame(glxzviewer->GetFrame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fXZViewer = new TEveViewer("OrthoXZViewer", ("Orthographic XZ Viewer for TPC "+std::to_string(fTPCID.TPC)+" Cryostat "+std::to_string(fTPCID.Cryostat)).c_str() );
  fXZViewer->SetGLViewer(glxzviewer, glxzviewer->GetFrame());
  fXZViewer->IncDenyDestroy();
  eve->GetViewers()->AddElement(fXZViewer);
  glxzviewer->CurrentCamera().Setup(tpcBox, kFALSE);
  
  fXZLabel = new TGLabel(left, new TGHotString("x[cm]"));
  left->AddFrame(fXZLabel, new TGLayoutHints(kLHintsBottom | kLHintsCenterX));

  fYLabel = new TGLabel(right, new TGHotString("y[cm]"));
  right->AddFrame(fYLabel, new TGLayoutHints(kLHintsLeft));
  
  auto glyzviewer = new TGLEmbeddedViewer(right, fPad);
  glyzviewer->SetOrthoCamera(TGLViewer::kCameraOrthoZOY, 2.5, 0, centerArr, 0, 0);
  glyzviewer->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
  glyzviewer->CurrentCamera().SetFixDefCenterVec(centerArr[0], centerArr[1], centerArr[2]);
  right->AddFrame(glyzviewer->GetFrame(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fYZViewer = new TEveViewer("OrthoYZViewer", ("Orthographic YZ Viewer for TPC "+std::to_string(fTPCID.TPC)+" Cryostat "+std::to_string(fTPCID.Cryostat)).c_str());
  fYZViewer->SetGLViewer(glyzviewer, glyzviewer->GetFrame());
  fYZViewer->IncDenyDestroy();
  eve->GetViewers()->AddElement(fYZViewer);
  glyzviewer->CurrentCamera().Setup(tpcBox, kFALSE);

  fYZLabel = new TGLabel(right, new TGHotString("z[cm]"));
  right->AddFrame(fYZLabel, new TGLayoutHints(kLHintsBottom | kLHintsCenterX));

  fMain->MapSubwindows();
  fMain->Resize();
  fMain->MapWindow();

  browse->StopEmbedding("TPCOrtho");
  browse->ShowCloseTab(kTRUE);

  browse->SanitizeTabCounts();
}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::analyze(art::Event const & e)
{
  fXZViewer->GetGLViewer()->DeleteOverlayAnnotations();
  fYZViewer->GetGLViewer()->DeleteOverlayAnnotations();

  auto xzoverlay = fXZViewer->GetGLViewer()->GetCameraOverlay();
  xzoverlay->SetShowOrthographic(kTRUE);
  xzoverlay->SetOrthographicMode(TGLCameraOverlay::kGridBack);

  auto yzoverlay = fYZViewer->GetGLViewer()->GetCameraOverlay();
  yzoverlay->SetShowOrthographic(kTRUE);
  yzoverlay->SetOrthographicMode(TGLCameraOverlay::kGridBack);

  // Implementation of required member function here.
  mf::LogWarning("TPCOrthoViewerMaker") << "Making scenes in TPCOrthoViewerMaker.\n";
  int null[] = {(SingleAlgHolder<ALGS>::get()->makeEvent(e), 0)...};
  (void)null; //convince the compiler that null is used without generating any code


}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::beginJob()
{
  // Implementation of optional member function here.
  int null[] = {0, (SingleAlgHolder<ALGS>::get()->initialize(), 0)..., 0};
  (void)null; //convince the compiler that null is used without generating any code

  auto glxzview = fXZViewer->GetGLViewer(); //Temporary set up of GLViewer.  This should be fcl-controlled in the future...
  glxzview->UseLightColorSet();
  //glxzview->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);

  auto glyzview = fYZViewer->GetGLViewer(); //Temporary set up of GLViewer.  This should be fcl-controlled in the future...
  glyzview->UseLightColorSet();
  //glyzview->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::beginRun(art::Run const & r)
{
  // Implementation of optional member function here.
}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::beginSubRun(art::SubRun const & sr)
{
  // Implementation of optional member function here.
  TEveSceneList* scenes = new TEveSceneList("OrthoScenes");
  int null[] = { (scenes->AddElement(SingleAlgHolder<ALGS>::get()->makeGlobal()), 0)... };
  (void)null;

  for(auto scene = scenes->BeginChildren(); scene != scenes->EndChildren(); ++scene)
  {
    fXZViewer->AddScene((TEveScene*)(*scene));
    fYZViewer->AddScene((TEveScene*)(*scene));
  }
}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::endJob()
{
  // Implementation of optional member function here.
}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::endRun(art::Run const & r)
{
  // Implementation of optional member function here.
}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::endSubRun(art::SubRun const & sr)
{
  // Implementation of optional member function here.
}

template <std::string& NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::reconfigure(fhicl::ParameterSet const & p)
{
  size_t TPCNum = p.get<size_t>("TPCNum");
  size_t CryoNum = p.get<size_t>("CryoNum");
  fTPCID = geo::TPCID(TPCNum, CryoNum);
  if(!art::ServiceHandle<geo::Geometry>()->HasTPC(fTPCID)) throw cet::exception("BadTPCID") << "Got bad TPCID for TPC " << TPCNum << " and cryostat " 
                                                                                            << CryoNum << " in TPCOrthoViewerMaker.\n";

  int null[] = {0, (SingleAlgHolder<ALGS>::get()->reconfigure(p), 0)..., 0};
  (void)null; //convince the compiler that null is used without generating any code
}

template <std::string& NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::respondToCloseInputFile(art::FileBlock const & fb)
{
  // Implementation of optional member function here.
}

template <std::string& NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::respondToCloseOutputFiles(art::FileBlock const & fb)
{
  // Implementation of optional member function here.
}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::respondToOpenInputFile(art::FileBlock const & fb)
{
  // Implementation of optional member function here.
}

template <std::string &NAME, class ...ALGS>
void eved::TPCOrthoViewerMaker<NAME, ALGS...>::respondToOpenOutputFiles(art::FileBlock const & fb)
{
  // Implementation of optional member function here.
}

//Call this in a separate file to make your viewer using this template: 
//#typedef eved::TPCOrthoViewerMaker<ALG1, ALG2, ALG3> YourViewerName
//DEFINE_ART_MODULE(YourViewerName)
