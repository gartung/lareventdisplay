// \file: TrackMakerInt.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: declares an ART service interface

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
#include "lardata/RecoBase/Track.h"

#ifndef EVED_TRACKMAKERINT_H
#define EVED_TRACKMAKERINT_H

DECLARE_ART_SERVICE_INTERFACE(eved::VisMakerInt<recob::Track>, LEGACY)

#endif
