// \file: TWQGeoConstructor_module.cc
// \brief: Instantiation of a module to create scenes of reconstructed 3D objects
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/TWQGeoMaker.h"
#include "lareventdisplay/EveDisplay/Algs/SceneConstructor.h"

//Framework includes
#include "art/Framework/Core/ModuleMacros.h" 

#ifndef MODULES_TWQGEOCONSTRUCTOR_MODULE_CC
#define MODULES_TWQGEOCONSTRUCTOR_MODULE_CC

DEFINE_ART_MODULE(eved::SceneConstructor<eved::TWQGeoMaker>)

#endif
