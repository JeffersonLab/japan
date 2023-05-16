/**********************************************************\
* File: QwEnergyCalculator.h                              *
*                                                         *
* Author:  B.Waidyawansa                                  *
* Time-stamp: 05-24-2010                                  *
\**********************************************************/

#ifndef __QwVQWK_ENERGYCALCULATOR__
#define __QwVQWK_ENERGYCALCULATOR__

// System headers
#include <vector>

// Root headers
#include <TTree.h>

// Qweak headers
#include "QwVQWK_Channel.h"
#include "QwBPMStripline.h"
#include "QwCombinedBPM.h"
#include "VQwBPM.h"
#include "VQwHardwareChannel.h"
#include "QwMollerADC_Channel.h"

// Forward declarations
#ifdef __USE_DATABASE__
class QwDBInterface;
#endif // __USE_DATABASE__

class QwEnergyCalculator : public VQwDataElement{
  /******************************************************************
   *  Class:QwEnergyCalculator
   *         Perfroms the beam energy calculation using the beam angle
   *         amnd position at the target and the beam postition at the
   *         highest dispersive region on the beamline.
   *
   ******************************************************************/


 public:
  // Default constructor
  QwEnergyCalculator() { };
  QwEnergyCalculator(TString name){
    InitializeChannel(name,"derived");
  };
  QwEnergyCalculator(TString subsystem, TString name){
    InitializeChannel(subsystem, name,"derived");
  };
  QwEnergyCalculator(const QwEnergyCalculator& source)
  : VQwDataElement(source),fEnergyChange(source.fEnergyChange)
  { }
  virtual ~QwEnergyCalculator() { };

    void    InitializeChannel(TString name,TString datatosave);
    // new routine added to update necessary information for tree trimming
    void    InitializeChannel(TString subsystem, TString name, TString datatosave);

    void    LoadChannelParameters(QwParameterFile &paramfile){};

//------------------------------------------------------------------------------------
    void    SetRandomEventParameters(Double_t mean, Double_t sigma);
    void    RandomizeEventData(int helicity = 0, double time = 0.0);
    void    GetProjectedPosition(VQwBPM *device);
    size_t  GetNumberOfElements() {return fDevice.size();};
    TString GetSubElementName(Int_t index) {return fDevice.at(index)->GetElementName();};
    void    LoadMockDataParameters(QwParameterFile &paramfile);
//------------------------------------------------------------------------------------
    
    void    ClearEventData();
    Int_t   ProcessEvBuffer(UInt_t* buffer,
			    UInt_t word_position_in_buffer,UInt_t indexnumber);
    void    ProcessEvent();
    void    PrintValue() const;
    void    PrintInfo() const;
    void    SetRootSaveStatus(TString &prefix);

    Bool_t  ApplyHWChecks();//Check for harware errors in the devices
    Bool_t  ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
    Int_t   SetSingleEventCuts(Double_t mean, Double_t sigma);//two limts and sample size
    /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
    void    SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability, Double_t burplevel);
    void    SetEventCutMode(Int_t bcuts){
      bEVENTCUTMODE=bcuts;
      fEnergyChange.SetEventCutMode(bcuts);
    }
    Bool_t CheckForBurpFail(const VQwDataElement *ev_error);

    void    IncrementErrorCounters();
    void    PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
    UInt_t   GetEventcutErrorFlag(){//return the error flag
      return fEnergyChange.GetEventcutErrorFlag();
    }
    UInt_t   UpdateErrorFlag();

    void    UpdateErrorFlag(const QwEnergyCalculator *ev_error);
  

    void    Set(const VQwBPM* device,TString type, TString property ,Double_t tmatrix_ratio);
    void    Ratio(QwEnergyCalculator &numer,QwEnergyCalculator &denom);
    void    Scale(Double_t factor);

    virtual QwEnergyCalculator& operator=  (const QwEnergyCalculator &value);
    virtual QwEnergyCalculator& operator+= (const QwEnergyCalculator &value);
    virtual QwEnergyCalculator& operator-= (const QwEnergyCalculator &value);

    void    AccumulateRunningSum(const QwEnergyCalculator& value, Int_t count=0, Int_t ErrorMask=0xFFFFFFF);
    void    DeaccumulateRunningSum(QwEnergyCalculator& value, Int_t ErrorMask=0xFFFFFFF);
    void    CalculateRunningAverage();

    void    ConstructHistograms(TDirectory *folder, TString &prefix);
    void    FillHistograms();

    void    ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void    ConstructBranch(TTree *tree, TString &prefix);
    void    ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file);
    void    FillTreeVector(std::vector<Double_t> &values) const;

    VQwHardwareChannel* GetEnergy(){
      return &fEnergyChange;
    };

    const VQwHardwareChannel* GetEnergy() const{
      return const_cast<QwEnergyCalculator*>(this)->GetEnergy();
    };

#ifdef __USE_DATABASE__
    std::vector<QwDBInterface> GetDBEntry();
    std::vector<QwErrDBInterface> GetErrDBEntry();
#endif // __USE_DATABASE__

 protected:
    QwMollerADC_Channel fEnergyChange;


 private:
    std::vector <const VQwBPM*> fDevice;
    std::vector <Double_t> fTMatrixRatio;
    std::vector <TString>  fProperty;
    std::vector <TString>  fType;
    Int_t    fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode
    Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map
    Bool_t   bFullSave; // used to restrict the amount of data histogramed



};

#endif
