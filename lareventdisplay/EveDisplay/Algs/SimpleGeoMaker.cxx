// \file: SimpleGeoMaker.cxx
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
#include "lareventdisplay/EveDisplay/Algs/SimpleGeoMaker.h"
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
  SimpleGeoMaker::SimpleGeoMaker()
  {
  }

  void SimpleGeoMaker::reconfigure(const fhicl::ParameterSet& p)
  {
    fGeoTrans = p.get<double>("GeoTrans", 70);
    fGeoColor = p.get<short>("GeoColor", kBlue);
  }

  void SimpleGeoMaker::makeEvent(const art::Event&)
  {
  }

  void SimpleGeoMaker::initialize()
  {
  }
  
  TEveScene* SimpleGeoMaker::makeGlobal()
  {
    TEveScene* retVal = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene("geo", "Simple Geometry Scene"); //new TEveScene("geo");
    retVal->InitMainTrans(kTRUE);

    TEveElementList* beamline = new TEveElementList("beamline", "beamline", kTRUE, kTRUE);   

    //TEveScene* retVal = art::ServiceHandle<eved::EveDisplay>()->getEve()->GetGlobalScene();
 
    mf::LogWarning("SimpleGeoMaker") << "Trying to make a scene in SimpleGeoMaker::makeScene.\n";

    art::ServiceHandle<geo::Geometry> geom;
  
    //Auxiliary detector visualization
    for(const auto& auxDet: geom->AuxDetGeoVec())
    {
      const TGeoVolume* vol = auxDet->TotalVolume();
      if(vol == nullptr) 
      {
        mf::LogWarning("SimpleGeoMaker") << "Got bad auxDet.\n";
        continue;
      }
      auto* extract = new TEveGeoShapeExtract(vol->GetName());
      extract->SetShape((TGeoShape*)(vol->GetShape()->Clone()));

      //Come up with the rotation matrix
      TEveTrans* trans = new TEveTrans();
      double x, y, z, theta, phi, psi;
      GetEulerAngles(*auxDet, theta, phi, psi, x, y, z);
      trans->SetRotByAnyAngles(theta, phi, psi, "zxz");
      trans->SetPos(x, y, z);
      trans->SetScale(1.0, 1.0, 1.0);

      extract->SetTrans(trans->Array());

      auto shape = TEveGeoShape::ImportShapeExtract(extract);
      shape->SetMainTransparency(fGeoTrans);
      shape->SetMainColor(fGeoColor);
      shape->SetTitle(vol->GetName());
      shape->SetDrawFrame(kTRUE);
      shape->VizDB_Insert(shape->GetName(), kTRUE, kTRUE);

      //retVal->AddElement(shape);
      beamline->AddElement(shape);
      //art::ServiceHandle<eved::EveDisplay>()->getEve()->AddGlobalElement(shape);
      mf::LogWarning("SimpleGeoMaker") << "Added AuxDet named " << vol->GetName() << " to scene " << retVal->GetName() << ".\n";
    }
    retVal->AddElement(beamline);

    //Cryostat visualization
    TEveElementList* cryoL = new TEveElementList("cryo", "Cryostats", kTRUE, kTRUE);

    for(const auto& cryo: geom->IterateCryostats())
    {
      const TGeoVolume* vol = cryo.Volume();
      if(vol == nullptr) continue;
      auto* extract = new TEveGeoShapeExtract(vol->GetName());
      extract->SetShape((TGeoShape*)(vol->GetShape()->Clone()));

      //Come up with the rotation matrix
      TEveTrans* trans = new TEveTrans();
      double x, y, z, theta, phi, psi;
      GetEulerAngles(cryo, theta, phi, psi, x, y, z);
      trans->SetRotByAnyAngles(theta, phi, psi, "zxz");
      trans->SetPos(x, y, z);
      trans->SetScale(1.0, 1.0, 1.0);
      
      extract->SetTrans(trans->Array());

      auto shape = TEveGeoShape::ImportShapeExtract(extract);
      shape->SetMainTransparency(fGeoTrans);
      shape->SetMainColor(fGeoColor);
      shape->SetTitle(vol->GetName());
      shape->SetDrawFrame(kTRUE);
      shape->VizDB_Insert(shape->GetName(), kTRUE, kTRUE);

      //retVal->AddElement(shape);
      cryoL->AddElement(shape);
      //art::ServiceHandle<eved::EveDisplay>()->getEve()->AddGlobalElement(shape);
    }
    retVal->AddElement(cryoL);

    //TPC visualization
    TEveElementList* tpcL = new TEveElementList("tpc", "TPCs", kTRUE, kTRUE);

    for(const auto& tpc: geom->IterateTPCs())
    {
      const TGeoVolume* vol = tpc.TotalVolume();
      if(vol == nullptr) 
      {
        mf::LogWarning("SimpleGeoMaker") << "Got bad volume for a TPC.\n";
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
      shape->VizDB_Insert(shape->GetName(), kTRUE, kTRUE);
 
      //retVal->AddElement(shape);
      tpcL->AddElement(shape);
      mf::LogWarning("SimpleGeoMaker") << "Added TPC named " << vol->GetName() << " to scene " << retVal->GetName() << ".\n";
    }
    retVal->AddElement(tpcL);

    //Wire visualization
    //When this functionality is restored, make these children of TPCs?
    /*for(const auto& wire: geom->IterateWires())
    {
      const TGeoVolume* vol = wire.Node()->GetVolume();
      if(vol == nullptr) continue;
      auto* extract = new TEveGeoShapeExtract(vol->GetName());
      extract->SetShape((TGeoShape*)(vol->GetShape()->Clone()));

      //Come up with the rotation matrix
      TEveTrans* trans = new TEveTrans();
      double x, y, z, theta, phi, psi;
      GetEulerAngles(wire, theta, phi, psi, x, y, z);
      trans->SetRotByAnyAngles(theta, phi, psi, "zxz");
      trans->SetPos(x, y, z);
      trans->SetScale(1.0, 1.0, 1.0);
                                                 
      extract->SetTrans(trans->Array());

      auto shape = TEveGeoShape::ImportShapeExtract(extract);
      shape->VizDB_Insert(shape->GetName(), kTRUE, kTRUE);

      retVal->AddElement(shape);
      //art::ServiceHandle<eved::EveDisplay>()->getEve()->AddGlobalElement(shape);
    }*/

    mf::LogWarning("SimpleGeoMaker") << "Render state of view " << retVal->GetName() << " is " << retVal->GetRnrSelf() << ".  Render children state is " 
                                     << retVal->GetRnrChildren() << ".\n";

    return retVal;
  }
}
#endif //__CINT__
