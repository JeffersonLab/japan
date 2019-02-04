/**********************************************************\
* File: QwHelicityCorrelatedFeedback.h                     *
*                                                          *
* Author: Rakitha Beminiwattha                             *
* Time-stamp: <2010-09-28>                                 *
\**********************************************************/
#ifndef __QwHelicityCorrelatedFeedback__
#define __QwHelicityCorrelatedFeedback__

#include "QwHelicityPattern.h" 

#include "QwEPICSControl.h"
#include "GreenMonster.h"
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"

#include "QwParameterFile.h"
#include <time.h>
///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwHelicityCorrelatedFeedback : public QwHelicityPattern {
  /******************************************************************
   *  Class: QwHelicityCorrelatedFeedback
   *
   *
   ******************************************************************/

 public:
  QwHelicityCorrelatedFeedback(QwSubsystemArrayParity &event):QwHelicityPattern(event){ 
     //Currently pattern type based runningasymmetry accumulation works only with pattern size of 4
    fFBRunningAsymmetry.resize(kHelModes,event);
    fHelModeGoodPatternCounter.resize(kHelModes,0);

    fEnableBurstSum=kFALSE;
    fGoodPatternCounter=0;
    fHAGoodPatternCounter=0;
    fPFGoodPatternCounter=0;
    fPatternCounter=0;


/*     fFeedbackStatus=kTRUE; */
/*     if (fEPICSCtrl.Get_FeedbackStatus()>0) */
/*       fEPICSCtrl.Set_FeedbackStatus(0); */
/*     if (fFeedbackStatus){ */
/*       fFeedbackStatus=kFALSE; */
/*       fEPICSCtrl.Set_FeedbackStatus(1.0); */
/*     } */

    CheckFeedbackStatus();

    fPreviousHelPat=0;//at the beginning of the run this is non existing
    fCurrentHelPatMode=-1;//at the beginning of the run this is non existing

    fPITASetpointPOS=0;
    fPrevPITASetpointPOS=0;
    fPITASetpointNEG=0;
    fPrevPITASetpointNEG=0;
    fPITA_MIN_Charge_asym=1;//default value is 1ppm

    //initialize setpoints to zero
    //HA IA
    for(Int_t i=0;i<4;i++){
      fPrevHAIASetpoint[i]=0;
      fHAIASetpoint[i]=0;
    }
    //PITA
    fPrevPITASetpointPOS=0;
    fPrevPITASetpointNEG=0;
    fPITASetpointPOS=0;
    fPITASetpointNEG=0;
	

    fTargetCharge.InitializeChannel("q_targ","derived");
    fRunningCharge.InitializeChannel("q_targ","derived");
    fAsymBCM7.InitializeChannel("bcm7","derived");
    fAsymBCM8.InitializeChannel("bcm8","derived");

    fChargeAsymmetry0.InitializeChannel("q_targ","derived");//this is the charge asym at the beginning of the feedback loop
    fPreviousChargeAsymmetry.InitializeChannel("q_targ","derived");//charge asymmetry at the previous feedback loop
    fCurrentChargeAsymmetry.InitializeChannel("q_targ","derived");//current charge asymmetry 

    fIAAsymmetry0.InitializeChannel("q_targ","derived");//this is the charge asymmetry of the IA at the beginning of the feedback loop
    fPreviousIAAsymmetry.InitializeChannel("q_targ","derived");//this is the charge asymmetry of the IA at the previous feedback loop
    fCurrentIAAsymmetry.InitializeChannel("q_targ","derived");//current charge asymmetry of the IA

    fScalerChargeRunningSum.InitializeChannel("sca_bcm");
    fScalerCharge.InitializeChannel("sca_bcm");

    fTargetParameter.InitializeChannel("x_targ","derived");

    fTargetXDiffRunningSum.InitializeChannel("x_targ","derived");//to access the published Target X diff
    fTargetXPDiffRunningSum.InitializeChannel("xp_targ","derived");//to access the published Target XP diff
    fTargetYDiffRunningSum.InitializeChannel("y_targ","derived");//to access the published Target Y diff
    fTargetYPDiffRunningSum.InitializeChannel("yp_targ","derived");//to access the published Target YP diff

    fAsymBCM78DDRunningSum.InitializeChannel("bcm78dd","derived");
    fYieldBCM8RunningSum.InitializeChannel("q_targ","derived");
    fAsymUSLumiSumRunningSum.InitializeChannel("uslumisum","derived");
  
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    out_file_IA = fopen("/local/scratch/qweak/Feedback_IA_log.txt", "a");
    //out_file_IA = fopen("/dev/shm/Feedback_IA_log.txt", "a");    
    
    fprintf(out_file_IA,"%22s \n",asctime (timeinfo));
    fprintf(out_file_IA,"Pat num. \t  A_q[mode]\t  IA Setpoint \t  IA Previous Setpoint \n");
    fclose(out_file_IA);
    //    out_file_PITA = fopen("Feedback_PITA_log.txt", "wt");
 
    out_file_PITA = fopen("/local/scratch/qweak/Feedback_PITA_log.txt", "a");
    out_file_HA_IA = fopen("/local/scratch/qweak/Feedback_HA_IA_log.txt", "a"); 
    fprintf(out_file_PITA,"%22s \n",asctime (timeinfo));
    fprintf(out_file_PITA,
	    "%10s %22s +- %16s %16s %26s %26s %26s %26s\n",
	    "Pat num.", "Charge Asym [ppm]", "Asym Error", "Correction",
	    "New PITA Setpoint[+]", "Old PITA Setpoint[+]",
    	    "New PITA Setpoint[-]", "Old PITA Setpoint[-]");
    fclose(out_file_PITA);

    fprintf(out_file_HA_IA,"%22s \n",asctime (timeinfo));
    fprintf(out_file_HA_IA,
	    "%10s %22s  %16s %16s %26s %23s \n",
	    "Pat num.", "Charge Asym(ppm)", "Asym Error", "Correction",
	    "New IA Setpoint", "Old IA Setpoint");
    fclose(out_file_HA_IA);

    
  
  };
    
    
    ~QwHelicityCorrelatedFeedback() { 
      //        if (!fFeedbackStatus){
      //	  fFeedbackStatus=kFALSE;
      fEPICSCtrl.Set_FeedbackStatus(0);
	  //	}

    };  
    ///inherited from QwHelicityPattern
    void CalculateAsymmetry();
    void ClearRunningSum();
    void AccumulateRunningSum();
    void CalculateRunningAverage();
    void ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void FillTreeVector(std::vector<Double_t> &values) const;

    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);

    /// \brief Load deltaA_q, no.of good patterns to accumulate and other neccessary feedback parameters
    void LoadParameterFile(TString filename);

    /// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
    void GetTargetChargeStat();
    /// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width for given mode
    void GetTargetChargeStat(Int_t mode);
    /// \brief retrieves the target  position angle parameters (X,XP,Y,YP) mean, error and width
    void GetTargetPositionStat();
    /// \brief retrieves the Hall A charge asymmetry,asymmetry error ,asymmetry width for given mode
    void GetHAChargeStat(Int_t mode);

 

    /// \brief Feed the Hall C IA set point based on the charge asymmetry 
    void FeedIASetPoint(Int_t mode);

    /// \brief Feed the Hall A IA set point based on the charge asymmetry 
    void FeedHAIASetPoint(Int_t mode);

    /// \brief Feed the Hall C PITA set point based on the charge asymmetry 
    void FeedPITASetPoints();

 
    /// \brief Feed the IA set point based on the charge asymmetry 
    void FeedPCPos();
    /// \brief Feed the IA set point based on the charge asymmetry 
    void FeedPCNeg();

    /// \brief Log the last IA  feedback information
    void LogParameters(Int_t mode);

    /// \brief Log the last PITA feedback information
    void LogParameters();

    /// \brief Log the Pos/angle information
    void LogPFParameters();



    /// \brief Log the last Hall A IA feedback information
    void LogHAParameters(Int_t mode);

    /// \brief Set Clean=0 or 1 in the GreenMonster
    void UpdateGMClean(Int_t state);

    /// \brief Update last feedback setting into scan variables in the GreenMonster
    void UpdateGMScanParameters();

    /// \brief Initiates the PITA feedback if the charge asymmetry passed the quality cut
    Bool_t ApplyPITAFeedback();
    /// \brief Initiates the Hall A IA feedback if the Hall A charge asymmetry passed the quality cut
    Bool_t ApplyHAIAFeedback();

    /// \brief Initiates the IA feedback if the Hall C charge asymmetry have passed the quality cut
    Bool_t ApplyIAFeedback(Int_t mode);

    /// \brief Initiates the Helicity magnet feedback if the position differences and/or angle differences have passed the quality cut
    Bool_t ApplyHMFeedback();



    /// \brief Check to see no.of good patterns accumulated after the last feedback is greater than a set value 
    Bool_t IsPatternsAccumulated(){
      if (fGoodPatternCounter>=fAccumulatePatternMax)
	return kTRUE;

      return kFALSE;
    };
    /// \brief Check to see no.of good patterns accumulated after the last position/angle feedback correction
    Bool_t IsPFPatternsAccumulated(){
      if (fPFGoodPatternCounter>=fPFAccumulatePatternMax)
	return kTRUE;

      return kFALSE;
    };

    /// \brief Check to see no.of good patterns accumulated after the last position/angle feedback correction
    Bool_t IsHAPatternsAccumulated(){
      if (fHAGoodPatternCounter>=fHAAccumulatePatternMax)
	return kTRUE;

      return kFALSE;
    };


    /// \brief Check neccessary conditions and apply IA setponts based on the charge asym for all four modes
    void ApplyFeedbackCorrections();
    /// \brief Check to see no.of good patterns accumulated after the last feedback is greater than a set value for given mode
    Bool_t IsPatternsAccumulated(Int_t mode);

    /// \brief Returns the type of the last helicity pattern based on following pattern history
    ///1. +--+ +--+
    ///2. +--+ -++-
    ///3. -++- +--+
    ///4. -++- -++-

    Int_t GetLastGoodHelicityPatternType();


    void CalculateRunningAverage(Int_t mode);
    void ClearRunningSum(Int_t mode);

 private:
    /// \brief Returns the charge asymmetry stats when required by feedback caluculations.
    Double_t GetChargeAsym(){
      return fChargeAsymmetry;
    };
    Double_t GetChargeAsymError(){
      return fChargeAsymmetryError;
    };
    Double_t GetChargeAsymWidth(){
      return fChargeAsymmetryWidth;
    };

    //Define separate running sums for differents helicity pattern modes
    /// \brief The types of helicity patterns based on following pattern history
    ///1. +--+ +--+
    ///2. +--+ -++-
    ///3. -++- +--+
    ///4. -++- -++-
 
    
    TString GetHalfWavePlateState();
    UInt_t GetHalfWavePlate2State();
    void    CheckFeedbackStatus();

    static const Int_t kHelPat1=1001;//to compare with current or previous helpat
    static const Int_t kHelPat2=110;
    static const Int_t kHelModes=4;//kHelModes

    std::vector<QwSubsystemArrayParity> fFBRunningAsymmetry;
    Int_t fCurrentHelPat;
    Int_t fPreviousHelPat;

    Int_t fCurrentHelPatMode;
    
    Double_t fChargeAsymmetry;//current charge asym
    Double_t fChargeAsymmetryError;//current charge asym precision
    Double_t fChargeAsymmetryWidth;//current charge asym width

    //position/angle parameters
    Double_t fTargetXDiff;
    Double_t fTargetXDiffError;
    Double_t fTargetXDiffWidth;

    Double_t fTargetXPDiff;
    Double_t fTargetXPDiffError;
    Double_t fTargetXPDiffWidth;

    Double_t fTargetYDiff;
    Double_t fTargetYDiffError;
    Double_t fTargetYDiffWidth;

    Double_t fTargetYPDiff;
    Double_t fTargetYPDiffError;
    Double_t fTargetYPDiffWidth;

    Double_t f3C12XDiff;
    Double_t f3C12XDiffError;
    Double_t f3C12XDiffWidth;
    
    Double_t f3C12YDiff;
    Double_t f3C12YDiffError;
    Double_t f3C12YDiffWidth;

    Double_t f3C12YQ;
    Double_t f3C12YQError;
    Double_t f3C12YQWidth;

    Double_t fAsymBCM78DD;
    Double_t fAsymBCM78DDError;
    Double_t fAsymBCM78DDWidth;

    Double_t fBCM8Yield;

    
    Double_t fAsymBCMUSLumiSum;
    Double_t fAsymBCMUSLumiSumError;
    Double_t fAsymBCMUSLumiSumWidth;

    Double_t fChargeAsym[kHelModes];//current charge asym
    Double_t fChargeAsymError[kHelModes];//current charge asym precision
    Double_t fChargeAsymWidth[kHelModes];//current charge asym width

    Double_t fHAChargeAsym[kHelModes];//Hall A current charge asym
    Double_t fHAChargeAsymError[kHelModes];//Hall A current charge asym precision
    Double_t fHAChargeAsymWidth[kHelModes];//Hall A current charge asym width

    Int_t fAccumulatePatternMax; //Hall C PITA Correction interval in units of patterns 
    Int_t fHAAccumulatePatternMax; //Hall A IA Correction interval in units of patterns 
    Int_t fPFAccumulatePatternMax; //HC Position/Angle difference  Correction interval in units of patterns 
    Double_t  fChargeAsymPrecision; //Charge asymmetry precision in ppm


    //Keep a record of optimal values for IA, PC POS and NEG
    Double_t fOptimalIA;
    Double_t fOptimalPCP;
    Double_t fOptimalPCN;
    //current IA and PC values
    Double_t fCurrentIA;
    Double_t fCurrentPCP;
    Double_t fCurrentPCN;

    //Hall C IA Slopes for 4 modes and their errors
    Double_t fIASlopeA[kHelModes];
    Double_t fDelta_IASlopeA[kHelModes];

   //Hall A IA Slopes for 4 modes and their errors
    Double_t fHAIASlopeA[kHelModes];
    Double_t fHADelta_IASlopeA[kHelModes];

    //Hall C IA setpoints for 4 modes 
    Double_t fIASetpoint[kHelModes];//New setpont calculated based on the charge asymmetry
    Double_t fPrevIASetpoint[kHelModes];//previous setpoint

    //Hall A IA setpoints for 4 modes 
    Double_t fHAIASetpoint[kHelModes];//New setpont calculated based on the charge asymmetry
    Double_t fPrevHAIASetpoint[kHelModes];//previous setpoint

    Double_t fIASetpointlow;//lower and upper limits for IA dac hardware counts
    Double_t fIASetpointup;

    //PITA Slopes for halfwave plate IN & OUT
    Double_t fPITASlopeIN;//IHWP1 IN IHWP2 OUT
    Double_t fPITASlopeOUT;//IHWP1 OUT IHWP2 OUT
    Double_t fPITASlopeOUT_IN;//IHWP1 OUT IHWP2 IN
    Double_t fPITASlope;

    //PITA setpoints for pos hel and neg hel
    Double_t fPITASetpointPOS;
    Double_t fPITASetpointNEG;
    Double_t fPITASetpointPOS_t0_IN;//Initial PC positive HW setpoint
    Double_t fPITASetpointNEG_t0_IN;//Initial PC negative HW setpoint
    Double_t fPITASetpointPOS_t0_OUT;//Initial PC positive HW setpoint
    Double_t fPITASetpointNEG_t0_OUT;//Initial PC negative HW setpoint
    Bool_t fInitialCorrection;//Is true at the beginning so that t_0 correction is appiled before doing any correction
    
    
    Double_t fPrevPITASetpointPOS;//previous setpoint
    Double_t fPrevPITASetpointNEG;//previous setpoint
    Double_t fPITASetpointlow;//lower and upper limits for PITA dac hardware counts
    Double_t fPITASetpointup;
    Double_t fPITA_MIN_Charge_asym;//Minimum charge asymmetry at which feedback correction is applied. see QwHelicityCorrelatedFeedback::FeedPITASetPoints()

    
    
    ///  Create an EPICS control event
    QwEPICSControl fEPICSCtrl;
    GreenMonster   fScanCtrl;

    //Pattern counter
    Int_t fGoodPatternCounter;//increment the quartet number - reset after each PITA feedback operation
    Int_t fHAGoodPatternCounter;//increment the quartet number - reset after each Hall A IA feedback operation
    Int_t fPFGoodPatternCounter;//increment the quartet number - reset after each position/angle feedback operation

    Int_t fPatternCounter;//increment the quartet number - reset after each feedback operation

    
    std::vector<Int_t> fHelModeGoodPatternCounter;//count patterns for each mode seperately - reset after each feedback operation

    // Keep four VQWK channels, one each for pattern history 1, 2, 3, and 4
    // Use AddToRunningSum to add the asymmetry for the current pattern
    // into the proper pattern history runnign sum.

    QwBeamCharge   fTargetCharge;
    QwBeamCharge   fRunningCharge;
    QwBeamCharge   fChargeAsymmetry0;//this is the charge asym at the beginning of the feedback loop
    QwBeamCharge   fPreviousChargeAsymmetry;//charge asymmetry at the previous feedback loop
    QwBeamCharge   fCurrentChargeAsymmetry;//current charge asymmetry 

    QwBeamCharge   fIAAsymmetry0;//this is the charge asymmetry of the IA at the beginning of the feedback loop
    QwBeamCharge   fPreviousIAAsymmetry;//this is the charge asymmetry of the IA at the previous feedback loop
    QwBeamCharge   fCurrentIAAsymmetry;//current charge asymmetry of the IA

    QwSIS3801D24_Channel   fScalerCharge;//for Hall A feedback
    QwSIS3801D24_Channel   fScalerChargeRunningSum;//for Hall A feedback

    QwBeamCharge   fTargetParameter;//to access the published postions/angles
    
    QwBeamCharge   fTargetXDiffRunningSum;//to access the published Target X diff
    QwBeamCharge   fTargetXPDiffRunningSum;//to access the published Target XP diff
    QwBeamCharge   fTargetYDiffRunningSum;//to access the published Target Y diff
    QwBeamCharge   fTargetYPDiffRunningSum;//to access the published Target YP diff
    QwBeamCharge   f3C12XDiffRunningSum;//to access the published 3c12 X diff
    QwBeamCharge   f3C12YDiffRunningSum;//to access the published 3c12 Y diff
    QwBeamCharge   f3C12YQRunningSum;//to access the published 3c12 eff. charge

    QwBeamCharge   fAsymBCM7;//to access bcm7 asymmetry
    QwBeamCharge   fAsymBCM8;//to access bcm8 asymmetry
    QwBeamCharge   fAsymBCM78DDRunningSum;//to accumulate bcm78 DD asymmetry
    QwBeamCharge   fYieldBCM8RunningSum;//to access bcm8 Yield

    QwBeamCharge   fAsymUSLumiSumRunningSum;//to accumulate US Lumi sum


    //log file
    FILE *out_file_PITA;
    FILE *out_file_IA;
    FILE *IHWP_State;
    FILE *out_file_HA_IA;
    FILE *out_file_PC_IN_pos;
    FILE *out_file_PC_IN_neg;
    FILE *out_file_PC_OUT_pos;
    FILE *out_file_PC_OUT_neg;

    Bool_t fHalfWaveIN;
    Bool_t fHalfWaveOUT;
    Int_t fIHWP;

    Bool_t fHalfWaveRevert;
    Bool_t fAutoIHWP;

    TString fHalfWavePlateStatus;
    UInt_t fDefaultHalfWavePlateStatus;
    UInt_t fPreviousHalfWavePlateStatus;

    Bool_t fPITAFB;
    Bool_t fHAIAFB;
    Bool_t fIAFB;
    Bool_t fFeedbackStatus;
    Bool_t fFeedbackDamping;


    time_t rawtime;
    struct tm * timeinfo;

  
};

#endif
