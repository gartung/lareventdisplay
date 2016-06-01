// \file: OrthoViewer_module.cc
// \author: Andrew Olivier
// \brief: module to produce a 3D view of the TPC(s)

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/OrthoSceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/OrthoGeoMaker.h"
#include "lareventdisplay/EveDisplay/Algs/GenericViewerMaker.h"
#include "lareventdisplay/EveDisplay/Algs/CopySceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/TrackMakerInt.h"
#include "lareventdisplay/EveDisplay/Algs/SimPartMakerInt.h"
#include "lardata/RecoBase/Track.h"
#include "SimulationBase/MCParticle.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h"

//ROOT includes

#ifndef EVED_OrthoVIEWER_MODULE_CC
#define EVED_OrthoVIEWER_MODULE_CC

//To view visualization elements in the EVE-based event display, we need to make a viewer.  
//To make a basic 3D viewer, I have provided the GenericViewerMaker module template.  
//GenericViewerMaker takes as parameters a name and a list of algorithms with which to make scenes.  
//Each TEveScene shows a group of related visualization elements that can be controlled together.  

namespace
{
  std::string Ortho("Ortho");
  std::string Reco("OrthoReco");
  std::string Truth("OrthoTruth");
}
namespace eved
{
  //typedef eved::GenericViewerMaker<Ortho, eved::GenericSceneMaker<reco, recob::Track, simb::MCParticle>, eved::SimpleGeoMaker> OrthoViewer;
  //test of scene with multiple data products

  //typedef eved::GenericViewerMaker<Ortho, eved::OrthoSceneMaker<reco, recob::Track>, eved::OrthoSceneMaker<truth, simb::MCParticle>, eved::OrthoGeoMaker> OrthoViewer;

  typedef eved::GenericViewerMaker<Ortho, eved::CopySceneMaker<Reco>, eved::CopySceneMaker<Truth>, eved::OrthoGeoMaker> OrthoViewer;
  
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

DEFINE_ART_MODULE(eved::OrthoViewer)

//We have made a 3D viewer called Ortho that will have the following scenes: 
//1.) A scene named "reco" that will draw all recob::Tracks in each event as TEveTracks
//2.) A simplified 3D view of the geometry.  
//To use this view with the EVE-based event display, include an analyzer module named "OrthoViewer" in the end_paths of your fcl file that also has the 
//"EveDisplay" service configured.  
#endif
