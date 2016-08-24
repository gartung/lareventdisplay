// author: Andrew Olivier
// date: 6/18/2016
// brief: Editor class for TEveWvFrmDigit.  Will draw usual TEveGeoShape editor with added histogram of waveform

//LArSoft includes
#include "lareventdisplay/EveDisplay/VisElem/TEveWvFrmDigit.h"

//ROOT includes
#include "TEveShapeEditor.h"
#include "TGFrame.h"
#include "TH1D.h"
#include "TRootEmbeddedCanvas.h"

#ifndef VIS_TEVEWVFRMDIGITEDITOR
#define VIS_TEVEWVFRMDIGITEDITOR

namespace vis
{
  class TEveWvFrmDigitEditor: public TEveShapeEditor
  {
    private:
      TEveWvFrmDigitEditor(const TEveWvFrmDigitEditor&); //not implemented
      TEveWvFrmDigitEditor& operator=(const TEveWvFrmDigitEditor&); //not implemented

    public:
      TEveWvFrmDigitEditor(const TGWindow* p = nullptr, int width = 170, int height = 30, UInt_t options = kChildFrame, Pixel_t back = GetDefaultFrameBackground());
      virtual ~TEveWvFrmDigitEditor(); //we need our own destructor here because of the pointers we own
      virtual void SetModel(TObject* obj); //we need to override this to take TEveWvFrmDigits instead of TEveShapes
      
    protected:
      TRootEmbeddedCanvas* fEmCan; //embedded canvas for waveform histogram
      TH1D*                fHisto; //waveform histogram      
      TEveWvFrmDigit*      fModel; //The digit we are editing

    public:
      ClassDef(TEveWvFrmDigitEditor, 1);
  };
}

#endif //VIS_TEVEWVFRMDIGITEDITOR
