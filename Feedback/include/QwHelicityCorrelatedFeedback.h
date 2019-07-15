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
    // fPFGoodPatternCounter=0;
fPFUGoodPatternCounter=0;
fPFVGoodPatternCounter=0;
 fXGoodPatternCounter=0;
 fYGoodPatternCounter=0;
    fPatternCounter=0;
fHCIAGoodPatternCounter=0;
 fHAIAGoodPatternCounter=0;
 fHBIAGoodPatternCounter=0;


/*     fFeedbackStatus=kTRUE; */
/*     if (fEPICSCtrl.Get_FeedbackStatus()>0) */
/*       fEPICSCtrl.Set_FeedbackStatus(0); */
/*     if (fFeedbackStatus){ */
/*       fFeedbackStatus=kFALSE; */
/*       fEPICSCtrl.Set_FeedbackStatus(1.0); */
/*     } */

    CheckFeedbackStatus();
  
    //fPreviousHelPat=0;//at the beginning of the run this is non existing
    //fCurrentHelPatMode=-1;//at the beginning of the run this is non existing

    // fPITASetpointPOS=0;
    //fPrevPITASetpointPOS=0;
    //fPITASetpointNEG=0;
    //fPrevPITASetpointNEG=0;
    fPITA_MIN_Charge_asym=1;//default value is 1ppm

  

    //initialize setpoints to zero
    //HA IA
    

    //Amali2019
    for(Int_t i=1;i<9;i++){
    fPrevPITASetpoint[i]=0;
   fPITASetpoint[i]=0;
   }

    //  for(Int_t j=1;j<9;j++){
    //fPrevPOSXYSetpoint[j]=0;
    //fPOSXYSetpoint[j]=0;
    // }

fPITAPOSUSetpoint1=0;
fPITAPOSUSetpoint2=0;
fPITAPOSUSetpoint5=0;
 fPITAPOSUSetpoint6=0;

fPrevPITAPOSUSetpoint1=0;
fPrevPITAPOSUSetpoint2=0;
fPrevPITAPOSUSetpoint5=0;
fPrevPITAPOSUSetpoint6=0;

fPOSXYSetpoint1=0;
fPOSXYSetpoint2=0;
fPOSXYSetpoint3=0;
fPOSXYSetpoint4=0;
fPOSXYSetpoint5=0;
fPOSXYSetpoint6=0;
fPOSXYSetpoint7=0;
fPOSXYSetpoint8=0;

fPrevPOSXYSetpoint1=0;
fPrevPOSXYSetpoint2=0;
fPrevPOSXYSetpoint3=0;
fPrevPOSXYSetpoint4=0;
fPrevPOSXYSetpoint5=0;
fPrevPOSXYSetpoint6=0;
fPrevPOSXYSetpoint7=0;
fPrevPOSXYSetpoint8=0;

 fPITAPOSVSetpoint3=0; 
 fPITAPOSVSetpoint4=0; 
 fPITAPOSVSetpoint7=0; 
 fPITAPOSVSetpoint8=0; 


 fPrevPITAPOSVSetpoint3=0; 
 fPrevPITAPOSVSetpoint4=0; 
 fPrevPITAPOSVSetpoint7=0; 
 fPrevPITAPOSVSetpoint8=0; 

   

 for(Int_t i=1;i<5;i++){
      fPrevHCIASetpoint[i]=0;
      fHCIASetpoint[i]=0;
    }

 for(Int_t i=1;i<5;i++){
      fPrevHBIASetpoint[i]=0;
      fHBIASetpoint[i]=0;
    }

