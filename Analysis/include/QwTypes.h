#ifndef QWTYPES_H
#define QWTYPES_H

// C and C++ headers
#include <map>
#include <iostream>

// ROOT basic types
#include <Rtypes.h>
class TString;

// Qweak headers
#include "QwUnits.h"

// Enumerator types for regions and directions
enum EQwRegionID {
  kRegionIDNull = 0,
  kRegionID1,    kRegionID2,   kRegionID3,
  kRegionIDTrig, kRegionIDCer, kRegionIDScanner,
  kNumRegions
};
inline std::ostream& operator<< (std::ostream& stream, const EQwRegionID& i) {
  stream << "?123TCS#"[i]; return stream;
}
typedef std::map < char, EQwRegionID > QwRegionMap;
inline QwRegionMap CreateRegionMap()
{
  QwRegionMap map;
  map['?'] = kRegionIDNull;
  map['1'] = kRegionID1;
  map['2'] = kRegionID2;
  map['3'] = kRegionID3;
  map['T'] = kRegionIDTrig;
  map['C'] = kRegionIDCer;
  map['S'] = kRegionIDScanner;
  return map;
}
static const QwRegionMap kQwRegionMap = CreateRegionMap();


enum EQwDirectionID {
  kDirectionNull = 0,
  kDirectionX, kDirectionY,
  kDirectionU, kDirectionV,
  kDirectionR, kDirectionPhi,
  kDirectionLeft, kDirectionRight,
  kNumDirections
};
inline std::ostream& operator<< (std::ostream& stream, const EQwDirectionID& i) {
  stream << "?xyuvrfLR#"[i]; return stream;
}
typedef std::map < char, EQwDirectionID > QwDirectionMap;
inline QwDirectionMap CreateDirectionMap()
{
  QwDirectionMap map;
  map['?'] = kDirectionNull;
  map['x'] = kDirectionX;
  map['y'] = kDirectionY;
  map['u'] = kDirectionU;
  map['v'] = kDirectionV;
  map['r'] = kDirectionR;
  map['f'] = kDirectionPhi;
  map['L'] = kDirectionLeft;
  map['R'] = kDirectionRight;
  return map;
}
static const QwDirectionMap kQwDirectionMap = CreateDirectionMap();

// Currently up and down are considered two packages.
enum EQwDetectorPackage {
  kPackageNull = 0,
  kPackage1,
  kPackage2,
  kNumPackages
};
// NOTE: Packages will be defined with respect to the *fixed magnet octants*.
// This means that after a rotation of 45 deg from the vertical position,
// one package will be identified as kPackageUpLeft (name?), and the other
// package as kPackageDownRight. (wdc, based on discussion with pking)
inline std::ostream& operator<< (std::ostream& stream, const EQwDetectorPackage& i) {
  stream << "?12#"[i]; return stream;
}
typedef std::map < char, EQwDetectorPackage > QwPackageMap;
inline QwPackageMap CreatePackageMap()
{
  QwPackageMap map;
  map['?'] = kPackageNull;
  map['1'] = kPackage1;
  map['2'] = kPackage2;
  return map;
}
static const QwPackageMap kQwPackageMap = CreatePackageMap();

enum EQwDetectorType {
  kTypeNull = 0,
  kTypeSciFiber,	// Scintillating Fiber detector
  kTypeDriftHDC,	// HDC Drift chamber
  kTypeDriftVDC,	// VDC Drift chamber
  kTypeTrigscint,	// Trigger scintillator
  kTypeCerenkov,	// Cerenkov detector
  kTypeScanner,		// Focal plane scanner
  kNumTypes
};
inline std::ostream& operator<< (std::ostream& stream, const EQwDetectorType& i) {
  stream << "?fhvtcs#"[i]; return stream;
}
typedef std::map < char, EQwDetectorType > QwTypeMap;
inline QwTypeMap CreateTypeMap()
{
  QwTypeMap map;
  map['?'] = kTypeNull;
  map['f'] = kTypeSciFiber;
  map['h'] = kTypeDriftHDC;
  map['v'] = kTypeDriftVDC;
  map['t'] = kTypeTrigscint;
  map['c'] = kTypeCerenkov;
  map['s'] = kTypeScanner;
  return map;
}
static const QwTypeMap kQwTypeMap = CreateTypeMap();

