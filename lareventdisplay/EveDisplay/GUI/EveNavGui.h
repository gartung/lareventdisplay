//Created by Andrew Olivier on 3/13/2016
//aoliv23@lsu.edu
//\brief: GUI element to handle event navigation
//Inspired by evdb::ButtonBar and ROOT's ali_esd.C example

#include "TQObject.h"
#include "RQ_OBJECT.h"
#include "TGTextEntry.h"
#include "TGButton.h"
#include "TEveBrowser.h"

#ifndef EVED_EVENAVGUI
#define EVED_EVENAVGUI

class TGMainFrame;
class TGHorizontalFrame;
class TGPictureButton;
class TGLayoutHints;
class TGLabel;

namespace eved
{
  class EveNavGui;
}

class eved::EveNavGui: public TObject
{
  RQ_OBJECT("eved::EveNavGui") //needed for ROOT to recognize that this object can interact with GUI buttons
  
  public:
    EveNavGui(TEveBrowser* browse);
    virtual ~EveNavGui();

  public:
    //Event navigation functions; called by GUI buttons
    void Prev();
    void Next();
    void Reload();
    void GoTo();

    void SetRunEvent(int run, int event);

  private:
    //GUI elements from ROOT
    TGHorizontalFrame* fButtonBar;
    //TGLayoutHints* fLayout;
    
    //Navigation buttons
    TGTextButton* fNext;
    TGTextButton* fPrev;
    TGTextButton* fReload;
    TGTextButton* fGoTo;

    TGTextEntry*  fEvent;
    TGTextEntry*  fRun;
    //TGTextEntry* fSubRun; //need to implement this later

    TGLabel* fRunEventLabel;

  ClassDef(EveNavGui, 0); //Needed for ROOT to recognize this class
};

#endif
