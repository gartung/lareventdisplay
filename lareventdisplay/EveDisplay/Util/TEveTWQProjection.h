// \file: TEveTWQProjection.h
// \brief: Declaration of a projection to time versus wire versus charge view for use with ROOT's EVE library
// \author: Andrew Olivier aolivier@ur.rochester.edu

#ifndef VUTL_TEVETWQPROJECTION
#define VUTL_TEVETWQPROJECTION

//LArSoft includes
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

//ROOT includes
#include "TEveProjections.h"

namespace vutl
{
  class TEveTWQProjection: public TEveProjection
  {
    public:
      #ifndef __CINT__
      TEveTWQProjection(geo::PlaneID plane = geo::PlaneID()): TEveProjection(), fPlane(plane) {}
      virtual ~TEveTWQProjection() = default;
      virtual void ProjectPoint(Float_t& x, Float_t& y, Float_t& z, Float_t d, TEveProjection::EPProc_e proc) override;
      virtual Bool_t Is2D() const override { return kTRUE; }
      virtual Bool_t Is3D() const override { return kFALSE; }
      #endif
    private:
      geo::PlaneID fPlane;
    public:
      ClassDef(TEveTWQProjection, 0);
  };
}

#endif