// Enumerator type for the instrument type, used in subsystems that have to
// distinguish between various detector types.
enum EQwPMTInstrumentType {
  kQwUnknownPMT = 0,	// Unknown PMT type
  kQwIntegrationPMT,	// Integration PMT
  kQwScalerPMT,	        // Scaler PMT
  kQwCombinedPMT,	// Combined PMT
  kNumInstrumentTypes   // This should be the last enum; it provides the number of know types.
};

enum EQwBeamInstrumentType {
  kQwUnknownDeviceType = 0,
  kQwBPMStripline,
  kQwQPD,
  kQwLinearArray,
  kQwBCM,
  kQwCombinedBCM,
  kQwCombinedBPM,
  kQwEnergyCalculator,
  kQwHaloMonitor,
  kQwBPMCavity,
  kQwClock,
  kBeamDevTypes  // This should be the last enum; it provides the number of know types.
};

// Enumerator type for the electronics module type
enum EQwModuleType {
  kUnknownModuleType = 0,
  kV775_TDC,
  kV792_ADC,
  kF1TDC,
  kSIS3801,
  kNumModuleTypes
};

//Beamline device errorflags
static const UInt_t kErrorFlag_VQWK_Sat   = 0x01; // in Decimal 1 to identify a VQWK is saturating
static const UInt_t kErrorFlag_sample     = 0x2;  // in Decimal 2   for sample size check
static const UInt_t kErrorFlag_SW_HW      = 0x4;  // in Decimal 4   HW_sum==SW_sum check
static const UInt_t kErrorFlag_Sequence   = 0x8;  // in Decimal 8   sequence number check
static const UInt_t kErrorFlag_SameHW     = 0x10; // in Decimal 16  check to see ADC returning same HW value
static const UInt_t kErrorFlag_ZeroHW     = 0x20; // in Decimal 32  check to see ADC returning zero
static const UInt_t kErrorFlag_EventCut_L = 0x40; // in Decimal 64  check to see ADC failed lower limit of the event cut
static const UInt_t kErrorFlag_EventCut_U = 0x80; // in Decimal 128 check to see ADC failed upper limit of the event cut

static const UInt_t kBCMErrorFlag = 0x100; // in Decimal 256 to identify the single event cut is failed for a BCM (regular or combo)
static const UInt_t kErrorFlag_BlinderFail = 0x200;// in Decimal 512 to identify the blinder flag
static const UInt_t kBPMErrorFlag = 0x400; // in Decimal 1024 to identify the single event cut is failed for a BPM (Stripline or cavity or comboBPM)
static const UInt_t kPMTErrorFlag = 0x800; // in Decimal 2048 to identify the single event cut is failed for a PMT (Combined or regular)
static const UInt_t kBModFFBErrorFlag = 0x1000; // in Decimal 4096 (2^12) to identify the FFB OFF periods for Energy modulation
static const UInt_t kBModErrorFlag = 0x8000; // in Decimal 32768 (2^15) to identify the single event cut is failed for a BMod channel
static const UInt_t kEventCutMode3 = 0x10000;  // in Decimal 65536 to identify the mode 3 where we only flag event cut failed events 
static const UInt_t kBeamStabilityError= 0x10000000;//in Decimal 2^28(268435456) to identify the a stability cut
static const UInt_t kBeamTripError= 0x8000000;// in Decimal 2^27(134217728) to identify the an event within a beam trip range set by ring parameters
static const UInt_t kGlobalCut    = 0x4000000;// in Decimal 2^26 to identify the single event cut is a global cut
static const UInt_t kLocalCut     = 0x2000000;// in Decimal 2^25 to identify the single event cut is a local cut
static const UInt_t kStabilityCut = 0x1000000;// in Decimal 2^24 (16777216) to identify the single event cut is a stability cut. NOT IN USE CURRENTLY
static const UInt_t kPreserveError = 0x2FF;//when AND-ed with this it will only keep HW errors and blinder

//To generate the error code based on global/local and stability cut value
UInt_t GetGlobalErrorFlag(TString evtype,Int_t evMode,Double_t stabilitycut);


EQwPMTInstrumentType GetQwPMTInstrumentType(TString name);
TString GetQwPMTInstrumentTypeName(EQwPMTInstrumentType type);

EQwBeamInstrumentType GetQwBeamInstrumentType(TString name);
TString GetQwBeamInstrumentTypeName(EQwBeamInstrumentType type);

static const UInt_t kInvalidSubelementIndex = 999999;