for(Int_t i=1;i<5;i++){
      fPrevHAIASetpoint[i]=0;
      fHAIASetpoint[i]=0;
    }

    //PITA
    // fPrevPITASetpointPOS=0;
    //fPrevPITASetpointNEG=0;
    //fPITASetpointPOS=0;
    //fPITASetpointNEG=0;
	

    fTargetCharge.InitializeChannel("q_targ","derived");
    //fHCTargetCharge.InitializeChannel("q_targC","derived");
    fRunningCharge.InitializeChannel("q_targ","derived");
    
    //    fAsymBCM7.InitializeChannel("bcm7","derived");
    //fAsymBCM8.InitializeChannel("bcm8","derived");

    fChargeAsymmetry0.InitializeChannel("q_targ","derived");//this is the charge asym at the beginning of the feedback loop
    fPreviousChargeAsymmetry.InitializeChannel("q_targ","derived");//charge asymmetry at the previous feedback loop
    fCurrentChargeAsymmetry.InitializeChannel("q_targ","derived");//current charge asymmetry 

    fIAAsymmetry0.InitializeChannel("q_targ","derived");//this is the charge asymmetry of the IA at the beginning of the feedback loop
    fPreviousIAAsymmetry.InitializeChannel("q_targ","derived");//this is the charge asymmetry of the IA at the previous feedback loop
    fCurrentIAAsymmetry.InitializeChannel("q_targ","derived");//current charge asymmetry of the IA

    //fScalerChargeRunningSum.InitializeChannel("sca_bcm");
    //fScalerCharge.InitializeChannel("sca_bcm");

    // fHCChargeRunningSum.InitializeChannel("q_targC");
    //fHCCharge.InitializeChannel("q_targC");

/* fHCChargeAsymmetry0.InitializeChannel("q_targC","derived");//this is the charge asym at the beginning of the feedback loop */
/*      fPreviousHCChargeAsymmetry.InitializeChannel("q_targC","derived");//charge asymmetry at the previous feedback loop */
/*     fCurrentHCChargeAsymmetry.InitializeChannel("q_targC","derived");//current charge asymmetry  */
    fTargetParameter.InitializeChannel("tempvar","derived");

    fTargetHCChargeRunningSum.InitializeChannel("q_targC","derived");
    fTargetHAChargeRunningSum.InitializeChannel("q_targA","derived");
    fTargetHBChargeRunningSum.InitializeChannel("q_targB","derived");
    fXYPosXDiffRunningSum.InitializeChannel("xy_pos_x","derived");
    fXYPosYDiffRunningSum.InitializeChannel("xy_pos_y","derived");

     fTargetXDiffRunningSum.InitializeChannel("x_targ","derived");//to access the published Target X diff 
     /* fTargetXPDiffRunningSum.InitializeChannel("xp_targ","derived");//to access the published Target XP diff  */
     fTargetYDiffRunningSum.InitializeChannel("y_targ","derived");//to access the published Target Y diff 
    /* fTargetYPDiffRunningSum.InitializeChannel("yp_targ","derived");//to access the published Target YP diff */

    /* fAsymBCM78DDRunningSum.InitializeChannel("bcm78dd","derived"); */
    /* fYieldBCM8RunningSum.InitializeChannel("q_targ","derived"); */
    /* fAsymUSLumiSumRunningSum.InitializeChannel("uslumisum","derived"); */
 
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    out_file_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_IA_log.txt", "a");
    // out_file_IA = fopen("/local/scratch/qweak/Feedback_IA_log.txt", "a");

    //out_file_IA = fopen("/dev/shm/Feedback_IA_log.txt", "a");    
    
    fprintf(out_file_IA,"%22s \n",asctime (timeinfo));

    fprintf(out_file_IA,"Pat num. \t  A_q[mode]\t  IA Setpoint \t  IA Previous Setpoint \n");
    fclose(out_file_IA);
    //    out_file_PITA = fopen("Feedback_PITA_log.txt", "wt");

  //out_file_PITA = fopen("/local/scratch/qweak/Feedback_PITA_log.txt", "a");
  // out_file_HA_IA = fopen("/local/scratch/qweak/Feedback_HA_IA_log.txt", "a"); 

   out_file_PITA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITA_log.txt", "a");
   out_file_HA_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_HA_IA_log.txt", "a"); 
    fprintf(out_file_PITA,"%22s \n",asctime (timeinfo));
    fprintf(out_file_PITA,
	    "%9s %15s  %12s %12s %15s %15s %15s %15s\n",
	    "Pat num.,", "Charge Aq [ppm],", "+-Aq Error,", "Correction,",
	    "New PITA Setpoint[1],", "Old PITA Setpoint[1],",
    	    "New PITA Setpoint[5],", "Old PITA Setpoint[5],");
    fclose(out_file_PITA);

    /* fprintf(out_file_HA_IA,"%22s \n",asctime (timeinfo)); */
    /* fprintf(out_file_HA_IA, */
    /* 	    "%10s %22s  %16s %16s %26s %23s \n", */
    /* 	    "Pat num.", "Charge Aq(ppm)", "Aq Error", "Correction", */
    /* 	    "New IA Setpoint", "Old IA Setpoint"); */
    /* fclose(out_file_HA_IA); */

