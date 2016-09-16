// \file: DEdxVsResRangeView_module.cc
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Viewer that draws a plot of dE/dx vs. residual range for all calorimetry objects in each event.  

//ART includes
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Core/ModuleMacros.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/CanvasViewer.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"

//ROOT includes
#include "TH2D.h"
#include "TCanvas.h"

namespace eved
{
  class DEdxAlg //algorithm class to be used as HistoViewer's ALG template parameter
  {
    public:
      DEdxAlg(): fCanvas(nullptr), fHisto(nullptr) {} //initialization will be done in reconfigure instead
      ~DEdxAlg() = default;
      void draw(const art::Event& e);
      void reconfigure(const fhicl::ParameterSet& p);
      TCanvas* beginJob(); 
    
    private:
      std::string fCaloLabel; //The label of the calorimetry objects we will draw
      size_t      fPlane; //The plane we will use
      double      fdEdxMin; 
      double      fdEdxMax;
      size_t         fNdEdxBins;
      double      fResRangeMin;
      double      fResRangeMax;
      size_t         fNResRangeBins;
      TCanvas*    fCanvas;//the canvas we will drawn on
      TH2D*       fHisto; //The histogram we will draw.  
  };

  void DEdxAlg::draw(const art::Event& e)
  {
    //Interesting note: If you omitt the following line, your histogram will accumulate points from all events.  I haven't thought of a reason not to just use 
    //TFileService for this yet (which I think you could view under the "File" tab), but it works if you find a use for it.  
    fHisto->Reset();

    art::Handle<std::vector<anab::Calorimetry>> caloHand;
    e.getByLabel(fCaloLabel, caloHand);

    mf::LogWarning("dEdxDraw") << "Got " << caloHand->size() << " anab::Calorimetry objects to draw.\n";

    for(const auto& calo: (*caloHand))
    {
      if(calo.PlaneID().Plane == fPlane)
      {
        for(size_t ptIt = 0; ptIt < calo.dEdx().size(); ++ptIt)
        {
          fHisto->Fill(calo.ResidualRange().at(ptIt), calo.dEdx().at(ptIt), 1.0);
        }
      }
    }
    fHisto->Draw("colz");
    fCanvas->Update();
    fCanvas->Draw("colz");
  }

  void DEdxAlg::reconfigure(const fhicl::ParameterSet& p)
  {
    const auto pset = p.get<fhicl::ParameterSet>("dEdxDrawAlg");
    fCaloLabel = pset.get<std::string>("CaloLabel");
    fPlane = pset.get<size_t>("Plane");
    fdEdxMin = pset.get<double>("dEdxMin");
    fdEdxMax = pset.get<double>("dEdxMax");
    fNdEdxBins = pset.get<size_t>("NdEdxBins");
    fResRangeMin = pset.get<double>("ResRangeMin");
    fResRangeMax = pset.get<double>("ResRangeMax");
    fNResRangeBins = pset.get<size_t>("NResRangeBins");
  }

  TCanvas* DEdxAlg::beginJob()
  {
    fCanvas = new TCanvas("dedxvsresrangecan", "frac{dE}{dx} vs. Residual Range Canvas");
    fHisto = new TH2D("dedxvsresrange", ";Residual Range[cm];\frac{dE}{dx}[MeV/cm];;\frac{dE}{dx} vs. Residual Range;", 
                      fNResRangeBins, fResRangeMin, fResRangeMax, fNdEdxBins, fdEdxMin, fdEdxMax);
    return fCanvas;
  }

}

namespace
{
  std::string dEdx("dE/dx");
}

namespace eved
{
  typedef CanvasViewer<eved::DEdxAlg, dEdx> DEdxViewer;
}

DEFINE_ART_MODULE(eved::DEdxViewer)