//=======
// Enumerator increments
inline EQwRegionID& operator++ (EQwRegionID &region, int) {
  return region = EQwRegionID(region + 1);
}
inline EQwDirectionID& operator++ (EQwDirectionID &direction, int) {
  return direction = EQwDirectionID(direction + 1);
}
inline EQwDetectorPackage& operator++ (EQwDetectorPackage &package, int) {
  return package = EQwDetectorPackage(package + 1);
}
inline EQwDetectorType& operator++ (EQwDetectorType &type, int) {
  return type = EQwDetectorType(type + 1);
}


/// Helicity enumerator (don't use this as a signed int)
enum EQwHelicity {
  kHelicityUndefined,
  kHelicityPositive,
  kHelicityNegative
};
/// Use the static map kMapHelicity to get the helicity sign
/// e.g.: kMapHelicity[kHelicityPositive] will return +1
typedef std::map < EQwHelicity, int > QwHelicityMap;
inline QwHelicityMap CreateHelicityMap()
{
  QwHelicityMap map;
  map[kHelicityUndefined] = 0;
  map[kHelicityPositive] = +1;
  map[kHelicityNegative] = -1;
  return map;
}
static const QwHelicityMap kMapHelicity = CreateHelicityMap();


///
/// \ingroup QwAnalysis
class QwDetectorID
{
 public:
  QwDetectorID()
  : fRegion(kRegionIDNull),fPackage(kPackageNull),fOctant(-1),
    fPlane(-1),fDirection(kDirectionNull),fElement(-1) { };

  QwDetectorID(const EQwRegionID region, 
               const EQwDetectorPackage package, 
               const Int_t octant,
               const Int_t plane,
               const EQwDirectionID direction, 
               const Int_t wire)
  : fRegion(region),fPackage(package),fOctant(octant),
    fPlane(plane),fDirection(direction),fElement(wire) { };

 public:
  EQwRegionID        fRegion;    ///< region 1, 2, 3, triggg. scint or cerenkov
  EQwDetectorPackage fPackage;   ///< which arm of the rotator or octant number
  Int_t              fOctant;    ///< octant of this detector
  Int_t              fPlane;     ///< R or theta index for R1; plane index for R2 & R3
  EQwDirectionID     fDirection; ///< direction of the wire plane X,Y,U,V etc - Rakitha (10/23/2008)
  Int_t              fElement;   ///< trace number for R1; wire number for R2 & R3; PMT number for others
  
  friend std::ostream& operator<<(std::ostream& os, const QwDetectorID &detectorID) {
    os << " Region ";
    os <<  detectorID.fRegion;
    os << " Package ";
    os << detectorID.fPackage;
    os << " Octant ";
    os << detectorID.fOctant;
    os << " Plane ";
    os << detectorID.fPlane;
    os << " Direction";
    os <<detectorID.fDirection;
    os << " fElement ";
    os << detectorID.fElement;

    return os;
  }
};



///
/// \ingroup QwAnalysis
class QwElectronicsID
{
 public:
 QwElectronicsID():fModule(-1),fChannel(-1){};
 QwElectronicsID(const int slot,const int chan):fModule(slot),fChannel(chan){};

 public:
  int fModule;       //F1TDC slot number or module number
  int fChannel;      //channel number
};


///
/// \ingroup QwAnalysis
class QwDelayLineID{
 public:
 QwDelayLineID():fBackPlane(-1),fLineNumber(-1),fSide(-1){};
 QwDelayLineID(const int backplane, const int linenumber, const int side):fBackPlane(backplane),fLineNumber(linenumber),fSide(side){};

 Int_t fBackPlane;
 Int_t fLineNumber;
 Int_t fSide;
};

///  Double Wien configuration
enum EQwWienMode {
  kWienIndeterminate = 0,
  kWienForward,
  kWienBackward, 
  kWienVertTrans,
  kWienHorizTrans
};
std::string WienModeName(EQwWienMode type);
EQwWienMode WienModeIndex(TString name);

///  Definitions for beam parameter quantities; use these types rather than
///  the raw "QwVQWK_Channel" to allow for future specification.
class QwVQWK_Channel;
typedef class QwVQWK_Channel QwBeamCharge;
typedef class QwVQWK_Channel QwBeamPosition;
typedef class QwVQWK_Channel QwBeamAngle;
typedef class QwVQWK_Channel QwBeamEnergy;

#endif
