/**********************************************************\
* File: VQwBCM.h                                           *
*                                                          *
* Author: (??) & J.C Cornejo                               *
* Time-stamp: <2011-05-26>                                 *
\**********************************************************/

#ifndef __VQWBCM__
#define __VQWBCM__

// System headers
#include <vector>
#include <boost/shared_ptr.hpp>

// ROOT headers
#include <TTree.h>

#include "QwParameterFile.h"
#include "VQwDataElement.h"
#include "VQwHardwareChannel.h"

// Forward declarations
#ifdef __USE_DATABASE__
class QwDBInterface;
class QwErrDBInterface;
#endif // __USE_DATABASE__

template<typename T> class QwCombinedBCM;
template<typename T> class QwBCM;

/**
 * \ingroup QwAnalysis_BeamLine
 */
class VQwBCM : public VQwDataElement {
  /***************************************************************
   *  Class:  VQwBCM
   *          Pure Virtual base class for the BCMs in the beamline.
   *          Through use of the Create factory function, one can 
   *          get a concrete instance of a templated QwBCM.
   *
   ***************************************************************/
  template <typename T> friend class QwBCM;
  template <typename T> friend class QwCombinedBCM;

protected:
  VQwBCM(VQwDataElement& beamcurrent): fBeamCurrent_ref(beamcurrent) { };
  VQwBCM(VQwDataElement& beamcurrent, TString name): fBeamCurrent_ref(beamcurrent) { };

public:
  virtual ~VQwBCM() { };

  // VQwDataElement virtual functions
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0) = 0;
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  virtual void  FillHistograms() = 0;
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  virtual void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability, Double_t burplevel) = 0;
  virtual void Ratio( const VQwBCM &numer, const VQwBCM &denom)
    { std::cerr << "Ratio not defined! (VQwBCM)" << std::endl; }
  virtual void ClearEventData() = 0;

  // Virtual functions delegated to sub classes
  virtual void  InitializeChannel(TString name, TString datatosave) = 0;
  // new routine added to update necessary information for tree trimming
  virtual void  InitializeChannel(TString subsystem, TString name, TString datatosave) = 0;

  virtual void LoadChannelParameters(QwParameterFile &paramfile) = 0;
  virtual Bool_t NeedsExternalClock() = 0;
  virtual void SetExternalClockPtr( const VQwHardwareChannel* clock) = 0;
  virtual void SetExternalClockName( const std::string name) = 0;
  virtual Double_t GetNormClockValue() = 0;

  virtual void SetDefaultSampleSize(Int_t sample_size) = 0;
  virtual void SetEventCutMode(Int_t bcuts) = 0;
  virtual UInt_t UpdateErrorFlag(){return this->GetEventcutErrorFlag();};
  virtual void UpdateErrorFlag(const VQwBCM *ev_error) = 0;
  virtual void SetPedestal(Double_t ped) = 0;
  virtual void SetCalibrationFactor(Double_t calib) = 0;
  virtual void RandomizeEventData(int helicity, double time) = 0;
  virtual void EncodeEventData(std::vector<UInt_t> &buffer) = 0;
  virtual Bool_t ApplySingleEventCuts() = 0;//Check for good events by setting limits on the devices readings
  virtual void IncrementErrorCounters() = 0;
  virtual void  ProcessEvent() = 0;
  virtual void Scale(Double_t factor) = 0;
  virtual void CalculateRunningAverage() = 0;
  virtual void AccumulateRunningSum(const VQwBCM& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF) = 0;
  virtual void DeaccumulateRunningSum(VQwBCM& value, Int_t ErrorMask=0xFFFFFFF) = 0;
  virtual void ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values) = 0;
  virtual void ConstructBranch(TTree *tree, TString &prefix) = 0;
  virtual void ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist) = 0;
  virtual void FillTreeVector(std::vector<Double_t> &values) const = 0;

//----------------------------------------------------------------------------------------------------------------
  virtual void ApplyResolutionSmearing()
    {std::cerr << "ApplyResolutionSmearing is undefined!!!\n";}
  virtual void    FillRawEventData()
    {std::cerr << "FillRawEventData for VQwBPM not implemented!\n";};
  virtual void GetProjectedCharge(VQwBCM *device){};
  virtual size_t GetNumberOfElements(){return size_t(1);}
  virtual TString GetSubElementName(Int_t subindex) 
  {
    std::cerr << "GetSubElementName()  is not implemented!! for device: " << GetElementName() << "\n";
    return TString("OBJECT_UNDEFINED"); // Return an erroneous TString
  }
//----------------------------------------------------------------------------------------------------------------

#ifdef __USE_DATABASE__
  virtual std::vector<QwDBInterface> GetDBEntry() = 0;
  virtual std::vector<QwErrDBInterface> GetErrDBEntry() = 0;
#endif // __USE_DATABASE__

  virtual Double_t GetValue() = 0;
  virtual Double_t GetValueError() = 0;
  virtual Double_t GetValueWidth() = 0;

//------------------------------------------------------------------------------------------------------------------------------------------

  //  virtual const VQwHardwareChannel* GetCharge() const = 0; //Replace this with a const_cast use of the protected function; define a pure virtual protected function that is non-const; In the QwBCM class, change the defined function to be the protected non-const version.

  virtual const VQwHardwareChannel* GetCharge() const{
    return const_cast<VQwBCM*>(this)->GetCharge();
  }

protected:
  virtual VQwHardwareChannel* GetCharge() = 0;
  
//------------------------------------------------------------------------------------------------------------------------------------------

public:

  virtual void  SetRandomEventParameters(Double_t mean, Double_t sigma) = 0;
  virtual void  SetRandomEventAsymmetry(Double_t asymmetry) = 0 ;
  virtual void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency) =0;

  // Operators
  virtual VQwBCM& operator=  (const VQwBCM &value) =0;
  virtual VQwBCM& operator+= (const VQwBCM &value) =0;
  virtual VQwBCM& operator-= (const VQwBCM &value) =0;

  // This one is for QwCombinedBCM (could be done differently)
  virtual void SetBCMForCombo(VQwBCM* bcm, Double_t weight, Double_t sumqw ) = 0;


  // Factory function to produce appropriate BCM
  static VQwBCM* Create(TString subsystemname, TString type, TString name, TString clock = "");
  static VQwBCM* Create(const VQwBCM& source); // Create a generic BCM (define properties later)
  static VQwBCM* CreateCombo(TString subsystemname, TString type, TString name);
  static VQwBCM* CreateCombo(const VQwBCM& source); // Create a generic BCM (define properties later)


protected:
  VQwDataElement& fBeamCurrent_ref;

};

typedef boost::shared_ptr<VQwBCM> VQwBCM_ptr;

#endif // __VQWBCM__
