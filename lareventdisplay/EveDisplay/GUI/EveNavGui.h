//Created by Andrew Olivier on 3/13/2016
//aoliv23@lsu.edu
//\brief: GUI element to handle event navigation
//Inspired by evdb::ButtonBar and ROOT's ali_esd.C example

#include "TQObject.h"
#include "RQ_OBJECT.h"
#include "TGTextEntry.h"
#include "TGButton.h"
#include "TEveBrowser.h"
#include "TGFrame.h"

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

class eved::EveNavGui: public TGCompositeFrame
{
  //RQ_OBJECT("eved::EveNavGui") //needed for ROOT to recognize that this object can interact with GUI buttons
  //Above is no longer needed if we are a TGCompositeFrame
  
  public:
    EveNavGui(TGWindow* parent);
    virtual ~EveNavGui();

  public:
    //Event navigation functions; called by GUI buttons
    void Prev();
    void Next();
    void Reload();
    void GoTo();

    //Additional control functions; called by GUI buttons
    void Print(); //Print entire display window to a graphics file 

    void SetRunSubRunEvent(size_t run, size_t subrun, size_t event);

  private:
    //GUI elements from ROOT
    TGVerticalFrame* fButtonBar;
    //TGLayoutHints* fLayout;
    
    //Navigation buttons
    TGTextButton* fNext;
    TGTextButton* fPrev;
    TGTextButton* fReload;
    TGTextButton* fGoTo;

    //Control buttons
    TGTextButton* fPrint;

    TGTextEntry*  fEvent;
    TGTextEntry*  fRun;
    TGTextEntry* fSubRun; 
    
    TGTextEntry* fPrintFile;

    TGLabel* fRunLabel;
    TGLabel* fSubRunLabel;
    TGLabel* fEventLabel;

    TGLabel* fPrintLabel;

  ClassDef(EveNavGui, 0); //Needed for ROOT to recognize this class
};

#endif
