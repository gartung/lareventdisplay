// \file: HitMakerInt.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: declares an ART service interface

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
#include "lardataobj/RecoBase/Wire.h"

#ifndef EVED_WIREMAKERINT_H
#define EVED_WIREMAKERINT_H

DECLARE_ART_SERVICE_INTERFACE(eved::VisMakerInt<recob::Wire>, LEGACY)

#endif
