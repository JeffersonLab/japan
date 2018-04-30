/*!
 * \file   VQwSubsystem.h
 * \brief  Definition of the pure virtual base class of all subsystems
 *
 * \author P. M. King
 * \date   2007-05-08 15:40
 */

#ifndef __VQWSUBSYSTEM__
#define __VQWSUBSYSTEM__

// System headers
#include <iostream>
#include <vector>

// ROOT headers
#include "Rtypes.h"
#include "TString.h"
#include "TDirectory.h"
#include "TTree.h"

// Qweak headers
#include "MQwHistograms.h"
// Note: the factory header is included here because every subsystem
// has to register itself with a subsystem factory.
#include "QwFactory.h"

// Forward declarations
class VQwHardwareChannel;
class QwSubsystemArray;
class QwParameterFile;


/**
 *  \class   VQwSubsystem
 *  \ingroup QwAnalysis
 *  \brief   The pure virtual base class of all subsystems
 *
 * Virtual base class for the classes containing the event-based
 * information from each tracking subsystem and the parity analysis.
 * The subclasses VQwSubsystemTracking and VQwSubsystemParity are base
 * classes for the classes containing the event-based information
 * from each tracking subsystem and parity analysis, respectively.
 *
 * \dot
 * digraph example {
 *   node [shape=box, fontname=Helvetica, fontsize=10];
 *   VQwSubsystem [ label="VQwSubsystem" URL="\ref VQwSubsystem"];
 *   VQwSubsystemParity [ label="VQwSubsystemParity" URL="\ref VQwSubsystemParity"];
 *   VQwSubsystemTracking [ label="VQwSubsystemTracking" URL="\ref VQwSubsystemTracking"];
 *   VQwSubsystem -> VQwSubsystemParity;
 *   VQwSubsystem -> VQwSubsystemTracking;
 * }
 * \enddot
 *
 * This will define the interfaces used in communicating with the
 * CODA routines.
 */
class VQwSubsystem: virtual public VQwSubsystemCloneable, public MQwHistograms {

 public:

  /// Constructor with name
  VQwSubsystem(const TString& name)
  : MQwHistograms(),
    fSystemName(name), fEventTypeMask(0x0), fIsDataLoaded(kFALSE),
    fCurrentROC_ID(-1), fCurrentBank_ID(-1) {
    ClearAllBankRegistrations();
  }
  /// Copy constructor by object
  VQwSubsystem(const VQwSubsystem& orig)
  : MQwHistograms(orig),
    fPublishList(orig.fPublishList),
    fROC_IDs(orig.fROC_IDs),
    fBank_IDs(orig.fBank_IDs)
  {
    fSystemName = orig.fSystemName;
    fIsDataLoaded = orig.fIsDataLoaded;
    fCurrentROC_ID = orig.fCurrentROC_ID;
    fCurrentBank_ID = orig.fCurrentBank_ID;
  }

  /// Default destructor
  virtual ~VQwSubsystem() { }


  /// \brief Define options function (note: no virtual static functions in C++)
  static void DefineOptions() { /* No default options defined */ };
  /// Process the command line options
  virtual void ProcessOptions(QwOptions &options) { };


  TString GetSubsystemName() const {return fSystemName;};
  Bool_t  HasDataLoaded() const  {return fIsDataLoaded;}

  /// \brief Set the parent of this subsystem to the specified array
  void SetParent(QwSubsystemArray* parent);
  /// \brief Get the parent of this subsystem
  QwSubsystemArray* GetParent(const unsigned int parent = 0) const;
  /// \brief Get the sibling with specified name
  VQwSubsystem* GetSibling(const std::string& name) const;


 public:

  /// \brief Publish a variable name to the parent subsystem array
  Bool_t PublishInternalValue(const TString& name, const TString& desc, const VQwHardwareChannel* value) const;
  /// \brief Publish all variables of the subsystem
  virtual Bool_t PublishInternalValues() const {
    return kTRUE; // when not implemented, this returns success
  };
  /// \brief Try to publish an internal variable matching the submitted name
  virtual Bool_t PublishByRequest(TString device_name){
    return kFALSE; // when not implemented, this returns failure
  };

  /// \brief Request a named value which is owned by an external subsystem;
  ///        the request will be handled by the parent subsystem array
  Bool_t RequestExternalValue(const TString& name, VQwHardwareChannel* value) const;

