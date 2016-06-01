// \file: TPCOnlyGeoMaker.cxx
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
#include "larcore/SimpleTypesAndConstants/geo_types.h"
#include "lareventdisplay/EveDisplay/Algs/TPCOnlyGeoMaker.h"
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
#include "TEveProjectionManager.h"
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
  TPCOnlyGeoMaker::TPCOnlyGeoMaker()
  {
  }

  void TPCOnlyGeoMaker::reconfigure(const fhicl::ParameterSet& p)
  {
    auto pset = p.get<fhicl::ParameterSet>("TPCOnlyGeo");
    fGeoTrans = pset.get<double>("GeoTrans", 70);
    fGeoColor = pset.get<short>("GeoColor", kBlue);
    size_t TPCNum = pset.get<size_t>("TPCNum");
    size_t cryoNum = pset.get<size_t>("CryoNum");
    fTPCID = geo::TPCID(TPCNum, cryoNum);
    if(!art::ServiceHandle<geo::Geometry>()->HasTPC(fTPCID)) throw cet::exception("BadTPCID") << "Got bad TPCID for TPC " << TPCNum << " and cryostat "
                                                                                              << cryoNum << " in TPCOrthoViewerMaker.\n";
  }

  void TPCOnlyGeoMaker::makeEvent(const art::Event&)
  {
  }

  void TPCOnlyGeoMaker::initialize()
  {
  }
  
  TEveScene* TPCOnlyGeoMaker::makeGlobal()
  {
    TEveScene* retVal = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene("TPCOnly", "TPCOnly Geometry Scene"); //new TEveScene("geo");
    retVal->InitMainTrans(kTRUE);

    mf::LogWarning("TPCOnlyGeoMaker") << "Trying to make a scene in TPCOnlyGeoMaker::makeScene.\n";

    //Axes visualization
    /*TEveProjectionManager* man(TEveProjection::kPT_3D);
    auto axes = new TEveProjectionAxes(man);
    axes->VizDB_Insert(axes->GetElementName(), kFALSE, kTRUE);
    retVal->AddElement(axes);*/

    art::ServiceHandle<geo::Geometry> geom;
  
    //TPC visualization
    auto& tpc = geom->TPC(fTPCID);
    const TGeoVolume* vol = tpc.TotalVolume();
    if(vol == nullptr) 
    {
      mf::LogWarning("TPCOnlyGeoMaker") << "Got bad volume for a TPC.\n";
      return retVal;
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
 
    retVal->AddElement(shape);
    mf::LogWarning("TPCOnlyGeoMaker") << "Added TPC named " << vol->GetName() << " to scene " << retVal->GetName() << ".\n";

    mf::LogWarning("TPCOnlyGeoMaker") << "Render state of view " << retVal->GetName() << " is " << retVal->GetRnrSelf() << ".  Render children state is " 
                                     << retVal->GetRnrChildren() << ".\n";

    return retVal;
  }
}
#endif //__CINT__
