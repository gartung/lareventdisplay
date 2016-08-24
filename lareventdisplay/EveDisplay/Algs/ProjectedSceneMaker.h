// \file: ProjectedSceneMaker.h
// \brief: Constructs a scene for the EVE-based event display by projecting all elements into time-wire space
// \author: Andrew Olivier aolivier@ur.rochester.edu

//LArSoft includes
#include "lareventdisplay/EveDisplay/Algs/CopySceneMaker.h"
#include "lareventdisplay/EveDisplay/Util/TEveUserProjectionManager.h"
#include "lareventdisplay/EveDisplay/Util/TEveTWQProjection.h"
#include "lareventdisplay/EveDisplay/Algs/DestroyRecursive.h"

//ROOT includes
#include "TEveScene.h"

namespace eved
{
  //template paramters:
  //name: same meaning as in CopySceneMaker: name of the scene to be copied and projected
  //PROJ: class derived from TEveProjection that will be used to project elements of copied scene

  template <std::string& name, class PROJ>
  class ProjectedSceneMaker: public CopySceneMaker<name>
  {
    public:
      ProjectedSceneMaker();
      virtual ~ProjectedSceneMaker() = default;

      virtual TEveScene* makeGlobal();
      virtual void makeEvent();

    private:
      TEveUserProjectionManager* fPMan;
      PROJ* fProj;
      TEveScene* fProjScene;
  };

  template <std::string& name, class PROJ>
  ProjectedSceneMaker::ProjectedSceneMaker(): CopySceneMaker<name>(), fProjScene(nullptr)
  {
    fProj = new PROJ();
    fPMan = new TEveUserProjectionManager(fProj);
  }

  template <std::string& name, class PROJ>
  TEveScene* ProjectedSceneMaker::makeGlobal()
  {
    fProjScene = fPMan->ImportElements(CopySceneMaker<name>::makeGlobal(), nullptr);
    return fProjScene;
  }

  template <std::string& name, class PROJ>
  ProjectedSceneMaker::makeEvent()
  {
    DestroyRecursive(fProjScene);
    fProjScene = fPMan->ImportElements(CopySceneMaker<name>::makeGlobal(), nullptr);
  }
}
