////////////////////////////////////////////////////////////////////////
/// \file SimulationDrawingOption_plugin.cc
///
/// \version $Id: RecoDrawingOptions_plugin.cc,v 1.1 2010/11/11 18:11:22 p-novaart Exp $
/// \author  brebel@fnal.gov

// Framework includes

/// LArSoft includes
#include "lareventdisplay/EventDisplay/SimulationDrawingOptions.h"

#include <iostream>

namespace evd {

  //......................................................................
  SimulationDrawingOptions::SimulationDrawingOptions(fhicl::ParameterSet const& pset, 
						     art::ActivityRegistry& /* reg */)
  {
    this->reconfigure(pset);
  }
  
  //......................................................................
  SimulationDrawingOptions::~SimulationDrawingOptions() 
  {
  }

  //......................................................................
  void SimulationDrawingOptions::reconfigure(fhicl::ParameterSet const& pset)
  {
    fShowMCTruthText         = pset.get< bool        >("ShowMCTruthText",         true);
    fShowMCTruthVectors      = pset.get< bool        >("ShowMCTruthVectors",      true);
    fShowMCTruthTrajectories = pset.get< bool        >("ShowMCTruthTrajectories", true);
    fShowMCTruthColors       = pset.get< bool        >("ShowMCTruthColors",       true);
    fShowMCTruthFullSize     = pset.get< bool        >("ShowMCTruthFullSize",     true);
    fMinEnergyDeposition     = pset.get< double      >("MinimumEnergyDeposition"      );
    fG4ModuleLabel           = pset.get< std::string >("G4ModuleLabel"                ); 
  }
  
}

namespace evd {

  DEFINE_ART_SERVICE(SimulationDrawingOptions)

} // namespace evd
////////////////////////////////////////////////////////////////////////
