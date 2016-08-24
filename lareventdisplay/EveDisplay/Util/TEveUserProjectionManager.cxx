// \file: TEveUserProjectionManager.cxx
// \brief: Implementation of a projection manager for EVE that accepts user-defined projections
// \author: Andrew Olivier aolivier@ur.rochester.edu

#include "lareventdisplay/EveDisplay/Util/TEveUserProjectionManager.h"
#include "TEveProjections.h"

ClassImp(vutl::TEveUserProjectionManager)

namespace vutl
{
  void TEveUserProjectionManager::SetProjection(TEveProjection* proj)
  {
    static const TEveException eH("TEveUserProjectionManager::SetProjection");
    if(fProjection != nullptr && fProjection->Is2D() != proj->Is2D())
    {
      throw eH + "switching between 2D and 3D projections is still not allowed"; 
    }
    
    //Delete fProjection to avoid a memory leak.  See destructor comments in header for why (I think) this is safe.
    delete fProjection; fProjection = nullptr;

    fProjection = proj;
    fProjection->SetCenter(fCenter);
    UpdateName();
  }
}