/* out_file_PITAPOS = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITAPOS_log.txt", "a");  */
/*     fprintf(out_file_PITAPOS,"%22s \n",asctime (timeinfo)); */
/*     fprintf(out_file_PITAPOS, */
/* 	    "%9s %15s %15s %12s %12s %12s %15s %15s %12s %15s %15s\n", */
/* 	    "Pat num.,", "DiffX [um],", "+-DiffX Error,","DiffY[um],","+-DiffY Error", "Correction POSU,", */
/* 	    "New PITAPOSU Setpoint[1],", "Old PITAPOSU Setpoint[1],","Correctoon POSV,", */
/*     	    "New PITAPOSV Setpoint[3],", "Old PITAPOSV Setpoint[3],"); */
/*     fclose(out_file_PITAPOS); */

out_file_PITAPOSU = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITAPOSU_log.txt", "a"); 
    fprintf(out_file_PITAPOSU,"%22s \n",asctime (timeinfo));
    fprintf(out_file_PITAPOSU,
	    "%9s %15s %15s %12s %12s %12s\n",
	    "Pat num.,", "DiffX [um],", "+-DiffX Error,", "Correction POSU,",
	    "New PITAPOSU Setpoint[1],", "Old PITAPOSU Setpoint[1],");
    fclose(out_file_PITAPOSU);

out_file_PITAPOSV = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITAPOSV_log.txt", "a"); 
    fprintf(out_file_PITAPOSV,"%22s \n",asctime (timeinfo));
    fprintf(out_file_PITAPOSV,
	    "%9s %15s %15s %12s %12s %12s\n",
	    "Pat num.,", "DiffY [um],", "+-DiffY Error,", "Correction POSV,",
	    "New PITAPOSV Setpoint[1],", "Old PITAPOSV Setpoint[1],");
    fclose(out_file_PITAPOSV);

out_file_PITAPOSXY = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITAPOSXY_log.txt", "a"); 
    fprintf(out_file_PITAPOSXY,"%22s \n",asctime (timeinfo));
    fprintf(out_file_PITAPOSXY,
	    "%9s %15s %15s %12s %12s %12s\n",
	    "Pat num.,", "DiffX [um],", "DiffY [um],", "Correction POSXY,",
	    "New PITAPOSXY Setpoint[1],", "Old PITAPOSXY Setpoint[1],");
    fclose(out_file_PITAPOSXY);


  out_file_HC_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_HC_IA_log.txt", "a");

    fprintf(out_file_HC_IA,"%22s \n",asctime (timeinfo));
    fprintf(out_file_HC_IA,
  	    "%10s %22s  %16s %16s %26s %23s \n",
  	    "Pat num.", "Charge Asym(ppm)", "Asym Error", "Correction",
  	    "New IA Setpoint", "Old IA Setpoint");
    fclose(out_file_HC_IA);

out_file_HA_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_HA_IA_log.txt", "a");

    fprintf(out_file_HA_IA,"%22s \n",asctime (timeinfo));
    fprintf(out_file_HA_IA,
  	    "%10s %22s  %16s %16s %26s %23s \n",
  	    "Pat num.", "Charge Asym(ppm)", "Asym Error", "Correction",
  	    "New IA Setpoint", "Old IA Setpoint");
    fclose(out_file_HA_IA);
 
out_file_HB_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_HB_IA_log.txt", "a");

    fprintf(out_file_HB_IA,"%22s \n",asctime (timeinfo));
    fprintf(out_file_HB_IA,
  	    "%10s %22s  %16s %16s %26s %23s \n",
  	    "Pat num.", "Charge Asym(ppm)", "Asym Error", "Correction",
  	    "New IA Setpoint", "Old IA Setpoint");
    fclose(out_file_HB_IA);
 
 
  
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
 void ClearHCRunningSum();
    void AccumulateRunningSum(QwHelicityCorrelatedFeedback& entry);
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
 void GetTargetPositionStatU();
 void GetTargetPositionStatV();
  void GetPositionStatXY();
    /// \brief retrieves the Hall A charge asymmetry,asymmetry error ,asymmetry width for given mode
    void GetHAChargeStat(Int_t mode);
