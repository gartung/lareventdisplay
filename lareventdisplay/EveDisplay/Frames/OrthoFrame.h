// \file: OrthoFrame.h
// \brief: A graphics frame that holds two TEveViewers in 2D mode to show the event in 2 2D views
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

#ifndef FRAMES_ORTHOFRAME_H
#define FRAMES_ORTHOFRAME_H

namespace eved
{
  class OrthoFrame: public TGMainFrame
  {
    public:
      OrthoFrame(const TGWindow* parent);

      #ifndef __CINT__
      void reconfigure(fhicl::ParameterSet const& p); 
      #endif
      void DeleteAnnotations();
      void RequestScenes();
 
    private:
      TEveViewer* fXZViewer; //owned by an EVE class
      TEveViewer* fYZViewer; 
      std::vector<std::string> fSceneNames; //Both viewers get the same list of scenes
      TGSplitFrame* fSplit;
      TGLabel* fXLabel;
      TGLabel* fXZLabel;
      TGLabel* fYLabel;
      TGLabel* fYZLabel;
      TEvePad* fPad;
      #ifndef __CINT__
      //geo::TPCID fTPCID; //Number of the TPC and cryostat we will draw.  Checked for validity against the geometry service
      //Get this from the GeoConfig service instead
      #endif

    public:
      ClassDef(OrthoFrame, 1);
  };
}

#endif
