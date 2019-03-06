/**********************************************************\
* File: QwADC18_Channel.h                                  *
*                                                          *
* Author: P. M. King, W. Deconinck, B. Michaels            *
\**********************************************************/

#ifndef __QwADC18_CHANNEL__
#define __QwADC18_CHANNEL__

// System headers
#include <vector>

// Qweak headers
#include "VQwHardwareChannel.h"
#include "MQwMockable.h"

// Forward declarations
class TTree;
class QwBlinder;
class QwParameterFile;
#ifdef __USE_DATABASE__
class QwErrDBInterface;
#endif

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwADC18_Channel: public VQwHardwareChannel, public MQwMockable {
/****************************************************************//**
 *  Class: QwADC18_Channel
 *         Base class containing decoding functions for the HAPPEX 18-bit ADC
 *         The functions in this class will decode a single channel
 *         worth of ADC18_Channel data and provide the components
 *         through member functions.
 ******************************************************************/
 public:
  static Int_t GetBufferOffset(Int_t moduleindex, Int_t channelindex);
  static void  PrintErrorCounterHead();
  static void  PrintErrorCounterTail();

  static const Double_t kTimePerSample;

  using MQwMockable::LoadMockDataParameters;

  using VQwHardwareChannel::GetRawValue;
  using VQwHardwareChannel::GetValue;
  using VQwHardwareChannel::GetValueM2;
  using VQwHardwareChannel::GetValueError;
  using VQwHardwareChannel::GetValueWidth;

  using VQwHardwareChannel::AccumulateRunningSum;
  using VQwHardwareChannel::DeaccumulateRunningSum;

 public:
  QwADC18_Channel(): MQwMockable() {
    InitializeChannel("","");
    SetADC18SaturationLimt(8.5);//FIXME set the default saturation limit
  };
  QwADC18_Channel(TString name, TString datatosave = "raw"): MQwMockable() {
    InitializeChannel(name, datatosave);
    SetADC18SaturationLimt(8.5);//FIXME set the default saturation limit
  };
  QwADC18_Channel(const QwADC18_Channel& value):
    VQwHardwareChannel(value), MQwMockable(value),
    fNumberOfSamples_map(value.fNumberOfSamples_map),
    fSaturationABSLimit(value.fSaturationABSLimit)
  {
    *this = value;
  };
  QwADC18_Channel(const QwADC18_Channel& value, VQwDataElement::EDataToSave datatosave):
    VQwHardwareChannel(value,datatosave), MQwMockable(value),
    fNumberOfSamples_map(value.fNumberOfSamples_map),
    fSaturationABSLimit(value.fSaturationABSLimit)
  {
    *this = value;
  };
  virtual ~QwADC18_Channel() { };

  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString name, TString datatosave);

  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave);

  void LoadChannelParameters(QwParameterFile &paramfile);

  // Will update the default sample size for the module.
  void SetDefaultSampleSize(size_t num_samples_map) {
    // This will be checked against the no.of samples read by the module
    fNumberOfSamples_map = num_samples_map;
  };

  void  ClearEventData();

  /// Internally generate random event data
  void  RandomizeEventData(int helicity = 0.0, double time = 0.0);

  /// Forces the event "number of samples" varible to be what was expected from the mapfile.
  /// NOTE: this should only be used in mock data generation!
  void  ForceMapfileSampleSize() {fNumberOfSamples = fNumberOfSamples_map;};

//------------------------------------------------------------------------------------------
  virtual void SmearByResolution(double resolution);
