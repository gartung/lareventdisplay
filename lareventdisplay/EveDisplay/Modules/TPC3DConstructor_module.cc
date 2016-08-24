// \file: TPCViewerConstructor_module.cc
// \brief: Instantiation of a module to manage a basic window with a 3D viewer for EVE
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/ViewerConstructor.h"
#include "lareventdisplay/EveDisplay/Frames/BasicViewerFrame.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h"

#ifndef ALGS_TPCVIEWERCONSTRUCTOR_MODULE_CC
#define ALGS_TPCVIEWERCONSTRUCTOR_MODULE_CC

DEFINE_ART_MODULE(eved::ViewerConstructor<eved::BasicViewerFrame>)

#endif
