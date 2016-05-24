// \file: TPC3DViewer_module.cc
// \author: Andrew Olivier
// \brief: module to produce a 3D view of the TPC(s)

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/GenericSceneMaker.h"
#include "lareventdisplay/EveDisplay/Algs/SimpleGeoMaker.h"
#include "lareventdisplay/EveDisplay/Algs/GenericViewerMaker.h"
#include "lardata/RecoBase/Track.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h"

//ROOT includes

#ifndef EVED_TPC3DVIEWER_MODULE_CC
#define EVED_TPC3DVIEWER_MODULE_CC

//To view visualization elements in the EVE-based event display, we need to make a viewer.  
//To make a basic 3D viewer, I have provided the GenericViewerMaker module template.  
//GenericViewerMaker takes as parameters a name and a list of algorithms with which to make scenes.  
//Each TEveScene shows a group of related visualization elements that can be controlled together.  

namespace
{
  std::string reco("reco");
  std::string TPC3D("TPC3D");
}
namespace eved
{
  typedef eved::GenericViewerMaker<TPC3D, eved::GenericSceneMaker<reco, recob::Track>, eved::SimpleGeoMaker> TPC3DViewer;
  
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

DEFINE_ART_MODULE(eved::TPC3DViewer)

//We have made a 3D viewer called TPC3D that will have the following scenes: 
//1.) A scene named "reco" that will draw all recob::Tracks in each event as TEveTracks
//2.) A simplified 3D view of the geometry.  
//To use this view with the EVE-based event display, include an analyzer module named "TPC3DViewer" in the end_paths of your fcl file that also has the 
//"EveDisplay" service configured.  
#endif