void GetHCChargeStat();
void GetHAChargeStat();
void GetHBChargeStat();
 

    /// \brief Feed the Hall C IA set point based on the charge asymmetry 
    void FeedIASetPoint(Int_t mode);

    /// \brief Feed the Hall A IA set point based on the charge asymmetry 
    void FeedHAIASetPoint(Int_t mode);

    /// \brief Feed the Hall C PITA set point based on the charge asymmetry 
    void FeedPITASetPoints();

/// \brief Feed the Hall C IA set point based on the charge asymmetry 
    void FeedHCIASetPoints();
    void FeedHAIASetPoints();
    void FeedHBIASetPoints();
    void FeedPITAPOSUSetPoints();
    void FeedPITAPOSVSetPoints();
    void FeedPOSXYSetPoints();
    

 
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
  void LogPFUParameters();
 void LogPFVParameters();
 void LogXYParameters();



    /// \brief Log the last Hall A IA feedback information
    void LogHAParameters(Int_t mode);

  /// \brief Log the last Hall C IA feedback information
    void LogHCParameters();
 void LogHAParameters();
 void LogHBParameters();

    /// \brief Set Clean=0 or 1 in the GreenMonster
    void UpdateGMClean(Int_t state);

    /// \brief Update last feedback setting into scan variables in the GreenMonster
    void UpdateGMScanParameters();

    /// \brief Initiates the PITA feedback if the charge asymmetry passed the quality cut
    Bool_t ApplyPITAFeedback();
Bool_t ApplyPITAPOSUFeedback();
Bool_t ApplyPITAPOSVFeedback();
 Bool_t ApplyPITAPOSFeedback();
  Bool_t ApplyPOSXYFeedback();
    /// \brief Initiates the Hall A IA feedback if the Hall A charge asymmetry passed the quality cut
    Bool_t ApplyHAIAFeedback();

/// \brief Initiates the Hall C IA feedback if the Hall A charge asymmetry passed the quality cut
    Bool_t ApplyHCIAFeedback();
 Bool_t ApplyHBIAFeedback();


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
     //  Bool_t IsPFPatternsAccumulated(){
     //if (fPFGoodPatternCounter>=fPFAccumulatePatternMax)
     //	return kTRUE;

     //return kFALSE;
     //};

 Bool_t IsPFUPatternsAccumulated(){
     if (fPFUGoodPatternCounter>=fPFUAccumulatePatternMax)
     	return kTRUE;

     return kFALSE;
     };

Bool_t IsPFVPatternsAccumulated(){
     if (fPFVGoodPatternCounter>=fPFVAccumulatePatternMax)
     	return kTRUE;

     return kFALSE;
     };


Bool_t IsPosXPatternsAccumulated(){
     if (fXGoodPatternCounter>=fXYAccumulatePatternMax)
     	return kTRUE;

     return kFALSE;
     };

Bool_t IsPosYPatternsAccumulated(){
     if (fYGoodPatternCounter>=fXYAccumulatePatternMax)
     	return kTRUE;

     return kFALSE;
     };


    /// \brief Check to see no.of good patterns accumulated after the last position/angle feedback correction
    Bool_t IsHAPatternsAccumulated(){
      if (fHAIAGoodPatternCounter>=fHAIAAccumulatePatternMax)
	return kTRUE;

      return kFALSE;
    };

   /// \brief Check to see no.of good patterns accumulated after the last position/angle feedback correction
    Bool_t IsHCPatternsAccumulated(){
      if (fHCIAGoodPatternCounter>=fHCIAAccumulatePatternMax)
	return kTRUE;

      return kFALSE;
    };

