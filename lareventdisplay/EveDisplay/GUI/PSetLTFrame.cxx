// \File: PSetLTFrame.cxx
// \Brief: Implementation of a frame that holds a list tree of fhicl parameter set values.  Users can edit parameter set keys' values.  
// \Author: Andrew Olivier aoliv23@lsu.edu

//Framework includes
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/make_ParameterSet.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/PSetLTFrame.h"

//ROOT includes
#include "TGLayout.h"
#include "TGText.h"

//One-off helper function defined in the anonymous namespace to avoid polluting the global namespace.  Move to a new file by itself if you want to
//reuse this function
namespace
{
  void CheckParents(TGListTreeItem* item)
  {
    item->CheckItem();
    auto parent = item->GetParent();
    if(parent != nullptr) CheckParents(item->GetParent());
  }
}

ClassImp(eved::PSetLTFrame)

eved::PSetLTFrame::PSetLTFrame(const TGWindow* parent): TGCompositeFrame(parent, 200, 800), fSelected(nullptr)
{
  //List tree of FHICL parameter sets.  Hopefully makes structure of nested parameter sets easier to understand.  
  //First, we need a TGCanvas for some reason...
  fLTCanvas.reset(new TGCanvas(this));
  fListTree.reset(new TGListTree(fLTCanvas->GetViewPort()));
  fListTree->SetCanvas(fLTCanvas.get());
  fListTree->Associate(this);
  fLTCanvas->SetContainer(fListTree.get());
  AddFrame(fLTCanvas.get(), new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY));

  //Add entries to group modules and services
  fListTree->AddRoot("services");
  fListTree->AddRoot("modules");

  //Add fListTree to this frame.  Replaced by adding fLTCanvas above.
  //AddFrame(fListTree.get(), new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY));

  //Connect DoubleClicked(TGListTreeItem*) signal from fListTree to this frame's EditPSet(TGListTreeItem*).  This 
  //lets users double-click a list tree entry to move its "value" (which could be another pset) into the text edit widget defined below.  
  fListTree->Connect("DoubleClicked(TGListTreeItem*, Int_t)", "eved::PSetLTFrame", this, "EditPSet(TGListTreeItem*, Int_t)");

  //Text entry field for editing parameter values
  fTextEditor.reset(new TGTextEdit(this)); //TODO: Maybe include a welcome message and instructions here.  
  AddFrame(fTextEditor.get(), new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY));

  //Button to copy text from fTextEditor into fSelected.  
  fUpdateButton.reset(new TGTextButton(this, "&Update"));
  fUpdateButton->SetToolTipText("Update the fhicl parameter currently selected to the value in the text editor");
  fUpdateButton->SetHeight(50);
  AddFrame(fUpdateButton.get(), new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 5, 5, 3, 4));

  //Connect Clicked() from fUpdateButton to our Update() function.  
  fUpdateButton->Connect("Clicked()", "eved::PSetLTFrame", this, "Update()");

  //Usual TGCompositeFrame stuff
  Layout();
  MapSubwindows();
  MapWindow();
}

void eved::PSetLTFrame::AddPSet(const fhicl::ParameterSet& pset)
{
  TGListTreeItem* parent = nullptr;
  std::string psetName;
  if(pset.get_if_present<std::string>("module_label", psetName)) 
  {
    parent = fListTree->FindItemByPathname("modules");
    if(fListTree->FindItemByPathname(("modules/"+psetName).c_str()) != nullptr)
    {
      mf::LogWarning("DuplicatedPSet") << "Not adding duplicated ParameterSet named " << psetName << " to parameter set editor.\n";
      return;
    }
  }
  else
  {
    try
    {
      psetName = pset.get<std::string>("service_type"); //Use this method so that we will throw if this pset is neither a module nor a service.
                                                          //TODO: Catch exceptions here and add a useful message.  I need to find out what kind of 
                                                          //      exception fhicl throws.
      parent = fListTree->FindItemByPathname("services");

      if(fListTree->FindItemByPathname(("services/"+psetName).c_str()) != nullptr)
      {
        mf::LogWarning("DuplicatedPSet") << "Not adding duplicated ParameterSet named " << psetName << " to parameter set editor.\n";
        return;
      }
    }
    catch(fhicl::exception& e)
    {
      LOG_ERROR("PSetLTFrame") << "Caught exception \n\n" << e << "\n\nwhile looking for key service_type in AddPSet().  So, the following parameter set "
                               << "will not appear in the PSetLTFrame:\n\n" << pset.to_indented_string() << "\n";
      return;
      //throw e;
    }
  }

  //if(parent == nullptr) throw cet::exception("LTNotFound") << "In PSetLTFrame::AddPSet, failed to find pset with name " << psetName << ".\n";
  
  fListTree->AddItem(parent, AddKey(psetName, pset));
}

