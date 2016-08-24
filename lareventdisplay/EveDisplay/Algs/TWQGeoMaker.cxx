// \file: TWQGeoMaker.cxx
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
#include "lareventdisplay/EveDisplay/Algs/TWQGeoMaker.h"
#include "lareventdisplay/EveDisplay/GUI/EveDisplay.h"
#include "lareventdisplay/EveDisplay/Util/TEveUserProjectionManager.h"
#include "lareventdisplay/EveDisplay/Util/TEveTWQProjection.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lareventdisplay/EveDisplay/Config/CoordConfig.h"
#include "lareventdisplay/EveDisplay/Algs/DestroyRecursive.h"

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
#include "TEveArrow.h"
#include "TEveLine.h"
#include "TEveText.h"
#include "TGTab.h"

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
  TWQGeoMaker::TWQGeoMaker()
  {
  }

  void TWQGeoMaker::reconfigure(const fhicl::ParameterSet& p)
  {
    auto pset = p.get<fhicl::ParameterSet>("TWQGeo");
    fGeoTrans = pset.get<double>("GeoTrans", 70);
    fGeoColor = pset.get<short>("GeoColor", kBlue);
    size_t TPCNum = pset.get<size_t>("TPCNum");
    size_t cryoNum = pset.get<size_t>("CryoNum");
    fTPCID = geo::TPCID(TPCNum, cryoNum);
    if(!art::ServiceHandle<geo::Geometry>()->HasTPC(fTPCID)) throw cet::exception("BadTPCID") << "Got bad TPCID for TPC " << TPCNum << " and cryostat "
                                                                                              << cryoNum << " in TPCOrthoViewerMaker.\n";
  }

  TEveScene* TWQGeoMaker::makeGlobal() //Event(const art::Event&)
  {
    art::ServiceHandle<geo::Geometry> geom;
    auto& tpc = geom->TPC(fTPCID);
    art::ServiceHandle<eved::EveDisplay> eved;

    for(size_t plane = 0; plane < tpc.Nplanes(); ++plane)
    {
      fScenes.push_back(art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene(("TWQGeo"+std::to_string(plane)).c_str()));
    }

    return *(fScenes.begin());
  }


  void TWQGeoMaker::initialize()
  {
  }
  
  void TWQGeoMaker::makeEvent(const art::Event&) //used to be makeGlobal, but I want this to be called per event now since it depends on the window 
                                                       //geometry
  {
    /*TEveScene* retVal = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene("TWQ", "TWQ Geometry Scene"); //new TEveScene("geo");
    retVal->InitMainTrans(kTRUE);*/

    mf::LogWarning("TWQGeoMaker") << "Trying to make a scene in TWQGeoMaker::makeScene.\n";

    art::ServiceHandle<geo::Geometry> geom;
  
    //TPC visualization
    auto& tpc = geom->TPC(fTPCID);
    //const TGeoVolume* vol = tpc.TotalVolume();
    /*if(vol == nullptr) 
    {
      mf::LogWarning("TWQGeoMaker") << "Got bad volume for a TPC.\n";
      //return retVal;
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
 
    //retVal->AddElement(shape);*/

    detinfo::DetectorProperties const* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

    art::ServiceHandle<eved::EveDisplay> eved;
    auto twqWindow = eved->getEve()->GetBrowser()->GetTabRight()->GetContainer();
    auto width = twqWindow->GetWidth();
    auto height = twqWindow->GetHeight();

    //art::ServiceHandle<eved::CoordConfig> config;
    double conversion = detprop->NumberTimeSamples()*width/height*(tpc.Nplanes()+0.5); //detprop->NumberTimeSamples()*config->GetFrameWidth()/config->GetFrameHeight()*(tpc.Nplanes()+1);

    /*for(size_t plane = 0; plane < tpc.Nplanes(); ++plane)
    {
      auto projMan = new vutl::TEveUserProjectionManager(new vutl::TEveTWQProjection(geo::PlaneID(fTPCID, plane)));
      auto scene = art::ServiceHandle<eved::EveDisplay>()->getEve()->SpawnNewScene(("TWQGeo"+std::to_string(plane)).c_str());*/

    for(size_t plane = 0; plane < fScenes.size(); ++plane)
    {
      auto scene = fScenes[plane];
      DestroyRecursive(scene); 
      //auto projMan = new vutl::TEveUserProjectionManager(new vutl::TEveTWQProjection(geo::PlaneID(fTPCID, plane)));
      //scene->AddElement(projMan->ImportElements(shape)); 

      //Attempt at implementing simple well-behaved axes.  I am in the process of developing this, so I will probably hardcode a few values for now.  
      //Everything here should eventually come from a fcl parameter set.  
      auto yaxis = new TEveArrow(0, detprop->NumberTimeSamples(), 0, 0, 0, 0);
      yaxis->SetTubeR(0.01);
      yaxis->SetMainColorRGB(1.f, 0, 0);
      yaxis->VizDB_Insert("YAxis", kTRUE, kTRUE);

      double tickLength = 500.; //arbitrary value for testing
      size_t fontSize  = 0.1;

      //Add tick marks to the y axis
      for(size_t tick = 0; tick <= 10; ++tick) //arbitrarily use 100 tick marks for testing
      {
        auto line = new TEveLine(2); 
        line->SetNextPoint(0, tick*detprop->NumberTimeSamples()/10., -tickLength/2.);
        line->SetNextPoint(0, tick*detprop->NumberTimeSamples()/10., tickLength/2.);
        line->SetMainColorRGB(1.f, 0, 0);
        scene->AddElement(line);
        
        auto label = new TEveText(std::to_string((int)(tick*detprop->NumberTimeSamples()/10.)).c_str());
        label->PtrMainTrans()->SetPos(0, tick*detprop->NumberTimeSamples()/10, -500);
        label->SetMainColorRGB(1.f, 0, 0);
        label->SetFontSize(fontSize);
        scene->AddElement(label);
      }
      
      //Label the y axis as a whole
      auto yAxisLabel = new TEveText("T [TDC Ticks]");
      yAxisLabel->PtrMainTrans()->SetPos(0, detprop->NumberTimeSamples()+20., -500);
      yAxisLabel->SetMainColorRGB(1.f, 0, 0);
      yAxisLabel->SetFontSize(fontSize);
      scene->AddElement(yAxisLabel);
  
      //Build the z axis
      auto zaxis = new TEveArrow(0, 0, conversion, 0, 0, 0);
      zaxis->SetMainColorRGB(0, 0, 1.f);
      zaxis->SetTubeR(0.001); //why does this arrow seem to be ~10x thicker than the y arrow?  
      zaxis->SetConeR(0.004);
      zaxis->VizDB_Insert("ZAxis", kTRUE, kTRUE);

      //Add tick marks to the z axis
      size_t nWires = tpc.Plane(plane).Nwires();
      for(size_t tick = 0; tick <= 20; ++tick) //arbitrarily use 100 tick marks for testing
      {
        auto line = new TEveLine(2);
        line->SetNextPoint(0, -tickLength/2., tick*conversion/20.);
        line->SetNextPoint(0, tickLength/2., tick*conversion/20.);
        line->SetMainColorRGB(0, 0, 1.f);
        scene->AddElement(line);

        auto label = new TEveText(std::to_string((int)(nWires/20.*tick)).c_str());
        label->PtrMainTrans()->SetPos(0, -300, tick*conversion/20.);
        label->SetMainColorRGB(0, 0, 1.f);
        label->SetFontSize(fontSize);
        scene->AddElement(label);
      }
      
      //Label the z axis as a whole
      auto zAxisLabel = new TEveText("W [Wire Number]");
      zAxisLabel->PtrMainTrans()->SetPos(0, -300, conversion+20.);
      zAxisLabel->SetMainColorRGB(0, 0, 1.f);
      zAxisLabel->SetFontSize(fontSize);
      scene->AddElement(zAxisLabel);

      //Hand the axes to the scene for this view
      scene->AddElement(yaxis);
      scene->AddElement(zaxis);  
    }
  }
}
#endif //__CINT__
