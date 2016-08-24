// \file: DestroyRecursive.h
// \brief: Utility function to recursively destroy all children of a TEveElement
// \author: Andrew Olivier aolivier@ur.rochester.edu

//ROOT includes
#include "TEveElement.h"

#ifndef ALGS_DESTROYRECURSIVE_H
#define ALGS_DESTROYRECURSIVE_H

namespace eved
{
  void DestroyRecursive(TEveElement* el)
  {
    for(auto child = el->BeginChildren(); child != el->EndChildren(); ++child)
    {
      DestroyRecursive(*child);
    }
    el->DestroyElements();
    return;
  }
}

#endif