  /// \brief Return a pointer to a varialbe to the parent subsystem array to be
  ///        delivered to a different subsystem.
  
  virtual const VQwHardwareChannel* ReturnInternalValue(const TString& name) const{
    std::cout << " VQwHardwareChannel::ReturnInternalValue for value name, " << name.Data()
              << " define the routine in the respective subsystem to process this!  " <<std::endl;
    return 0;
  };

  /// \brief Return a named value to the parent subsystem array to be
  ///        delivered to a different subsystem.
  virtual Bool_t ReturnInternalValue(const TString& name,
                                      VQwHardwareChannel* value) const {
    return kFALSE;
  };
  
  virtual std::vector<TString> GetParamFileNameList();
  virtual std::map<TString, TString> GetDetectorMaps();

 protected:
  /// Map of published internal values
  std::map<TString, VQwHardwareChannel*> fPublishedInternalValues;
  /// List of parameters to be published (loaded at the channel map)
  std::vector<std::vector<TString> > fPublishList;

  void UpdatePublishedValue(const TString& name, VQwHardwareChannel* data_channel) {
    fPublishedInternalValues[name] = data_channel;
  };

 public:

  /// \brief Parse parameter file to find the map files
  virtual Int_t LoadDetectorMaps(QwParameterFile& file);
  /// Mandatory map file definition
  virtual Int_t LoadChannelMap(TString mapfile) = 0;
  /// Mandatory parameter file definition
  virtual Int_t LoadInputParameters(TString mapfile) = 0;
  /// Optional geometry definition
  virtual Int_t LoadGeometryDefinition(TString mapfile) { return 0; };
  /// Optional event cut file
  virtual Int_t LoadEventCuts(TString mapfile) { return 0; };

  /// Set event type mask
  void SetEventTypeMask(const UInt_t mask) { fEventTypeMask = mask; };
  /// Get event type mask
  UInt_t GetEventTypeMask() const { return fEventTypeMask; };


  virtual void  ClearEventData() = 0;

