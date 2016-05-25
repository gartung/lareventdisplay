///
/// \file    SRNavState.cxx
/// \brief   Holds information about what action to take next.
/// \author  Andrew Olivier aoliv23@lsu.edu
/// Based heavily on EventDisplayBase/NavState.cxx
///
#include "lareventdisplay/EveDisplay/GUI/SRNavState.h"

#include "TROOT.h"
#include "TApplication.h"

#include "stddef.h"

namespace eved{

  static sr_nav_states gsSRNavState    = sr_nav_states::kNEXT_EVENT;
  static size_t gsSRTargetRun   = 0;
  static size_t gsSRTargetSubRun = 0;
  static size_t gsSRTargetEvent = 0;

  //......................................................................

  sr_nav_states SRNavState::Which() { return gsSRNavState; }

  //......................................................................

  void SRNavState::Set(sr_nav_states which)
  {
    // only allow the state to change if it has not been previously set
    // to kSEQUENTIAL_ONLY, this should be made more intelligent, ie
    // if we can only do sequential access because we are using the 
    // socket input source, then we shouldn't be allowed to press the
    // other buttons
    if(gsSRNavState != sr_nav_states::kSEQUENTIAL_ONLY)
      gsSRNavState = which;

    gROOT->GetApplication()->Terminate();
  }

  //......................................................................

  void SRNavState::SetTarget(size_t run, size_t subrun, size_t event)
  {
    gsSRTargetRun = run;
    gsSRTargetSubRun = subrun;
    gsSRTargetEvent = event;
  }

  //......................................................................

  size_t SRNavState::TargetRun() { return gsSRTargetRun; }

  //......................................................................

  size_t SRNavState::TargetEvent() { return gsSRTargetEvent; }

  size_t SRNavState::TargetSubRun() { return gsSRTargetSubRun; }

}// namespace
////////////////////////////////////////////////////////////////////////
