/**
 * \class	QwComptonPhotonDetector	QwComptonPhotonDetector.h
 *
 * \brief	Class for the analysis of Compton photon detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwComptonPhotonDetector class is defined as a parity subsystem that
 * contains all data modules of the photon detector (SIS3320, V775, V792).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *
 */

#ifndef __QwComptonPhotonDetector__
#define __QwComptonPhotonDetector__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwSIS3320_Channel.h"
#include "QwPMT_Channel.h"
#include "MQwV775TDC.h"

class QwComptonPhotonDetector:
  public VQwSubsystemParity,
  public MQwV775TDC,
  public MQwSubsystemCloneable<QwComptonPhotonDetector> {

  private:
    /// Private default constructor (not implemented, will throw linker error on use)
    QwComptonPhotonDetector();

  public:

    /// Constructor with name
    QwComptonPhotonDetector(TString name): VQwSubsystem(name), VQwSubsystemParity(name) { };
    /// Copy constructor
    QwComptonPhotonDetector(const QwComptonPhotonDetector& source)
    : VQwSubsystem(source),VQwSubsystemParity(source),
      fSamplingADC(source.fSamplingADC),
      fMultiQDC_Channel(source.fMultiQDC_Channel),
      fMultiTDC_Channel(source.fMultiTDC_Channel)
    { }
    /// Virtual destructor
    virtual ~QwComptonPhotonDetector() { };


    // Handle command line options
    static void DefineOptions(QwOptions &options);
    void ProcessOptions(QwOptions &options);

    /* derived from VQwSubsystem */
    Int_t LoadChannelMap(TString mapfile);
    Int_t LoadInputParameters(TString pedestalfile);
    Int_t LoadEventCuts(TString & filename);
    Bool_t SingleEventCuts();
    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

    void  ClearEventData();
    void  ProcessEvent();
    Bool_t IsGoodEvent();

    void RandomizeEventData(int helicity = 0);
    void EncodeEventData(std::vector<UInt_t> &buffer);

    VQwSubsystem& operator=  (VQwSubsystem *value);
    VQwSubsystem& operator+= (VQwSubsystem *value);
    VQwSubsystem& operator-= (VQwSubsystem *value);
    void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
    void Scale(Double_t factor);

    Int_t LoadEventCuts(TString filename) { return 0; };
    Bool_t ApplySingleEventCuts() { return kTRUE; };
    void IncrementErrorCounters() {};
    void PrintErrorCounters() const {};
    UInt_t GetEventcutErrorFlag() { return 0; };

    //update the error flag in the subsystem level from the top level routines related to stability checks. This will uniquely update the errorflag at each channel based on the error flag in the corresponding channel in the ev_error subsystem
    void UpdateErrorFlag(const VQwSubsystem *ev_error){
    };
    Bool_t CheckRunningAverages(Bool_t ) { return kTRUE; };

    void AccumulateRunningSum(VQwSubsystem* value);
    //remove one entry from the running sums for devices
    void DeaccumulateRunningSum(VQwSubsystem* value){
    };
    void CalculateRunningAverage();

    using VQwSubsystem::ConstructHistograms;
    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();

    using VQwSubsystem::ConstructTree;
    void  ConstructTree(TDirectory *folder, TString &prefix);
    void  FillTree();
    void  DeleteTree();

    using VQwSubsystem::ConstructBranchAndVector;
    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  ConstructBranch(TTree *tree, TString& prefix) { };
    void  ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) { };
    void  FillTreeVector(std::vector<Double_t> &values) const;

    Bool_t Compare(VQwSubsystem *source);
    Bool_t CompareADC(VQwSubsystem *source);
    Bool_t CompareTDC(VQwSubsystem *source);
    Bool_t CompareQDC(VQwSubsystem *source);

    void PrintValue() const;
    void PrintInfo() const;

    QwSIS3320_Channel* GetSIS3320Channel(const TString name);

  protected:

    /// Expert tree
    TTree* fTree;
    /// Expert tree fields
    Int_t fTree_fNEvents;

    /// Mapping from ROC/subbank to channel type
    enum ChannelType_t { kUnknown, kSamplingADC, kIntegratingADC, kIntegratingTDC };
    std::map< Int_t, ChannelType_t > fMapping;

    /// List of sampling ADC channels
    typedef std::map< Int_t, std::vector <std::vector <Int_t> > > SamplingADC_Mapping_t;
    SamplingADC_Mapping_t fSamplingADC_Mapping;
    std::vector< QwSIS3320_Channel > fSamplingADC;

    /// List of integrating QDC channels
    typedef std::map< Int_t, std::vector< std::vector< Int_t > > > IntegratingADC_Mapping_t;
    IntegratingADC_Mapping_t fMultiQDC_Mapping;
    std::vector< QwPMT_Channel > fMultiQDC_Channel;
    std::vector< std::vector< QwPMT_Channel > > fMultiQDC_Events;

    /// List of integrating TDC channels
    typedef std::map< Int_t, std::vector< std::vector< Int_t > > > IntegratingTDC_Mapping_t;
    IntegratingTDC_Mapping_t fMultiTDC_Mapping;
    std::vector< QwPMT_Channel > fMultiTDC_Channel;
    std::vector< std::vector< QwPMT_Channel > > fMultiTDC_Events;

  private:

    // Number of good events
    Int_t fGoodEventCount;

    static const Bool_t kDebug = kTRUE;

};

#endif // __QwComptonPhotonDetector__