//------------------------------------------------------------------------------------------

  void  SetEventData(Double_t value);
  void  SetRawEventData();

  /// Encode the event data into a CODA buffer
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  /// Decode the event data from a CODA buffer
  Bool_t IsHeaderWord(UInt_t word) const;
  Bool_t IsDataWord(UInt_t word) const;
  UInt_t ProcessDACWord(UInt_t word) const;
  UInt_t ProcessPeakBaseWord(UInt_t word) const;
  std::pair<Int_t,UInt_t> ProcessDiffDivWord(UInt_t word) const;
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0);

  void PrettyPrintBuffer(UInt_t* buffer, UInt_t length, UInt_t wrap = 8) const;

  /// Process the event data according to pedestal and calibration factor
  void  ProcessEvent();

  QwADC18_Channel& operator=  (const QwADC18_Channel &value);
  void AssignScaledValue(const QwADC18_Channel &value, Double_t scale);
  void AssignValueFrom(const VQwDataElement* valueptr);
  void AddValueFrom(const VQwHardwareChannel* valueptr);
  void SubtractValueFrom(const VQwHardwareChannel* valueptr);
  void MultiplyBy(const VQwHardwareChannel* valueptr);
  void DivideBy(const VQwHardwareChannel* valueptr);
  void ArcTan(const QwADC18_Channel &value);

  QwADC18_Channel& operator+= (const QwADC18_Channel &value);
  QwADC18_Channel& operator-= (const QwADC18_Channel &value);
  QwADC18_Channel& operator*= (const QwADC18_Channel &value);

  VQwHardwareChannel& operator+=(const VQwHardwareChannel* input);
  VQwHardwareChannel& operator-=(const VQwHardwareChannel* input);
  VQwHardwareChannel& operator*=(const VQwHardwareChannel* input);
  VQwHardwareChannel& operator/=(const VQwHardwareChannel* input);

  const QwADC18_Channel operator+ (const QwADC18_Channel &value) const;
  const QwADC18_Channel operator- (const QwADC18_Channel &value) const;
  const QwADC18_Channel operator* (const QwADC18_Channel &value) const;
  void Sum(const QwADC18_Channel &value1, const QwADC18_Channel &value2);
  void Difference(const QwADC18_Channel &value1, const QwADC18_Channel &value2);
  void Ratio(const QwADC18_Channel &numer, const QwADC18_Channel &denom);
  void Product(const QwADC18_Channel &value1, const QwADC18_Channel &value2);
  void DivideBy(const QwADC18_Channel& denom);
  void AddChannelOffset(Double_t Offset);
  void Scale(Double_t Offset);

  inline void AccumulateRunningSum(const QwADC18_Channel& value){
    AccumulateRunningSum(value, value.fGoodEventCount);
  }
  void AccumulateRunningSum(const QwADC18_Channel& value, Int_t count);
  void AccumulateRunningSum(const VQwHardwareChannel *value, Int_t count){
    const QwADC18_Channel *tmp_ptr = dynamic_cast<const QwADC18_Channel*>(value);
    if (tmp_ptr != NULL) AccumulateRunningSum(*tmp_ptr, count);
  };
  ////deaccumulate one value from the running sum
  inline void DeaccumulateRunningSum(const QwADC18_Channel& value){
    AccumulateRunningSum(value, -1);
  };

  void CalculateRunningAverage();

  Bool_t MatchSequenceNumber(size_t seqnum);
  Bool_t MatchNumberOfSamples(size_t numsamp);

  /*Event cut related routines*/
  //check values read from modules are at desired level
  Bool_t ApplySingleEventCuts(Double_t LL, Double_t UL);
  //check values read from modules are at desired level by comparing upper and lower limits (fULimit and fLLimit) set on this channel
  Bool_t ApplySingleEventCuts();
  // report number of events failed due to HW and event cut faliure
  void PrintErrorCounters() const;

  // FIXME Set the absolute staturation limit in volts
  void SetADC18SaturationLimt(Double_t sat_volts = 8.5){
    fSaturationABSLimit = sat_volts;
  }

  // Get the absolute staturation limit in volts
  Double_t GetADC18SaturationLimt(){
    return fSaturationABSLimit;
  }


  // Check for harware errors in the devices. This will return the device error code.
  Int_t ApplyHWChecks();

  // Update the error counters based on the internal fErrorFlag
  void IncrementErrorCounters();

  /*End*/

  Int_t GetRawValue(size_t element) const      { return fValue_Raw; };
  Double_t GetValue(size_t element) const      { return fValue; };
  Double_t GetValueM2(size_t element) const    { return fValueM2; };
  Double_t GetValueError(size_t element) const { return fValueError; };

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  Double_t GetAverageVolts() const;

  size_t GetSequenceNumber() const {return (fSequenceNumber);};
  size_t GetNumberOfSamples() const {return (fNumberOfSamples);};

  void   SetCalibrationToVolts(){SetCalibrationFactor(kADC18_VoltsPerBit);};

  friend std::ostream& operator<< (std::ostream& stream, const QwADC18_Channel& channel);
  void PrintValue() const;
  void PrintInfo() const;

  /// \brief Blind this channel as an asymmetry
  void Blind(const QwBlinder *blinder);
  /// \brief Blind this channel as a difference
  void Blind(const QwBlinder *blinder, const QwADC18_Channel& yield);

  void ScaledAdd(Double_t scale, const VQwHardwareChannel *value);

#ifdef __USE_DATABASE__
  // Error Counters exist in QwADC18_Channel, not in VQwHardwareChannel
  //
  void AddErrEntriesToList(std::vector<QwErrDBInterface> &row_list);
