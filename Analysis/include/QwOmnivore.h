#ifndef QWOMNIVORE_H
#define QWOMNIVORE_H

#include "VQwSubsystemParity.h"
#include "VQwSubsystemTracking.h"

//#include "QwHitContainer.h"
//#include "QwDetectorInfo.h"

/**
 *  \class QwOmnivore
 *  \ingroup QwAnalysis
 *  \brief An omnivorous subsystem.
 */
template<class VQwSubsystem_t>
class QwOmnivore: public VQwSubsystem_t {

  private:
    /// Private default constructor (not implemented, will throw linker error on use)
    QwOmnivore();

  public:
    /// Constructor with name
    QwOmnivore(const TString& name): VQwSubsystem(name),VQwSubsystem_t(name) { };
    /// Copy constructor
    QwOmnivore(const QwOmnivore& source)
    : VQwSubsystem(source),VQwSubsystem_t(source)
    { }
    /// Virtual destructor
    virtual ~QwOmnivore() { };

    /// Map file definition
    Int_t LoadChannelMap(TString mapfile) { return 0; };
    /// Parameter file definition
    Int_t LoadInputParameters(TString mapfile) { return 0; };
    /// Geometry definition for tracking subsystems
    Int_t LoadGeometryDefinition(TString mapfile) { return 0; };

    /// Load the event cuts file
    Int_t LoadEventCuts(TString filename) { return 0; };
    /// Apply the single event cuts
    Bool_t ApplySingleEventCuts() { return kTRUE; };
    /// Report the number of events failed due to HW and event cut failures
    void PrintErrorCounters() const { };
    /// Return the error flag to the main routine
    UInt_t GetEventcutErrorFlag() { return 0; };

    /// Increment error counters
    void IncrementErrorCounters() { };
    /// Update error flag
    void UpdateErrorFlag(const VQwSubsystem*) { };

    /// Get the hit list
    //void  GetHitList(QwHitContainer& grandHitContainer) { };
    /// Get the detector geometry information
    //Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detect_info) { return 0; };


    /// Clear event data
    void  ClearEventData() { };

    /// Process the configuration events
    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) {
      /// Om nom nom nom
      // TODO (wdc) configuration events seem to have num_words = 0xffffffff
      //UInt_t cheeseburger;
      //for (UInt_t word = 0; word < num_words; word++)
      //  cheeseburger += buffer[word]; // (addition to prevent compiler from optimizing local away)
      return 0; // my plate is empty
    };

    /// Process the event buffer
    Int_t ProcessEvBuffer(const UInt_t event_type, const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) {
      /// TODO:  Subsystems should be changing their ProcessEvBuffer routines to take the event_type as the first
      ///  arguement.  But in the meantime, default to just calling the non-event-type-aware ProcessEvBuffer routine.
      return this->ProcessEvBuffer(roc_id, bank_id, buffer, num_words);
    };
    /// TODO:  The non-event-type-aware ProcessEvBuffer routine should be replaced with the event-type-aware version.
    Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words) {
      /// Om nom nom nom
      UInt_t cheeseburger;
      for (UInt_t word = 0; word < num_words; word++)
        cheeseburger += buffer[word]; // (addition to prevent compiler from optimizing local away)
      return 0; // my plate is empty
    };

    /// Process the event
    void  ProcessEvent() { };

    /// Assignment/addition/subtraction operators
    VQwSubsystem& operator=  (VQwSubsystem *value) { return *this; };
    VQwSubsystem& operator+= (VQwSubsystem *value) { return *this; };
    VQwSubsystem& operator-= (VQwSubsystem *value) { return *this; };
    /// Sum/difference/ratio/scale operations
    void Sum(VQwSubsystem *value1, VQwSubsystem *value2) { };
    void Difference(VQwSubsystem *value1, VQwSubsystem *value2) { };
    void Ratio(VQwSubsystem *numer, VQwSubsystem *denom) { };
    void Scale(Double_t factor) { };

    /// Construct the histograms for this subsystem in a folder with a prefix
    void  ConstructHistograms(TDirectory *folder, TString &prefix) { };
    /// Fill the histograms for this subsystem
    void  FillHistograms() { };

    /// Construct the branch and tree vector
    void ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values) { };
    /// Fill the tree vector
    void FillTreeVector(std::vector<Double_t> &values) const { };
    /// Construct branch
    void ConstructBranch(TTree*, TString&) { };
    /// Construct branch
    void ConstructBranch(TTree*, TString&, QwParameterFile&) { };

    /// \brief Update the running sums for devices
    void AccumulateRunningSum(VQwSubsystem* value) { };
    void DeaccumulateRunningSum(VQwSubsystem* value) { };
    /// \brief Calculate the average for all good events
    void CalculateRunningAverage() { };
};

#endif
