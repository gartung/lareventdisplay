// \file: HitMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing recob::Hit in the EVE-based event display.  Implements HitMakerInt.h. 

//Framework includes
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/HitMaker.h"
#include "lareventdisplay/EveDisplay/Algs/HitMakerInt.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lareventdisplay/EventDisplay/Style.h"
#include "larcore/Geometry/Geometry.h"
#include "lareventdisplay/EveDisplay/Config/CoordConfig.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
//#include "lareventdisplay/EveDisplay/VisElem/TEveWvFrmDigit.h"

//ROOT includes
#include "TEveBox.h" //visualization class used here to represent raw wires
#include "TEveRGBAPalette.h" //Should we make our own class that derives from this to be shared by all TWQ objects?
#include "TEveElement.h"
#include "TGeoNode.h"
#include "TGTab.h"
#include "TEveTrans.h"

#ifndef EVED_WIREMAKER_SERVICE_CC
#define EVED_WIREMAKER_SERVICE_CC

//Note that we will place wires in TWQ space.  So, you will get really weird results if you use these TEveBoxes with objects placed in the "normal" 
//LArSoft 3D coordinate system.  

namespace eved
{
    void eved::HitMaker::reconfigure(const fhicl::ParameterSet& p)
    {
      fSigMin = p.get<short>("MinSignal");
      fSigMax = p.get<short>("MaxSignal");
      fSigCut = p.get<short>("ADCCut");
      fTransp = p.get<double>("Transp");
    }

    TEveElement* eved::HitMaker::MakeVis(const recob::Hit& digit) //implementation of pure virtual function
    {
      art::ServiceHandle<geo::Geometry> geom;

      auto wires = geom->ChannelToWire(digit.Channel());
      if(wires.size() > 1) throw cet::exception("Disambig") << "Got " << wires.size() << " wires for channel " << digit.Channel() << ".  You will need to "
                                                            << "implement your own digit drawer that handles disambiguation.\n";
  
      auto wireID = wires[0]; //I think the geometry service would have thrown long before this if we couldn't find any wires at all. 
      auto wire = geom->Wire(wireID);

      std::string name("Channel ");
      name += std::to_string(digit.Channel());
      std::string title("recob::Hit\n"+name);

      //Conversion factor to make all of the wires in the current plane fill a standard sized TWQ viewer window
      art::ServiceHandle<eved::CoordConfig> coordConfig;
      const auto& tpc = geom->TPC(wireID.Cryostat, wireID.TPC);
      const auto& plane = geom->Plane(wireID.Plane, wireID.TPC, wireID.Cryostat);
      detinfo::DetectorProperties const* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

      art::ServiceHandle<eved::EveDisplay> eved;
      auto twqWindow = eved->getEve()->GetBrowser()->GetTabRight()->GetContainer();
      auto width = twqWindow->GetWidth();
      auto height = twqWindow->GetHeight();

      double conversion = detprop->NumberTimeSamples()*width/height*(tpc.Nplanes()+0.5)/plane.Nwires(); 
      mf::LogWarning("HitMaker") << "In HitMaker, conversion*nHits is " << conversion*plane.Nwires() << "\n";

      auto retVal = new TEveBox(name.c_str(), title.c_str());

      //Get a color mapping for charge from EVE for now.  In the future, we should probably follow the original event display's example and handle this via a 
      //service.  
      auto palette = new TEveRGBAPalette(fSigMin, fSigMax);
      //retVal->SetPalette(palette);

      //Set up box vertices.  conversion is the thickness of one wire in the current TWQ viewer.  See TEveBox docum for an explanation.
      double xHThick = 1./2.; //x coordinate should never be visible to the user, but we have to set it
      double yHThick = (digit.EndTick()-digit.StartTick())/2.;
      double zHThick = conversion/2.;     

      //Box bottom
      retVal->SetVertex(0, -xHThick, -yHThick, -zHThick);
      retVal->SetVertex(1, xHThick, -yHThick, -zHThick);
      retVal->SetVertex(2, xHThick, -yHThick, zHThick);
      retVal->SetVertex(3, -xHThick, -yHThick, zHThick);
      
      //Box top
      retVal->SetVertex(4, -xHThick, yHThick, -zHThick);
      retVal->SetVertex(5, xHThick, yHThick, -zHThick);
      retVal->SetVertex(6, xHThick, yHThick, zHThick);
      retVal->SetVertex(7, -xHThick, yHThick, zHThick);

      //Set box color from integral of charge
      auto colorArr = palette->ColorFromValue(digit.Integral());
      retVal->SetMainColorRGB(colorArr[0], colorArr[1], colorArr[2]);
      retVal->SetFillColor(retVal->GetMainColor());
      retVal->SetLineColor(retVal->GetMainColor());
      retVal->SetDrawFrame(kFALSE);

      //Set center position from wire and peak time
      retVal->RefMainTrans().SetPos(1., digit.PeakTime(), wireID.Wire*conversion);

      retVal->SetMainTransparency(fTransp); //100); //The one transparency function to rule them all!  This is the magic function that lets other objects be drawn over the 
                                        //digits' boxes.  

      retVal->SetPickable(kTRUE);
      retVal->VizDB_Insert(retVal->GetName(), kTRUE, kTRUE);

      return (TEveElement*)retVal;
    } 
    
}

DEFINE_ART_SERVICE_INTERFACE_IMPL(eved::HitMaker, eved::VisMakerInt<recob::Hit>)

#endif