TGListTreeItemStd* eved::PSetLTFrame::AddKey(const std::string& key, const fhicl::ParameterSet& value)
{
  if(value.is_empty()) return AddKey(key, "{}"); //empty parameter set

  auto newKey = AddKey(key, "Users should never see this!"); //value.to_indented_string()); //This value should never be used, so set a 
                                                             //message for debugging and consider removing later.  

  auto children = value.get_names();
  for(const auto& child: children)
  {
    try
    {
      std::string cVal;
      if(value.get_if_present(child, cVal)) mf::LogWarning("PSetLTFrame") << "Value for key " << child << " in parameter set " << key << " is " 
                                                                          << cVal << ".\n";
    }
    catch(cet::exception& e) {} //I just want to print some information.  Parameters that are vectors shouldn't really matter.

    if(value.is_key_to_table(child))
    {
      mf::LogWarning("PSetLTFrame") << "In AddKey, adding child parameter set named " << child << " to parent " << key << "\n";
      fListTree->AddItem(newKey, AddKey(child, value.get<fhicl::ParameterSet>(child)));
    }
    else //FHICL won't convert std::vector<T> to std::string at the time of writing.  So, catch exceptions and try different strategies for getting 
         //parameters.  I will try to stringify parameters here for now, but I think I should move this messiness elsewhere eventually.
    {
      std::string newValue;
      try
      {
        newValue = value.get<std::string>(child);
      }
      catch(cet::exception& e)
      {
        //The following code could deftinitely be optimized if it turns out to matter.  
        try
        {
          auto valVec = value.get<std::vector<std::string>>(child);  
          newValue.append("[");
          for(const auto& val: valVec)
          {
            newValue.append(val);
            newValue.append(", ");
          }
          auto commaPos = newValue.rfind(", ");
          if(commaPos != std::string::npos) newValue.erase(newValue.rfind(", "), 2); //TODO: There is obviously a better way to do this
          newValue.append("]");
        }
        catch(cet::exception& e)
        {
          newValue.clear();
          newValue.append("[");
          auto dblValVec = value.get<std::vector<std::vector<std::string>>>(child);
          for(const auto& valV: dblValVec)
          {
            newValue.append("[");
            for(const auto& val: valV)
            {
              newValue.append(val);
              newValue.append(", ");
            }
            auto commaPos = newValue.rfind(", ");
            if(commaPos != std::string::npos) newValue.erase(newValue.rfind(", "), 2);
            newValue.append("] ");
            newValue.append(", ");
          }
          auto commaPos = newValue.rfind(", ");
          if(commaPos != std::string::npos) newValue.erase(newValue.rfind(", "), 2);
          newValue.append("]");
          //This is the deepest nesting I have seen in fhicl files so far.  Ideally, there should be a type-independent way to do this, but I don't know 
          //what it is at the moment and doubt that it will ever be used.  If you needed really deeply-nested vectors, I think you could do this with a 
          //recursive template as long as you are careful to put a max limit on recursion!  
        }
      } //Supposedly, fhicl converts nested vectors into std::vector<std::string> for us.  If only it did that for regular vectors...
        //@^ Apparently, this is not true...
      fListTree->AddItem(newKey, AddKey(child, newValue));
    }
  }

  return newKey;
}

TGListTreeItemStd* eved::PSetLTFrame::AddKey(const std::string& key, const std::string& value)
{
  auto newKey = new TGListTreeItemStd();
  newKey->CheckItem(kFALSE); //TGListTreeItemStds are checked by default!  
  newKey->SetText(key.c_str());
  mf::LogWarning("PSetLTFrame") << "In AddKey(), setting value " << value << " for key " << key << "\n";
  newKey->SetUserData(new std::string(value), kFALSE); //ROOT tries to delete user data as a TObject, so don't rely on the list tree to delete things! 
  return newKey;
}

