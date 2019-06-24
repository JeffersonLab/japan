/********************************************************\
* File: VQwClock.h                                       *
*                                                        *
* Author: Juan Carlos Cornejo <cornejo@jlab.org>         *
* Time-stamp: <2011-06-16>                               *
\********************************************************/

#ifndef __VQWCLOCK__
#define __VQWCLOCK__

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
#endif // __USE_DATABASE__

template<typename T> class QwClock;

/**
 * \ingroup QwAnalysis_BeamLine
 */
class VQwClock : public VQwDataElement {
  /***************************************************************
   *  Class:  VQwClock
   *          Pure Virtual base class for the clocks in the datastream
   *          Through use of the Create factory function, one can 
   *          get a concrete instance of a templated QwClock.
   *
   ***************************************************************/
public:
  VQwClock() { }; // Do not use this function!!
  VQwClock(const VQwClock& source)
  : VQwDataElement(source)
  { }
  virtual ~VQwClock() {};

  // VQwDataElement virtual functions
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0) = 0;
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  virtual void  FillHistograms() = 0;
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  virtual void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability, Double_t burplevel) = 0;
  virtual void Ratio( const VQwClock &numer, const VQwClock &denom)
    { std::cerr << "Ratio not defined! (VQwClock)" << std::endl; }
  virtual void ClearEventData() = 0;

  // Virtual functions delegated to sub classes
  virtual void  InitializeChannel(TString subsystem, TString name, TString datatosave, TString type = "") = 0;

  virtual void LoadChannelParameters(QwParameterFile &paramfile) = 0;

  virtual void SetEventCutMode(Int_t bcuts) = 0;
  virtual void SetPedestal(Double_t ped) = 0;
  virtual void SetCalibrationFactor(Double_t calib) = 0;
  virtual Bool_t ApplySingleEventCuts() = 0;//Check for good events by stting limits on the devices readings
  virtual void IncrementErrorCounters() = 0;
  virtual void  ProcessEvent() = 0;
  virtual void Scale(Double_t factor) = 0;
  virtual void CalculateRunningAverage() = 0;
  virtual void AccumulateRunningSum(const VQwClock& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF) = 0;
  virtual void DeaccumulateRunningSum(VQwClock& value, Int_t ErrorMask=0xFFFFFFF) = 0;
  virtual void ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values) = 0;
  virtual void ConstructBranch(TTree *tree, TString &prefix) = 0;
  virtual void ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist) = 0;
  virtual void FillTreeVector(std::vector<Double_t> &values) const = 0;

#ifdef __USE_DATABASE__
  virtual std::vector<QwDBInterface> GetDBEntry() = 0;
#endif // __USE_DATABASE__

  // Operators
  virtual VQwClock& operator=  (const VQwClock &value) =0;
  virtual VQwClock& operator+= (const VQwClock &value) =0;
  virtual VQwClock& operator-= (const VQwClock &value) =0;

  // Factory function to produce appropriate Clock
  static VQwClock* Create(TString subsystemname, TString type, TString name);
  static VQwClock* Create(const VQwClock& source);

  // These are related to those hardware channels that need to normalize
  // to an external clock
  virtual Double_t GetNormClockValue() = 0;
  virtual Double_t GetStandardClockValue() = 0;

  virtual const VQwHardwareChannel* GetTime() const = 0;

};

typedef boost::shared_ptr<VQwClock> VQwClock_ptr;

#endif // __VQWCLOCK__
