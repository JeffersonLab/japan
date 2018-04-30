/**
 * \class	QwComptonElectronDetector	QwComptonElectronDetector.h
 *
 * \brief	Class for the analysis of Compton electron detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwComptonElectronDetector class is defined as a parity subsystem that
 * contains all data modules of the electron detector (V1495).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *
 * A. Narayan
 */

#ifndef __QwComptonElectronDetector__
#define __QwComptonElectronDetector__

// System headers
#include <vector>

// Boost math library for random number generation
#include <boost/multi_array.hpp>

// Qweak headers
#include "VQwSubsystemParity.h"

class QwComptonElectronDetector:
  public VQwSubsystemParity,
  public MQwSubsystemCloneable<QwComptonElectronDetector> {

  private:
    /// Private default constructor (not implemented, will throw linker error on use)
    QwComptonElectronDetector();

  public:

    /// Constructor with name
    QwComptonElectronDetector(const TString& name): VQwSubsystem(name), VQwSubsystemParity(name),
      fIsConfigOnly(kFALSE) {
      fStripsRaw.resize(NPlanes);
      fStripsRaw_v2.resize(NPlanes);
      fStrips.resize(NPlanes);
      fStripsRawEv.resize(NPlanes);
      fStripsEv.resize(NPlanes);
      fStripsRawScal.resize(NPlanes);
      //!!fDAQinfo.resize(NModules);//to read in the words from DAQ info
      fPlane1Mask.resize(StripsPerPlane);
      fPlane2Mask.resize(StripsPerPlane);
      fPlane3Mask.resize(StripsPerPlane);
      fPlane4Mask.resize(StripsPerPlane);
      fEvTrig.resize(NModules);
      fAcTrig.resize(NModules);
      fMinWidth.resize(NModules);
      fFirmwareRevision.resize(NModules);
      fPWTL.resize(NModules);
      fPWTL2.resize(NModules);
      fHoldOff.resize(NModules);
      fPipelineDelay.resize(NModules);
      for (Int_t i = 0; i < NPlanes; i++) {
        fStripsRaw[i].resize(StripsPerPlane);
        fStripsRaw_v2[i].resize(StripsPerPlane);
        fStrips[i].resize(StripsPerPlane);
        fStripsRawEv[i].resize(StripsPerPlane);
        fStripsEv[i].resize(StripsPerPlane);
        fStripsRawScal[i].resize(StripsPerPlane);
      }
    };
    /// Copy constructor
    QwComptonElectronDetector(const QwComptonElectronDetector& source)
    : VQwSubsystem(source),VQwSubsystemParity(source),
      fStrips(source.fStrips),fStripsRaw(source.fStripsRaw),
      fStripsEv(source.fStripsEv),fStripsRawEv(source.fStripsRawEv),
      fStripsRawScal(source.fStripsRawScal),
      fStripsRaw_v2(source.fStripsRaw_v2),fDAQinfo(source.fDAQinfo),
      fPlane1Mask(source.fPlane1Mask),
      fPlane2Mask(source.fPlane2Mask),
      fPlane3Mask(source.fPlane3Mask),
      fPlane4Mask(source.fPlane4Mask),
      fEvTrig(source.fEvTrig),
      fAcTrig(source.fAcTrig),
      fMinWidth(source.fMinWidth),
      fFirmwareRevision(source.fFirmwareRevision),
      fPWTL(source.fPWTL),
      fPWTL2(source.fPWTL2),
      fHoldOff(source.fHoldOff),
      fPipelineDelay(source.fPipelineDelay)
   { }
    /// Virtual destructor
    virtual ~QwComptonElectronDetector() { };


    /* derived from VQwSubsystem */
    Int_t LoadChannelMap(TString mapfile);
    Int_t LoadInputParameters(TString pedestalfile);
    Int_t LoadEventCuts(TString & filename);
    Bool_t SingleEventCuts();
    Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
    Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

    void  ClearEventData();
    void  ProcessEvent();
    Bool_t IsGoodEvent();

    void EncodeEventData(std::vector<UInt_t> &buffer) { };

    VQwSubsystem& operator=  (VQwSubsystem *value);
    VQwSubsystem& operator+= (VQwSubsystem *value);
    VQwSubsystem& operator-= (VQwSubsystem *value);
    VQwSubsystem& operator*= (VQwSubsystem *value);

    void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
    void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
    void Scale(Double_t factor);

    Int_t LoadEventCuts(TString filename) { return 0; };
    Bool_t ApplySingleEventCuts() { return kTRUE; };
    void IncrementErrorCounters() {};
    void PrintErrorCounters() const { };
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
//    void  FillDB(QwDatabase *db, TString datatype){};

    size_t GetNumberOfEvents() const { return (fNumberOfEvents); };
    void SetNumberOfEvents(UInt_t nevents) {
     fNumberOfEvents = nevents;
    };
    Bool_t Compare(VQwSubsystem *source);
    void PrintValue() const;

    void SetCalibrationFactor(const Double_t factor) { fCalibrationFactor = factor; };
    Double_t GetCalibrationFactor() const { return fCalibrationFactor; };


  protected:
    /// Expert tree
    TTree* fTree;
    /// Expert tree fields
    Int_t fTree_fNEvents;

    std::vector< std::vector <Int_t> > fSubbankIndex;

    /// 1st dimension:NPlanes;2nd dimension: NStrips;
    /// List of V1495 accumulation mode strips    
    std::vector< std::vector <Double_t> > fStrips;
    std::vector< std::vector <Double_t> > fStripsRaw;
    /// List of V1495 single event mode strips
    std::vector< std::vector <Double_t> > fStripsEv;
    std::vector< std::vector <Double_t> > fStripsRawEv; 
    /// List of V1495 scaler counts
    std::vector< std::vector <Double_t> > fStripsRawScal;
    std::vector< std::vector <Double_t> > fStripsRaw_v2;
    /// to read in the DAQ info configuration subbank
    std::vector< std::vector <Double_t> >fDAQinfo;
    std::vector <UInt_t> slave_header,firmwareRevision,portAmask,portBmask,portDmask,portEmask,widthInfo,trigInfo;

    std::vector <UInt_t> fPlane1Mask,fPlane2Mask,fPlane3Mask,fPlane4Mask,fEvTrig,fAcTrig,fMinWidth;
    std::vector <UInt_t> fFirmwareRevision,fPWTL,fPWTL2,fHoldOff,fPipelineDelay;
    //    boost::multi_array<Double_t, 2> array_type;
    //    array_type fStrips(boost::extents[NPlanes][StripsPerPlane]);
    //   array_type fStripsRaw(boost::extents[NPlanes][StripsPerPlane]);

 /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms()
   */

  std::vector<Double_t> fComptonElectronVector;


  private:

    static const Bool_t kDebug = kTRUE;
    Double_t fCalibrationFactor;
    Double_t fOffset;
    Double_t edet_x2;
    Double_t edet_TotalNumberTracks;
    Int_t fTreeArrayNumEntries;
    Int_t fTreeArrayIndex;
    UInt_t fNumberOfEvents; //! Number of triggered events
    Int_t eff23;
    Int_t effall;
    Double_t effedet;
    Bool_t fIsConfigOnly;
   
    Int_t fGoodEventCount;

    /// Mapping from ROC/subbank to channel type
    enum ChannelType_t { kUnknown, kV1495Accum, kV1495Accum_v2, kV1495Single, kV1495Scaler};
    std::map< Int_t, ChannelType_t > fMapping;

        // Assign static const member fields
    static const Int_t NModules = 2;///number of slave modules(!!change to 2?)
    static const Int_t NPlanes = 4;///number of diamond detector planes
    static const Int_t StripsPerModule = 32;///number of strips in each v1495 slave module
    static const Int_t StripsPerPlane = 64;///total number of strips in each detecor
    static const Int_t NInfoWords = 8;///in the CRL structure, we had used 8 words to store per slave
};

#endif // __QwComptonElectronDetector__
