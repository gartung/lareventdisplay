// \file BasicViewerFrame.h
// \brief Declares a class for a MainFrame that holds a single TEveViewer.  More GUI features could be added here. 
// \author Andrew Olivier aolivier@ur.rochester.edu

//ART includes
#ifndef __CINT__
#ifndef __ROOTCLING__
#include "fhiclcpp/ParameterSet.h"
#endif
#endif

//ROOT includes
#include "TGFrame.h"
#include "TEveViewer.h"

//c++ includes
#include <vector>

#ifndef GUI_BASICVIEWERFRAME_H
#define GUI_BASICVIEWERFRAME_H

namespace eved
{
  class BasicViewerFrame: public TGMainFrame
  {
    public:
      BasicViewerFrame(const TGWindow* parent);
      //I don't think we need a destructor since we don't own any resources.  I'd use a smart pointer if I owned resources anyway.   
 
      #ifndef __CINT__
      #ifndef __ROOTCLING__
      void reconfigure(fhicl::ParameterSet const& p); 
      #endif
      #endif
      void DeleteAnnotations();
      void RequestScenes();
 
    private:
      TEveViewer* fViewer; //owned by an EVE class
      std::vector<std::string> fSceneNames; //Names of TEveScenes we will request when RequestScenes() is called

    public:
      ClassDef(BasicViewerFrame, 0);
  };
}

#endif