Bool_t IsHBPatternsAccumulated(){
      if (fHBIAGoodPatternCounter>=fHBIAAccumulatePatternMax)
	return kTRUE;

      return kFALSE;
    };


    /// \brief Check neccessary conditions and apply IA setponts based on the charge asym for all four modes
    void ApplyFeedbackCorrections();
    /// \brief Check to see no.of good patterns accumulated after the last feedback is greater than a set value for given mode
    Bool_t IsPatternsAccumulated(Int_t mode);
 Bool_t IsPFPatternsAccumulated();
 // Bool_t IsPFUPatternsAccumulated();
 // Bool_t IsPFVPatternsAccumulated();
 Bool_t IsHCIAPatternsAccumulated();
 Bool_t IsHAIAPatternsAccumulated();
 Bool_t IsHBIAPatternsAccumulated();
 // Bool_t IsPosXYPatternsAccumulated();


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

    std::vector<const char*> vBPM ={"bpm4aWS",
				    "bpm4eWS",
				    "bpm0i02aWS",
				    "bpm1i06WS",
				    "bpm12WS",
				    "bpm0r05WS",
				    "bpm0l06WS",
				    "bpm0l05WS",
				    "bpm0i05WS",
				    "bpm2i02WS",
				    "bpm2i01WS",
				    "bcm0l02",
				    "bcm_an_ds3"};
    std::vector< std::pair<Double_t, Double_t> > vWireSumAsymmetry; //pair< mean, width>

    std::vector<const char*> vBPM1 ={"bpm0i02X",
    				     "bpm0i02Y",
    				     "bpm1i06X",
    				     "bpm1i06Y",
    				     "bpm4eX",
    				     "bpm4eY",
    				     "bpm4aX",
    				     "bpm4aY",
    				     "bpm12X",
    				     "bpm12Y",
    				     "bpm0r05X",
    				     "bpm0r05Y",
    				     "bpm0l06X",
    				     "bpm0l06Y",
    				     "bpm0l05X",
    				     "bpm0l05Y",
    				     "bpm0i05X",
    				     "bpm0i05Y",
    				     "bpm2i02X",
    				     "bpm2i02Y",
    				     "bpm2i01X",
    				     "bpm2i01Y"};
    
 

				   
 std::vector< std::pair<Double_t, Double_t> > vWireSumPosition; //pair< mean, width> 

    Double_t fChargeAsymmetry;//current charge asym
    Double_t fChargeAsymmetryError;//current charge asym precision
    Double_t fChargeAsymmetryWidth;//current charge asym width

Double_t fTargetHCCharge;//Hall C current charge asym
    Double_t fTargetHCChargeError;//Hall C current charge asym precision
    Double_t fTargetHCChargeWidth;//Hall C current charge asym width


Double_t fTargetHACharge;//Hall C current charge asym
    Double_t fTargetHAChargeError;//Hall C current charge asym precision
    Double_t fTargetHAChargeWidth;//Hall C current charge asym width


Double_t fTargetHBCharge;//Hall C current charge asym
    Double_t fTargetHBChargeError;//Hall C current charge asym precision
    Double_t fTargetHBChargeWidth;//Hall C current charge asym width

    /* //position/angle parameters */
    Double_t fTargetXDiff;
    Double_t fTargetXDiffError;
    Double_t fTargetXDiffWidth;

    /* Double_t fTargetXPDiff; */
    /* Double_t fTargetXPDiffError; */
    /* Double_t fTargetXPDiffWidth; */

    Double_t fTargetYDiff;
    Double_t fTargetYDiffError;
    Double_t fTargetYDiffWidth;

    Double_t fXDiff;
    Double_t fXDiffError;
    Double_t fXDiffWidth;

    Double_t fYDiff;
    Double_t fYDiffError;
    Double_t fYDiffWidth;

    /* Double_t fTargetYPDiff; */
    /* Double_t fTargetYPDiffError; */
    /* Double_t fTargetYPDiffWidth; */

    /* Double_t f3C12XDiff; */
    /* Double_t f3C12XDiffError; */
    /* Double_t f3C12XDiffWidth; */
    
    /* Double_t f3C12YDiff; */
    /* Double_t f3C12YDiffError; */
    /* Double_t f3C12YDiffWidth; */

    /* Double_t f3C12YQ; */
    /* Double_t f3C12YQError; */
    /* Double_t f3C12YQWidth; */

    /* Double_t fAsymBCM78DD; */
    /* Double_t fAsymBCM78DDError; */
    /* Double_t fAsymBCM78DDWidth; */

    /* Double_t fBCM8Yield; */

    
    /* Double_t fAsymBCMUSLumiSum; */
    /* Double_t fAsymBCMUSLumiSumError; */
    /* Double_t fAsymBCMUSLumiSumWidth; */

    Double_t fChargeAsym[kHelModes];//current charge asym
    Double_t fChargeAsymError[kHelModes];//current charge asym precision
    Double_t fChargeAsymWidth[kHelModes];//current charge asym width

    Double_t fHAChargeAsym[kHelModes];//Hall A current charge asym
    Double_t fHAChargeAsymError[kHelModes];//Hall A current charge asym precision
    Double_t fHAChargeAsymWidth[kHelModes];//Hall A current charge asym width

