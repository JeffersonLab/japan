/*!
 * \file   VQwDataElement.h
 * \brief  Definition of the pure virtual base class of all data elements
 *
 * \author P. M. King
 * \date   2007-05-08 15:40
 */

#ifndef __VQWDATAELEMENT__
#define __VQWDATAELEMENT__

// System headers
#include <vector>
#include <iostream>

// Root headers
#include "Rtypes.h"
#include "TString.h"
#include "TDirectory.h"

// Qweak headers
#include "QwLog.h"
#include "QwTypes.h"
#include "MQwHistograms.h"

class QwParameterFile;
class VQwHardwareChannel;

/**
 *  \class   VQwDataElement
 *  \ingroup QwAnalysis
 *  \brief   The pure virtual base class of all data elements
 *
 * Each stream of data inherits from this virtual base class, which requires
 * some standard operations on it such as ratios, summing, subtraction.  The
 * specific implementation of those operation is left to be implemented by the
 * implemented inherited classes, but this class sets up the structure.
 *
 * As an example, all individual VQWK channels inherit from this class and
 * implement the pure virtual functions of VQwDataElement.
 *
 * \dot
 * digraph example {
 *   node [shape=box, fontname=Helvetica, fontsize=10];
 *   VQwDataElement [ label="VQwDataElement" URL="\ref VQwDataElement"];
 *   QwVQWK_Channel [ label="QwVQWK_Channel" URL="\ref QwVQWK_Channel"];
 *   VQwDataElement -> QwVQWK_Channel;
 * }
 * \enddot
 */
class VQwDataElement: public MQwHistograms {
 public:
  /// Flag to be used to decide which data needs to be histogrammed and
  /// entered in the tree
  enum EDataToSave {kRaw = 0, kDerived, kMoments};


 public:

  /// Default constructor
  VQwDataElement()
  : MQwHistograms(),
    fElementName(""),
    fNumberOfDataWords(0),
    fGoodEventCount(0),
    fSubsystemName(""),
    fModuleType(""),
    fErrorFlag(0),
    fErrorConfigFlag(0)
    { };
  /// Copy constructor
  VQwDataElement(const VQwDataElement& value)
  : MQwHistograms(value),
    fElementName(value.fElementName),
    fNumberOfDataWords(value.fNumberOfDataWords),
    fGoodEventCount(value.fGoodEventCount),
    fSubsystemName(value.fSubsystemName),
    fModuleType(value.fModuleType),
    fErrorFlag(value.fErrorFlag),
    fErrorConfigFlag(value.fErrorConfigFlag)
    { };
  /// Virtual destructor
  virtual ~VQwDataElement() { };

  virtual void CopyFrom(const VQwDataElement& value){
    fElementName       = value.fElementName;
    //    fNumberOfDataWords = value.fNumberOfDataWords;
    fGoodEventCount    = value.fGoodEventCount;
    fSubsystemName     = value.fSubsystemName;
    fModuleType        = value.fModuleType;
    fErrorFlag         = value.fErrorFlag;
    fErrorConfigFlag   = value.fErrorConfigFlag;
  }

  /*! \brief Is the name of this element empty? */
  Bool_t IsNameEmpty() const { return fElementName.IsNull(); }
  /*! \brief Set the name of this element */
  void SetElementName(const TString &name) { fElementName = name; }
  /*! \brief Get the name of this element */
  virtual const TString& GetElementName() const { return fElementName; }

  virtual void LoadChannelParameters(QwParameterFile &paramfile){};

  virtual void LoadMockDataParameters(QwParameterFile &paramfile) {
  std::cerr << "LoadMockDataParameters is not defined!" << std::endl;
  };

