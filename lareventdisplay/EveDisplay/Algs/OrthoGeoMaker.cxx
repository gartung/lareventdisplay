// \file: OrthoGeoMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: A simplified visualization of the geometry

//Framework includes
#ifndef __CINT__
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//LArSoft includes
#include "larcore/Geometry/Geometry.h"
#include "lareventdisplay/EveDisplay/Algs/OrthoGeoMaker.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"

//c++ includes
#include <vector>
#include <string>

//ROOT includes
#include "TEveScene.h"
#include "TEveGeoShapeExtract.h" //Visualization for individual geometry components
#include "TEveGeoShape.h"
#include "TGeoVolume.h"
#include "TEveTrans.h"
#include "TGeoNode.h"
#include "TEveViewer.h"
#include "TEveElement.h"
#include "TEveProjectionAxes.h"
#include "TEveProjectionManager.h"

namespace //helper functions
{
  template <class T> //It turns out that, although the LArSoft geometry service does not use a base class, all LArSoft geometry classes have LocalToWorld 
                     //methods.  Time to take advantage of this fact...
  void GetEulerAngles(const T& det, double& theta, double& phi, double& psi, double& centerX, double& centerY, double& centerZ) //return angles and center by reference
  {
    double center[3] = {0, 0, 0}, local[3] = {0, 0, 0};
    det.LocalToWorld(local, center);
    double x[3] = {1, 0, 0}, y[3] = {0, 1, 0}, z[3] = {0, 0, 1}, gX[3] = {0}, gY[3] = {0}, gZ[3] = {0}; 

    det.LocalToWorldVect(x, gX);
    det.LocalToWorldVect(y, gY);
    det.LocalToWorldVect(z, gZ);

    theta = std::atan2(gZ[0], -gZ[1]);
    psi = std::atan2(gX[2], gY[2]);
    phi = std::atan2(gZ[0], std::sin(theta)*gZ[2]);

    centerX = center[0];
    centerY = center[1];
    centerZ = center[2];
    
    return; 
  }
}

namespace eved
{
  OrthoGeoMaker::OrthoGeoMaker(): fMan(new TEveProjectionManager(TEveProjection::kPT_RPhi))
  {
  }

  void OrthoGeoMaker::reconfigure(const fhicl::ParameterSet& p)
  {
    fGeoTrans = p.get<double>("GeoTrans", 70);
    fGeoColor = p.get<short>("GeoColor", kBlue);
  }

  void OrthoGeoMaker::makeEvent(const art::Event&)
  {
  }

  void OrthoGeoMaker::initialize()
  {
  }
  
  TEveScene* OrthoGeoMaker::makeGlobal()
  {
    TEveScene* retVal = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene("geo", "Ortho Geometry Scene"); //new TEveScene("geo");
    retVal->InitMainTrans(kTRUE);

    mf::LogWarning("OrthoGeoMaker") << "Trying to make a scene in OrthoGeoMaker::makeScene.\n";

    art::ServiceHandle<geo::Geometry> geom;
  
    //TPC visualization
    TEveElementList* tpcL = new TEveElementList("tpc", "TPCs", kTRUE, kTRUE);

    for(const auto& tpc: geom->IterateTPCs())
    {
      const TGeoVolume* vol = tpc.TotalVolume();
      if(vol == nullptr) 
      {
        mf::LogWarning("OrthoGeoMaker") << "Got bad volume for a TPC.\n";
        continue;
      }
      auto* extract = new TEveGeoShapeExtract(vol->GetName());
      extract->SetShape((TGeoShape*)(vol->GetShape()->Clone()));

      //Come up with the rotation matrix
      TEveTrans* trans = new TEveTrans();
      double x, y, z, theta, phi, psi;
      GetEulerAngles(tpc, theta, phi, psi, x, y, z);
      trans->SetRotByAnyAngles(theta, phi, psi, "zxz");
      trans->SetPos(x, y, z);
      trans->SetScale(1.0, 1.0, 1.0);
                                                
      extract->SetTrans(trans->Array());

      auto shape = TEveGeoShape::ImportShapeExtract(extract);
      shape->SetMainTransparency(fGeoTrans);
      shape->SetMainColor(fGeoColor);
      shape->SetTitle(vol->GetName());
      shape->SetDrawFrame(kTRUE);
      auto proj = fMan->ImportElements(shape);
      proj->SetElementName((std::string(proj->GetElementName())+"Ortho").c_str());
      proj->VizDB_Insert(shape->GetName(), kTRUE, kTRUE);
 
      //retVal->AddElement(shape);
      tpcL->AddElement(proj);
      mf::LogWarning("OrthoGeoMaker") << "Added TPC named " << vol->GetName() << " to scene " << retVal->GetName() << ".\n";
    }
    retVal->AddElement(tpcL);

    mf::LogWarning("OrthoGeoMaker") << "Render state of view " << retVal->GetName() << " is " << retVal->GetRnrSelf() << ".  Render children state is " 
                                     << retVal->GetRnrChildren() << ".\n";

    return retVal;
  }
}
#endif //__CINT__
