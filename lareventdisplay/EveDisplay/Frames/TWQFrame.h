// \file: TWQFrame.h
// \brief: A graphics frame that holds multiple TEveViewers in 2D mode to show the event in wire plane views
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#ifndef __CINT__
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/TPCGeo.h"
#endif

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
#include "TH1D.h"
#include "TRootEmbeddedCanvas.h"
#include "TEveBoxSet.h"
#include "TObject.h"

#ifndef FRAMES_TWQFRAME_H
#define FRAMES_TWQFRAME_H

namespace eved
{
  class TWQFrame;
}

namespace eved
{
  //Helper class for the frame we will create for each TPC view
  class OrthoViewerFrame: public TGCompositeFrame //Composite frame in case I want to add labels or controls later
  {
    public:
      #ifndef __CINT__
      OrthoViewerFrame(TWQFrame* parent, geo::PlaneID plane, unsigned int width, unsigned int height);
      #endif
      OrthoViewerFrame(TWQFrame* parent, unsigned int width, unsigned int height);

      #ifndef __CINT__
      void reconfigure(fhicl::ParameterSet const& p);
      #endif
      void RequestScenes();
      void DeleteAnnotations();
      #ifndef __CINT__
      void SetPlane(geo::PlaneID plane); //recalculate area to be shown based on new plane
      #endif

    private:
      TEveViewer* fViewer; //owned by EVE
      std::vector<std::string> fSceneNames; //list of scenes we will request from EVE
      //geo::PlaneID fPlane; //To help with figuring out zoom settings 

    public:
      ClassDef(OrthoViewerFrame, 1);
  };

  class TWQFrame: public TGCompositeFrame //TGMainFrame
  {
    public:
      TWQFrame(const TGWindow* parent);

      #ifndef __CINT__
      void reconfigure(fhicl::ParameterSet const& p); 
      #endif
      void DeleteAnnotations();
      void RequestScenes();
      void RespondToClicked(TObject* obj); //Respond to TGLViewer's Clicked() signal
 
    private:
      std::vector<eved::OrthoViewerFrame*> fViewerFrames; //the number of these we need is determined at runtime from the geometry service
      TH1D* fWireHisto; //Displays the waveform of the selected wire
      TRootEmbeddedCanvas* fWireCan; //Frame to hold a canvas for fWireHisto
      void FillWireHisto(TEveBoxSet* box); //Fill fWireHisto when a wire is selected.  Called by RespondToClicked()
      //TGLabel* fXLabel;
      //TGLabel* fXZLabel;
      //TGLabel* fYLabel;
      //TGLabel* fYZLabel;
      //TEvePad* fPad;
      //Eventually, I want to add a histogram that can show a particular wire's waveform.  I have to get wire drawing working first!  

    public:
      ClassDef(TWQFrame, 1);
  };
}

#endif