void eved::PSetLTFrame::EditPSet(TGListTreeItem* item, Int_t /*button*/)
{
  std::string name = item->GetText();
  //Build a ParameterSet for display out of this element and its children.
  
  fhicl::ParameterSet pset; //Based on the comments in fhicl::intermediate_table, I think I can use a ParameterSet here.  We will try to 
                            //interpret it later anyway, so I don't see any harm in this step.  I just hope this doesn't slow us down.  
  SetKey(item, pset, false); 

  fTextEditor->SetText(new TGText(pset.to_indented_string().c_str()));
  fTextEditor->Update();
  fSelected = item;
  //TODO: Add some kind of protection against deleting changes accidentally when we set a new pset to edit. 
}

bool eved::PSetLTFrame::UpdatePSet(fhicl::ParameterSet& pset) //We might CHANGE pset here!  pset should be a module or a service pset.  
{
  std::string name;
  TGListTreeItem* found = nullptr;
  if(pset.get_if_present("module_label", name)) found = fListTree->FindItemByPathname(("modules/"+name).c_str());
  else if(pset.get_if_present("service_type", name)) found = fListTree->FindItemByPathname(("services/"+name).c_str());
  else return false;
  //else throw cet::exception("BadPset") << "In PSetLTFrame::UpdatePSet, got parameter set \n" << pset.to_indented_string() << "\n that contains neither "
  //                                     << "module_label nor service_provider.  So, we cannot get its name!  UpdatePSet expects to be given a "
  //                                     << "parameter set for either a service or a module.\n";
  //The blank parameter set strikes again!  Do nothing if we can't identify the type of this parameter set.

  if(found != nullptr)
  {
    bool changed = found->IsChecked();
    if(changed)
    {
      mf::LogWarning("PSetLTFrame") << "Changing parameterSet " << name << " from\n" << pset.to_indented_string() << "\nin UpdatePSet().\n";
      for(auto child = found->GetFirstChild(); child != nullptr; child = child->GetNextSibling())
      {
        SetKey(child, pset, true);
      }
    }

    mf::LogWarning("PSetLTFrame") << "Parameter set " << name << " is now \n" << pset.to_indented_string() << "\n";

    return changed;
  }

  return false;
}

void eved::PSetLTFrame::SetKey(TGListTreeItem* key, fhicl::ParameterSet& pset, bool reqChanged)
{
  //Looping over children is based on convention buried in the source code of TGListTreeItem as of the time of writing.  
  //fNextsibling (return by NextSibling()) and fFirstchild (returned by FirstChild()) are both set to 0 (interpretted as nullptr)
  //by default.  So, we can tell if a list tree item has children by checking whether FirstChild() returns nullptr.  Similarly, 
  //we can loop over children by looping until NextSibling() returns nullptr.

  if(reqChanged && !key->IsChecked()) return; //All keys that have been changed or have children that have been changed will be checked by convention. 
                                //So, don't do anything if we haven't changed this key.  

  auto first = key->GetFirstChild();
  if(first != nullptr) //If key has children.  
  {
    fhicl::ParameterSet newPset; //Build a new ParameterSet to be added to pset.  We will replace existing psets.  

    auto child = first;
    SetKey(child, newPset, reqChanged);
    while((child = child->GetNextSibling())) //As I recall, pointer assignment will return false when assigning to nullptr.  This just 
                                             //seems to be how ROOT expects us to do things.  
    {
      SetKey(child, newPset, reqChanged);
    }

    pset.put_or_replace(key->GetText(), newPset);
  }
  else
  {
    auto data = (std::string*)(key->GetUserData());
    if(data == nullptr) data = new std::string("No Data!");
    if(data->compare("{}") == 0) pset.put_or_replace(key->GetText(), fhicl::ParameterSet()); //Support for empty parameter sets.  Otherwise, fhiclcpp inserts 
                                                                                              //"{}" with the quotation marks into pset.  Then, this key can no 
                                                                                              //longer be cast to fhicl::ParameterSet. std::string::compare()
                                                                                              //returns 0 for equal strings.  
    else 
    {
      //Remove \n from data.
      for(auto pos = data->find("\n"); pos != std::string::npos; pos = data->find("\n")) data->erase(pos, 2);
      pset.put_or_replace(key->GetText(), *data);
    }
  }
}

