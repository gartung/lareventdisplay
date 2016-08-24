// author: Andrew Olivier
// date: 6/18/2016
// brief: Implmentation of an editor for TEveWvFrmDigit that will draw the waveform as a histogram

//LArSoft includes
#include "lareventdisplay/EveDisplay/VisElem/TEveWvFrmDigitEditor.h"

//ROOT includes
#include "TGLayout.h"
#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"

//Framework includes
#include "cetlib/exception.h"

ClassImp(vis::TEveWvFrmDigitEditor)

namespace vis
{
  TEveWvFrmDigitEditor::TEveWvFrmDigitEditor(const TGWindow* p, int width, int height, UInt_t options, 
                                             Pixel_t back): TEveShapeEditor(p, width, height, options, back), fEmCan(nullptr), 
                                             fHisto(nullptr), fModel(nullptr)
  {
    MakeTitle("TEveWvFrmDigit");
    
    fEmCan = new TRootEmbeddedCanvas(0, this, 500, 170);
    fEmCan->SetName("Waveform");
    auto can = fEmCan->GetCanvas();

    can->Modified();
    can->Update();
 
    AddFrame(fEmCan, new TGLayoutHints(kLHintsBottom | kLHintsExpandY));
  }

  TEveWvFrmDigitEditor::~TEveWvFrmDigitEditor()
  {
    //We do not own fModel, so don't delete it!
    if(fHisto != nullptr) { delete fHisto; fHisto = nullptr; }
    if(fEmCan != nullptr) { delete fEmCan; fEmCan = nullptr; } 
  }

  void TEveWvFrmDigitEditor::SetModel(TObject* obj)
  {
    fModel = dynamic_cast<vis::TEveWvFrmDigit*>(obj);

    if(fModel == nullptr) throw cet::exception("WvFrmEditor") << "Could not cast object to TEveWvFrmDigit in TEveWvFrmDigitEditor::SetModel().\n";
    //It occurs to me that it might not be the best of ideas to have this class depend on cetlib.  If you have a problem because of this, feel free to use 
    //something like std::exception or a ROOT class instead.  
    //auto shape = (TEveGeoShape*)(obj); //I don't think we actually need this
    
    TEveShapeEditor::SetModel(obj); //shape);

    auto can = fEmCan->GetCanvas();
    can->cd();

    auto wvfrm = fModel->fWvFrm;
    if(fHisto != nullptr) 
    {
      delete fHisto; 
      fHisto = nullptr; 
    }

    size_t size = 1;
    if(wvfrm.size() >0) size = wvfrm.size();
    fHisto = new TH1D("Waveform", ";Time[TDC Ticks];Amplitude;Waveform Histogram;", size, 0, size);

    for(size_t it = 0; it < wvfrm.size(); ++it)
    {
      fHisto->Fill(it, wvfrm[it]);
    }

    fHisto->Draw();

    can->Modified();
    can->Update();
  }
}
