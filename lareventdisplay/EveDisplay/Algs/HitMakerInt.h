// \file: HitMakerInt.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: declares an ART service interface

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
#include "lardata/RecoBase/Hit.h"

#ifndef EVED_HITMAKERINT_H
#define EVED_HITMAKERINT_H

DECLARE_ART_SERVICE_INTERFACE(eved::VisMakerInt<recob::Hit>, LEGACY)

#endif