Double_t fHCChargeAsym;//Hall C current charge asym
    Double_t fHCChargeAsymError;//Hall C current charge asym precision
    Double_t fHCChargeAsymWidth;//Hall C current charge asym width


    Int_t fAccumulatePatternMax; //Hall C PITA Correction interval in units of patterns 
    Int_t fHAAccumulatePatternMax; //Hall A IA Correction interval in units of patterns 
    Int_t fPFAccumulatePatternMax; //HC Position/Angle difference  Correction interval in units of patterns 
Int_t fPFUAccumulatePatternMax;
Int_t fPFVAccumulatePatternMax;
  Int_t fHCIAAccumulatePatternMax; //Hall C PITA Correction interval in units of patterns 
 Int_t fHBIAAccumulatePatternMax;
Int_t fHAIAAccumulatePatternMax;
Int_t fXYAccumulatePatternMax;
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
    //Double_t fIASetpoint[kHelModes];//New setpont calculated based on the charge asymmetry
    //Double_t fPrevIASetpoint[kHelModes];//previous setpoint

    //Hall A IA setpoints for 4 modes 
    //  Double_t fHAIASetpoint[kHelModes];//New setpont calculated based on the charge asymmetry
    // Double_t fPrevHAIASetpoint[kHelModes];//previous setpoint

    Double_t fPITASetpoint[];//New setpont calculated based on the charge asymmetry
    Double_t fPrevPITASetpoint[];
    // Double_t fPOSXYSetpoint[];
    //Double_t fPrevPOSXYSetpoint[];//previous setpoin

Double_t fHCIASetpoint[];//New setpont calculated based on the charge asymmetry
    Double_t fPrevHCIASetpoint[];//previous setpoin

Double_t fHAIASetpoint[];//New setpont calculated based on the charge asymmetry
    Double_t fPrevHAIASetpoint[];//previous setpoin

Double_t fHBIASetpoint[];//New setpont calculated based on the charge asymmetry
    Double_t fPrevHBIASetpoint[];//previous setpoin

Double_t fPITAPOSUSetpoint1;
Double_t fPITAPOSUSetpoint2;
Double_t fPITAPOSUSetpoint5;
 Double_t fPITAPOSUSetpoint6;

Double_t fPrevPITAPOSUSetpoint1;
Double_t fPrevPITAPOSUSetpoint2;
Double_t fPrevPITAPOSUSetpoint5;
Double_t fPrevPITAPOSUSetpoint6;

 Double_t fPITAPOSVSetpoint3; 
 Double_t fPITAPOSVSetpoint4; 
 Double_t fPITAPOSVSetpoint7; 
 Double_t fPITAPOSVSetpoint8; 


 Double_t fPrevPITAPOSVSetpoint3;
 Double_t fPrevPITAPOSVSetpoint4; 
 Double_t fPrevPITAPOSVSetpoint7; 
 Double_t fPrevPITAPOSVSetpoint8; 

 Double_t fPOSXYSetpoint1;
Double_t fPOSXYSetpoint2;
Double_t fPOSXYSetpoint3;
Double_t fPOSXYSetpoint4;
Double_t fPOSXYSetpoint5;
Double_t fPOSXYSetpoint6;
Double_t fPOSXYSetpoint7;
Double_t fPOSXYSetpoint8;

 Double_t fPrevPOSXYSetpoint1;
 Double_t fPrevPOSXYSetpoint2;
 Double_t fPrevPOSXYSetpoint3;
 Double_t fPrevPOSXYSetpoint4;
 Double_t fPrevPOSXYSetpoint5;
 Double_t fPrevPOSXYSetpoint6;
 Double_t fPrevPOSXYSetpoint7;
 Double_t fPrevPOSXYSetpoint8;

    Double_t fIASetpointlow;//lower and upper limits for IA dac hardware counts
    Double_t fIASetpointup;

Double_t fHCIASetpointlow;//lower and upper limits for IA dac hardware counts
    Double_t fHCIASetpointup;