#endif

 protected:
  QwADC18_Channel& operator/= (const QwADC18_Channel &value);

 private:

  // Connection to header
  static QwADC18_Channel* fCurrentHeader;
  const QwADC18_Channel* fHeader;

  // Module status
  Bool_t fModuleStatus;
  UInt_t fModuleNumber;
  UInt_t fModuleCSR;

  // Raw quantities
  Int_t  fDiff_Raw;
  UInt_t fDiff_Div;
  UInt_t fBase_Raw;
  UInt_t fPeak_Raw;
  // Diff with a divider must be floating point
  Double_t fValue_Raw; // = fDiff_Raw / fDiff_Div

  // Processed quantities
  Double_t fValue;
  Double_t fValueM2;
  Double_t fValueError;

  // Previous values
  UInt_t fBase_Raw_Prev, fBase_Raw_Same_Count;
  UInt_t fPeak_Raw_Prev, fPeak_Raw_Same_Count;

 private:
  static const Bool_t kDEBUG;
  static const Int_t  kHeaderWordsPerBank; //no.of header words per bank in the CODA buffer
  static const Int_t  kFooterWordsPerBank; //no.of footer words per bank in the CODA buffer
  static const Int_t  kHeaderWordsPerModule; //no.of header words per module in the CODA buffer
  static const Int_t  kFooterWordsPerModule; //no.of footer words per module in the CODA buffer
  static const Int_t  kDataWordsPerChannel; //no.of data words per channel in the CODA buffer
  static const Int_t  kMaxChannels;     //no.of channels per module

 private:
  static const UInt_t mask31x;   // = 0x80000000;   // Header bit mask
  static const UInt_t mask3026x; // = 0x7c000000;   // Module number mask
  static const UInt_t mask250x;  // = 0x03ffffff;   // Event number mask
  static const UInt_t mask3029x; // = 0x60000000;   // Channel number mask
  static const UInt_t mask2625x; // = 0x06000000;   // Divider value mask
  static const UInt_t mask2422x; // = 0x01c00000;   // Data type mask
  static const UInt_t mask21x;   // = 0x00200000;   // Data type 0 value sign mask
  static const UInt_t mask200x;  // = 0x001fffff;   // Data type 0 value field mask
  static const UInt_t mask2118x; // = 0x003c0000;   // Data types 1-2 sample number mask
  static const UInt_t mask170x;  // = 0x0003ffff;   // Data types 1-2 value field mask
  static const UInt_t mask150x;  // = 0x0000ffff;   // Data type 4 value field mask


  /// Pointer to the running sum for this channel
  QwADC18_Channel* fRunningSum;

  /*! \name ADC Calibration                    */
  // @{
  static const Double_t kADC18_VoltsPerBit;
  //@}


  /*! \name Channel information data members   */

  /*! \name Channel configuration data members */
  // @{

  size_t fSequenceNumber;      ///< Event sequence number for this channel
  size_t fPreviousSequenceNumber; ///< Previous event sequence number for this channel
  size_t fNumberOfSamples;     ///< Number of samples read by the module
  size_t fNumberOfSamples_map; ///< Number of samples expected to be read by the module

  // Set of error counters for each HW test.
  Int_t fErrorCount_HWSat;    ///< check to see ADC channel is saturated
  Int_t fErrorCount_sample;   ///< for sample size check
  Int_t fErrorCount_SW_HW;    ///< HW_diff == SW_diff check
  Int_t fErrorCount_CSR;      ///< Unexpected CSR
  Int_t fErrorCount_Bad0;     ///< Bad0 data
  Int_t fErrorCount_Bad1;     ///< Bad1 data
  Int_t fErrorCount_Sequence; ///< sequence number check
  Int_t fErrorCount_SameHW;   ///< check to see ADC peak or base returning same HW value
  Int_t fErrorCount_ZeroHW;   ///< check to see ADC peak or base returning zero


  Int_t fNumEvtsWithEventCutsRejected; ///< Counts the Event cut rejected events



  Int_t fADC_Same_NumEvt; ///< Keep track of how many events with same ADC value returned
  Int_t fSequenceNo_Prev; ///< Keep the sequence number of the last event
  Int_t fSequenceNo_Counter; ///< Internal counter to keep track of the sequence number



  Double_t fSaturationABSLimit;///<absolute value of the ADC18 saturation volt


  const static Bool_t bDEBUG=kTRUE;///<debugging display purposes

  ///<For ADC18 data element trimming uses
  Bool_t bHw_sum;
  Bool_t bHw_sum_raw;
  Bool_t bBlock;
  Bool_t bBlock_raw;
  Bool_t bNum_samples;
  Bool_t bDevice_Error_Code;
  Bool_t bSequence_number;

};

#endif
