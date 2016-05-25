// \file: SimPartMakerInt.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: declares an ART service interface

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
#include "SimulationBase/MCParticle.h"

#ifndef EVED_SIMPARTMAKERINT_H
#define EVED_SIMPARTMAKERINT_H

DECLARE_ART_SERVICE_INTERFACE(eved::VisMakerInt<simb::MCParticle>, LEGACY)

#endif