Double_t fHAIASetpointlow;//lower and upper limits for IA dac hardware counts
    Double_t fHAIASetpointup;


Double_t fHBIASetpointlow;//lower and upper limits for IA dac hardware counts
    Double_t fHBIASetpointup;

    Double_t fxU;
    Double_t fxV;
    Double_t fyV;
    Double_t fyU;

    //PITA Slopes for halfwave plate IN & OUT
    Double_t fPITASlopeIN;//IHWP1 IN IHWP2 OUT
    Double_t fPITASlopeOUT;//IHWP1 OUT IHWP2 OUT
    Double_t fPITASlopeOUT_IN;//IHWP1 OUT IHWP2 IN
    Double_t fPITASlope;

 Double_t fPITAPOSUSlope;
 Double_t fPITAPOSVSlope;
 Double_t fPITAPOSUSlopeIN;
 Double_t fPITAPOSUSlopeOUT;
 Double_t fPITAPOSVSlopeIN;
 Double_t fPITAPOSVSlopeOUT;


   Double_t fHCIASlopeIN;//IHWP1 IN 
    Double_t fHCIASlopeOUT;//IHWP1 out
     Double_t fHCIASlope;

Double_t fHAIASlopeIN;//IHWP1 IN 
    Double_t fHAIASlopeOUT;//IHWP1 out
     Double_t fHAIASlope;

Double_t fHBIASlopeIN;//IHWP1 IN 
    Double_t fHBIASlopeOUT;//IHWP1 out
     Double_t fHBIASlope;


    //PITA setpoints for pos hel and neg hel
    Double_t fPITASetpointPOS;
    Double_t fPITASetpointNEG;
    Double_t fPITASetpointPOS_t0_IN;//Initial PC positive HW setpoint
    Double_t fPITASetpointNEG_t0_IN;//Initial PC negative HW setpoint
    Double_t fPITASetpointPOS_t0_OUT;//Initial PC positive HW setpoint
    Double_t fPITASetpointNEG_t0_OUT;//Initial PC negative HW setpoint
    Bool_t fInitialCorrection;//Is true at the beginning so that t_0 correction is appiled before doing any correction
    
    //Amali2019
    Double_t fPITASetpoint1;
    Double_t fPITASetpoint2;
    Double_t fPITASetpoint3;
    Double_t fPITASetpoint4;
    Double_t fPITASetpoint5;
    Double_t fPITASetpoint6;
    Double_t fPITASetpoint7;
    Double_t fPITASetpoint8;

    Double_t fHCIASetpoint1;
    Double_t fHCIASetpoint2;
    Double_t fHCIASetpoint3;
    Double_t fHCIASetpoint4;

    Double_t fHAIASetpoint1;
    Double_t fHAIASetpoint2;
    Double_t fHAIASetpoint3;
    Double_t fHAIASetpoint4;

    Double_t fHBIASetpoint1;
    Double_t fHBIASetpoint2;
    Double_t fHBIASetpoint3;
    Double_t fHBIASetpoint4;

    Double_t fPrevPITASetpointPOS;//previous setpoint
    Double_t fPrevPITASetpointNEG;//previous setpoint
    Double_t fPITASetpointlow;//lower and upper limits for PITA dac hardware counts
    Double_t fPITASetpointup;
    Double_t fPITA_MIN_Charge_asym;//Minimum charge asymmetry at which feedback correction is applied. see QwHelicityCorrelatedFeedback::FeedPITASetPoints()

    //Amali2019
    Double_t fPrevPITASetpoint1;
    Double_t fPrevPITASetpoint2; 
    Double_t fPrevPITASetpoint3; 
    Double_t fPrevPITASetpoint4; 
    Double_t fPrevPITASetpoint5; 
    Double_t fPrevPITASetpoint6; 
    Double_t fPrevPITASetpoint7; 
    Double_t fPrevPITASetpoint8;  

   Double_t fPrevHCIASetpoint1;
   Double_t fPrevHCIASetpoint2;
   Double_t fPrevHCIASetpoint3;
   Double_t fPrevHCIASetpoint4;


   Double_t fPrevHAIASetpoint1;
   Double_t fPrevHAIASetpoint2;
   Double_t fPrevHAIASetpoint3;
   Double_t fPrevHAIASetpoint4;


   Double_t fPrevHBIASetpoint1;
   Double_t fPrevHBIASetpoint2;
   Double_t fPrevHBIASetpoint3;
   Double_t fPrevHBIASetpoint4;

    ///  Create an EPICS control event
    QwEPICSControl fEPICSCtrl;
    GreenMonster   fScanCtrl;

    //Pattern counter
    Int_t fGoodPatternCounter;//increment the quartet number - reset after each PITA feedback operation
    Int_t fHAGoodPatternCounter;//increment the quartet number - reset after each Hall A IA feedback operation
    Int_t fHCIAGoodPatternCounter;//increment the quartet number - reset after each Hall C IA feedback operation
 Int_t fHAIAGoodPatternCounter;
 Int_t fHBIAGoodPatternCounter;
    //Int_t fPFGoodPatternCounter;//increment the quartet number - reset after each position/angle feedback operation
