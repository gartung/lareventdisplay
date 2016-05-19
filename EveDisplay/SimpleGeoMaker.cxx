// \file: SimpleGeoMaker.cxx
// \author: Andrew Olivier aoliv23@lsu.edu
// \brief: A simplified visualization of the geometry

//Framework includes
#ifndef __CINT__
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Event.h"
#include "cetlib/exception.h"

//LArSoft includes
#include "Geometry/Geometry.h"
#include "EveDisplay/SimpleGeoMaker.h"

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

namespace //helper functions
{
  template <class T> //It turns out that, although the LArSoft geometry service does not use a base class, all LArSoft geometry classes have LocalToWorld 
                     //methods.  Time to take advantage of this fact...
  void GetEulerAngles(T det, double& theta, double& phi, double& psi, double& centerX, double& centerY, double& centerZ) //return angles and center by reference
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

  void SimpleGeoMaker::initialize()
  {
  }

  void SimpleGeoMaker::reconfigure(const fhicl::ParameterSet& p)
  {
  }

  TEveScene* SimpleGeoMaker::makeScene(const art::Event& e)
  {
    TEveScene* retVal = new TEveScene("geo");

    art::ServiceHandle<geo::Geometry> geom;
  
    //Auxiliary detector visualization
    for(const auto& auxDet: geom->AuxDetGeoVec())
    {
      const TGeoVolume* vol = auxDet->TotalVolume();
      auto* extract = new TEveGeoShapeExtract(vol->GetName());
      extract->SetShape(vol->GetShape());

      //Come up with the rotation matrix
      TEveTrans* trans = new TEveTrans();
      double x, y, z, theta, phi, psi;
      GetEulerAngles(*auxDet, theta, phi, psi, x, y, z);
      trans->SetRotByAngles(theta, phi, psi);
      trans->SetPos(x, y, z);

      extract->SetTrans(trans->Array());

      auto shape = new TEveGeoShape(vol->GetName());
      shape->ImportShapeExtract(extract);

      retVal->AddElement(shape);
    }

    //Cryostat visualization
    for(const auto& cryo: geom->IterateCryostats())
    {
      const TGeoVolume* vol = cryo.Volume();
      auto* extract = new TEveGeoShapeExtract(vol->GetName());
      extract->SetShape(vol->GetShape());

      //Come up with the rotation matrix
      TEveTrans* trans = new TEveTrans();
      double x, y, z, theta, phi, psi;
      GetEulerAngles(cryo, theta, phi, psi, x, y, z);
      trans->SetRotByAngles(theta, phi, psi);
      trans->SetPos(x, y, z);
      
      extract->SetTrans(trans->Array());

      auto shape = new TEveGeoShape(vol->GetName());
      shape->ImportShapeExtract(extract);

      retVal->AddElement(shape);
    }

    //TPC visualization
    for(const auto& tpc: geom->IterateTPCs())
    {
      const TGeoVolume* vol = tpc.TotalVolume();
      auto* extract = new TEveGeoShapeExtract(vol->GetName());
      extract->SetShape(vol->GetShape());

      //Come up with the rotation matrix
      TEveTrans* trans = new TEveTrans();
      double x, y, z, theta, phi, psi;
      GetEulerAngles(tpc, theta, phi, psi, x, y, z);
      trans->SetRotByAngles(theta, phi, psi);
      trans->SetPos(x, y, z);
                                                
      extract->SetTrans(trans->Array());

      auto shape = new TEveGeoShape(vol->GetName());
      shape->ImportShapeExtract(extract);
      
      retVal->AddElement(shape);
    }

    //Wire visualization
    for(const auto& wire: geom->IterateWires())
    {
      const TGeoVolume* vol = wire.Node()->GetVolume();
      auto* extract = new TEveGeoShapeExtract(vol->GetName());
      extract->SetShape(vol->GetShape());

      //Come up with the rotation matrix
      TEveTrans* trans = new TEveTrans();
      double x, y, z, theta, phi, psi;
      GetEulerAngles(wire, theta, phi, psi, x, y, z);
      trans->SetRotByAngles(theta, phi, psi);
      trans->SetPos(x, y, z);
                                                 
      extract->SetTrans(trans->Array());

      auto shape = new TEveGeoShape(vol->GetName());
      shape->ImportShapeExtract(extract);

      retVal->AddElement(shape);
    }

    return retVal;
  }
}
#endif //__CINT__