  virtual Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) = 0;

  virtual Int_t ProcessEvBuffer(const UInt_t event_type, const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words){
    /// TODO:  Subsystems should be changing their ProcessEvBuffer routines to take the event_type as the first
    ///  argument.  But in the meantime, default to just calling the non-event-type-aware ProcessEvBuffer routine.
    if (((0x1 << (event_type - 1)) & this->GetEventTypeMask()) == 0) return 0;
    else return this->ProcessEvBuffer(roc_id, bank_id, buffer, num_words);
  };
  /// TODO:  The non-event-type-aware ProcessEvBuffer routine should be replaced with the event-type-aware version.
  virtual Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) = 0;

  virtual void  ProcessEvent() = 0;
  /*! \brief Request processed data from other subsystems for internal
   *         use in the second event processing stage.  Not all derived
   *         classes will require data from other subsystems.
   */
  virtual void  ExchangeProcessedData() { };
  /*! \brief Process the event data again, including data from other
   *         subsystems.  Not all derived classes will require
   *         a second stage of event data processing.
   */
  virtual void  ProcessEvent_2() { };


  /// \brief Perform actions at the end of the event loop
  virtual void  AtEndOfEventLoop(){QwDebug << fSystemName << " at end of event loop" << QwLog::endl;};


  // Not all derived classes will have the following functions
  virtual void  RandomizeEventData(int helicity = 0, double time = 0.0) { };
  virtual void  EncodeEventData(std::vector<UInt_t> &buffer) { };


  /// \name Histogram construction and maintenance
  // @{
  /// Construct the histograms for this subsystem
  virtual void  ConstructHistograms() {
    TString tmpstr("");
    ConstructHistograms((TDirectory*) NULL, tmpstr);
  };
  /// Construct the histograms for this subsystem in a folder
  virtual void  ConstructHistograms(TDirectory *folder) {
    TString tmpstr("");
    ConstructHistograms(folder, tmpstr);
  };
  /// Construct the histograms for this subsystem with a prefix
  virtual void  ConstructHistograms(TString &prefix) {
    ConstructHistograms((TDirectory*) NULL, prefix);
  };
  /// \brief Construct the histograms for this subsystem in a folder with a prefix
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  /// \brief Fill the histograms for this subsystem
  virtual void  FillHistograms() = 0;
  // @}


  /// \name Tree and branch construction and maintenance
  /// The methods should exist for all subsystems and are therefore defined
  /// as pure virtual.
  // @{
  /// \brief Construct the branch and tree vector
  virtual void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values) = 0;
  /// \brief Construct the branch and tree vector
  virtual void ConstructBranchAndVector(TTree *tree, std::vector<Double_t>& values) {
    TString tmpstr("");
    ConstructBranchAndVector(tree,tmpstr,values);
  };
  /// \brief Construct the branch and tree vector
  virtual void ConstructBranch(TTree *tree, TString& prefix) = 0;
  /// \brief Construct the branch and tree vector based on the trim file
  virtual void ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) = 0;
  /// \brief Fill the tree vector
  virtual void FillTreeVector(std::vector<Double_t>& values) const = 0;
  // @}


 

  /// \name Expert tree construction and maintenance
  /// These functions are not purely virtual, since not every subsystem is
  /// expected to implement them.  They are intended for expert output to
  /// trees.
  // @{
  /// \brief Construct the tree for this subsystem
  virtual void  ConstructTree() {
    TString tmpstr("");
    ConstructTree((TDirectory*) NULL, tmpstr);
  };
  /// \brief Construct the tree for this subsystem in a folder
  virtual void  ConstructTree(TDirectory *folder) {
    TString tmpstr("");
    ConstructTree(folder, tmpstr);
  };
  /// \brief Construct the tree for this subsystem with a prefix
  virtual void  ConstructTree(TString &prefix) {
    ConstructTree((TDirectory*) NULL, prefix);
  };
  /// \brief Construct the tree for this subsystem in a folder with a prefix
  virtual void  ConstructTree(TDirectory *folder, TString &prefix) { return; };
  /// \brief Fill the tree for this subsystem
  virtual void  FillTree() { return; };
  /// \brief Delete the tree for this subsystem
  virtual void  DeleteTree() { return; };
  // @}

  /// \brief Print some information about the subsystem
  virtual void  PrintInfo() const;

  /// \brief Assignment
  /// Note: Must be called at the beginning of all subsystems routine
  /// call to operator=(VQwSubsystem *value) by VQwSubsystem::operator=(value)
  virtual VQwSubsystem& operator=(VQwSubsystem *value);


  virtual void PrintDetectorMaps(Bool_t status) const;

 protected:

  /*! \brief Clear all registration of ROC and Bank IDs for this subsystem
   */
  void  ClearAllBankRegistrations();

  /*! \brief Tell the object that it will decode data from this ROC and sub-bank
   */
  virtual Int_t RegisterROCNumber(const UInt_t roc_id, const UInt_t bank_id = 0);

  /*! \brief Tell the object that it will decode data from this sub-bank in the ROC currently open for registration
   */
  Int_t RegisterSubbank(const UInt_t bank_id);

  Int_t GetSubbankIndex() const { return GetSubbankIndex(fCurrentROC_ID, fCurrentBank_ID); }
  Int_t GetSubbankIndex(const UInt_t roc_id, const UInt_t bank_id) const;
  void  SetDataLoaded(Bool_t flag){fIsDataLoaded = flag;};




 protected:
  Int_t FindIndex(const std::vector<UInt_t> &myvec, const UInt_t value) const ;

 protected:

  TString  fSystemName; ///< Name of this subsystem

  UInt_t   fEventTypeMask; ///< Mask of event types

  Bool_t   fIsDataLoaded; ///< Has this subsystem gotten data to be processed?

  std::vector<TString> fDetectorMapsNames;
  std::map<TString, TString> fDetectorMaps;
 protected:

  Int_t fCurrentROC_ID; ///< ROC ID that is currently being processed
  Int_t fCurrentBank_ID; ///< Bank ID that is currently being processed

  /// Vector of ROC IDs associated with this subsystem
  std::vector<UInt_t> fROC_IDs;
  /// Vector of Bank IDs per ROC ID associated with this subsystem
  std::vector< std::vector<UInt_t> > fBank_IDs;

  /// Vector of pointers to subsystem arrays that contain this subsystem
  std::vector<QwSubsystemArray*> fArrays;


 protected:

  // Comparison of type
  Bool_t Compare(VQwSubsystem* source) {
    return (typeid(*this) == typeid(*source));
  }

 private:

  // Private constructor (not implemented, will throw linker error on use)
  VQwSubsystem();

}; // class VQwSubsystem


#endif // __VQWSUBSYSTEM__
