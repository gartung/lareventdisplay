// \file: RawDigitTestViewer_module.cc
// \author: Andrew Olivier
// \brief: module to produce 3D orthographic views of a TPC

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/GenericSceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/CopySceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TPCOnlyGeoMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TPCOrthoViewerMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/SimPartMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/RawDigitMakerInt.h"
#include "lardata/RecoBase/Track.h"
#include "SimulationBase/MCParticle.h"
#include "lardata/RawData/RawDigit.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h"

//ROOT includes

#ifndef EVED_RAWDIGITTESTVIEWER_MODULE_CC
#define EVED_RAWDIGITTESTVIEWER_MODULE_CC

//To view visualization elements in the EVE-based event display, we need to make a viewer.  
//To make a basic 3D viewer, I have provided the GenericViewerMaker module template.  
//GenericViewerMaker takes as parameters a name and a list of algorithms with which to make scenes.  
//Each TEveScene shows a group of related visualization elements that can be controlled together.  

namespace
{
  std::string rawL("raw");
  std::string reco("reco");
  std::string truth("truth");
  std::string TPCOrtho("TPCOrtho");
}
namespace eved
{
  //typedef eved::GenericViewerMaker<TPC3D, eved::GenericSceneMaker<reco, recob::Track, simb::MCParticle>, eved::SimpleGeoMaker> TPC3DViewer;
  //test of scene with multiple data products

  typedef eved::TPCOrthoViewerMaker<TPCOrtho, eved::GenericSceneMaker<truth, simb::MCParticle>, eved::CopySceneMaker<reco>, eved::GenericSceneMaker<rawL, raw::RawDigit>, eved::TPCOnlyGeoMaker> RawDigitTestViewer;

  //typedef eved::TPCOrthoViewerMaker<TPCOrtho, eved::GenericSceneMaker<raw, raw::RawDigit>, eved::CopySceneMaker<reco>, eved::CopySceneMaker<truth>, eved::TPCOnlyGeoMaker> RawDigitTestViewer;
  
  //The above class templates are defined in headers under LArSoft includes. The purpose of these templates is to simplify writing custom event displays for 
  //the vast majority of users.  If you want to:
  //1.) Write an event display with a new selection of products from existing modules, you just need to write one line of template ugliness like this.  
  //2.) Write an event display with one or more new visualization elements:
  //   a.) For each new visualization element, write an algorithm that produces that element from a LArSoft data product you want to visualize.  Then, call 
  //       one line of template ugliness in each algorithm file to make a module that produces your new visualization element.  Your new element must be 
  //       derived from TEveElement.  
  //   b.) Write one additional line of template ugliness like this to view your objects in 3D.  
  //I hope to write class templates like GenericViewerMaker for each of EVE's axis modes and a 2D histogram mode in the near future.    
}

DEFINE_ART_MODULE(eved::RawDigitTestViewer)

//We have made a 3D viewer called TPC3D that will have the following scenes: 
//1.) A scene named "reco" that will draw all recob::Tracks in each event as TEveTracks
//2.) A simplified 3D view of the geometry.  
//To use this view with the EVE-based event display, include an analyzer module named "TPC3DViewer" in the end_paths of your fcl file that also has the 
//"EveDisplay" service configured.  
#endif