  /*! \brief Clear the event data in this element */
  virtual void  ClearEventData(){
    fErrorFlag=0;
  };
  /*! \brief Process the CODA event buffer for this element */
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement=0) = 0;

  /*! \brief Get the number of data words in this data element */
  size_t GetNumberOfDataWords() {return fNumberOfDataWords;}

  UInt_t GetGoodEventCount() const { return fGoodEventCount; };

  
  virtual void AssignValueFrom(const VQwDataElement* valueptr){
    std::cerr << "Operation AssignValueFrom not defined!" << std::endl;
  };

  /*! \brief Addition-assignment operator */
  virtual VQwDataElement& operator+= (const VQwDataElement &value)
    { std::cerr << "Operation += not defined!" << std::endl; return *this; }
  /*! \brief Subtraction-assignment operator */
  virtual VQwDataElement& operator-= (const VQwDataElement &value)
    { std::cerr << "Operation -= not defined!" << std::endl; return *this; }

  /*! \brief Sum operator */
  virtual void Sum(const VQwDataElement &value1, const VQwDataElement &value2)
    { std::cerr << "Sum not defined!" << std::endl; }
  /*! \brief Difference operator */
  virtual void Difference(const VQwDataElement &value1, const VQwDataElement &value2)
    { std::cerr << "Difference not defined!" << std::endl; }
  /*! \brief Ratio operator */
  virtual void Ratio(const VQwDataElement &numer, const VQwDataElement &denom)
  { std::cerr << "Ratio not defined for element"<< fElementName<< "!" << std::endl; }

  /*! \brief Construct the histograms for this data element */
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  /*! \brief Fill the histograms for this data element */
  virtual void  FillHistograms() = 0;

  /*! \brief Print single line of value and error of this data element */
  virtual void PrintValue() const { }
  /*! \brief Print multiple lines of information about this data element */
  virtual void PrintInfo() const { std::cout << GetElementName() << std::endl; }


  /*! \brief set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  virtual void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability){std::cerr << "SetSingleEventCuts not defined!" << std::endl; };
  /*! \brief report number of events failed due to HW and event cut failure */
  virtual void PrintErrorCounters() const {};

  virtual Bool_t  CheckForBurpFail(const VQwDataElement *ev_error){
    return kFALSE;
  };

  /*! \brief return the error flag on this channel/device*/
  virtual UInt_t GetEventcutErrorFlag(){
    //first condition check for global/local status and second condition check to see non-zero HW error codes
    if (((fErrorConfigFlag & kGlobalCut) == kGlobalCut) && (fErrorFlag)>0){
      // we care only about global cuts
      //std::cout<<"fErrorFlag "<<(fErrorFlag & kGlobalCut)<<std::endl;
      return fErrorFlag+fErrorConfigFlag;//pass the error codes and configuration codes
    }
    return 0;
  }

  /// \brief Update the error flag based on the error flags of internally
  ///        contained objects
  ///        Return paramter is the "Eventcut Error Flag".
  virtual UInt_t UpdateErrorFlag() {return GetEventcutErrorFlag();};

  // These are related to those hardware channels that need to normalize
  // to an external clock
  virtual void SetNeedsExternalClock(Bool_t needed) {};   // Default is No!
  virtual Bool_t NeedsExternalClock() { return kFALSE; }; // Default is No!
  virtual std::string GetExternalClockName() {  return ""; }; // Default is none
  virtual void SetExternalClockPtr( const VQwHardwareChannel* clock) {};
  virtual void SetExternalClockName( const std::string name) {};
  virtual Double_t GetNormClockValue() { return 1.;}

  
  
  /*! \brief Return the name of the inheriting subsystem name*/
  TString GetSubsystemName() const {
    return fSubsystemName;
  }

   /*! \brief Set the name of the inheriting subsystem name*/
  void SetSubsystemName(TString sysname){
    fSubsystemName=sysname;
  }
  
   /*! \brief Return the type of the beam instrument*/
  TString GetModuleType() const {
    return fModuleType;
  }

   /*! \brief set the type of the beam instrument*/
  void SetModuleType(TString ModuleType){
    fModuleType=ModuleType;
  }

 protected:
  /*! \brief Set the number of data words in this data element */
  void SetNumberOfDataWords(const UInt_t &numwords) {fNumberOfDataWords = numwords;}

  /// Arithmetic assignment operator:  Should only copy event-based data
  virtual VQwDataElement& operator=(const VQwDataElement& value) {
    if(this != &value){
      MQwHistograms::operator=(value);
      fGoodEventCount    = value.fGoodEventCount;
      fErrorFlag         = value.fErrorFlag;
    }
    return *this;
  }

  //  The most basic version of UpdateErrorFlag, which should get hidden
  //  by all the derived class versions.
  virtual void UpdateErrorFlag(const UInt_t& error){fErrorFlag |= (error);};

 protected:
  TString fElementName; ///< Name of this data element
  UInt_t  fNumberOfDataWords; ///< Number of raw data words in this data element
  Int_t fGoodEventCount; ///< Number of good events accumulated in this element


  // Name of the inheriting subsystem
  TString  fSubsystemName;
  // Data module Type
  TString  fModuleType;

  /*! \name Event error flag                    */
  /*! \brief This the standard error code generated for the channel that contains the global/local/stability flags and the Device error code (Unique error code for HW failures)*/
// @{
  UInt_t fErrorFlag;
  UInt_t fErrorConfigFlag; ///<contains the global/local/stability flags
//@}
}; // class VQwDataElement

#endif // __VQWDATAELEMENT__
