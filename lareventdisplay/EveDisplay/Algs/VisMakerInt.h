// \file: VisMakerInt
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Interface template for an algorithm that maps data products to visualization elements. This allows runtime-reconfigurable event 
//         display algorithms for the EVE-based event display

//ART includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Persistency/Common/Ptr.h"
#include "art/Persistency/Common/PtrVector.h"
#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
//CINT is evil!
#ifndef __CINT__
#include "art/Framework/Services/Registry/ServiceMacros.h"

//ROOT includes
#include "TEveElement.h"

#ifndef EVED_VISMAKERINT_H
#define EVED_VISMAKERINT_H

namespace eved
{
//template parameter PROD: a LArSoft data product

  template <class PROD>
  class VisMakerInt
  {
    public: 
      VisMakerInt(const fhicl::ParameterSet& p, art::ActivityRegistry &reg) { reconfigure(p); }
      virtual ~VisMakerInt() = default;
  
      virtual void reconfigure(const fhicl::ParameterSet& p)
      {
        fDataLabel = p.get<std::string>("DataLabel");
      }

      virtual TEveElement* MakeVis(const PROD& prod) = 0; //Must be implemented by user
                                                          //Note that you can, and almost always should, return a pointer to something interesting derived from
                                                          //TEveElement.  

      virtual bool SelectDataProduct(const PROD& prod) { return true; } //Optional function to refuse to draw data products

      virtual const std::string GetLabel() { return fDataLabel; }

    private:
      std::string fDataLabel; //label of the module that made the data products we want to use
  };
}

//For each data product we want to use, we need a line like this: (I think in a separate file)
//DECLARE_ART_SERVICE_INTERFACE(eved::VisMakerInt<PROD>, LEGACY)
 
#endif //CINT is evil!
#endif //include guard
