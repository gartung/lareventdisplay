// \file: TEveUserProjectionManager.h
// \brief: Declaration of a projection manager for EVE that accepts user-defined projections
// \author: Andrew Olivier aolivier@ur.rochester.edu

//ROOT includes
#include "TEveProjectionManager.h"
#include "TEveProjections.h"

#ifndef VUTL_TEVEUSERPROJECTIONMANAGER
#define VUTL_TEVEUSERPROJECTIONMANAGER

namespace vutl
{
  class TEveUserProjectionManager: public TEveProjectionManager
  {
    private:
      #ifndef __CINT__
      TEveUserProjectionManager(const TEveUserProjectionManager&) = delete;
      TEveUserProjectionManager& operator=(const TEveUserProjectionManager&) = delete;
      #endif
    public:
      TEveUserProjectionManager(TEveProjection::EPType_e type = TEveProjection::kPT_Unknown): TEveProjectionManager(type) {}
      TEveUserProjectionManager(TEveProjection* proj): TEveProjectionManager(TEveProjection::kPT_Unknown) {fProjection = proj;}
      //User-provided projection; owned by the user
      ~TEveUserProjectionManager() { delete fProjection; fProjection = nullptr; } //I think this is safe because the c++ standard supposedly says that 
                                                                                  //deletion of a nullptr is safe.  So, when TEveProjectionManager's virtual 
                                                                                  //destructor is called AFTER this one, the worst that will happend is it 
                                                                                  //will try to delete one of the "standard" projections that is a nullptr 
                                                                                  //anyway.  
      void SetProjection(TEveProjection* proj); //overload of base class method to facilitate user-defined projections 
    ClassDef(TEveUserProjectionManager, 1); //projection manager for user-defined projections
  };
}

#endif
