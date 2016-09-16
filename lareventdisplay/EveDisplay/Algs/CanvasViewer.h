////////////////////////////////////////////////////////////////////////
// Class:       CanvasViewer
// Module Type: analyzer
// File:        CanvasViewer_module.cc
// Brief:       Template for modules that make histograms on an event-by-event basis for viewing in the EVE-based event display
//
// Generated at Thu May 26 12:50:32 2016 by Andrew Olivier using artmod
// from cetpkgsupport v1_10_01.
////////////////////////////////////////////////////////////////////////

//ART includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//ROOT includes
#include "TCanvas.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"

//c++ includes
#include <memory> //for std::unique_ptr

namespace eved
{

  //Template parameters:
  //ALG: An algorithm to draw histograms that has the following functions:
  //a.) void reconfigure(const fhicl::ParameterSet& p) //Get configuration parameters.  This will be called after the default constructor for your 
  //                                                   //algorithm is called
  //b.) void draw(const art::Event& e) //Fill and draw histogram for each event
  //c.) TCanvas* beginJob() //returns canvas so it can be embedded
  //
  //NAME: The name this tab will have in the Eve display
  //This module will handle embedding your histogram and any other GUI configuration tasks found necessary.  
  
  template <class ALG, std::string& NAME>
  class CanvasViewer : public art::EDAnalyzer {
  public:
    explicit CanvasViewer(fhicl::ParameterSet const & p);
    // The destructor generated by the compiler is fine for classes
    // without bare pointers or other resource use.
  
    // Plugins should not be copied or assigned.
    CanvasViewer(CanvasViewer const &) = delete;
    CanvasViewer(CanvasViewer &&) = delete;
    CanvasViewer & operator = (CanvasViewer const &) = delete;
    CanvasViewer & operator = (CanvasViewer &&) = delete;
  
    // Required functions.
    void analyze(art::Event const & e) override;
  
    // Selected optional functions.
    void beginJob() override;
    //void beginRun(art::Run const & r) override;
    //void beginSubRun(art::SubRun const & sr) override;
    //void endJob() override;
    //void endRun(art::Run const & r) override;
    //void endSubRun(art::SubRun const & sr) override;
    void reconfigure(fhicl::ParameterSet const & p) override;
    //void respondToCloseInputFile(art::FileBlock const & fb) override;
    //void respondToCloseOutputFiles(art::FileBlock const & fb) override;
    //void respondToOpenInputFile(art::FileBlock const & fb) override;
    //void respondToOpenOutputFiles(art::FileBlock const & fb) override;
  
  private:
  
    // Declare member data here.
    std::unique_ptr<ALG> fAlg;
  };
  
  template <class ALG, std::string& NAME>  
  eved::CanvasViewer<ALG, NAME>::CanvasViewer(fhicl::ParameterSet const & p)
    :
    EDAnalyzer(p), fAlg(new ALG())
   // More initializers here.
  {
    reconfigure(p);
  }
  
  template <class ALG, std::string& NAME>
  void eved::CanvasViewer<ALG, NAME>::analyze(art::Event const & e)
  {
    // Implementation of required member function here.
    fAlg->draw(e);
  }
  
  template <class ALG, std::string& NAME>
  void eved::CanvasViewer<ALG, NAME>::beginJob()
  {
    // Implementation of optional member function here.
    art::ServiceHandle<eved::EveDisplay> eve;
  
    auto browse = eve->getEve()->GetBrowser();  
  
    browse->ShowCloseTab(kFALSE);
    browse->StartEmbedding();
    TCanvas* can = fAlg->beginJob();
    can->Update();
    can->Draw();
    browse->AppendPad();
    browse->StopEmbedding(NAME.c_str());
    browse->ShowCloseTab(kTRUE);
  }
  
  /*void eved::CanvasViewer::beginRun(art::Run const & r)
  {
    // Implementation of optional member function here.
  }*/
  
  /*void eved::CanvasViewer::beginSubRun(art::SubRun const & sr)
  {
    // Implementation of optional member function here.
  }*/
  
  /*void eved::CanvasViewer::endJob()
  {
    // Implementation of optional member function here.
  }*/
  
  /*void eved::CanvasViewer::endRun(art::Run const & r)
  {
    // Implementation of optional member function here.
  }*/
  
  /*void eved::CanvasViewer::endSubRun(art::SubRun const & sr)
  {
    // Implementation of optional member function here.
  }*/
  
  template <class ALG, std::string& NAME>
  void eved::CanvasViewer<ALG, NAME>::reconfigure(fhicl::ParameterSet const & p)
  {
    // Implementation of optional member function here.
    fAlg->reconfigure(p);
  }
  
  
  /*void eved::CanvasViewer::respondToCloseInputFile(art::FileBlock const & fb)
  {
    // Implementation of optional member function here.
  }*/
  
  /*void eved::CanvasViewer::respondToCloseOutputFiles(art::FileBlock const & fb)
  {
    // Implementation of optional member function here.
  }*/
  
  /*void eved::CanvasViewer::respondToOpenInputFile(art::FileBlock const & fb)
  {
    // Implementation of optional member function here.
  }*/
  
  /*void eved::CanvasViewer::respondToOpenOutputFiles(art::FileBlock const & fb)
  {
    // Implementation of optional member function here.
  }*/
}

//DEFINE_ART_MODULE(eved::CanvasViewer)
//Call this in your ALG file instead: DEFINE_ART_MODULE(eved::CanvasViewer<ALG>)