void eved::PSetLTFrame::Update()
{
  if(fSelected == nullptr) return; //don't do anything if nothing is selected
  mf::LogWarning("PSetLTFrame") << "PSetLTFrame::Update() was called with fSelected named " << fSelected->GetText() << "\n";
  std::string value = fTextEditor->GetText()->AsString().Data(); 
  
  //Note: By default, we only include the names of keys in list tree items that are parameter sets themselves since the list tree is 
  //      used to encode the structure of a parameter set.  The following code allows for the use of "ParameterSet literals" via 
  //      ROOT's text editor.
  //Make sure that this is a valid parameter set.  Steps to build a new ParameterSet were taken from evdb::ServiceTable in nutools.  

  //if(fSelected->GetFirstChild() == nullptr) //TODO: I think this prevents us from providing a parameter set where there wasn't one before.  Is that ideal?
  if((value.find('{') == std::string::npos) || (value.find('}') == std::string::npos)) //Support for empty parameter sets.  
                                                                                       //Should I be worried about escaped '{' characters?  
  {
    //fhicl::ParameterSet::to_indented_string() returns a parameter set with its label, so extract the parameter or parameter set's value.  
    auto pos = value.find(':');
    value = value.substr(pos+2, std::string::npos);

    //fhiclcpp adds quotation marks around parameters in to_indented_string().  So, we need to extract the leading and trailing quotes.
    value.erase(value.find('"'), 1);
    value.erase(value.rfind('"'), 1);

    //We also seem to accumulate some "\n" characters that cause problems for strings that need to be converted to numbers later.  
    //After trying to compile with no \ns, I think I need to make sure that we don't get more than one in a row
    for(auto escPos = value.find("\n\n"); escPos != std::string::npos; escPos = value.find("\n\n")) value.erase(escPos, 2);

    fSelected->SetUserData(new std::string(value)); //.substr(pos, value.length()-pos)));
    fSelected->CheckItem();
    ::CheckParents(fSelected);
    fSelected->CheckAllChildren();
  }
  else
  {
    try
    {
      fhicl::ParameterSet pset;
      fhicl::intermediate_table itable;
      for(auto escPos = value.find("\n\n"); escPos != std::string::npos; escPos = value.find("\n\n")) value.erase(escPos, 2);
      fhicl::parse_document(value, itable);
      fhicl::make_ParameterSet(itable, pset);
      
      //Now that we have a valid parameter set, build a new list tree fragment.  Right now, we are not updating parents.  
      //Replace fSelected with a new key that links to its rebuilt children
      auto newItem = AddKey(fSelected->GetText(), pset.get<fhicl::ParameterSet>(fSelected->GetText())); 
      auto parent = fSelected->GetParent();
      fListTree->DeleteItem(fSelected);
      fSelected = newItem;
      fListTree->AddItem(parent, newItem);
      
      ::CheckParents(fSelected); //Implement convention that checked items have either been changed themselves or have children that have been changed.  
                                 //Avoids need to loop over parameter sets that do not need to be changed.  
                                 //Relies on NOT displaying check boxes by list tree items!  This is the default for TGListTreeItemStd as of the 
                                 //time of writing.  
      fSelected->CheckAllChildren(); //Mark new children as changed.
      //fSelected->SetUserData(new std::string("Users should never see this!")); //new std::string(pset.to_indented_string())); //For now, we never use this 
                                                                               //value.  So, include it for debugging and consider removing later.  
    }
    catch(fhicl::exception& e)
    {
      LOG_ERROR("PSetLTFrame") << "Caught exception \n\n" << e << "\n\nwhen parsing new parameter set for key " << fSelected->GetText() << ".  "
                               << "Keeping the value of " << fSelected->GetText() << " as \n\n" << value << "\n\nand returning to editor.\n";
    }
  } 
}
