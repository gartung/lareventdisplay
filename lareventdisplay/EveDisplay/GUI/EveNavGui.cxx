//Implementation of GUI for event navigation
#include "lareventdisplay/EveDisplay/GUI/EveNavGui.h"
#include <string>

//ROOT includes
#include "TGFrame.h"
#include "TEveBrowser.h"
#include "TGButton.h"
#include "TGLayout.h"
#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TGPicture.h"
#include "TGLayout.h"

#include "lareventdisplay/EveDisplay/GUI/SRNavState.h"

/*namespace 
{
  //Anonymous helper function to find the top-level window given any window in a hierarchy
  const TGWindow* GetTop(const TGWindow* child)
  {
    auto parent = child->GetParent();
    if(parent != nullptr) return GetTop(parent);
    else return child; 
  }
}*/

eved::EveNavGui::EveNavGui(TGWindow* parent): TGCompositeFrame(parent)
{
  //TEveBrowser derives from TGMainFrame indirectly, so we can use this browser as our mainframe
  //browse->StartEmbedding(TRootBrowser::kLeft);

  //TGMainFrame* main = new TGMainFrame(gClient->GetRoot(), 1000, 600);
  //main->SetWindowName("Nav GUI");
  //main->SetCleanup(kDeepCleanup);

  auto hints = new TGLayoutHints(kLHintsExpandX);
  fButtonBar = new TGVerticalFrame(this); //browse);

  fPrev = new TGTextButton(fButtonBar, "Previous", 500);
  fPrev->SetToolTipText("Go to previous event");
  fPrev->Connect("Clicked()", "eved::EveNavGui", this, "Prev()");
  fButtonBar->AddFrame(fPrev, hints);

  fNext = new TGTextButton(fButtonBar, "Next", 500);
  fNext->SetToolTipText("Go to next event");
  fNext->Connect("Clicked()", "eved::EveNavGui", this, "Next()");
  fButtonBar->AddFrame(fNext, hints);

  fReload = new TGTextButton(fButtonBar, "Reload", 500);
  fReload->SetToolTipText("Reload event");
  fReload->Connect("Clicked()", "eved::EveNavGui", this, "Reload()");
  fButtonBar->AddFrame(fReload, hints);

  fGoTo = new TGTextButton(fButtonBar, "GoTo", 500);
  fGoTo->SetToolTipText("Goto event");
  fGoTo->Connect("Clicked()", "eved::EveNavGui", this, "GoTo()");
  fButtonBar->AddFrame(fGoTo, hints);

  fRunLabel = new TGLabel(fButtonBar, new TGHotString("[Run]"));
  fButtonBar->AddFrame(fRunLabel, hints);  

  fRun = new TGTextEntry(fButtonBar, new TGTextBuffer(128));
  fRun->Connect("ReturnPressed()", "eved::EveNavGui", this, "GoTo()");
  fRun->Resize(500, 20);
  fButtonBar->AddFrame(fRun, hints);

  fSubRunLabel = new TGLabel(fButtonBar, new TGHotString("[SubRun]"));
  fButtonBar->AddFrame(fSubRunLabel, hints);

  fSubRun = new TGTextEntry(fButtonBar, new TGTextBuffer(128));
  fSubRun->Connect("ReturnPressed()", "eved::EveNavGui", this, "GoTo()");
  fSubRun->Resize(500, 20);
  fButtonBar->AddFrame(fSubRun, hints);

  fEventLabel = new TGLabel(fButtonBar, new TGHotString("[Event]"));      
  fButtonBar->AddFrame(fEventLabel, hints);

  fEvent = new TGTextEntry(fButtonBar, new TGTextBuffer(128));
  fEvent->Connect("ReturnPressed()", "eved::EveNavGui", this, "GoTo()");
  fEvent->Resize(500, 20);
  fButtonBar->AddFrame(fEvent, hints);

  fPrint = new TGTextButton(fButtonBar, "Print", 500);
  fPrint->SetToolTipText("Print Window");
  fPrint->Connect("Clicked()", "eved::EveNavGui", this, "Print()");
  fButtonBar->AddFrame(fPrint, hints);

  fPrintLabel = new TGLabel(fButtonBar, new TGHotString("File for Printing"));
  fButtonBar->AddFrame(fPrintLabel, hints);

  fPrintFile = new TGTextEntry(fButtonBar, new TGTextBuffer(128));
  fPrintFile->SetText("");
  fPrintFile->Connect("ReturnPressed()", "eved::EveNavGui", this, "Print()");
  fPrintFile->Resize(500, 20);
  fButtonBar->AddFrame(fPrintFile, hints);

  this->AddFrame(fButtonBar, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsTop));
  this->MapSubwindows();
  this->Resize();
  this->MapWindow();

  /*browse->AddFrame(fButtonBar);
  browse->MapSubwindows();
  browse->Resize();
  browse->MapWindow();*/

  //browse->StopEmbedding();
  //browse->SetTabTitle("Event Control", 0);
}

eved::EveNavGui::~EveNavGui() //TODO: Change all of these to std::unique_ptr.   
{
  delete fRun; fRun = nullptr;
  delete fEvent; fEvent = nullptr;
  delete fSubRun; fSubRun = nullptr;
  delete fPrint; fPrint = nullptr;
  delete fRunLabel; fRunLabel = nullptr;
  delete fSubRunLabel; fSubRunLabel = nullptr;
  delete fEventLabel; fEventLabel = nullptr;
  delete fPrintLabel; fPrintLabel = nullptr;
  delete fGoTo; fGoTo = nullptr;
  delete fReload; fReload = nullptr;
  delete fNext; fNext = nullptr;
  delete fPrev; fPrev = nullptr;
  delete fPrintFile; fPrintFile = nullptr;
  delete fButtonBar; fButtonBar = nullptr;
}

void eved::EveNavGui::Print()
{
  //First, find the top-level window
  auto top = GetMainFrame(); //::GetTop(GetParent()); //This returns the SYSTEM window!  

  //Next, get the name of the file we are printing to.  Construct a default name if the user has not provided one.
  std::string name = fPrintFile->GetText();
  if(name == "") 
  {
    name = "eved_Run"+std::string(fRun->GetText())+"_Subrun"+std::string(fSubRun->GetText())+"_Event"+std::string(fEvent->GetText())+".png";
    //TODO: It might be helpful if we could get the current file name from ART...  
  }
  top->SaveAs(name.c_str()); //See TGObject documentation for an explanation.  ROOT infers a file type based on the file name.  I think you will get a 
                             //c++ file if you don't provide an extension!  
}

void eved::EveNavGui::Next()
{
  SRNavState::Set(sr_nav_states::kNEXT_EVENT);
}

void eved::EveNavGui::Prev()
{
  SRNavState::Set(sr_nav_states::kPREV_EVENT);
}

void eved::EveNavGui::Reload()
{
  SRNavState::Set(sr_nav_states::kRELOAD_EVENT);
}

void eved::EveNavGui::GoTo()
{
  size_t run = atoi(fRun->GetText());
  size_t event = atoi(fEvent->GetText());
  int subrun = atoi(fSubRun->GetText());
  SRNavState::SetTarget(run, subrun, event);
  SRNavState::Set(sr_nav_states::kGOTO_EVENT);
}

void eved::EveNavGui::SetRunSubRunEvent(size_t run, size_t subrun, size_t event)
{
  fRun->SetText(std::to_string(run).c_str());
  fEvent->SetText(std::to_string(event).c_str());
  fSubRun->SetText(std::to_string(subrun).c_str());
}
