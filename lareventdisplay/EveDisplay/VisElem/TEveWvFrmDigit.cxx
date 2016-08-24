// author: Andrew Olivier
// date: 6/18/2016
// brief: Implementation of TEveWvFrmDigit: a base class for visualization of digits that record waveforms

//LArSoft includes
#include "lareventdisplay/EveDisplay/VisElem/TEveWvFrmDigit.h"

//ROOT includes
#include "TEveTrans.h"
#include "TEveManager.h"
#include "TEveGeoShape.h"

ClassImp(vis::TEveWvFrmDigit)

namespace vis
{
  TEveWvFrmDigit::TEveWvFrmDigit(): TEveGeoShape(), fWvFrm()
  {
  }
  TEveWvFrmDigit::TEveWvFrmDigit(const char* name, const char* title): TEveGeoShape(name, title), fWvFrm()
  {
  }

  TEveWvFrmDigit::TEveWvFrmDigit(const char* name, const char* title, TGeoVolume* vol, TEveTrans* trans, std::vector<short> wvfrm): TEveGeoShape(name, title), fWvFrm(wvfrm) 
  {
    /*auto* extract = new TEveGeoShapeExtract(vol->GetName());
    extract->SetShape((TGeoShape*)(vol->GetShape()->Clone())); */
    TEveGeoManagerHolder gmgr(fgGeoMangeur);
    TEveManager::TRedrawDisabler redrawOff(gEve);
    SetShape((TGeoShape*)(vol->GetShape()->Clone()));
    
    //extract->SetTrans(trans->Array());
    RefMainTrans().SetTrans(*trans);

    //ImportShapeExtract(extract);
    SetName(name);
    SetTitle(title);
    SetDrawFrame(kTRUE);
    fPickable = kTRUE;
    this->ElementChanged();
  }

  TEveWvFrmDigit::TEveWvFrmDigit(const char* name, const char* title, TGeoVolume* vol, double x, double y, double z, double theta, double phi, double psi, std::vector<short> wvfrm): TEveGeoShape(name, title), fWvFrm(wvfrm)
  { 
    /*auto* extract = new TEveGeoShapeExtract(vol->GetName());
    extract->SetShape((TGeoShape*)(vol->GetShape()->Clone()));*/
    SetShape((TGeoShape*)(vol->GetShape()->Clone()));
    TEveManager::TRedrawDisabler redrawOff(gEve);

    RefMainTrans().SetRotByAnyAngles(theta, phi, psi, "zxz");
    RefMainTrans().SetPos(x, y, z);
    RefMainTrans().SetScale(1.0, 1.0, 1.0);

    SetName(name);
    SetTitle(title);

    //ImportShapeExtract(extract);
    SetDrawFrame(kTRUE);
    fPickable = kTRUE;    
    this->ElementChanged();
  }

  void TEveWvFrmDigit::Paint(Option_t*)
  {
    TEveGeoShape::Paint();
  }
}
