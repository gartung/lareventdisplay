// \file: SimPartMaker.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: Runtime-choosable algorithm for visualizaing simb::MCParticle in the EVE-based event display.  Implements SimPartMakerInt.h. 

//Framework includes
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/SimPartMaker.h"
#include "lareventdisplay/EveDisplay/Algs/SimPartMakerInt.h"
#include "SimulationBase/MCParticle.h"
#include "lareventdisplay/EventDisplay/Style.h"
#include "larcore/Geometry/Geometry.h"

//ROOT includes
#include "TEveTrack.h"
#include "TEveElement.h"
#include "TEveTrackPropagator.h"
#include "TLorentzVector.h"
#include "TEveVSDStructs.h" //because I haven't yet found a way to set a track's vertex without this

//c++ includes
#include <algorithm> //for std::find

#ifndef EVED_SIMPARTMAKER_SERVICE_CC
#define EVED_SIMPARTMAKER_SERVICE_CC

namespace eved
{

    void eved::SimPartMaker::reconfigure(const fhicl::ParameterSet& p)
    {
      fMinE = p.get<double>("MinE"); //in GeV

      art::ServiceHandle<geo::Geometry> geom;
      double xlo(0), xhi(0), ylo(0), yhi(0), zlo(0), zhi(0);
      geom->WorldBox(&xlo, &xhi, &ylo, &yhi, &zlo, &zhi);
      double rMax = std::sqrt((xhi-xlo)*(xhi-xlo)+(yhi-ylo)*(yhi-ylo))*10./2.; //multiply by 10 to get mm instead of geometry service's cm

      fMaxR = p.get<double>("MaxR", rMax); //in mm
      fMaxZ = p.get<double>("MaxZ", (zhi-zlo)*10./2.); //in mm; multiply default value by 10 to get mm from geometry service's cm
      fPDGsToSkip = p.get<std::vector<int>>("PDGsToSkip");
      //mf::LogWarning("SimPartMaker") << "In reconfigure, MinE is " << fMinE << ", fMaxR is " << fMaxR << ", and fMaxZ is " << fMaxZ << ".\n";
    }

    bool eved::SimPartMaker::SelectDataProduct(const simb::MCParticle& part)
    {
      //mf::LogWarning("SimPartMaker") << "In SelectDataProduct, MinE is " << fMinE << " and particle E0 is " << part.Trajectory().begin()->second.E() << "\n";
      return (part.Trajectory().begin()->second.E()>fMinE)&&(part.PdgCode() < 1e4)&&(std::find(fPDGsToSkip.begin(), fPDGsToSkip.end(), part.PdgCode()) == fPDGsToSkip.end()); //simb::MCParticle energy values are in GeV
    }

    TEveElement* eved::SimPartMaker::MakeVis(const simb::MCParticle& track) //implementation of pure virtual function
    {
      //TODO: Getting tracks that appear to "bounce" off of a plane at high z.  Strongly suspect that this is a problem with the propagator, but I plan 
      //to print these tracks' points first.  Maybe LArSoft stops drawing a true track as soon as it leaves the TPC?  "Bounced" true tracks are 
      //probably outside the readout window anyway?
      auto traj = track.Trajectory();
      TEveMCTrack vtx;
      vtx.SetProductionVertex(traj.begin()->first);
      vtx.SetMomentum(traj.begin()->second);
      vtx.SetCalcMass(track.Mass());
      vtx.SetPdgCode(track.PdgCode());

      vtx.SetStatusCode(track.StatusCode());
 
      auto prop = new TEveTrackPropagator(); //configure track propagator
      prop->SetMaxR(fMaxR);
      prop->SetMaxZ(fMaxZ);
      prop->SetMagField(0.); //Get this from LArSoft mag field service?  I don't know whether there is a default version.  If not, this line would 
                             //cause trouble for experiments without mag field service implementations.

      auto retVal = new TEveTrack(&vtx, prop); //This is the TEveTrack we will return

      for(auto pt = (++traj.begin()); pt != std::prev(traj.end()); ++pt) //loop over track trajectory points; 0th point is the vertex included above
      {
        TEveVectorT<double> pos, mom;
        pos.Set(pt->first.Vect());
        mom.Set(pt->second.Vect());
        retVal->AddPathMark(TEvePathMarkD(TEvePathMarkD::kReference, pos, mom, pt->first.T())); 
      }

      //FIXME: Make sure TEveTrackPropagator does not propagate to end of bounds.  Using kDecay may not be physically correct in many cases, but it does what we
      //want for now.  
      TEveVectorT<double> pos, mom;
      pos.Set(track.EndPosition().Vect());
      mom.Set(track.EndMomentum().Vect());
      retVal->AddPathMark(TEvePathMarkD(TEvePathMarkD::kDecay, pos, mom, track.EndPosition().T())); 
      
      retVal->MakeTrack();
      auto P0 = traj.begin()->second;
      auto R0 = traj.begin()->first;
      retVal->SetTitle(("simb::MCParticle\n PDG Code="+std::to_string(track.PdgCode())+", process= "+track.Process()+", end process= "+track.EndProcess()+
                        "\nP0=("+std::to_string(P0.X())+","+std::to_string(P0.Y())+","+std::to_string(P0.Z())+")[GeV/c]\nR0=("+std::to_string(R0.X())+"[cm],"
                        +std::to_string(R0.Y())+","+std::to_string(R0.Z())+")\nT0="+std::to_string(R0.T())+"[s?], E0="+std::to_string(P0.E())+"[GeV]").c_str());
      retVal->SetMainColor(evd::Style::ColorFromPDG(track.PdgCode()));  
      retVal->SetMarkerStyle(1); //fcl parameters?
      retVal->SetMarkerColor(evd::Style::ColorFromPDG(track.PdgCode()));
      
      retVal->SetRnrPoints(kFALSE); 

      return (TEveElement*)retVal;
    } 
    
}

DEFINE_ART_SERVICE_INTERFACE_IMPL(eved::SimPartMaker, eved::VisMakerInt<simb::MCParticle>)

#endif
