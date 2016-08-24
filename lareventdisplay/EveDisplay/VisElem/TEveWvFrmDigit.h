// author: Andrew Olivier
// date: 6/18/2016
//Visualization object for digit-like data products 
//What's in a TEveWvFrmDigit? 
//* a detector volume that recorded the digit
//* a recorded waveform (I have TDCs and ADCs in mind, but could be others if you can make them fit)
//* a position for the detector volume
//
//TEveWvFrmDigit will:
//* Draw itself as its parent volume (I have in mind things like LArSoft's AuxDetSensitiveGeo as well as WireGeos)
//* Coming (hopefully) Soon: provide its waveform to be drawn by an "Editor" object; not implemented here

//ROOT includes
#include "TEveGeoShape.h"
#include "TGeoVolume.h"
#include "TEveGeoShapeExtract.h"

//c++ includes
#include <vector>

#ifndef VIS_TEVEWVFRMDIGIT
#define VIS_TEVEWVFRMDIGIT

namespace vis
{
  class TEveWvFrmDigit: public TEveGeoShape //we will be drawn as a TEveGeoShape
  {
    friend class TEveWvFrmDigitEditor;

    private:
      TEveWvFrmDigit(const TEveWvFrmDigit&); //not implemented
      TEveWvFrmDigit& operator=(const TEveWvFrmDigit&); //not implemented

    public:
      TEveWvFrmDigit(); //ROOT seems to need a default constructor
      TEveWvFrmDigit(const char* name, const char* title = nullptr);
      virtual ~TEveWvFrmDigit() {} 
      TEveWvFrmDigit(const char* name, const char* title, TGeoVolume* vol, TEveTrans* trans, std::vector<short> wvfrm);
      TEveWvFrmDigit(const char* name, const char* title, TGeoVolume* vol, double x, double y, double z, double theta, double phi, double psi, std::vector<short> wvfrm);

      const std::vector<short>& GetWvFrm() const { return fWvFrm; }

      virtual void Paint(Option_t*);

    protected:
      std::vector<short> fWvFrm; //waveform recorded in this digit

    public:
      ClassDef(TEveWvFrmDigit, 1); //base class for visualization of digits that have waveforms
  };
}

#endif //VIS_TEVEWVFRMDIGIT
