// \File: PSetLTFrame.h
// \Brief: Frame to display all fhicl::ParameterSets from this ART job in a list tree (like the file browser in TBrowser).  
// \Author: Andrew Olivier aoliv23@lsu.edu

//ROOT includes
#include "TGFrame.h"
#include "TGListTree.h"
#include "TGTextEdit.h"
#include "TGButton.h"
#include "TGCanvas.h"

//c++ includes
#include <memory>

#ifndef __CINT__ //CINT and BOOST don't mix well!
//Framework includes
#include "fhiclcpp/ParameterSet.h"
#endif

#ifndef GUI_PSETLTFRAME_H
#define GUI_PSETLTFRAME_H

namespace eved
{
  class PSetLTFrame: public TGCompositeFrame 
  {
    public:
      PSetLTFrame(const TGWindow* parent); //constructor
      //As long as we use smart pointers like unique_ptr for our members, we shouldn't need to define our own destructor.
 
      #ifndef __CINT__     
      void AddPSet(const fhicl::ParameterSet& pset); //Add a new parameter set to the list tree.  
                                                                   //Returns the list tree item for this parameter set.
                                                                   //Looks for a key named "module_label" (which ART *should* 
                                                                   //insert into the pset for modules' configurations according 
                                                                   //to the Mu2e online documentation) to determine whether 
                                                                   //pset configures a module or a service.  
      #endif

      void EditPSet(TGListTreeItem* item, Int_t); //To be connected to signal "Clicked(TGListTreeItem*)" from TGListTree.  Edit the value assigned to the key 
                                                  //in item.  The value in item's GetUserData() should show up in fTextEntry when this is called.  

      #ifndef __CINT__
      bool UpdatePSet(fhicl::ParameterSet& pset); //Updates pset based on which parameters are "checked".  Users should not be able to check list tree 
                                                  //entries.  Returns true if pset has been updated and false otherwise.  
      #endif

      void Update(); //When fUpdateButton is pressed, copy the text from fTextEditor into fSelected and restructure the list tree if needed.  

    private:
      #ifndef __CINT__
      std::unique_ptr<TGListTree> fListTree; //List tree that shows fcl parameter sets.
      std::unique_ptr<TGTextEdit> fTextEditor; //Text editor widget to edit the selected FHICL parameter.
      std::unique_ptr<TGTextButton> fUpdateButton; //Button clicked to update fSelected with the text in fTextEditor
      std::unique_ptr<TGCanvas> fLTCanvas; //Canvas just for the list tree

      TGListTreeItem* fSelected; //List tree item currently being edited.  Observer pointer.  DO NOT DELETE.  
      #endif
 
      #ifndef __CINT__
      TGListTreeItemStd* AddKey(const std::string& key, const fhicl::ParameterSet& value);
      TGListTreeItemStd* AddKey(const std::string& key, const std::string& value); 
                                                                                                //Add a new item to the list tree as a child of parent 
                                                                                                //with parameter named key mapped to value.  Returns the 
                                                                                                //TGListTreeItem created to hold this key-value pair.

      void SetKey(TGListTreeItem* key, fhicl::ParameterSet& pset, bool reqChange); //Set parameter set's value for key's GetText() to key's GetUserData() 
                                                                                   //(assuming user data is a std::string* by convention in AddKey()).  
                                                                                   //Then, recursively do the same for all of key's children.  Do nothing
                                                                                   //if reqChange is true and key is not checked.  
      #endif

    public:
      ClassDef(PSetLTFrame, 1);
  };
}

#endif
