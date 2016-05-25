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

#include "lareventdisplay/EveDisplay/GUI/SRNavState.h"

eved::EveNavGui::EveNavGui(TEveBrowser* browse)
{
  //TEveBrowser derives from TGMainFrame indirectly, so we can use this browser as our mainframe
  browse->StartEmbedding(TRootBrowser::kLeft);

  TGMainFrame* main = new TGMainFrame(gClient->GetRoot(), 1000, 600);
  main->SetWindowName("Nav GUI");
  main->SetCleanup(kDeepCleanup);

  fButtonBar = new TGHorizontalFrame(main); //browse);

  fPrev = new TGTextButton(fButtonBar, "Previous", 150);
  fPrev->SetToolTipText("Go to previous event");
  fPrev->Connect("Clicked()", "eved::EveNavGui", this, "Prev()");
  fButtonBar->AddFrame(fPrev);

  fNext = new TGTextButton(fButtonBar, "Next", 150);
  fNext->SetToolTipText("Go to next event");
  fNext->Connect("Clicked()", "eved::EveNavGui", this, "Next()");
  fButtonBar->AddFrame(fNext);

  fReload = new TGTextButton(fButtonBar, "Reload", 150);
  fReload->SetToolTipText("Reload event");
  fReload->Connect("Clicked()", "eved::EveNavGui", this, "Reload()");
  fButtonBar->AddFrame(fReload);

  fGoTo = new TGTextButton(fButtonBar, "GoTo", 150);
  fGoTo->SetToolTipText("Goto event");
  fGoTo->Connect("Clicked()", "eved::EveNavGui", this, "GoTo()");
  fButtonBar->AddFrame(fGoTo);

  fRun = new TGTextEntry(fButtonBar, new TGTextBuffer(128));
  fRun->Connect("ReturnPressed()", "eved::EveNavGui", this, "GoTo()");
  fRun->Resize(75, 20);
  fButtonBar->AddFrame(fRun);

  fEvent = new TGTextEntry(fButtonBar, new TGTextBuffer(128));
  fEvent->Connect("ReturnPressed()", "eved::EveNavGui", this, "GoTo()");
  fEvent->Resize(75, 20);
  fButtonBar->AddFrame(fEvent);

  fRunEventLabel = new TGLabel(fButtonBar, new TGHotString("[Run/Event]"));
  fButtonBar->AddFrame(fRunEventLabel);
 
  //fSubRun here similar to fEvent above

  main->AddFrame(fButtonBar);
  main->MapSubwindows();
  main->Resize();
  main->MapWindow();

  /*browse->AddFrame(fButtonBar);
  browse->MapSubwindows();
  browse->Resize();
  browse->MapWindow();*/

  browse->StopEmbedding();
  browse->SetTabTitle("Event Control", 0);
}

eved::EveNavGui::~EveNavGui()
{
  delete fRun; fRun = nullptr;
  delete fEvent; fEvent = nullptr;
  delete fRunEventLabel; fRunEventLabel = nullptr;
  delete fGoTo; fGoTo = nullptr;
  delete fReload; fReload = nullptr;
  delete fNext; fNext = nullptr;
  delete fPrev; fPrev = nullptr;
  delete fButtonBar; fButtonBar = nullptr;
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
  //int subrun = atoi(fSubRun->GetText());
  //Here, you will probably have to rewrite NavState to handle subruns
  SRNavState::SetTarget(run, 0, event);
  SRNavState::Set(sr_nav_states::kGOTO_EVENT);
}

void eved::EveNavGui::SetRunEvent(int run, int event)
{
  char runtxt[128];
  char evttxt[128];
  
  sprintf(runtxt, "%d", run);
  sprintf(evttxt, "%d", event); //should probably replace these with "modern" versions
  fRun->SetText(runtxt);
  fEvent->SetText(evttxt);
}
