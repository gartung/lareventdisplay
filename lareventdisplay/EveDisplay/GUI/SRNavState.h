///
/// \file    SRNavState.h
/// \brief   Holds information about what action to take next.  Expands upon evdb::NavState to support selecting subruns which is very important to LArIAT!
/// \author  Andrew Olivier aoliv23@lsu.edu
//  Based heavily on EventDisplayBase/NavState.h
///

#include "stddef.h" //size_t

#ifndef EVED_SRNAVSTATE_H
#define EVDB_SRNAVSTATE_H

#ifndef __CINT__

namespace eved {
  enum class sr_nav_states {
    kNEXT_EVENT,
    kPREV_EVENT,
    kRELOAD_EVENT,
    kGOTO_EVENT,
    kSEQUENTIAL_ONLY
  };
  
  ///
  /// Encapsulate some data about where the display should go next
  ///
  /// Since there can only be one state, these are implemented as a
  /// set of static methods
  ///
  class SRNavState {
  public:
    static sr_nav_states  Which();
    static void Set(sr_nav_states which);
    static void SetTarget(size_t run, size_t subrun, size_t event);
    static size_t  TargetRun();
    static size_t  TargetSubRun();
    static size_t  TargetEvent();
  private:
    SRNavState() { }
  };
}

#endif //__CINT__

#endif