Int_t fPFUGoodPatternCounter;
Int_t fPFVGoodPatternCounter;
Int_t fXGoodPatternCounter;
Int_t fYGoodPatternCounter;
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

    QwBeamCharge fHCCharge;
   QwBeamCharge fHCChargeRunningSum;
QwBeamCharge   fPreviousHCChargeAsymmetry;
 QwBeamCharge   fHCChargeAsymmetry0;
 QwBeamCharge   fCurrentHCChargeAsymmetry;



    /* QwSIS3801D24_Channel   fScalerCharge;//for Hall A feedback */
    /* QwSIS3801D24_Channel   fScalerChargeRunningSum;//for Hall A feedback */

     QwBeamCharge   fTargetParameter;//to access the published postions/angles 
     QwBeamCharge   fTargetHCChargeRunningSum;
     QwBeamCharge   fTargetHAChargeRunningSum;
     QwBeamCharge   fTargetHBChargeRunningSum;
    QwBeamCharge   fTargetXDiffRunningSum;//to access the published Target X diff
    /* QwBeamCharge   fTargetXPDiffRunningSum;//to access the published Target XP diff */
    QwBeamCharge   fTargetYDiffRunningSum;//to access the published Target Y diff 
    QwBeamCharge   fXYPosXDiffRunningSum;//to access the published Target X diff for 2 by 2 xy pos feedback
    QwBeamCharge   fXYPosYDiffRunningSum;//to access the published Target Y diff for 2 by 2 xy pos feedback
    /* QwBeamCharge   fTargetYPDiffRunningSum;//to access the published Target YP diff */
    /* QwBeamCharge   f3C12XDiffRunningSum;//to access the published 3c12 X diff */
    /* QwBeamCharge   f3C12YDiffRunningSum;//to access the published 3c12 Y diff */
    /* QwBeamCharge   f3C12YQRunningSum;//to access the published 3c12 eff. charge */

    /* QwBeamCharge   fAsymBCM7;//to access bcm7 asymmetry */
    /* QwBeamCharge   fAsymBCM8;//to access bcm8 asymmetry */
    /* QwBeamCharge   fAsymBCM78DDRunningSum;//to accumulate bcm78 DD asymmetry */
    /* QwBeamCharge   fYieldBCM8RunningSum;//to access bcm8 Yield */

    /* QwBeamCharge   fAsymUSLumiSumRunningSum;//to accumulate US Lumi sum */


    //log file
    FILE *out_file_PITA;
    FILE *out_file_PITAPOSXY;
    FILE *out_file_IA;
    FILE *IHWP_State;
    FILE *out_file_HA_IA;
 FILE *out_file_HC_IA;
FILE *out_file_HB_IA;
    FILE *out_file_PC_IN_pos;
    FILE *out_file_PC_IN_neg;
    FILE *out_file_PC_OUT_pos;
    FILE *out_file_PC_OUT_neg;
    FILE *out_file_PITAPOS;
 FILE *out_file_PITAPOSU;
FILE *out_file_PITAPOSV;

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
   Bool_t fHCIAFB;
Bool_t fHBIAFB;
   Bool_t fIAFB;
   Bool_t fPITAPOSUFB;
   Bool_t fPITAPOSVFB;
  Bool_t fPOSXYFB;
    Bool_t fFeedbackStatus;
    Bool_t fFeedbackDamping;


    time_t rawtime;
    struct tm * timeinfo;

  
};

#endif
