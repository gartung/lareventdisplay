// \file: RawDigitMakerInt.h
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: declares an ART service interface

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/VisMakerInt.h"
#include "lardataobj/RawData/RawDigit.h"

#ifndef EVED_RAWDIGITMAKERINT_H
#define EVED_RAWDIGITMAKERINT_H

DECLARE_ART_SERVICE_INTERFACE(eved::VisMakerInt<raw::RawDigit>, LEGACY)

#endif
