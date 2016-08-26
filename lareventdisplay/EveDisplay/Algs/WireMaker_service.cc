// \file: WireMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing recob::Wire in the EVE-based event display.  Implements WireMakerInt.h. 

//Framework includes
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/WireMaker.h"
#include "lareventdisplay/EveDisplay/Algs/WireMakerInt.h"
#include "lardata/RawData/RawDigit.h"
#include "lareventdisplay/EventDisplay/Style.h"
#include "larcore/Geometry/Geometry.h"
#include "lareventdisplay/EveDisplay/Config/CoordConfig.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
//#include "lareventdisplay/EveDisplay/VisElem/TEveWvFrmDigit.h"

//ROOT includes
#include "TEveBoxSet.h" //visualization class used here to represent raw wires
#include "TEveRGBAPalette.h" //Should we make our own class that derives from this to be shared by all TWQ objects?
#include "TEveElement.h"
#include "TGeoNode.h"
#include "TGTab.h"

#ifndef EVED_WIREMAKER_SERVICE_CC
#define EVED_WIREMAKER_SERVICE_CC

//Note that we will place wires in TWQ space.  So, you will get really weird results if you use these TEveBoxSets with objects placed in the "normal" 
//LArSoft 3D coordinate system.  

namespace eved
{
    void eved::WireMaker::reconfigure(const fhicl::ParameterSet& p)
    {
      fSigMin = p.get<short>("MinSignal");
      fSigMax = p.get<short>("MaxSignal");
      fSigCut = p.get<short>("ADCCut");
      fTransp = p.get<double>("Transp");
    }

    TEveElement* eved::WireMaker::MakeVis(const recob::Wire& digit) //implementation of pure virtual function
    {
      art::ServiceHandle<geo::Geometry> geom;

      auto wires = geom->ChannelToWire(digit.Channel());
      if(wires.size() > 1) throw cet::exception("Disambig") << "Got " << wires.size() << " wires for channel " << digit.Channel() << ".  You will need to "
                                                            << "implement your own digit drawer that handles disambiguation.\n";
  
      auto wireID = wires[0]; //I think the geometry service would have thrown long before this if we couldn't find any wires at all. 
      auto wire = geom->Wire(wireID);
      auto wvfrm = digit.Signal();

      std::string name("Channel ");
      name += std::to_string(digit.Channel());
      std::string title("recob::Wire\n"+name+"\nADC size: "+std::to_string(wvfrm.size()));

      /*auto retVal = new TEveBoxSet(name.c_str(), title.c_str());
      retVal->Reset(TEveBoxSet::kBT_AABoxFixedDim, kFALSE, wvfrm.size());
      retVal->SetDefHeight(1); //1 TDC tick
      retVal->SetDefDepth(1); //this dimension should not be used, but we need to set it anyway

      //Get a color mapping for charge from EVE for now.  In the future, we should probably follow the original event display's example and handle this via a 
      //service.  
      auto palette = new TEveRGBAPalette(fSigMin, fSigMax);
      retVal->SetPalette(palette);*/

      //Conversion factor to make all of the wires in the current plane fill a standard sized TWQ viewer window
      art::ServiceHandle<eved::CoordConfig> coordConfig;
      const auto& tpc = geom->TPC(wireID.Cryostat, wireID.TPC);
      const auto& plane = geom->Plane(wireID.Plane, wireID.TPC, wireID.Cryostat);
      //double conversion = wvfrm.size()*(coordConfig->GetFrameHeight())*(tpc.Nplanes())/plane.Nwires()/coordConfig->GetFrameWidth();
      detinfo::DetectorProperties const* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

      art::ServiceHandle<eved::EveDisplay> eved;
      auto twqWindow = eved->getEve()->GetBrowser()->GetTabRight()->GetContainer();
      auto width = twqWindow->GetWidth();
      auto height = twqWindow->GetHeight();

      double conversion = detprop->NumberTimeSamples()*width/height*(tpc.Nplanes()+0.5)/plane.Nwires(); //detprop->NumberTimeSamples()*(coordConfig->GetFrameWidth())/plane.Nwires()/coordConfig->GetFrameHeight()*(tpc.Nplanes()+1);
      mf::LogWarning("WireMaker") << "In WireMaker, conversion*nWires is " << conversion*plane.Nwires() << "\n";

      auto retVal = new TEveBoxSet(name.c_str(), title.c_str());
      retVal->SetDefHeight(1); //1 TDC tick
      retVal->SetDefDepth(conversion); 
      //retVal->SetDefWidth(conversion); //this dimension should not be used, but we need to set it anyway
      retVal->Reset(TEveBoxSet::kBT_AABoxFixedDim, kFALSE, wvfrm.size());

      //Get a color mapping for charge from EVE for now.  In the future, we should probably follow the original event display's example and handle this via a 
      //service.  
      auto palette = new TEveRGBAPalette(fSigMin, fSigMax);
      retVal->SetPalette(palette);

      for(size_t tdc = 0; tdc < wvfrm.size(); ++tdc) //Make a box for each ADC entry that is color coded based on amplitude.
                                    //We will use a "conventional" for loop here because the iterator represents TDC ticks.
      {
        short adc = wvfrm[tdc];
        //if(adc > fSigCut)
        //{
          retVal->AddBox(0, tdc, wireID.Wire*conversion); //null parameter since this is a 2D view, t, w
          retVal->DigitValue(adc); //q
        //}
      }

      //retVal->SetAlwaysSecSelect(kTRUE); //Make individual boxes selectable. 

      retVal->SetMainTransparency(fTransp); //100); //The one transparency function to rule them all!  This is the magic function that lets other objects be drawn over the 
                                        //digits' boxes.  
      //retVal->SetSelectViaFrame(kTRUE);
      //retVal->SetHighlightFrame(kTRUE);
      retVal->SetDisableLighting(kTRUE);
      
      //Rather dangerous to use TEveElement::Set/GetUserData().  However, the only alternatives I see are to draw all digits in some capacity, which would 
      //slow down the TWQ viewer too much, or to write my own class that derives from TEveBoxSet but stores a boost::any or something else appropriate.  
      //To be compatible with both 
      //retVal->SetUserData(); 
      retVal->SetPickable(kTRUE);
      retVal->RefitPlex(); //Supposed to help with memory access
      retVal->VizDB_Insert(retVal->GetName(), kTRUE, kTRUE);

      return (TEveElement*)retVal;
    } 
    
}

DEFINE_ART_SERVICE_INTERFACE_IMPL(eved::WireMaker, eved::VisMakerInt<recob::Wire>)

#endif
