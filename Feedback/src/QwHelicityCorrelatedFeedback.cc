/**********************************************************\
* File: QwHelicityCorrelatedFeedback.cc                   *
*                                                         *
* Author:Author: Rakitha Beminiwattha                     *
* Time-stamp:Time-stamp: <2010-09-28>                     *
\**********************************************************/

#include "QwHelicityCorrelatedFeedback.h"
#include "TSystem.h"

/*****************************************************************/


/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwHelicityCorrelatedFeedback::DefineOptions(QwOptions &options)

{

  options.AddOptions("Helicity Correlated Feedback")("Auto-IHWP-Flip", po::value<bool>()->default_value(false)->zero_tokens(),"Set Half wave plate IN. The default is Half wave plate OUT");
  options.AddOptions("Helicity Correlated Feedback")("Half-wave-plate-revert", po::value<bool>()->default_value(false)->zero_tokens(),"Revert half-wave plate status. The default is determined via EPIC");
  //options.AddOptions("Helicity Correlated Feedback")("Half-wave-plate-OUT", po::value<bool>()->default_value(true)->zero_tokens(),"Half wave plate OUT");
  options.AddOptions("Helicity Correlated Feedback")("PITA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the PITA charge feedback");
  //options.AddOptions("Helicity Correlated Feedback")("IA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the IA charge feedback");
  //options.AddOptions("Helicity Correlated Feedback")("HA-IA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the Hall A IA charge feedback");
     options.AddOptions("Helicity Correlated Feedback")("HC-IA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the Hall C IA charge feedback");
options.AddOptions("Helicity Correlated Feedback")("HA-IA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the Hall A IA charge feedback");
options.AddOptions("Helicity Correlated Feedback")("HB-IA-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the Hall B IA charge feedback");
 options.AddOptions("Helicity Correlated Feedback")("PITAPOSU-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the U POSITION feedback");
 options.AddOptions("Helicity Correlated Feedback")("PITAPOSV-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the V POSITION feedback");
 options.AddOptions("Helicity Correlated Feedback")("POSXY-Feedback", po::value<bool>()->default_value(false)->zero_tokens(),"Run the xy POSITION feedback");
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::ProcessOptions(QwOptions &options)
{
  QwHelicityPattern::ProcessOptions(options);

  fHalfWaveRevert      = options.GetValue<bool>("Half-wave-plate-revert");
  fHalfWavePlateStatus = GetHalfWavePlateState();//read the IHWP for the first time when the feedback is starting.
  fAutoIHWP = options.GetValue<bool>("Auto-IHWP-Flip");



  printf("NOTICE \n QwFeedback got the half-wave plate status %s\n", fHalfWavePlateStatus.Data());

  if(fHalfWavePlateStatus.Contains("IN")) {
    if(fHalfWaveRevert) fHalfWaveIN = false;
    else                fHalfWaveIN = true;
    fDefaultHalfWavePlateStatus=1;
  }
  else {
    if(fHalfWaveRevert) fHalfWaveIN = true;
    else                fHalfWaveIN = false;
    fDefaultHalfWavePlateStatus=0;
  }

  fHalfWaveOUT = !fHalfWaveIN;

  if (fHalfWaveIN)
    printf("NOTICE \n Half-wave-plate-IN\n");
  else
    printf("NOTICE \n Half-wave-plate-OUT\n");

  if (GetHalfWavePlate2State()==1)
    printf("NOTICE \n Half-wave-plate-2 is IN \n");
  else if (GetHalfWavePlate2State()==0)
    printf("NOTICE \n Half-wave-plate-2 is OUT \n");

 


  fPITAFB = options.GetValue<bool>("PITA-Feedback");
  //  fHAIAFB = options.GetValue<bool>("HA-IA-Feedback");
  //fIAFB   = options.GetValue<bool>("IA-Feedback"); 
  fHCIAFB = options.GetValue<bool>("HC-IA-Feedback");
fHAIAFB = options.GetValue<bool>("HA-IA-Feedback");
fHBIAFB = options.GetValue<bool>("HB-IA-Feedback");
  fPITAPOSUFB = options.GetValue<bool>("PITAPOSU-Feedback");
  fPITAPOSVFB = options.GetValue<bool>("PITAPOSV-Feedback");
 fPOSXYFB = options.GetValue<bool>("POSXY-Feedback");

  if (fPITAFB)
    printf("NOTICE \n   PITA-Feedback is running.\n");
  else
    printf("NOTICE \n   PITA-Feedback is not running.\n");
  //if (fIAFB)
  //printf("NOTICE \n   IA-Feedback is running.\n");
  //else
  //printf("NOTICE \n   IA-Feedback is not running.\n");
  //if (fHAIAFB)
  //printf("NOTICE \n   Hall A IA-Feedback is running.\n");
  //else
  //printf("NOTICE \n   Hall A IA-Feedback is not running.\n");
  if (fHCIAFB)
     printf("NOTICE \n   Hall C IA-Feedback is running.\n");
   else
     printf("NOTICE \n   Hall C IA-Feedback is not running.\n");
if (fHAIAFB)
     printf("NOTICE \n   Hall A IA-Feedback is running.\n");
   else
     printf("NOTICE \n   Hall A IA-Feedback is not running.\n");
if (fHBIAFB)
     printf("NOTICE \n   Hall B IA-Feedback is running.\n");
   else
     printf("NOTICE \n   Hall B IA-Feedback is not running.\n");
 if (fPITAPOSUFB)
    printf("NOTICE \n   PITA-POSU is running.\n");
else
     printf("NOTICE \n   PITA-POSU Feedback is not running.\n");
 if (fPITAPOSVFB)
    printf("NOTICE \n   PITA-POSV is running.\n");
 else
  printf("NOTICE \n   PITA-POSV Feedback is not running.\n");
if (fPOSXYFB)
    printf("NOTICE \n   POS XY feedback is running.\n");
else
  printf("NOTICE \n   POS XY Feedback is not running.\n");


  if (!fPITAFB && !fHCIAFB && !fHAIAFB && !fHBIAFB && !fPITAPOSUFB && !fPITAPOSVFB && !fPOSXYFB){//no correction applied.
    fEPICSCtrl.Set_FeedbackStatus(0);
    exit(1); 
  }

  if (fAutoIHWP){
    printf("NOTICE \n   AUTO IHWP Flip is ON \n");
    //exit(1);
  }else{
    printf("NOTICE \n   AUTO IHWP Flip is OFF \n");
    //exit(1);
  }
  //exit(1);
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LoadParameterFile(TString filename){
  char buffer [10];
  TString prev_IHWP_State;

  QwParameterFile mapstr(filename.Data());  //Open the file
  TString varname, varvalue;
  UInt_t value;
  Double_t dvalue;
  fIASetpointlow=11000;
  fIASetpointup=40000;
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('#');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
      //Decode it.
      varname.ToLower();
      if (varname=="patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fAccumulatePatternMax=value;
      }
      else if (varname=="haf_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fHAAccumulatePatternMax=value;
      }else if(varname=="pf_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fPFAccumulatePatternMax=value;
 }else if(varname=="pfu_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fPFUAccumulatePatternMax=value;
 }else if(varname=="pfv_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fPFVAccumulatePatternMax=value;
        }else if(varname=="xy_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fXYAccumulatePatternMax=value;
	}  else if (varname=="cia_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fHCIAAccumulatePatternMax=value;
        }  else if (varname=="aia_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fHAIAAccumulatePatternMax=value;
    }  else if (varname=="bia_patterns"){
	value = QwParameterFile::GetUInt(varvalue);
	fHBIAAccumulatePatternMax=value;
      }else if (varname=="deltaaq"){
	dvalue = atof(varvalue.Data());
	fChargeAsymPrecision=dvalue;
      }
      else if (varname=="optimalia"){
	dvalue = atof(varvalue.Data());
	fOptimalIA = dvalue;
      }
      else if (varname=="optimalpcp"){
	dvalue = atof(varvalue.Data());
	fOptimalPCP = dvalue;
      }
      else if (varname=="optimalpcn"){
	dvalue = atof(varvalue.Data());
	fOptimalPCN = dvalue;
      }
      else if (varname=="a0"){
	dvalue = atof(varvalue.Data());
	fIASlopeA[0] = dvalue;
      }
      else if (varname=="deltaa0"){
	dvalue = atof(varvalue.Data());
	fDelta_IASlopeA[0] = dvalue;
      }
      else if (varname=="a1"){
	dvalue = atof(varvalue.Data());
	fIASlopeA[1] = dvalue;
      }
      else if (varname=="deltaa1"){
	dvalue = atof(varvalue.Data());
	fDelta_IASlopeA[1] = dvalue;
      }
      else if (varname=="a2"){
	dvalue = atof(varvalue.Data());
	fIASlopeA[2] = dvalue;
      }
      else if (varname=="deltaa2"){
	dvalue = atof(varvalue.Data());
	fDelta_IASlopeA[2] = dvalue;
      }
      else if (varname=="a3"){
	dvalue = atof(varvalue.Data());
	fIASlopeA[3] = dvalue;
      }
      else if (varname=="deltaa3"){
	dvalue = atof(varvalue.Data());
	fDelta_IASlopeA[3] = dvalue;
      }
      else if (varname=="haa0"){
	dvalue = atof(varvalue.Data());
	fHAIASlopeA[0] = dvalue;
      }
      else if (varname=="haa1"){
	dvalue = atof(varvalue.Data());
	fHAIASlopeA[1] = dvalue;
      }
      else if (varname=="haa2"){
	dvalue = atof(varvalue.Data());
	fHAIASlopeA[2] = dvalue;
      }
      else if (varname=="haa3"){
	dvalue = atof(varvalue.Data());
	fHAIASlopeA[3] = dvalue;
      }
      else if (varname=="ia_low"){
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fIASetpointlow = dvalue;
      }
      else if (varname=="ia_up"){
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fIASetpointup = dvalue;
      }
      else if (varname=="hc_ia_up"){
      dvalue = atof(varvalue.Data());
      if (dvalue>0)
        fHCIASetpointup = dvalue;
      }
      else if (varname=="hc_ia_low"){
      dvalue = atof(varvalue.Data());
      if (dvalue>0)
        fHCIASetpointlow = dvalue;
      }
      else if (varname=="ha_ia_up"){
      dvalue = atof(varvalue.Data());
      if (dvalue>0)
        fHAIASetpointup = dvalue;
      }
      else if (varname=="ha_ia_low"){
      dvalue = atof(varvalue.Data());
      if (dvalue>0)
        fHAIASetpointlow = dvalue;
      }
else if (varname=="hb_ia_up"){
      dvalue = atof(varvalue.Data());
      if (dvalue>0)
        fHBIASetpointup = dvalue;
      }
      else if (varname=="hb_ia_low"){
      dvalue = atof(varvalue.Data());
      if (dvalue>0)
        fHBIASetpointlow = dvalue;
      }
      else if (varname=="pitaslope_in"){//IHWP1 IN IHWP2 OUT
	dvalue = atof(varvalue.Data());
	fPITASlopeIN = dvalue;
      }
      else if (varname=="pitaslope_out"){//IHWP1 OUT IHWP2 OUT
	dvalue = atof(varvalue.Data());
	fPITASlopeOUT = dvalue;
      }
      else if (varname=="pitaslope_out_in"){//IHWP1 OUT IHWP2 IN
	dvalue = atof(varvalue.Data());
	fPITASlopeOUT_IN = dvalue;
      }

      else if (varname=="pitaposuslope_in"){//IHWP1 IN IHWP2 OUT
	dvalue = atof(varvalue.Data());
	fPITAPOSUSlopeIN = dvalue;
      }
      else if (varname=="pitaposvslope_out"){//IHWP1 OUT IHWP2 OUT
	dvalue = atof(varvalue.Data());
	fPITAPOSVSlopeOUT = dvalue;
      }
 else if (varname=="pitaposuslope_out"){//IHWP1 IN IHWP2 OUT
	dvalue = atof(varvalue.Data());
	fPITAPOSUSlopeOUT = dvalue;
      }
      else if (varname=="pitaposvslope_in"){//IHWP1 OUT IHWP2 OUT
	dvalue = atof(varvalue.Data());
	fPITAPOSVSlopeIN = dvalue;
      }

      else if (varname=="hallciaslope_in"){//IHWP1 OUT IHWP2 IN
      dvalue = atof(varvalue.Data());
      fHCIASlopeIN = dvalue;
      }
      else if (varname=="hallciaslope_out"){//IHWP1 OUT IHWP2 IN
      dvalue = atof(varvalue.Data());
      fHCIASlopeOUT = dvalue;
      }

    else if (varname=="hallaiaslope_in"){//IHWP1 OUT IHWP2 IN
      dvalue = atof(varvalue.Data());
      fHAIASlopeIN = dvalue;
      }
      else if (varname=="hallaiaslope_out"){//IHWP1 OUT IHWP2 IN
      dvalue = atof(varvalue.Data());
      fHAIASlopeOUT = dvalue;
      }
    else if (varname=="hallbiaslope_in"){//IHWP1 OUT IHWP2 IN
      dvalue = atof(varvalue.Data());
      fHBIASlopeIN = dvalue;
      }
      else if (varname=="hallbiaslope_out"){//IHWP1 OUT IHWP2 IN
      dvalue = atof(varvalue.Data());
      fHBIASlopeOUT = dvalue;
      }
     else if (varname=="xu"){
	dvalue = atof(varvalue.Data());
	fxU = dvalue;
      }
     else if (varname=="xv"){
	dvalue = atof(varvalue.Data());
	fxV = dvalue;
      }
      else if (varname=="yu"){
	dvalue = atof(varvalue.Data());
	fyU = dvalue;
      }
     else if (varname=="yv"){
	dvalue = atof(varvalue.Data());
	fyV = dvalue;
      }
      else if (varname=="pc_pos_t0_in"){
	dvalue = atof(varvalue.Data());
	fPITASetpointPOS_t0_IN = dvalue;
      } 
      else if (varname=="pc_neg_t0_in"){
	dvalue = atof(varvalue.Data());
	fPITASetpointNEG_t0_IN = dvalue;
      } 
      else if (varname=="pc_pos_t0_out"){
	dvalue = atof(varvalue.Data());
	fPITASetpointPOS_t0_OUT = dvalue;
      }
      else if (varname=="pc_neg_t0_out"){
	dvalue = atof(varvalue.Data());
	fPITASetpointNEG_t0_OUT = dvalue;
      } 
      else if (varname=="pc_up"){
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fPITASetpointup = dvalue;
      }
      else if (varname=="pc_low"){
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fPITASetpointlow = dvalue;
      }else if (varname=="min_charge_asym"){  
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fPITA_MIN_Charge_asym = dvalue;
      } else if (varname=="damping"){  
	dvalue = atof(varvalue.Data());
	if (dvalue>0)
	  fFeedbackDamping=kTRUE;
	else
	  fFeedbackDamping=kFALSE;
      } 


      
    }
  }

  //Read the last know good PC set points
  // out_file_PC_IN_pos = fopen("/local/scratch/qweak/Last_good_PC_pos_IN", "r");//Open in read mode
  //out_file_PC_IN_neg = fopen("/local/scratch/qweak/Last_good_PC_neg_IN", "r");//Open in read mode      
  //out_file_PC_OUT_pos = fopen("/local/scratch/qweak/Last_good_PC_pos_OUT", "r");//Open in read mode
  //out_file_PC_OUT_neg = fopen("/local/scratch/qweak/Last_good_PC_neg_OUT", "r");//Open in read mode

  // if (out_file_PC_IN_pos &&  out_file_PC_IN_neg && out_file_PC_OUT_pos && out_file_PC_OUT_neg){
  //UInt_t tmp;
  // fscanf(out_file_PC_IN_pos,"%d",&tmp);
  // fPITASetpointPOS_t0_IN=tmp;
  //fscanf(out_file_PC_IN_neg,"%d",&tmp);
  //fPITASetpointNEG_t0_IN=tmp;
  //fscanf(out_file_PC_OUT_pos,"%d",&tmp);
  //fPITASetpointPOS_t0_OUT=tmp;
  //fscanf(out_file_PC_OUT_neg,"%d",&tmp);
  //fPITASetpointNEG_t0_OUT=tmp;
  //fclose(out_file_PC_IN_pos);
  //fclose(out_file_PC_IN_neg);
  //fclose(out_file_PC_OUT_pos);
  //fclose(out_file_PC_OUT_neg);
    
  //}



  QwMessage<<"Hall A patternMax for PITA = "<<fAccumulatePatternMax<<QwLog::endl;
  QwMessage<<"Hall C patternMax for IA = "<<fHCIAAccumulatePatternMax<<QwLog::endl;
 QwMessage<<"Hall A patternMax for IA = "<<fHAIAAccumulatePatternMax<<QwLog::endl;
 QwMessage<<"Hall B patternMax for IA = "<<fHBIAAccumulatePatternMax<<QwLog::endl;
  QwMessage<<"PFU patternMax = "<<fPFUAccumulatePatternMax<<QwLog::endl;
  QwMessage<<"PFV patternMax = "<<fPFVAccumulatePatternMax<<QwLog::endl;
  QwMessage<<"XY feedback patternMax = "<<fXYAccumulatePatternMax<<QwLog::endl;
  QwMessage<<"XU = "<<fxU<<QwLog::endl;
 QwMessage<<"XV = "<<fxV<<QwLog::endl;
 QwMessage<<"YU = "<<fyU<<QwLog::endl;
 QwMessage<<"YV = "<<fyV<<QwLog::endl;
  
  
 QwMessage<<"PITA slopes: H-wave IN "<<fPITASlopeIN<<" H-wave OUT "<<fPITASlopeOUT<<QwLog::endl;
 QwMessage<<"C IA slopes: H-wave IN "<<fHCIASlopeIN<<" H-wave OUT "<<fHCIASlopeOUT<<QwLog::endl;
 QwMessage<<"A IA slopes: H-wave IN "<<fHAIASlopeIN<<" H-wave OUT "<<fHAIASlopeOUT<<QwLog::endl;
 QwMessage<<"B IA slopes: H-wave IN "<<fHBIASlopeIN<<" H-wave OUT "<<fHBIASlopeOUT<<QwLog::endl;
 QwMessage<<"PITA PosU slopes: H-wave IN "<<fPITAPOSUSlopeIN<<" H-wave OUT "<<fPITAPOSUSlopeOUT<<QwLog::endl;
 QwMessage<<"PITA PosV slopes: H-wave IN "<<fPITAPOSVSlopeIN<<" H-wave OUT "<<fPITAPOSVSlopeOUT<<QwLog::endl;

  QwMessage<<"PC dac limits "<<fPITASetpointlow<<" to "<<fPITASetpointup<<QwLog::endl;

  

  // if (fAutoIHWP){
  //   IHWP_State = fopen("/local/scratch/qweak/Feedback_IHWP.txt", "r");//Open in read mode to get the previous IHWP state
  //   if (IHWP_State!=NULL){
  //     fgets(buffer , 10 ,IHWP_State);//read the previous IHWP mode
  //     //printf("NOTICE \n %s \n",buffer);
  //     prev_IHWP_State=buffer;
  //     if (prev_IHWP_State.Contains("IN") || prev_IHWP_State.Contains("OUT")){
  // 	if (prev_IHWP_State.Contains("IN"))
  // 	  fPreviousHalfWavePlateStatus=1;
  // 	else
  // 	  fPreviousHalfWavePlateStatus=0;
  // 	if (fHalfWavePlateStatus.IsNull())//EPICS IHWP state is missing
  // 	  fHalfWavePlateStatus=prev_IHWP_State;//in case EPICS IHWP state is not existing set the previous run IHWP state
  //     }
  //   }
  //   else
  //     prev_IHWP_State="";

  //   if (!prev_IHWP_State.Contains("IN") && !prev_IHWP_State.Contains("OUT") ){//first time the file created
  //     if (IHWP_State!=NULL)
  // 	fclose(IHWP_State); 
  //     IHWP_State = fopen("/local/scratch/qweak/Feedback_IHWP.txt", "w");
  //     printf("NOTICE \n previous half-wave plate status not found. Writing current IHWP state %s \n",fHalfWavePlateStatus.Data());
  //     if (IHWP_State!=NULL){
  // 	fprintf(IHWP_State,"%s",fHalfWavePlateStatus.Data());
  // 	fclose(IHWP_State);

  //     }
  //   }
  //   else{
  //     if (fDefaultHalfWavePlateStatus!=fPreviousHalfWavePlateStatus){//at the begging of the feedback, if the current IHWP is not the same as last known good IHWP state then reset the PC values
  // 	printf("NOTICE \n Half-wave plate status has changed from %s to %s \n",prev_IHWP_State.Data(),fHalfWavePlateStatus.Data());
  // 	if (fHalfWaveIN){
  // 	  if (fPITASetpointPOS_t0_IN && fPITASetpointNEG_t0_IN){
  // 	    //fEPICSCtrl.Set_Pockels_Cell_plus(fPITASetpointPOS_t0_IN);
  // 	    //fEPICSCtrl.Set_Pockels_Cell_minus(fPITASetpointNEG_t0_IN);
  // 	  }
  // 	} else{
  // 	  if (fPITASetpointPOS_t0_OUT && fPITASetpointNEG_t0_OUT){
  // 	    //fEPICSCtrl.Set_Pockels_Cell_plus(fPITASetpointPOS_t0_OUT);
  // 	    //fEPICSCtrl.Set_Pockels_Cell_minus(fPITASetpointNEG_t0_OUT);
  // 	  }
  // 	}	
  //     }
  //     printf("NOTICE \n previous half-wave plate status %s\n",prev_IHWP_State.Data());
  //     if (IHWP_State!=NULL)
  // 	fclose(IHWP_State); 
  //     IHWP_State = fopen("/local/scratch/qweak/Feedback_IHWP.txt", "w");
  //     printf("NOTICE \n Updating new IHWP %s \n",fHalfWavePlateStatus.Data());
  //     if (IHWP_State!=NULL){
  // 	fprintf(IHWP_State,"%s",fHalfWavePlateStatus.Data());   
  // 	fclose(IHWP_State);

  //     }
 
  //   }  
  // }

};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedIASetPoint(Int_t mode){
  // //calculate the new setpoint
  // fEPICSCtrl.Get_HallCIA(mode,fPrevIASetpoint[mode]);
  // if (fIASlopeA[mode]!=0)
  //   fIASetpoint[mode]=fPrevIASetpoint[mode] - fChargeAsym[mode]/fIASlopeA[mode];
  // else
  //   fIASetpoint[mode]=fPrevIASetpoint[mode];
  
  // if (fIASetpoint[mode]>fIASetpointup)
  //   fIASetpoint[mode]=fIASetpointup;
  // else if (fIASetpoint[mode]<fIASetpointlow)
  //   fIASetpoint[mode]=fIASetpointlow;

  // QwMessage<<"FeedIASetPoint("<<mode<<") "<<fChargeAsym[mode]<<"+/-"<<fChargeAsymError[mode]<<" new set point  "<<fIASetpoint[mode]<<QwLog::endl;
  // //send the new IA setpoint 

  // fEPICSCtrl.Set_HallCIA(mode,fIASetpoint[mode]);

  // //updating the standard asymmetry statistics
  // //commented out - rakithab 01-06-2011
  // //GetTargetChargeStat();
  // //fEPICSCtrl.Set_ChargeAsymmetry(fChargeAsymmetry,fChargeAsymmetryError,fChargeAsymmetryWidth);//updates the epics values

  // //Greenmonster stuffs
  // //fScanCtrl.SCNSetValue(1,0);
  // //fScanCtrl.SCNSetValue(2,0);
  // //fScanCtrl.CheckScan();
  // //fScanCtrl.PrintScanInfo();
  // //fScanCtrl.Close();  
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedHAIASetPoint(Int_t mode){
  // //calculate the new setpoint
  
  // fEPICSCtrl.Get_HallAIA(mode,fPrevHAIASetpoint[mode]);
  // if (fIASlopeA[mode]!=0){

  //   if (fHalfWaveIN)
  //     fHAIASetpoint[mode]=fPrevHAIASetpoint[mode] - fHAChargeAsym[mode]/fHAIASlopeA[0];//IHWP IN slope is stored at fHAIASlopeA[0]
  //   else
  //     fHAIASetpoint[mode]=fPrevHAIASetpoint[mode] - fHAChargeAsym[mode]/fHAIASlopeA[1];//IHWP OUT slope is stored at fHAIASlopeA[1]

  // } else
  //   fHAIASetpoint[mode]=fPrevHAIASetpoint[mode];
  
  // if ((fHAIASetpoint[mode]>fIASetpointup) || (fHAIASetpoint[mode]<fIASetpointlow)){//if correction is out-of bound no correction applied
  //   QwMessage<<"Hall A FeedIASetPoint out-of-bounds "<<fHAChargeAsym[mode]<<" +/- "<<fHAChargeAsymError[mode]<<" new set point[+]  "<<fHAIASetpoint[mode]<<" aborting correction for this time! "<<QwLog::endl;
  //   return;
  // }

  // QwMessage<<"FeedIASetPoint("<<mode<<") "<<fHAChargeAsym[mode]<<"+/-"<<fHAChargeAsymError[mode]<<" new set point  "<<fHAIASetpoint[mode]<<QwLog::endl;
  // //send the new IA setpoint 

  // for (Int_t i=0;i<4;i++)
  //   fEPICSCtrl.Set_HallAIA(i,fHAIASetpoint[mode]);//do the same correction to 4 DACs
   
  // // fScalerChargeRunningSum.ClearEventData();//reset the running sums
};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPITASetPoints(){


   QwMessage << "fRunningCharge.GetGoodEventCount()=="
   	    << fTargetCharge.GetGoodEventCount()
   	    << " at pattern number "
   	    << fCurrentPatternNumber
   	    << " before making the correction."<<QwLog::endl;

  //calculate the new setpoint
  if (fHalfWaveIN)
    fPITASlope=fPITASlopeIN;
  else{
    //    if (GetHalfWavePlate2State()==0)//IHWP2==OUT or No IHWP2 anymore
      fPITASlope=fPITASlopeOUT;
    //else
    //fPITASlope=fPITASlopeOUT_IN;//IHWP2==IN
  }

  

  // fEPICSCtrl.Get_Pockels_Cell_plus(fPrevPITASetpointPOS);
  //fEPICSCtrl.Get_Pockels_Cell_minus(fPrevPITASetpointNEG);

  //amali2019
  fEPICSCtrl.Get_RTP_PITA_1(fPrevPITASetpoint1);
  fEPICSCtrl.Get_RTP_PITA_2(fPrevPITASetpoint2);
  fEPICSCtrl.Get_RTP_PITA_3(fPrevPITASetpoint3);
  fEPICSCtrl.Get_RTP_PITA_4(fPrevPITASetpoint4);
  fEPICSCtrl.Get_RTP_PITA_5(fPrevPITASetpoint5);
  fEPICSCtrl.Get_RTP_PITA_6(fPrevPITASetpoint6);
  fEPICSCtrl.Get_RTP_PITA_7(fPrevPITASetpoint7);
  fEPICSCtrl.Get_RTP_PITA_8(fPrevPITASetpoint8);



  if (fPITASlope!=0) {
    Double_t correction = fChargeAsymmetry/fPITASlope;
QwMessage<<"................Correction............."<<fChargeAsymmetry/fPITASlope<<QwLog::endl;
QwMessage<<"................Aq............."<<fChargeAsymmetry<<QwLog::endl;
QwMessage<<"................slope............."<<fPITASlope<<QwLog::endl;
    //damp the correction when charge asymmetry is close to zero
    // if (fFeedbackDamping){
    //   if (TMath::Abs(fChargeAsymmetry)<0.01)//ppm
    // 	correction*=0.01;
    //   else if (TMath::Abs(fChargeAsymmetry)<0.1)//ppm
    // 	correction*=0.1;
    //   else if (TMath::Abs(fChargeAsymmetry)<1)//ppm
    // 	correction*=0.25;
    //   else if (TMath::Abs(fChargeAsymmetry)<2)//ppm
    // 	correction*=0.5; 
    //   else if (TMath::Abs(fChargeAsymmetry)<5)//ppm
    // 	correction*=0.75;
    // }

    //fPITASetpointPOS=fPrevPITASetpointPOS + correction;
    //fPITASetpointNEG=fPrevPITASetpointNEG - correction;
    //amali2019
    fPITASetpoint1=fPrevPITASetpoint1 - correction;
    fPITASetpoint2=fPrevPITASetpoint2 - correction;
    fPITASetpoint3=fPrevPITASetpoint3 - correction;
    fPITASetpoint4=fPrevPITASetpoint4 - correction;
    fPITASetpoint5=fPrevPITASetpoint5 + correction;
    fPITASetpoint6=fPrevPITASetpoint6 + correction;
    fPITASetpoint7=fPrevPITASetpoint7 + correction;
    fPITASetpoint8=fPrevPITASetpoint8 + correction;

  } else {
    //fPITASetpointPOS=fPrevPITASetpointPOS;
    //fPITASetpointNEG=fPrevPITASetpointNEG;
    fPITASetpoint1=fPrevPITASetpoint1; 
    fPITASetpoint2=fPrevPITASetpoint2;
    fPITASetpoint3=fPrevPITASetpoint3;
    fPITASetpoint4=fPrevPITASetpoint4;
    fPITASetpoint5=fPrevPITASetpoint5;
    fPITASetpoint6=fPrevPITASetpoint6;
    fPITASetpoint7=fPrevPITASetpoint7;
    fPITASetpoint8=fPrevPITASetpoint8;
  }




  // if ((fPITASetpointPOS>fPITASetpointup) || (fPITASetpointPOS<fPITASetpointlow) || (fPITASetpointNEG>fPITASetpointup) || (fPITASetpointNEG<fPITASetpointlow)){//if correction is out-of bound
  //   QwMessage<<"FeedPITASetPoint out-of-bounds "<<fChargeAsymmetry<<" +/- "<<fChargeAsymmetryError <<" new set point[+]  "<<fPITASetpointPOS<<" [-] "<<fPITASetpointNEG<<" aborting correction for this time! "<<"low"<<fPITASetpointlow<<QwLog::endl;

  //   return;//do nothing but can we reset to t_0 correction and start all over???????
  // }
  //Amali2019
  if ((fPITASetpoint1>fPITASetpointup) || (fPITASetpoint1<fPITASetpointlow) || (fPITASetpoint5>fPITASetpointup) || (fPITASetpoint5<fPITASetpointlow)){//if correction is out-of bound
     QwMessage<<"FeedPITASetPoint out-of-bounds "<<fChargeAsymmetry<<" +/- "<<fChargeAsymmetryError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fPITASetpoint1<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fPITASetpoint2<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 3  "<<fPITASetpoint3<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 4  "<<fPITASetpoint4<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 5  "<<fPITASetpoint5<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 6  "<<fPITASetpoint6<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 7  "<<fPITASetpoint7<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 8  "<<fPITASetpoint8<<" aborting correction for this time! "<<QwLog::endl;

     return;//do nothing but can we reset to t_0 correction and start all over???????
  }


  // QwMessage<<"FeedPITASetPoint "<<" "<<fChargeAsymmetry<<" +/- "<<fChargeAsymmetryError<<" new set point[+]  "<<fPITASetpointPOS<<" [-] "<<fPITASetpointNEG<<QwLog::endl;
  //Amali2019
  QwMessage<<"FeedPITASetPoint "<<" "<<fChargeAsymmetry<<" +/- "<<fChargeAsymmetryError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fPITASetpoint1<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fPITASetpoint2<<QwLog::endl;
   QwMessage<<" new set point 3  "<<fPITASetpoint3<<QwLog::endl;
   QwMessage<<" new set point 4  "<<fPITASetpoint4<<QwLog::endl;
   QwMessage<<" new set point 5  "<<fPITASetpoint5<<QwLog::endl;
   QwMessage<<" new set point 6  "<<fPITASetpoint6<<QwLog::endl;
   QwMessage<<" new set point 7  "<<fPITASetpoint7<<QwLog::endl;
   QwMessage<<" new set point 8  "<<fPITASetpoint8<<QwLog::endl;

  
  //send the new PITA setpoint
   // fEPICSCtrl.Set_Pockels_Cell_plus(fPITASetpointPOS);
   //fEPICSCtrl.Set_Pockels_Cell_minus(fPITASetpointNEG);

  //Amali2019
  fEPICSCtrl.Set_RTP_PITA_1(fPITASetpoint1);
  fEPICSCtrl.Set_RTP_PITA_2(fPITASetpoint2);
  fEPICSCtrl.Set_RTP_PITA_3(fPITASetpoint3);
  fEPICSCtrl.Set_RTP_PITA_4(fPITASetpoint4);
  fEPICSCtrl.Set_RTP_PITA_5(fPITASetpoint5);
  fEPICSCtrl.Set_RTP_PITA_6(fPITASetpoint6);
  fEPICSCtrl.Set_RTP_PITA_7(fPITASetpoint7);
  fEPICSCtrl.Set_RTP_PITA_8(fPITASetpoint8);






  /*
  if (fFeedbackStatus){
      fFeedbackStatus=kFALSE;
      fEPICSCtrl.Set_FeedbackStatus(2.0);//Setting to 2 seems to be not updating. Check back later
  }
  */

  //Greenmonster stuffs are commented out - rakithab
  //fScanCtrl.SCNSetValue(1,0);
  //fScanCtrl.SCNSetValue(2,0);
  //fScanCtrl.CheckScan();
  //fScanCtrl.PrintScanInfo();
  //fScanCtrl.Close();  
  ClearRunningSum();//reset the running sum only if PITA correction applied, the object fRunningAsymmetry is solely dedicated to PITA feedback


   QwMessage << "fTargetCharge.GetGoodEventCount()=="
   	    << fTargetCharge.GetGoodEventCount()
   	    << " at pattern number "
   	    << fCurrentPatternNumber
   	    << " after making the correction."<<QwLog::endl;

};

/**************************************************************************/




void QwHelicityCorrelatedFeedback::FeedPITAPOSUSetPoints(){

 QwMessage << "fTargetXDiffRunningSum.GetGoodEventCount()=="
	    << fTargetXDiffRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " before making the correction."<<QwLog::endl;

  //calculate the new setpoint
  if (fHalfWaveIN)
    fPITAPOSUSlope=fPITAPOSUSlopeIN;
  else{
      fPITAPOSUSlope=fPITAPOSUSlopeOUT;
    
  }

  

  //amali2019
  fEPICSCtrl.Get_RTP_PITA_1(fPrevPITAPOSUSetpoint1);
  fEPICSCtrl.Get_RTP_PITA_2(fPrevPITAPOSUSetpoint2);
  fEPICSCtrl.Get_RTP_PITA_5(fPrevPITAPOSUSetpoint5);
  fEPICSCtrl.Get_RTP_PITA_6(fPrevPITAPOSUSetpoint6);

 
  


  if (fPITAPOSUSlope!=0) {
    Double_t correction = fTargetXDiff/fPITAPOSUSlope;
QwMessage<<"................Correction............."<<fTargetXDiff/fPITAPOSUSlope<<QwLog::endl;
QwMessage<<"................DiffX............."<<fTargetXDiff<<QwLog::endl;
QwMessage<<"................slope............."<<fPITAPOSUSlope<<QwLog::endl;
    
   
    //amali2019
    fPITAPOSUSetpoint1=fPrevPITAPOSUSetpoint1 - correction;
    fPITAPOSUSetpoint2=fPrevPITAPOSUSetpoint2 + correction;
    fPITAPOSUSetpoint5=fPrevPITAPOSUSetpoint5 - correction;
    fPITAPOSUSetpoint6=fPrevPITAPOSUSetpoint6 + correction;
  

  } else {
  
    fPITAPOSUSetpoint1=fPrevPITAPOSUSetpoint1; 
    fPITAPOSUSetpoint2=fPrevPITAPOSUSetpoint2;
    fPITAPOSUSetpoint5=fPrevPITAPOSUSetpoint5;
    fPITAPOSUSetpoint6=fPrevPITAPOSUSetpoint6;
   
  }


  //Amali2019
  if ((fPITAPOSUSetpoint1>fPITASetpointup) || (fPITAPOSUSetpoint1<fPITASetpointlow) || (fPITAPOSUSetpoint5>fPITASetpointup) || (fPITAPOSUSetpoint5<fPITASetpointlow)){//if correction is out-of bound
     QwMessage<<"FeedPITAPOSUSetPoint out-of-bounds "<<fTargetXDiff<<" +/- "<<fTargetXDiffError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fPITAPOSUSetpoint1<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fPITAPOSUSetpoint2<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 5  "<<fPITAPOSUSetpoint5<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 6  "<<fPITAPOSUSetpoint6<<" aborting correction for this time! "<<QwLog::endl;
   

     return;//do nothing but can we reset to t_0 correction and start all over???????
  }


  //Amali2019
  QwMessage<<"FeedPITAPOSUSetPoint "<<" "<<fTargetXDiff<<" +/- "<<fTargetXDiffError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fPITAPOSUSetpoint1<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fPITAPOSUSetpoint2<<QwLog::endl;
   QwMessage<<" new set point 5  "<<fPITAPOSUSetpoint5<<QwLog::endl;
   QwMessage<<" new set point 6  "<<fPITAPOSUSetpoint6<<QwLog::endl;
 


  //Amali2019
  fEPICSCtrl.Set_RTP_PITAPOSU_1(fPITAPOSUSetpoint1);
  fEPICSCtrl.Set_RTP_PITAPOSU_2(fPITAPOSUSetpoint2);
  fEPICSCtrl.Set_RTP_PITAPOSU_5(fPITAPOSUSetpoint5);
  fEPICSCtrl.Set_RTP_PITAPOSU_6(fPITAPOSUSetpoint6);


  fTargetXDiffRunningSum.ClearEventData();

  QwMessage << "fTargetXDiffRunningSum.GetGoodEventCount()=="
	    << fTargetXDiffRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " after making the correction."<<QwLog::endl;
 
  // ClearRunningSum();//reset the running sum only if PITA correction applied, the object fRunningAsymmetry is solely dedicated to PITA feedback
};

/**************************************************************************/





void QwHelicityCorrelatedFeedback::FeedPITAPOSVSetPoints(){


  QwMessage << "fTargetYDiffRunningSum.GetGoodEventCount()=="
	    << fTargetYDiffRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " before making the correction."<<QwLog::endl;
 
  //calculate the new setpoint
  if (fHalfWaveIN)
    fPITAPOSVSlope=fPITAPOSVSlopeIN;
  else{
     fPITAPOSVSlope=fPITAPOSVSlopeOUT;
    
  }

  

  //amali2019
  fEPICSCtrl.Get_RTP_PITA_3(fPrevPITAPOSVSetpoint3);
  fEPICSCtrl.Get_RTP_PITA_4(fPrevPITAPOSVSetpoint4);
  fEPICSCtrl.Get_RTP_PITA_7(fPrevPITAPOSVSetpoint7);
  fEPICSCtrl.Get_RTP_PITA_8(fPrevPITAPOSVSetpoint8);
 


  if (fPITAPOSVSlope!=0) {
    Double_t correction = fTargetYDiff/fPITAPOSVSlope;
QwMessage<<"................Correction............."<<fTargetYDiff/fPITAPOSVSlope<<QwLog::endl;
QwMessage<<"................DiffY............."<<fTargetYDiff<<QwLog::endl;
QwMessage<<"................slope............."<<fPITAPOSVSlope<<QwLog::endl;
    
   
    //amali2019
    fPITAPOSVSetpoint3=fPrevPITAPOSVSetpoint3 - correction;
    fPITAPOSVSetpoint4=fPrevPITAPOSVSetpoint4 + correction;
    fPITAPOSVSetpoint7=fPrevPITAPOSVSetpoint7 - correction;
    fPITAPOSVSetpoint8=fPrevPITAPOSVSetpoint8 + correction;
  

  } else {
  
    fPITAPOSVSetpoint3=fPrevPITAPOSVSetpoint3; 
    fPITAPOSVSetpoint4=fPrevPITAPOSVSetpoint4;
    fPITAPOSVSetpoint7=fPrevPITAPOSVSetpoint7;
    fPITAPOSVSetpoint8=fPrevPITAPOSVSetpoint8;
   
  }


  //Amali2019
  if ((fPITAPOSVSetpoint3>fPITASetpointup) || (fPITAPOSVSetpoint3<fPITASetpointlow) || (fPITAPOSVSetpoint7>fPITASetpointup) || (fPITAPOSVSetpoint7<fPITASetpointlow)){//if correction is out-of bound
     QwMessage<<"FeedPITAPOSVSetPoint out-of-bounds "<<fTargetYDiff<<" +/- "<<fTargetYDiffError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fPITAPOSVSetpoint3<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fPITAPOSVSetpoint4<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 5  "<<fPITAPOSVSetpoint7<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new set point 6  "<<fPITAPOSVSetpoint8<<" aborting correction for this time! "<<QwLog::endl;
   

     return;//do nothing but can we reset to t_0 correction and start all over???????
  }


  //Amali2019
  QwMessage<<"FeedPITAPOSVSetPoint "<<" "<<fTargetYDiff<<" +/- "<<fTargetYDiffError<<QwLog::endl;
   QwMessage<<" new set point 3  "<<fPITAPOSVSetpoint3<<QwLog::endl;
   QwMessage<<" new set point 4  "<<fPITAPOSVSetpoint4<<QwLog::endl;
   QwMessage<<" new set point 7  "<<fPITAPOSVSetpoint7<<QwLog::endl;
   QwMessage<<" new set point 8  "<<fPITAPOSVSetpoint8<<QwLog::endl;
 


  //Amali2019
  fEPICSCtrl.Set_RTP_PITAPOSV_3(fPITAPOSVSetpoint3);
  fEPICSCtrl.Set_RTP_PITAPOSV_4(fPITAPOSVSetpoint4);
  fEPICSCtrl.Set_RTP_PITAPOSV_7(fPITAPOSVSetpoint7);
  fEPICSCtrl.Set_RTP_PITAPOSV_8(fPITAPOSVSetpoint8);



  fTargetYDiffRunningSum.ClearEventData();

  QwMessage << "fTargetYDiffRunningSum.GetGoodEventCount()=="
	    << fTargetYDiffRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " after making the correction."<<QwLog::endl;
 
 
  //ClearRunningSum();//reset the running sum only if PITA correction applied, the object fRunningAsymmetry is solely dedicated to PITA feedback
};

/***************************************************************/


void QwHelicityCorrelatedFeedback::FeedPOSXYSetPoints(){


  QwMessage << "fXYPosXDiffRunningSum.GetGoodEventCount()=="
	    << fXYPosXDiffRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " before making the correction."<<QwLog::endl;

  QwMessage << "fXYPosYDiffRunningSum.GetGoodEventCount()=="
	    << fXYPosYDiffRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " before making the correction."<<QwLog::endl;
 
  //calculate the new setpoint
  // if (fHalfWaveIN)
  //fPITAPOSVSlope=fPITAPOSVSlopeIN;
  //else{
  // fPITAPOSVSlope=fPITAPOSVSlopeOUT;
    
  //}

  

  fEPICSCtrl.Get_RTP_PITA_1(fPrevPOSXYSetpoint1);
  fEPICSCtrl.Get_RTP_PITA_2(fPrevPOSXYSetpoint2);
  fEPICSCtrl.Get_RTP_PITA_3(fPrevPOSXYSetpoint3);
  fEPICSCtrl.Get_RTP_PITA_4(fPrevPOSXYSetpoint4);
  fEPICSCtrl.Get_RTP_PITA_5(fPrevPOSXYSetpoint5);
  fEPICSCtrl.Get_RTP_PITA_6(fPrevPOSXYSetpoint6);
  fEPICSCtrl.Get_RTP_PITA_7(fPrevPOSXYSetpoint7);
  fEPICSCtrl.Get_RTP_PITA_8(fPrevPOSXYSetpoint8);
 


  if (fxU!=0 && fxV!=0 && fyU!=0 && fyV!=0) {
    Double_t a = fyV/((fxU * fyV)-(fyU * fxV)); 
    Double_t b = -fxV/((fxU * fyV)-(fyU * fxV));
    Double_t c = -fyU/((fxU * fyV)-(fyU * fxV));
    Double_t d = fxU/((fxU * fyV)-(fyU * fxV));
    Double_t correction1 = a*fXDiff;
    Double_t correction2 = b*fYDiff;
    Double_t correction3 = c*fXDiff;
    Double_t correction4 = d*fYDiff;
QwMessage<<"................Correction1............."<<a*fXDiff<<QwLog::endl;
QwMessage<<"................Correction2............."<<b*fYDiff<<QwLog::endl;
QwMessage<<"................Correction3............."<<c*fXDiff<<QwLog::endl;
QwMessage<<"................Correction4............."<<d*fYDiff<<QwLog::endl;
QwMessage<<"................DiffX............."<<fXDiff<<QwLog::endl;
QwMessage<<"................DiffY............."<<fYDiff<<QwLog::endl;
QwMessage<<"................a............."<<fyV/((fxU * fyV)-(fyU * fxV))<<QwLog::endl;
QwMessage<<"................b............."<<-fxV/((fxU * fyV)-(fyU * fxV))<<QwLog::endl;
QwMessage<<"................c............."<<-fyU/((fxU * fyV)-(fyU * fxV))<<QwLog::endl;
QwMessage<<"................d............."<<fxU/((fxU * fyV)-(fyU * fxV))<<QwLog::endl;
    
   
    //amali2019
    fPOSXYSetpoint1=fPrevPOSXYSetpoint1 - correction1 - correction2;
    fPOSXYSetpoint2=fPrevPOSXYSetpoint2 + correction1 + correction2;
    fPOSXYSetpoint3=fPrevPOSXYSetpoint3 - correction3 - correction4;
    fPOSXYSetpoint4=fPrevPOSXYSetpoint4 + correction3 + correction4;
    fPOSXYSetpoint5=fPrevPOSXYSetpoint5 - correction1 - correction2;
    fPOSXYSetpoint6=fPrevPOSXYSetpoint6 + correction1 + correction2;
    fPOSXYSetpoint7=fPrevPOSXYSetpoint7 - correction3 - correction4;
    fPOSXYSetpoint8=fPrevPOSXYSetpoint8 + correction3 + correction4;
   
  

  } else {
  
    fPOSXYSetpoint1=fPrevPOSXYSetpoint1;
    fPOSXYSetpoint2=fPrevPOSXYSetpoint2;
    fPOSXYSetpoint3=fPrevPOSXYSetpoint3;
    fPOSXYSetpoint4=fPrevPOSXYSetpoint4;
    fPOSXYSetpoint5=fPrevPOSXYSetpoint5;
    fPOSXYSetpoint6=fPrevPOSXYSetpoint6;
    fPOSXYSetpoint7=fPrevPOSXYSetpoint7;
    fPOSXYSetpoint8=fPrevPOSXYSetpoint8;
   
  }


  //Amali2019
  if ((fPOSXYSetpoint1>fPITASetpointup) || (fPOSXYSetpoint1<fPITASetpointlow)){//if correction is out-of bound
     QwMessage<<"FeedPOSXYSetPoint out-of-bounds "<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fPOSXYSetpoint1<<" aborting correction for this time! "<<QwLog::endl;
QwMessage<<" new set point 2  "<<fPOSXYSetpoint2<<" aborting correction for this time! "<<QwLog::endl;
QwMessage<<" new set point 3  "<<fPOSXYSetpoint3<<" aborting correction for this time! "<<QwLog::endl;
QwMessage<<" new set point 4  "<<fPOSXYSetpoint4<<" aborting correction for this time! "<<QwLog::endl;
QwMessage<<" new set point 5  "<<fPOSXYSetpoint5<<" aborting correction for this time! "<<QwLog::endl;
QwMessage<<" new set point 6  "<<fPOSXYSetpoint6<<" aborting correction for this time! "<<QwLog::endl;
QwMessage<<" new set point 7  "<<fPOSXYSetpoint7<<" aborting correction for this time! "<<QwLog::endl;
QwMessage<<" new set point 8  "<<fPOSXYSetpoint8<<" aborting correction for this time! "<<QwLog::endl;
   

     return;//do nothing but can we reset to t_0 correction and start all over???????
  }


  //Amali2019
  // QwMessage<<"FeedPITAPOSVSetPoint "<<" "<<fTargetYDiff<<" +/- "<<fTargetYDiffError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fPOSXYSetpoint1<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fPOSXYSetpoint2<<QwLog::endl;
   QwMessage<<" new set point 3  "<<fPOSXYSetpoint3<<QwLog::endl;
   QwMessage<<" new set point 4  "<<fPOSXYSetpoint4<<QwLog::endl;
   QwMessage<<" new set point 5  "<<fPOSXYSetpoint5<<QwLog::endl;
   QwMessage<<" new set point 6  "<<fPOSXYSetpoint6<<QwLog::endl;
   QwMessage<<" new set point 7  "<<fPOSXYSetpoint7<<QwLog::endl;
   QwMessage<<" new set point 8  "<<fPOSXYSetpoint8<<QwLog::endl;
 


  //Amali2019
  fEPICSCtrl.Set_RTP_POSXY_1(fPOSXYSetpoint1);
  fEPICSCtrl.Set_RTP_POSXY_2(fPOSXYSetpoint2);
  fEPICSCtrl.Set_RTP_POSXY_3(fPOSXYSetpoint3);
  fEPICSCtrl.Set_RTP_POSXY_4(fPOSXYSetpoint4);
  fEPICSCtrl.Set_RTP_POSXY_5(fPOSXYSetpoint5);
  fEPICSCtrl.Set_RTP_POSXY_6(fPOSXYSetpoint6);
  fEPICSCtrl.Set_RTP_POSXY_7(fPOSXYSetpoint7);
  fEPICSCtrl.Set_RTP_POSXY_8(fPOSXYSetpoint8);
  


  fXYPosXDiffRunningSum.ClearEventData();
  fXYPosYDiffRunningSum.ClearEventData();

  QwMessage << "fXYPosXDiffRunningSum.GetGoodEventCount()=="
	    << fXYPosXDiffRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " after making the correction."<<QwLog::endl;

 QwMessage << "fXYPosYDiffRunningSum.GetGoodEventCount()=="
	    << fXYPosYDiffRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " after making the correction."<<QwLog::endl;
 
 
  //ClearRunningSum();//reset the running sum only if PITA correction applied, the object fRunningAsymmetry is solely dedicated to PITA feedback
};


void QwHelicityCorrelatedFeedback::FeedHCIASetPoints(){

  QwMessage << "fTargetHCChargeRunningSum.GetGoodEventCount()=="
	    << fTargetHCChargeRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " before making the correction."<<QwLog::endl;

  //calculate the new setpoint
  if (fHalfWaveIN)
    fHCIASlope=fHCIASlopeIN;
  else{
      fHCIASlope=fHCIASlopeOUT;
  }

 
  fEPICSCtrl.Get_IA_HC_1(fPrevHCIASetpoint1);
  fEPICSCtrl.Get_IA_HC_2(fPrevHCIASetpoint2);
  fEPICSCtrl.Get_IA_HC_3(fPrevHCIASetpoint3);
  fEPICSCtrl.Get_IA_HC_4(fPrevHCIASetpoint4);
  


  if (fHCIASlope!=0) {
    Double_t correction = fTargetHCCharge/fHCIASlope;
QwMessage<<"................Correction............."<<fTargetHCCharge/fHCIASlope<<QwLog::endl;
QwMessage<<"................Aq............."<<fTargetHCCharge<<QwLog::endl;
QwMessage<<"................slope............."<<fHCIASlope<<QwLog::endl;
   


    fHCIASetpoint1=fPrevHCIASetpoint1 - correction;
    fHCIASetpoint2=fPrevHCIASetpoint2 - correction;
    fHCIASetpoint3=fPrevHCIASetpoint3 - correction;
    fHCIASetpoint4=fPrevHCIASetpoint4 - correction;


  } else {
  
 fHCIASetpoint1=fPrevHCIASetpoint1; 
 fHCIASetpoint2=fPrevHCIASetpoint2; 
 fHCIASetpoint3=fPrevHCIASetpoint3; 
 fHCIASetpoint4=fPrevHCIASetpoint4; 
   
  }


  if ((fHCIASetpoint1>fHCIASetpointup) || (fHCIASetpoint1<fHCIASetpointlow)) {//if correction is out-of bound
     QwMessage<<"FeedHCIASetPoint out-of-bounds "<<fTargetHCCharge<<" +/- "<<fTargetHCChargeError<<QwLog::endl;
   QwMessage<<" new HC IA set point 1  "<<fHCIASetpoint1<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HC IA set point 2  "<<fHCIASetpoint2<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HC IA set point 3  "<<fHCIASetpoint3<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HC IA set point 4  "<<fHCIASetpoint4<<" aborting correction for this time! "<<QwLog::endl;
  
     return;//do nothing but can we reset to t_0 correction and start all over???????
  }


 
  QwMessage<<"FeedHCIASetPoint "<<" "<<fTargetHCCharge<<" +/- "<<fTargetHCChargeError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fHCIASetpoint1<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fHCIASetpoint2<<QwLog::endl;
   QwMessage<<" new set point 3  "<<fHCIASetpoint3<<QwLog::endl;
   QwMessage<<" new set point 4  "<<fHCIASetpoint4<<QwLog::endl;

  
  //send the new PITA setpoint
  fEPICSCtrl.Set_IA_HC_1(fHCIASetpoint1);
 fEPICSCtrl.Set_IA_HC_2(fHCIASetpoint2);
 fEPICSCtrl.Set_IA_HC_3(fHCIASetpoint3);
 fEPICSCtrl.Set_IA_HC_4(fHCIASetpoint4);

fTargetHCChargeRunningSum.ClearEventData();

  QwMessage << "fTargetHCChargeRunningSum.GetGoodEventCount()=="
	    << fTargetHCChargeRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " after making the correction."<<QwLog::endl;
  
//  //ClearHCRunningSum();//reset the running sum only if PITA correction applied, the object fRunningAsymmetry is solely dedicated to PITA feedback
};

/********************************************************************************************************************/

void QwHelicityCorrelatedFeedback::FeedHAIASetPoints(){

  QwMessage << "fTargetHAChargeRunningSum.GetGoodEventCount()=="
	    << fTargetHAChargeRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " before making the correction."<<QwLog::endl;

  //calculate the new setpoint
  if (fHalfWaveIN)
    fHAIASlope=fHAIASlopeIN;
  else{
      fHAIASlope=fHAIASlopeOUT;
  }

 
  fEPICSCtrl.Get_IA_HA_1(fPrevHAIASetpoint1);
  fEPICSCtrl.Get_IA_HA_2(fPrevHAIASetpoint2);
  fEPICSCtrl.Get_IA_HA_3(fPrevHAIASetpoint3);
  fEPICSCtrl.Get_IA_HA_4(fPrevHAIASetpoint4);
  


  if (fHAIASlope!=0) {
    Double_t correction2 = fTargetHACharge/fHAIASlope;
QwMessage<<"................Correction2............."<<fTargetHACharge/fHAIASlope<<QwLog::endl;
QwMessage<<"................Aq............."<<fTargetHACharge<<QwLog::endl;
QwMessage<<"................slope............."<<fHAIASlope<<QwLog::endl;
   


    fHAIASetpoint1=fPrevHAIASetpoint1 - correction2;
    fHAIASetpoint2=fPrevHAIASetpoint2 - correction2;
    fHAIASetpoint3=fPrevHAIASetpoint3 - correction2;
    fHAIASetpoint4=fPrevHAIASetpoint4 - correction2;


  } else {
  
 fHAIASetpoint1=fPrevHAIASetpoint1; 
 fHAIASetpoint2=fPrevHAIASetpoint2; 
 fHAIASetpoint3=fPrevHAIASetpoint3; 
 fHAIASetpoint4=fPrevHAIASetpoint4; 
   
  }


  if ((fHAIASetpoint1>fHAIASetpointup) || (fHAIASetpoint1<fHAIASetpointlow)) {//if correction is out-of bound
     QwMessage<<"FeedHCIASetPoint out-of-bounds "<<fTargetHACharge<<" +/- "<<fTargetHAChargeError<<QwLog::endl;
   QwMessage<<" new HA IA set point 1  "<<fHAIASetpoint1<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HA IA set point 2  "<<fHAIASetpoint2<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HA IA set point 3  "<<fHAIASetpoint3<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HA IA set point 4  "<<fHAIASetpoint4<<" aborting correction for this time! "<<QwLog::endl;
  
     return;//do nothing but can we reset to t_0 correction and start all over???????
  }


 
  QwMessage<<"FeedHAIASetPoint "<<" "<<fTargetHACharge<<" +/- "<<fTargetHAChargeError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fHAIASetpoint1<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fHAIASetpoint2<<QwLog::endl;
   QwMessage<<" new set point 3  "<<fHAIASetpoint3<<QwLog::endl;
   QwMessage<<" new set point 4  "<<fHAIASetpoint4<<QwLog::endl;

  
  //send the new PITA setpoint
 fEPICSCtrl.Set_IA_HA_1(fHAIASetpoint1);
 fEPICSCtrl.Set_IA_HA_2(fHAIASetpoint2);
 fEPICSCtrl.Set_IA_HA_3(fHAIASetpoint3);
 fEPICSCtrl.Set_IA_HA_4(fHAIASetpoint4);

fTargetHAChargeRunningSum.ClearEventData();

  QwMessage << "fTargetHAChargeRunningSum.GetGoodEventCount()=="
	    << fTargetHAChargeRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " after making the correction."<<QwLog::endl;
  
//  //ClearHCRunningSum();//reset the running sum only if PITA correction applied, the object fRunningAsymmetry is solely dedicated to PITA feedback
};


/********************************************************************************************************/

void QwHelicityCorrelatedFeedback::FeedHBIASetPoints(){

  QwMessage << "fTargetHBChargeRunningSum.GetGoodEventCount()=="
	    << fTargetHBChargeRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " after making the correction."<<QwLog::endl;

  //calculate the new setpoint
  if (fHalfWaveIN)
    fHBIASlope=fHBIASlopeIN;
  else{
      fHBIASlope=fHBIASlopeOUT;
  }

 
  fEPICSCtrl.Get_IA_HB_1(fPrevHBIASetpoint1);
  fEPICSCtrl.Get_IA_HB_2(fPrevHBIASetpoint2);
  fEPICSCtrl.Get_IA_HB_3(fPrevHBIASetpoint3);
  fEPICSCtrl.Get_IA_HB_4(fPrevHBIASetpoint4);
  


  if (fHBIASlope!=0) {
    Double_t correction3 = fTargetHBCharge/fHBIASlope;
QwMessage<<"................Correction3............."<<fTargetHBCharge/fHBIASlope<<QwLog::endl;
QwMessage<<"................Aq............."<<fTargetHBCharge<<QwLog::endl;
QwMessage<<"................slope............."<<fHBIASlope<<QwLog::endl;
   


    fHBIASetpoint1=fPrevHBIASetpoint1 - correction3;
    fHBIASetpoint2=fPrevHBIASetpoint2 - correction3;
    fHBIASetpoint3=fPrevHBIASetpoint3 - correction3;
    fHBIASetpoint4=fPrevHBIASetpoint4 - correction3;


  } else {
  
 fHBIASetpoint1=fPrevHBIASetpoint1; 
 fHBIASetpoint2=fPrevHBIASetpoint2; 
 fHBIASetpoint3=fPrevHBIASetpoint3; 
 fHBIASetpoint4=fPrevHBIASetpoint4; 
   
  }


  if ((fHBIASetpoint1>fHBIASetpointup) || (fHBIASetpoint1<fHBIASetpointlow)) {//if correction is out-of bound
     QwMessage<<"FeedHBIASetPoint out-of-bounds "<<fTargetHBCharge<<" +/- "<<fTargetHBChargeError<<QwLog::endl;
   QwMessage<<" new HB IA set point 1  "<<fHBIASetpoint1<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HB IA set point 2  "<<fHBIASetpoint2<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HB IA set point 3  "<<fHBIASetpoint3<<" aborting correction for this time! "<<QwLog::endl;
   QwMessage<<" new HB IA set point 4  "<<fHBIASetpoint4<<" aborting correction for this time! "<<QwLog::endl;
  
     return;//do nothing but can we reset to t_0 correction and start all over???????
  }


 
  QwMessage<<"FeedHBIASetPoint "<<" "<<fTargetHBCharge<<" +/- "<<fTargetHBChargeError<<QwLog::endl;
   QwMessage<<" new set point 1  "<<fHBIASetpoint1<<QwLog::endl;
   QwMessage<<" new set point 2  "<<fHBIASetpoint2<<QwLog::endl;
   QwMessage<<" new set point 3  "<<fHBIASetpoint3<<QwLog::endl;
   QwMessage<<" new set point 4  "<<fHBIASetpoint4<<QwLog::endl;

  
  //send the new PITA setpoint
  fEPICSCtrl.Set_IA_HB_1(fHBIASetpoint1);
 fEPICSCtrl.Set_IA_HB_2(fHBIASetpoint2);
 fEPICSCtrl.Set_IA_HB_3(fHBIASetpoint3);
 fEPICSCtrl.Set_IA_HB_4(fHBIASetpoint4);

fTargetHBChargeRunningSum.ClearEventData();

  QwMessage << "fTargetHBChargeRunningSum.GetGoodEventCount()=="
	    << fTargetHBChargeRunningSum.GetGoodEventCount()
	    << " at pattern number "
	    << fCurrentPatternNumber
	    << " after making the correction."<<QwLog::endl;
  
//  //ClearHCRunningSum();//reset the running sum only if PITA correction applied, the object fRunningAsymmetry is solely dedicated to PITA feedback
};

/********************************************************************************************************************/


/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPCPos(){
}; 
/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPCNeg(){
}; 

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogParameters(Int_t mode){
  // out_file_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_IA_log.txt", "a");
  // //  fprintf(out_file," Feedback at %d current A_q[%d]:%5.8f+/-%5.8f IA Setpoint:%5.3f  IA Previous Setpoint:%5.3f\n",fQuartetNumber,mode,fChargeAsym[mode],fChargeAsymError[mode],fIASetpoint[mode],fPrevIASetpoint[mode]);
  // fprintf(out_file_IA," %10.0d A_q[%1.0d] %20.4f +/-  %20.4f  %20.2f  %20.2f\n",fQuartetNumber,mode,fChargeAsym[mode],fChargeAsymError[mode],fIASetpoint[mode],fPrevIASetpoint[mode]);
  // fclose(out_file_IA);
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogParameters(){
  // fEPICSCtrl.Set_ChargeAsymmetry(fChargeAsymmetry,fChargeAsymmetryError,fChargeAsymmetryWidth);//updates the epics values

  out_file_PITA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITA_log.txt", "a");
  

  fprintf(out_file_PITA,"%9.0d %+15.2f %15.2f %15.2f %25.2f %25.2f %25.2f %25.2f \n",fQuartetNumber,fChargeAsymmetry,fChargeAsymmetryError,TMath::Abs(fPITASetpoint1-fPrevPITASetpoint1),fPITASetpoint1,fPrevPITASetpoint1,fPITASetpoint5,fPrevPITASetpoint5);
  fclose(out_file_PITA);


};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogPFUParameters(){
  
 out_file_PITAPOSU = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITAPOSU_log.txt", "a");
  fprintf(out_file_PITAPOSU,"%9.0d %+15.2f %15.2f %15.2f %15.2f %15.2f \n",fQuartetNumber,fTargetXDiff,fTargetXDiffError,TMath::Abs(fPITAPOSUSetpoint1-fPrevPITAPOSUSetpoint1),fPITAPOSUSetpoint1,fPrevPITAPOSUSetpoint1);
  fclose(out_file_PITAPOSU); 
};



void QwHelicityCorrelatedFeedback::LogPFVParameters(){
 

  out_file_PITAPOSV = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITAPOSV_log.txt", "a");
  fprintf(out_file_PITAPOSV,"%9.0d %+15.2f %15.2f %15.2f %15.2f %15.2f \n",fQuartetNumber,fTargetYDiff,fTargetYDiffError,TMath::Abs(fPITAPOSVSetpoint3-fPrevPITAPOSVSetpoint3),fPITAPOSVSetpoint3,fPrevPITAPOSVSetpoint3);
  fclose(out_file_PITAPOSV); 
};


void QwHelicityCorrelatedFeedback::LogXYParameters(){
 

  out_file_PITAPOSXY = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_PITAPOSXY_log.txt", "a");
  fprintf(out_file_PITAPOSXY,"%9.0d %+15.2f %15.2f %15.2f %15.2f %15.2f \n",fQuartetNumber,fXDiff,fYDiff,TMath::Abs(fPOSXYSetpoint1-fPrevPOSXYSetpoint1),fPOSXYSetpoint1,fPrevPOSXYSetpoint1);
  fclose(out_file_PITAPOSXY); 
};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogHCParameters(){
 
   out_file_HC_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_HC_IA_log.txt", "a");
  //fQuartetNumber only available when we have good stable Hall C beam
     fprintf(out_file_HC_IA," %9.0d  %15.2f  %15.2f %15.0f %25.0f  %25.0f \n",fQuartetNumber,fTargetHCCharge,fTargetHCChargeError,TMath::Abs(fHCIASetpoint1-fPrevHCIASetpoint1),fHCIASetpoint1,fPrevHCIASetpoint1);
   fclose(out_file_HC_IA);
  
};


void QwHelicityCorrelatedFeedback::LogHAParameters(){
 
   out_file_HA_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_HA_IA_log.txt", "a");
  //fQuartetNumber only available when we have good stable Hall C beam
     fprintf(out_file_HA_IA," %9.0d  %15.2f  %15.2f %15.0f %25.0f  %25.0f \n",fQuartetNumber,fTargetHACharge,fTargetHAChargeError,TMath::Abs(fHAIASetpoint1-fPrevHAIASetpoint1),fHAIASetpoint1,fPrevHAIASetpoint1);
   fclose(out_file_HA_IA);
  
};

void QwHelicityCorrelatedFeedback::LogHBParameters(){
 
   out_file_HB_IA = fopen("/adaqfs/halla/apar/PREX/japan_feedback/LogFiles/Feedback_HB_IA_log.txt", "a");
  //fQuartetNumber only available when we have good stable Hall C beam
     fprintf(out_file_HB_IA," %9.0d  %15.2f  %15.2f %15.0f %25.0f  %25.0f \n",fQuartetNumber,fTargetHBCharge,fTargetHBChargeError,TMath::Abs(fHBIASetpoint1-fPrevHBIASetpoint1),fHBIASetpoint1,fPrevHBIASetpoint1);
   fclose(out_file_HB_IA);
  
};

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogHAParameters(Int_t mode){
  // fEPICSCtrl.Set_HAChargeAsymmetry(fHAChargeAsym[mode],fHAChargeAsymError[mode],fHAChargeAsymWidth[mode]);//updates the epics values
  // out_file_HA_IA = fopen("/local/scratch/qweak/Feedback_HA_IA_log.txt", "a");
  // //fQuartetNumber only available when we have good stable Hall C beam
  // fprintf(out_file_HA_IA," %10.0d  %20.2f  %15.2f %15.0f %20.0f  %20.0f \n",fQuartetNumber,fHAChargeAsym[mode],fHAChargeAsymError[mode],TMath::Abs(fHAIASetpoint[mode]-fPrevHAIASetpoint[mode]),fHAIASetpoint[mode],fPrevHAIASetpoint[mode]);
  // fclose(out_file_HA_IA);
  
};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::UpdateGMClean(Int_t state){
  //fScanCtrl.Open();
  
  if (state==0)
    fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  if (state)
    fScanCtrl.SCNSetStatus(SCN_INT_CLN);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();
  fScanCtrl.Close();  
  
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::UpdateGMScanParameters(){
};

/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::ApplyPITAFeedback(){
  Bool_t status=kFALSE;
  GetTargetChargeStat();//call the calculate running sum routine and access the q_targ published value
  if (fChargeAsymmetry==-1 && fChargeAsymmetryError == -1 && fChargeAsymmetryWidth==-1){//target asymmetry not published or accesible
    QwError<<"target asymmetry not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fChargeAsymmetryError=fChargeAsymmetryError*1e+6;//converts to ppm
  fChargeAsymmetry=fChargeAsymmetry*1e+6;//converts to ppm
  fChargeAsymmetryWidth=fChargeAsymmetryWidth*1e+6;//converts to ppm
  //  if (fChargeAsymmetryError>fChargeAsymPrecision){
  // QwError<<"Charge Asymmetry precision has not reached. Current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  // status=kFALSE;
  // fGoodPatternCounter=0;
  //}
  //else{
  //QwError<<"Charge Asymmetry precision, Current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    //UpdateGMClean(0);//set to not clean - Diabled the cfsocket comm-rakithab 11-27-2011
    FeedPITASetPoints();//set the new PITA values
    LogParameters();//Log PITA setting after feedback
    //UpdateGMClean(1);//set back to clean
    status=kTRUE;
    fGoodPatternCounter=0;//Reset after each feedback operation
    //  }


  return status;
};

/**********************************************************************/


Bool_t QwHelicityCorrelatedFeedback::ApplyPITAPOSUFeedback(){
  Bool_t status=kFALSE;
  GetTargetPositionStatU();//call the calculate running sum routine and access the x_targ published value
  if (fTargetXDiff==-1 && fTargetXDiffError == -1 && fTargetXDiffWidth==-1){//target asymmetry not published or accesible
    QwError<<"target position not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fTargetXDiffError=fTargetXDiffError*1e+3;//converts to ppm
  fTargetXDiff=fTargetXDiff*1e+3;//converts to ppm
  fTargetXDiffWidth=fTargetXDiffWidth*1e+3;//converts to ppm
  // if (fTargetXDiffError>fPositinPrecision){
  // QwError<<"Charge Asymmetry precision has not reached. Current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  // status=kFALSE;
  // fGoodPatternCounter=0;
  //}
  //else{
  //QwError<<"Charge Asymmetry precision, Current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    //UpdateGMClean(0);//set to not clean - Diabled the cfsocket comm-rakithab 11-27-2011
    FeedPITAPOSUSetPoints();//set the new PITA values
    LogPFUParameters();//Log PITA setting after feedback
    //UpdateGMClean(1);//set back to clean
    status=kTRUE;
     fPFUGoodPatternCounter=0;//Reset after each feedback operation
    // }


      return status;
  };


Bool_t QwHelicityCorrelatedFeedback::ApplyPITAPOSVFeedback(){
  Bool_t status=kFALSE;
  GetTargetPositionStatV();//call the calculate running sum routine and access the x_targ published value
  if (fTargetYDiff==-1 && fTargetYDiffError == -1 && fTargetYDiffWidth==-1){//target asymmetry not published or accesible
    QwError<<"target position not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fTargetYDiffError=fTargetYDiffError*1e+3;//converts to ppm
  fTargetYDiff=fTargetYDiff*1e+3;//converts to ppm
  fTargetYDiffWidth=fTargetYDiffWidth*1e+6;//converts to ppm
  // if (fTargetXDiffError>fPositinPrecision){
  // QwError<<"Charge Asymmetry precision has not reached. Current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  // status=kFALSE;
  // fGoodPatternCounter=0;
  //}
  //else{
  //QwError<<"Charge Asymmetry precision, Current value "<<fChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
    //UpdateGMClean(0);//set to not clean - Diabled the cfsocket comm-rakithab 11-27-2011
    FeedPITAPOSVSetPoints();//set the new PITA values
    LogPFVParameters();//Log PITA setting after feedback
    //UpdateGMClean(1);//set back to clean
    status=kTRUE;
    fPFVGoodPatternCounter=0;//Reset after each feedback operation
    // }


  return status;
   
};




Bool_t QwHelicityCorrelatedFeedback::ApplyPOSXYFeedback(){
  Bool_t status=kFALSE;
  GetPositionStatXY();//call the calculate running sum routine and access the x_targ published value
  if (fXDiff==-1 && fXDiffError == -1 && fXDiffWidth==-1 && fYDiff==-1 && fYDiffError == -1 && fYDiffWidth==-1){//target asymmetry not published or accesible
    QwError<<"target position not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fXDiffError=fXDiffError*1e+3;//converts to ppm
  fXDiff=fXDiff*1e+3;//converts to ppm
  fXDiffWidth=fXDiffWidth*1e+3;//converts to ppm
  fYDiffError=fYDiffError*1e+3;//converts to ppm
  fYDiff=fYDiff*1e+3;//converts to ppm
  fYDiffWidth=fYDiffWidth*1e+3;//converts to ppm
 
    FeedPOSXYSetPoints();//set the new PITA values
    LogXYParameters();//Log PITA setting after feedback
    //UpdateGMClean(1);//set back to clean
    status=kTRUE;
     fXGoodPatternCounter=0;//Reset after each feedback operation
     fYGoodPatternCounter=0;
    // }


      return status;
  };


/**********************************************************************/


Bool_t QwHelicityCorrelatedFeedback::ApplyHCIAFeedback(){
  Bool_t status=kFALSE;
  GetHCChargeStat();//call the calculate running sum routine and access the q_targ published value
  if (fTargetHCCharge==-1 && fTargetHCChargeError == -1 && fTargetHCChargeWidth==-1){//target asymmetry not published or accesible
    QwError<<"target asymmetry not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fTargetHCChargeError=fTargetHCChargeError*1e+6;//converts to ppm
  fTargetHCCharge=fTargetHCCharge*1e+6;//converts to ppm
  fTargetHCChargeWidth=fTargetHCChargeWidth*1e+6;//converts to ppm
  // if (fHCChargeAsymmetryError>fChargeAsymPrecision){
  //   QwError<<"Charge Asymmetry precision has not reached. Current value "<<fHCChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //   status=kFALSE;
  //   fHCGoodPatternCounter=0;
  // }
  // else{
  //   QwError<<"Charge Asymmetry precision, Current value "<<fHCChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //   if(fHCIAFB){
  //   //UpdateGMClean(0);//set to not clean - Diabled the cfsocket comm-rakithab 11-27-2011
    FeedHCIASetPoints();//set the new PITA values
      LogHCParameters();//Log PITA setting after feedback
    //UpdateGMClean(1);//set back to clean
    status=kTRUE;
    fHCIAGoodPatternCounter=0;//Reset after each feedback operation
  


  return status;
};



Bool_t QwHelicityCorrelatedFeedback::ApplyHAIAFeedback(){
  Bool_t status=kFALSE;
  GetHAChargeStat();//call the calculate running sum routine and access the q_targ published value
  if (fTargetHACharge==-1 && fTargetHAChargeError == -1 && fTargetHAChargeWidth==-1){//target asymmetry not published or accesible
    QwError<<"target asymmetry not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fTargetHAChargeError=fTargetHAChargeError*1e+6;//converts to ppm
  fTargetHACharge=fTargetHACharge*1e+6;//converts to ppm
  fTargetHAChargeWidth=fTargetHAChargeWidth*1e+6;//converts to ppm
  // if (fHCChargeAsymmetryError>fChargeAsymPrecision){
  //   QwError<<"Charge Asymmetry precision has not reached. Current value "<<fHCChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //   status=kFALSE;
  //   fHCGoodPatternCounter=0;
  // }
  // else{
  //   QwError<<"Charge Asymmetry precision, Current value "<<fHCChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //   if(fHCIAFB){
  //   //UpdateGMClean(0);//set to not clean - Diabled the cfsocket comm-rakithab 11-27-2011
    FeedHAIASetPoints();//set the new PITA values
      LogHAParameters();//Log PITA setting after feedback
    //UpdateGMClean(1);//set back to clean
    status=kTRUE;
    fHAIAGoodPatternCounter=0;//Reset after each feedback operation
  


  return status;
};


Bool_t QwHelicityCorrelatedFeedback::ApplyHBIAFeedback(){
  Bool_t status=kFALSE;
  GetHBChargeStat();//call the calculate running sum routine and access the q_targ published value
  if (fTargetHBCharge==-1 && fTargetHBChargeError == -1 && fTargetHBChargeWidth==-1){//target asymmetry not published or accesible
    QwError<<"target asymmetry not published or accesible"<<QwLog::endl;
    status=kFALSE;
  }
  fTargetHBChargeError=fTargetHBChargeError*1e+6;//converts to ppm
  fTargetHBCharge=fTargetHBCharge*1e+6;//converts to ppm
  fTargetHBChargeWidth=fTargetHBChargeWidth*1e+6;//converts to ppm
  // if (fHCChargeAsymmetryError>fChargeAsymPrecision){
  //   QwError<<"Charge Asymmetry precision has not reached. Current value "<<fHCChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //   status=kFALSE;
  //   fHCGoodPatternCounter=0;
  // }
  // else{
  //   QwError<<"Charge Asymmetry precision, Current value "<<fHCChargeAsymmetryError<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //   if(fHCIAFB){
  //   //UpdateGMClean(0);//set to not clean - Diabled the cfsocket comm-rakithab 11-27-2011
    FeedHBIASetPoints();//set the new PITA values
      LogHBParameters();//Log PITA setting after feedback
    //UpdateGMClean(1);//set back to clean
    status=kTRUE;
    fHBIAGoodPatternCounter=0;//Reset after each feedback operation
  


  return status;
};

/*****************************************************************/
//Bool_t QwHelicityCorrelatedFeedback::ApplyHAIAFeedback(){
  // Bool_t status=kFALSE;
  
  // GetHAChargeStat(0);
  // if (fHAChargeAsym[0]==-1 && fHAChargeAsymError[0] == -1 && fHAChargeAsymWidth[0]==-1){//target asymmetry not published or accesible
  //   QwError<<"Hall A asymmetry not published or accesible"<<QwLog::endl;
  // }else{
  //   fHAChargeAsymError[0]=fHAChargeAsymError[0]*1.0e+6;//converts to ppm
  //   fHAChargeAsym[0]=fHAChargeAsym[0]*1e+6;//converts to ppm
  //   fHAChargeAsymWidth[0]=fHAChargeAsymWidth[0]*1e+6;//converts to ppm
  //   if (fHAChargeAsymError[0]>fChargeAsymPrecision){
  //     QwError<<"Hall A Charge Asymmetry precision has not reached. Current value "<<fHAChargeAsymError[0]<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //     QwError<<"--------------------------------------------------------------------------------------------------------------------------------"<<QwLog::endl;
  //     fHAGoodPatternCounter=0;
  //   }else{
  //     //QwError<<"Hall A Charge Asymmetry precision, Current value "<<fHAChargeAsymError[0]<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //     if (fHAIAFB){
  // 	//UpdateGMClean(0);//set to not clean  - Diabled the cfsocket comm-rakithab 11-27-2011
  // 	FeedHAIASetPoint(0);
  // 	//UpdateGMClean(1);//set back to clean
  // 	//fHAGoodPatternCounter=0;    
  //     }  
  //     fHAGoodPatternCounter=0;    
  //     LogHAParameters(0);  
  //   }
  // }  

  // return status;
//};

/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::ApplyIAFeedback(Int_t mode){
  // Bool_t HCstatus=kFALSE;
  
  // if (mode<0 ||  mode>3){
  //   QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  //   return kFALSE;
  // }

  // QwMessage<<"ApplyIAFeedback["<<mode<<"]\n";
  
  // GetTargetChargeStat(mode);
  
  // if (fChargeAsym[mode]==-1 && fChargeAsymError[mode] == -1 && fChargeAsymWidth[mode]==-1){//target asymmetry not published or accesible
  //   QwError<<"target asymmetry not published or accesible"<<QwLog::endl;
  //   HCstatus=kFALSE;
  // }else{
  //   fChargeAsymError[mode]=fChargeAsymError[mode]*1e+6;//converts to ppm
  //   fChargeAsym[mode]=fChargeAsym[mode]*1e+6;//converts to ppm
  //   fChargeAsymWidth[mode]=fChargeAsymWidth[mode]*1e+6;//converts to ppm
  //   if (fChargeAsymError[mode]>fChargeAsymPrecision){
  //     QwError<<"Charge Asymmetry["<<mode<<"] precision not reached current value "<<fChargeAsymError[mode]<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //     QwError<<"--------------------------------------------------------------------------------------------------------------------------------"<<QwLog::endl;
  //     HCstatus=kFALSE;
  //     fHelModeGoodPatternCounter[mode]=0;
  //   }else
  //     HCstatus=kTRUE;
  // }
  
  
  // if (HCstatus && fIAFB){
  //   QwError<<"Charge Asymmetry["<<mode<<"] precision current value "<<fChargeAsymError[mode]<<" Expected "<<fChargeAsymPrecision<<QwLog::endl;
  //   //UpdateGMClean(0);//set to not clean  - Diabled the cfsocket comm-rakithab 11-27-2011
  //   FeedIASetPoint(mode);
  //   LogParameters(mode);  
  //   //UpdateGMClean(1);//set back to clean
  //   fHelModeGoodPatternCounter[mode]=0;
  //   fHAGoodPatternCounter=0;
  // }
  

  // return HCstatus;
};

Bool_t QwHelicityCorrelatedFeedback::ApplyHMFeedback(){
  // GetTargetPositionStat();//read running averages for target
  //for now the targer paremeters are simply published 
	    //  LogPFParameters();
	    //fPFGoodPatternCounter=0;//reset good pattern counter 
	    //  fTargetXDiffRunningSum.ClearEventData();//reset the running sums
  //fTargetXPDiffRunningSum.ClearEventData();//reset the running sums
  //fTargetYDiffRunningSum.ClearEventData();//reset the running sums
  //fTargetYPDiffRunningSum.ClearEventData();//reset the running sums
  //f3C12XDiffRunningSum.ClearEventData();//reset the running sums
  //f3C12YDiffRunningSum.ClearEventData();//reset the running sums
  // f3C12YQRunningSum.ClearEventData();//reset the running sums


  //return kTRUE;
};




/*****************************************************************/
void QwHelicityCorrelatedFeedback::ApplyFeedbackCorrections(){
  //Position Feedback
   if(fPITAPOSUFB){
  //QwMessage<<".....................Amali1..........................."<<QwLog::endl;
  if (IsPFUPatternsAccumulated()){

  fHalfWavePlateStatus = GetHalfWavePlateState();
      if(fHalfWavePlateStatus.Contains("IN")) {
	if(fHalfWaveRevert) fHalfWaveIN = false;
	else                fHalfWaveIN = true;
      }
      else {
	if(fHalfWaveRevert) fHalfWaveIN = true;
	else                fHalfWaveIN = false;
      }
      
      fHalfWaveOUT = !fHalfWaveIN;

      if (fHalfWaveIN)
	printf("NOTICE \n Half-wave-plate-IN\n");
      else
	printf("NOTICE \n Half-wave-plate-OUT\n");

    QwMessage<<"Initiating Position Feedback"<<QwLog::endl;
  
    //ApplyHMFeedback();
    ApplyPITAPOSUFeedback();
    // ApplyPITAPOSVFeedback();
  }
  }

if(fPITAPOSVFB){

  if (IsPFVPatternsAccumulated()){
fHalfWavePlateStatus = GetHalfWavePlateState();
      if(fHalfWavePlateStatus.Contains("IN")) {
	if(fHalfWaveRevert) fHalfWaveIN = false;
	else                fHalfWaveIN = true;
      }
      else {
	if(fHalfWaveRevert) fHalfWaveIN = true;
	else                fHalfWaveIN = false;
      }
      
      fHalfWaveOUT = !fHalfWaveIN;

      if (fHalfWaveIN)
	printf("NOTICE \n Half-wave-plate-IN\n");
      else
	printf("NOTICE \n Half-wave-plate-OUT\n");

    QwMessage<<"Initiating Position Feedback"<<QwLog::endl;
  
    //ApplyHMFeedback();
    ApplyPITAPOSVFeedback();
    // ApplyPITAPOSVFeedback();
  }
  }
  //End Position Feedback


if(fPOSXYFB){

  if (IsPosXPatternsAccumulated() && IsPosYPatternsAccumulated()){
fHalfWavePlateStatus = GetHalfWavePlateState();
      if(fHalfWavePlateStatus.Contains("IN")) {
	if(fHalfWaveRevert) fHalfWaveIN = false;
	else                fHalfWaveIN = true;
      }
      else {
	if(fHalfWaveRevert) fHalfWaveIN = true;
	else                fHalfWaveIN = false;
      }
      
      fHalfWaveOUT = !fHalfWaveIN;

      if (fHalfWaveIN)
	printf("NOTICE \n Half-wave-plate-IN\n");
      else
	printf("NOTICE \n Half-wave-plate-OUT\n");

    QwMessage<<"Initiating Position Feedback"<<QwLog::endl;
  
       ApplyPOSXYFeedback();
   
  }
  }
  //End Position Feedback


if(fHCIAFB){

  if (IsHCIAPatternsAccumulated()){
fHalfWavePlateStatus = GetHalfWavePlateState();
      if(fHalfWavePlateStatus.Contains("IN")) {
	if(fHalfWaveRevert) fHalfWaveIN = false;
	else                fHalfWaveIN = true;
      }
      else {
	if(fHalfWaveRevert) fHalfWaveIN = true;
	else                fHalfWaveIN = false;
      }
      
      fHalfWaveOUT = !fHalfWaveIN;

      if (fHalfWaveIN)
	printf("NOTICE \n Half-wave-plate-IN\n");
      else
	printf("NOTICE \n Half-wave-plate-OUT\n");

    QwMessage<<"Initiating HC-IA Feedback"<<QwLog::endl;
  
    //ApplyHMFeedback();
    ApplyHCIAFeedback();
    // ApplyPITAPOSVFeedback();
  }
  }

if(fHAIAFB){

  if (IsHAIAPatternsAccumulated()){
fHalfWavePlateStatus = GetHalfWavePlateState();
      if(fHalfWavePlateStatus.Contains("IN")) {
	if(fHalfWaveRevert) fHalfWaveIN = false;
	else                fHalfWaveIN = true;
      }
      else {
	if(fHalfWaveRevert) fHalfWaveIN = true;
	else                fHalfWaveIN = false;
      }
      
      fHalfWaveOUT = !fHalfWaveIN;

      if (fHalfWaveIN)
	printf("NOTICE \n Half-wave-plate-IN\n");
      else
	printf("NOTICE \n Half-wave-plate-OUT\n");

    QwMessage<<"Initiating HA-IA Feedback"<<QwLog::endl;
  
    //ApplyHMFeedback();
    ApplyHAIAFeedback();
    // ApplyPITAPOSVFeedback();
  }
  }


if(fHBIAFB){

  if (IsHBIAPatternsAccumulated()){
fHalfWavePlateStatus = GetHalfWavePlateState();
      if(fHalfWavePlateStatus.Contains("IN")) {
	if(fHalfWaveRevert) fHalfWaveIN = false;
	else                fHalfWaveIN = true;
      }
      else {
	if(fHalfWaveRevert) fHalfWaveIN = true;
	else                fHalfWaveIN = false;
      }
      
      fHalfWaveOUT = !fHalfWaveIN;

      if (fHalfWaveIN)
	printf("NOTICE \n Half-wave-plate-IN\n");
      else
	printf("NOTICE \n Half-wave-plate-OUT\n");

    QwMessage<<"Initiating HB-IA Feedback"<<QwLog::endl;
  
    //ApplyHMFeedback();
    ApplyHBIAFeedback();
    // ApplyPITAPOSVFeedback();
  }
  }

  //Hall C IA feedback 
  // if (fIAFB){
  //   for (Int_t i=0;i<kHelModes;i++){
  //     if (IsPatternsAccumulated(i)){
  // 	QwMessage<<"IsPatternsAccumulated for Hall C IA "<<QwLog::endl;
  // 	ApplyIAFeedback(i);//Hall C IA is not set up properly
  //     }
  //   }
  // }
  // //Hall A IA feedback, the condition fHAIAFB is checked inside the IsHAPatternsAccumulated() routine. This is done to report charge Aq even HA feedback is disabled.
  // if (IsHAPatternsAccumulated()){
  //   QwMessage<<"Initiating  Hall A IA Feedback"<<QwLog::endl;
  //   ApplyHAIAFeedback();
  // }  

  // if (IsHCPatternsAccumulated()){
  //QwMessage<<"Initiating  Hall C IA Feedback"<<QwLog::endl;
  //ApplyHCIAFeedback();
  //}  
  //End IA feedback

  //PITA feedback
  if (fPITAFB){
    if (IsPatternsAccumulated()){
      fHalfWavePlateStatus = GetHalfWavePlateState();
      if(fHalfWavePlateStatus.Contains("IN")) {
	if(fHalfWaveRevert) fHalfWaveIN = false;
	else                fHalfWaveIN = true;
      }
      else {
	if(fHalfWaveRevert) fHalfWaveIN = true;
	else                fHalfWaveIN = false;
      }
      
      fHalfWaveOUT = !fHalfWaveIN;

      if (fHalfWaveIN)
	printf("NOTICE \n Half-wave-plate-IN\n");
      else
	printf("NOTICE \n Half-wave-plate-OUT\n");


      //Check to see the IHWP has changed if we are running is Auto IHWP mode
      // if (fAutoIHWP){
      // 	if((fDefaultHalfWavePlateStatus==1 && fHalfWaveOUT) ||  (fDefaultHalfWavePlateStatus==0 && fHalfWaveIN)){ //IHWP at the start of the feedback is different from current value
      // 	  if (fHalfWaveOUT){
      // 	    fDefaultHalfWavePlateStatus=0;//update the defalut IHWP state to OUT 
      // 	    if (fPITASetpointPOS_t0_OUT && fPITASetpointNEG_t0_OUT){//reset the PC
      // 	      // fEPICSCtrl.Set_Pockels_Cell_plus(fPITASetpointPOS_t0_OUT);
      // 	      //fEPICSCtrl.Set_Pockels_Cell_minus(fPITASetpointNEG_t0_OUT);
      // 	    }
      // 	  }
      // 	  else{
      // 	    fDefaultHalfWavePlateStatus=1;//update the defalut IHWP state to IN
      // 	    if (fPITASetpointPOS_t0_IN && fPITASetpointNEG_t0_IN){//reset the PC
      // 	      //  fEPICSCtrl.Set_Pockels_Cell_plus(fPITASetpointPOS_t0_IN);
      // 	      //fEPICSCtrl.Set_Pockels_Cell_minus(fPITASetpointNEG_t0_IN);
      // 	    }
      // 	  }
      // 	  fGoodPatternCounter=0;//Reset after each feedback operation
      // 	  ClearRunningSum();//Then reset the running sum 

      // 	  //Update the new IHWP state in the text file
      // 	  if (IHWP_State!=NULL)
      // 	    fclose(IHWP_State); 
      // 	  IHWP_State = fopen("/local/scratch/qweak/Feedback_IHWP.txt", "w");
      // 	  printf("NOTICE \n Updating new IHWP %s \n",fHalfWavePlateStatus.Data());
      // 	  if (IHWP_State!=NULL){
      // 	    fprintf(IHWP_State,"%s",fHalfWavePlateStatus.Data());   
      // 	    fclose(IHWP_State);
      // 	  }

      //  return;//and start the correction loop all over 
      //	} 
	
      //      }

      ApplyPITAFeedback();//PITA corrections initiate here
    }
  }
  //End PITA feedback

   return;

 };

/*****************************************************************/
// Bool_t QwHelicityCorrelatedFeedback::IsPatternsAccumulated(Int_t mode){
//   if (fHelModeGoodPatternCounter[mode]>=fHAAccumulatePatternMax){
//     //QwMessage<<"fHelModeGoodPatternCounter["<<mode<<"]\n";
//     return kTRUE;
//   }

//   return kFALSE;
// };
/*****************************************************************/




// Bool_t QwHelicityCorrelatedFeedback::IsPFUPatternsAccumulated(){
//   if (fPFUGoodPatternCounter>=fPFUAccumulatePatternMax){
// 	return kTRUE;
// }
//       return kFALSE;
// };

// Bool_t QwHelicityCorrelatedFeedback::IsPFVPatternsAccumulated(){
//   if (fPFVGoodPatternCounter>=fPFVAccumulatePatternMax){
// 	return kTRUE;
// }
//       return kFALSE;
// };

Bool_t QwHelicityCorrelatedFeedback::IsHCIAPatternsAccumulated(){
  if (fHCIAGoodPatternCounter>=fHCIAAccumulatePatternMax){
	return kTRUE;
}
      return kFALSE;
};

Bool_t QwHelicityCorrelatedFeedback::IsHAIAPatternsAccumulated(){
  if (fHAIAGoodPatternCounter>=fHAIAAccumulatePatternMax){
	return kTRUE;
}
      return kFALSE;
};

Bool_t QwHelicityCorrelatedFeedback::IsHBIAPatternsAccumulated(){
  if (fHBIAGoodPatternCounter>=fHBIAAccumulatePatternMax){
	return kTRUE;
}
      return kFALSE;
};

Int_t QwHelicityCorrelatedFeedback::GetLastGoodHelicityPatternType(){
  return fCurrentHelPatMode;
};
/*****************************************************************/

//*****************************************************************
/**
 * Calculate asymmetries for the current pattern.
 */
void  QwHelicityCorrelatedFeedback::CalculateAsymmetry()
{

  Bool_t localdebug=kFALSE;

  if(localdebug)  std::cout<<"Entering QwHelicityPattern::CalculateAsymmetry \n";

  Int_t plushel  = 1;
  Int_t minushel = 0;
  Int_t checkhel = 0;
  Bool_t firstplushel=kTRUE;
  Bool_t firstminushel=kTRUE;

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();

  if (fIgnoreHelicity){
    //  Don't check to see if we have equal numbers of even and odd helicity states in this pattern.
    //  Build an asymmetry with even-parity phases as "+" and odd-parity phases as "-"
    for (size_t i = 0; i < (size_t) fPatternSize; i++) {
      Int_t localhel = 1;
      for (size_t j = 0; j < (size_t) fPatternSize/2; j++) {
  	localhel ^= ((i >> j)&0x1);
      }
      if (localhel == plushel) {
  	if (firstplushel) {
  	  fPositiveHelicitySum = fEvents.at(i);
  	  firstplushel = kFALSE;
  	} else {
  	  fPositiveHelicitySum += fEvents.at(i);
  	}
      } else if (localhel == minushel) {
  	if (firstminushel) {
  	  fNegativeHelicitySum = fEvents.at(i);
  	  firstminushel = kFALSE;
  	} else {
  	  fNegativeHelicitySum += fEvents.at(i);
  	}
      }
    }
  } else {
    //  
    //    fCurrentHelPat=0;
    for (size_t i = 0; i < (size_t) fPatternSize; i++) {
      if (fHelicity[i] == plushel) {
  	if (localdebug) 
  	  std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling fPositiveHelicitySum \n";
  	if (firstplushel) {
  	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with = \n";
  	  fPositiveHelicitySum = fEvents.at(i);
  	  firstplushel = kFALSE;
  	} else {
  	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with += \n";
  	  fPositiveHelicitySum += fEvents.at(i);
  	}
  	checkhel += 1;
      } else if (fHelicity[i] == minushel) {
  	if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling fNegativeHelicitySum \n";
  	if (firstminushel) {
  	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with = \n";
  	  fNegativeHelicitySum = fEvents.at(i);
  	  firstminushel = kFALSE;
  	} else {
  	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with += \n";
  	  fNegativeHelicitySum += fEvents.at(i);
  	}
  	checkhel -= 1;
      } else {
  	QwError<< "QwHelicityPattern::CalculateAsymmetry for event# " << i << "."
  	       << "Helicity should be "<<plushel<<" or "<<minushel<<" but is"<< fHelicity[i]
  	       << "; Asymmetry computation aborted!"<<QwLog::endl;
  	ClearEventData();
  	i = fPatternSize;
  	checkhel = -9999;
  	// This is an unknown helicity event.
      }
      
    }

  }

  if (checkhel == -9999) {
    QwError << " failed the checkhel == -9999 cut" << QwLog::endl;
    //do nothing the asymmetry computation has been aborted earlier in this function
    fPatternIsGood = kFALSE;
  } else if (checkhel!=0) {
    fPatternIsGood = kFALSE;
    // there is a different number of plus and minus helicity window.
    QwError<<" QwHelicityPattern::CalculateAsymmetry == \n"
  	   <<" you do not have the same number of positive and negative \n"
  	   <<" impossible to compute assymetry \n"
  	   <<" dropping every thing -- pattern number ="<<fCurrentPatternNumber<<QwLog::endl;
  } else {
    //  This is a good pattern.
    //  Calculate the asymmetry.

    // //Now set the HelPatMode 
    // if (fPreviousHelPat){
    //   if (fPreviousHelPat==kHelPat1 && fCurrentHelPat==kHelPat1)
    // 	fCurrentHelPatMode=0;
    //   else if (fPreviousHelPat==kHelPat1 && fCurrentHelPat==kHelPat2)
    // 	fCurrentHelPatMode=1;
    //   else if (fPreviousHelPat==kHelPat2 && fCurrentHelPat==kHelPat1)
    // 	fCurrentHelPatMode=2;
    //   else if (fPreviousHelPat==kHelPat2 && fCurrentHelPat==kHelPat2)
    // 	fCurrentHelPatMode=3;
    //   else
    // 	fCurrentHelPatMode=-1;//error
    // }
    // //save the current pattern size to previouspat
    // if (localdebug) 
    //   QwMessage<<"QwHelicityPattern::CalculateAsymmetry current helpat is "<<fCurrentHelPat<<" Prev pat "<<fPreviousHelPat<<" Mode "<<fCurrentHelPatMode<<" \n"; 
    // fPreviousHelPat=fCurrentHelPat;
    
    fPatternIsGood = kTRUE;
    fPatternCounter++;
    //std::cout<<" quartet count ="<<fQuartetNumber<<"\n";

    fYield.Sum(fPositiveHelicitySum,fNegativeHelicitySum);
    fYield.Scale(1.0/fPatternSize);
    fDifference.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
    fDifference.Scale(1.0/fPatternSize);
    if (! fIgnoreHelicity){
      //  Only blind the difference if we're using the real helicity.
      fBlinder.Blind(fDifference,fYield);
    }
    fAsymmetry.Ratio(fDifference,fYield);

    if (fAsymmetry.GetEventcutErrorFlag()==0){//good pattern
      //fPatternIsGood = kTRUE;
      fGoodPatternCounter++;//increment the quartet number - reset after each PITA feedback operation
      fQuartetNumber++;//Then increment the quartet number - continously count
    } else
      QwError << "fails on errorflag cut" << QwLog::endl;

    if (localdebug) QwDebug << " pattern number =" << fQuartetNumber << QwLog::endl;
  }

  return;
};


void QwHelicityCorrelatedFeedback::AccumulateRunningSum(QwHelicityCorrelatedFeedback &entry){
  // Bool_t bXDiff=kFALSE;
  Bool_t bXPDiff=kFALSE;
  // Bool_t bYDiff=kFALSE;
  Bool_t bYPDiff=kFALSE;
  Bool_t b3C12XDiff=kFALSE;
  Bool_t b3C12YDiff=kFALSE;
  Bool_t b3C12YQ=kFALSE;

  //  Get the current pattern number for reporting purposes
  fCurrentPatternNumber = entry.fCurrentPatternNumber;

  if (entry.fAsymmetry.GetEventcutErrorFlag()==0){//good pattern
    //fPatternIsGood = kTRUE;
    fGoodPatternCounter++;//increment the quartet number - reset after each PITA feedback operation
    fQuartetNumber++;//Then increment the quartet number - continously count
  } else
    QwError << "fails on errorflag cut" << QwLog::endl;

  QwHelicityPattern::AccumulateRunningSum(entry);

  if(entry.fAsymmetry.RequestExternalValue("q_targC", &fTargetParameter)){
    //fScalerChargeRunningSum.PrintValue();
    //fScalerChargeRunningSum.AccumulateRunningSum(fScalerCharge);
    if (fTargetParameter.GetEventcutErrorFlag()==0 && fAsymmetry.GetEventcutErrorFlag()==0){
      fTargetHCChargeRunningSum.AccumulateRunningSum(fTargetParameter);
      fHCIAGoodPatternCounter++;//update the good HA asymmetry counter
    }
  }

  if(entry.fAsymmetry.RequestExternalValue("q_targA", &fTargetParameter)){
    if (fTargetParameter.GetEventcutErrorFlag()==0 && fAsymmetry.GetEventcutErrorFlag()==0){
      fTargetHAChargeRunningSum.AccumulateRunningSum(fTargetParameter);
      fHAIAGoodPatternCounter++;//update the good HA asymmetry counter
    }
  }

  if(entry.fAsymmetry.RequestExternalValue("q_targB", &fTargetParameter)){
    if (fTargetParameter.GetEventcutErrorFlag()==0 && fAsymmetry.GetEventcutErrorFlag()==0){
      fTargetHBChargeRunningSum.AccumulateRunningSum(fTargetParameter);
      fHBIAGoodPatternCounter++;//update the good HA asymmetry counter
    }
  }
  
  if(entry.fAsymmetry.RequestExternalValue("x_targ", &fTargetParameter)){
    if (fTargetParameter.GetEventcutErrorFlag()==0 && fAsymmetry.GetEventcutErrorFlag()==0){
      fTargetXDiffRunningSum.AccumulateRunningSum(fTargetParameter);
      //  bXDiff=kTRUE;
      fPFUGoodPatternCounter++;
    }
  }

  if(entry.fAsymmetry.RequestExternalValue("xy_pos_x", &fTargetParameter)){
    if (fTargetParameter.GetEventcutErrorFlag()==0 && fAsymmetry.GetEventcutErrorFlag()==0){
      fXYPosXDiffRunningSum.AccumulateRunningSum(fTargetParameter);
      //  bXDiff=kTRUE;
      fXGoodPatternCounter++;
    }
  }

  if(entry.fAsymmetry.RequestExternalValue("xy_pos_y", &fTargetParameter)){
    if (fTargetParameter.GetEventcutErrorFlag()==0 && fAsymmetry.GetEventcutErrorFlag()==0){
      fXYPosYDiffRunningSum.AccumulateRunningSum(fTargetParameter);
      //  bXDiff=kTRUE;
      fYGoodPatternCounter++;
    }
  }

  if(entry.fAsymmetry.RequestExternalValue("y_targ", &fTargetParameter)){
    if (fTargetParameter.GetEventcutErrorFlag()==0 && fAsymmetry.GetEventcutErrorFlag()==0){
      fTargetYDiffRunningSum.AccumulateRunningSum(fTargetParameter);
      fPFVGoodPatternCounter++;
      // bYDiff=kTRUE;
    }
  }

  switch(fCurrentHelPatMode){
  case 0:
    fFBRunningAsymmetry[0].AccumulateRunningSum(entry.fAsymmetry);
    fHelModeGoodPatternCounter[0]++;
    break;
  case 1:
    fFBRunningAsymmetry[1].AccumulateRunningSum(entry.fAsymmetry);
    fHelModeGoodPatternCounter[1]++;
    break;
  case 2:
    fFBRunningAsymmetry[2].AccumulateRunningSum(entry.fAsymmetry);
    fHelModeGoodPatternCounter[2]++;
    break;
  case 3:
    fFBRunningAsymmetry[3].AccumulateRunningSum(entry.fAsymmetry);
    fHelModeGoodPatternCounter[3]++;
    break;
  }
};



void QwHelicityCorrelatedFeedback::CalculateRunningAverage(){
  QwHelicityPattern::CalculateRunningAverage();
};

void QwHelicityCorrelatedFeedback::CalculateRunningAverage(Int_t mode){
  fFBRunningAsymmetry[mode].CalculateRunningAverage();
};


//*****************************************************************
/**
/// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
*/
void QwHelicityCorrelatedFeedback::GetTargetChargeStat(){
  CalculateRunningAverage();

  if (fAsymmetry.RequestExternalValue("q_targ",&fTargetCharge)){
    QwMessage<<"Reading published charge value stats"<<QwLog::endl;
    fTargetCharge.PrintInfo();
    fChargeAsymmetry=fTargetCharge.GetValue();
    fChargeAsymmetryError=fTargetCharge.GetValueError();
    fChargeAsymmetryWidth=fTargetCharge.GetValueWidth();

    // //calculate mean BCM78DD asymmetry and bcm8 yield and update parameters to be publised in EPICS
    // fAsymBCM78DDRunningSum.CalculateRunningAverage();
    // fYieldBCM8RunningSum.CalculateRunningAverage();

    // fBCM8Yield=fYieldBCM8RunningSum.GetValue();

    // fAsymBCM78DD=fAsymBCM78DDRunningSum.GetValue()*1.0e+6;
    // fAsymBCM78DDError=fAsymBCM78DDRunningSum.GetValueError()*1.0e+6;
    // fAsymBCM78DDWidth=fAsymBCM78DDRunningSum.GetValueWidth()*1.0e+6;

    // //calculate mean USLumi Sum asymmetry and update parameters to be publised in EPICS
    // fAsymUSLumiSumRunningSum.CalculateRunningAverage();
    // fAsymBCMUSLumiSum=fAsymUSLumiSumRunningSum.GetValue()*1.0e+6;
    // fAsymBCMUSLumiSumError=fAsymUSLumiSumRunningSum.GetValueError()*1.0e+6;
    // fAsymBCMUSLumiSumWidth=fAsymUSLumiSumRunningSum.GetValueWidth()*1.0e+6;
    return ;
  }
  QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fChargeAsymmetry=-1;
  fChargeAsymmetryError=-1;
  fChargeAsymmetryWidth=-1;

  // fBCM8Yield=0;
  // fAsymBCM78DD=0;
  // fAsymBCM78DDError=0;
  // fAsymBCM78DDWidth=0;

  // fAsymBCMUSLumiSum=0;
  // fAsymBCMUSLumiSumError=0;
  // fAsymBCMUSLumiSumWidth=0;
  return;  
};

//*****************************************************************
/**
/// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
*/
void QwHelicityCorrelatedFeedback::GetTargetChargeStat(Int_t mode){
  if (!fIAFB){
     fChargeAsym[mode]=-1;
     fChargeAsymError[mode]=-1;
     fChargeAsymWidth[mode]=-1;
  }

  if (mode<0 ||  mode>3){
    QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
    return;
  }
  /*
  fFBRunningAsymmetry[mode].CalculateRunningAverage();
  if (fFBRunningAsymmetry[mode].RequestExternalValue("q_targ",&fTargetCharge)){
    QwMessage<<"Reading published charge value stats"<<QwLog::endl;
    fTargetCharge.PrintInfo();
    fChargeAsym[mode]=fTargetCharge.GetHardwareSum();
    fChargeAsymError[mode]=fTargetCharge.GetHardwareSumError();
    fChargeAsymWidth[mode]=fTargetCharge.GetHardwareSumWidth();
    return ;
  }
  */
  //QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fChargeAsym[mode]=-1;
  fChargeAsymError[mode]=-1;
  fChargeAsymWidth[mode]=-1;

  return;  
};

//*****************************************************************





//*****************************************************************
/**
/// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
*/
void QwHelicityCorrelatedFeedback::GetTargetPositionStatU(){
 
 fTargetXDiffRunningSum.CalculateRunningAverage();

  if (fAsymmetry.RequestExternalValue("x_targ",&fTargetParameter)){
    QwMessage<<"Reading published Position value stats"<<QwLog::endl;
      fTargetXDiffRunningSum.PrintInfo();
    fTargetXDiff=fTargetXDiffRunningSum.GetValue();
    fTargetXDiffError=fTargetXDiffRunningSum.GetValueError();
    fTargetXDiffWidth=fTargetXDiffRunningSum.GetValueWidth();

    
    return ;

  }


  // QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fTargetXDiff=-1;
  fTargetXDiffError=-1;
  fTargetXDiffWidth=-1;


   return; 
   
};



void QwHelicityCorrelatedFeedback::GetTargetPositionStatV(){
fTargetYDiffRunningSum.CalculateRunningAverage();

  if (fAsymmetry.RequestExternalValue("y_targ",&fTargetParameter)){
    QwMessage<<"Reading published Position value stats"<<QwLog::endl;
     fTargetYDiffRunningSum.PrintInfo();
    fTargetYDiff=fTargetYDiffRunningSum.GetValue();
    fTargetYDiffError=fTargetYDiffRunningSum.GetValueError();
    fTargetYDiffWidth=fTargetYDiffRunningSum.GetValueWidth();

    
    return ;
  }


  // QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fTargetYDiff=-1;
  fTargetYDiffError=-1;
  fTargetYDiffWidth=-1;


  return;



};

//*****************************************************************
void QwHelicityCorrelatedFeedback::GetPositionStatXY(){
 
 fXYPosXDiffRunningSum.CalculateRunningAverage();
 fXYPosYDiffRunningSum.CalculateRunningAverage();

  if (fAsymmetry.RequestExternalValue("xy_pos_x",&fTargetParameter) && fAsymmetry.RequestExternalValue("xy_pos_y",&fTargetParameter)){
    QwMessage<<"Reading published Position value stats"<<QwLog::endl;
      fXYPosXDiffRunningSum.PrintInfo();
      fXYPosYDiffRunningSum.PrintInfo();
    fXDiff=fXYPosXDiffRunningSum.GetValue();
    fXDiffError=fXYPosXDiffRunningSum.GetValueError();
    fXDiffWidth=fXYPosXDiffRunningSum.GetValueWidth();
    fYDiff=fXYPosYDiffRunningSum.GetValue();
    fYDiffError=fXYPosYDiffRunningSum.GetValueError();
    fYDiffWidth=fXYPosYDiffRunningSum.GetValueWidth();

    
    return ;

  }


  // QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fXDiff=-1;
  fXDiffError=-1;
  fXDiffWidth=-1;
  fYDiff=-1;
  fYDiffError=-1;
  fYDiffWidth=-1;


   return; 
   
};


void QwHelicityCorrelatedFeedback::GetTargetPositionStat(){
//   //compute the running averages
//    fTargetXDiffRunningSum.CalculateRunningAverage();
//    // fTargetXPDiffRunningSum.CalculateRunningAverage();
//    fTargetYDiffRunningSum.CalculateRunningAverage();  
//   // fTargetYPDiffRunningSum.CalculateRunningAverage();
//   // f3C12XDiffRunningSum.CalculateRunningAverage(); 
//   // f3C12YDiffRunningSum.CalculateRunningAverage(); 
//   // f3C12YQRunningSum.CalculateRunningAverage();


//   // //Update X stats
// fTargetXDiff=fTargetXDiffRunningSum.GetValue()*1.0e+3;
//   fTargetXDiffError=fTargetXDiffRunningSum.GetValueError()*1.0e+3;
//  fTargetXDiffWidth=fTargetXDiffRunningSum.GetValueWidth()*1.0e+3;

//   // //Update XP stats in nrad
//   // fTargetXPDiff=fTargetXPDiffRunningSum.GetValue()*1.0e+9;
//   // fTargetXPDiffError=fTargetXPDiffRunningSum.GetValueError()*1.0e+9;
//   // fTargetXPDiffWidth=fTargetXPDiffRunningSum.GetValueWidth()*1.0e+9;

//   // //Update Y stats
//    fTargetYDiff=fTargetYDiffRunningSum.GetValue()*1.0e+3;
// fTargetYDiffError=fTargetYDiffRunningSum.GetValueError()*1.0e+3;
//   fTargetYDiffWidth=fTargetYDiffRunningSum.GetValueWidth()*1.0e+3;

//   // //Update YP stats in nrad
//   // fTargetYPDiff=fTargetYPDiffRunningSum.GetValue()*1.0e+9;
//   // fTargetYPDiffError=fTargetYPDiffRunningSum.GetValueError()*1.0e+9;
//   // fTargetYPDiffWidth=fTargetYPDiffRunningSum.GetValueWidth()*1.0e+9;


//   // //Update 3c12X stats
//   // f3C12XDiff=f3C12XDiffRunningSum.GetValue()*1.0e+3;
//   // f3C12XDiffError=f3C12XDiffRunningSum.GetValueError()*1.0e+3;
//   // f3C12XDiffWidth=f3C12XDiffRunningSum.GetValueWidth()*1.0e+3;

//   // //Update 3c12Y stats
//   // f3C12YDiff=f3C12YDiffRunningSum.GetValue()*1.0e+3;
//   // f3C12YDiffError=f3C12YDiffRunningSum.GetValueError()*1.0e+3;
//   // f3C12YDiffWidth=f3C12YDiffRunningSum.GetValueWidth()*1.0e+3;

//   // //Update 3c12YQ stats
//   // f3C12YQ=f3C12YQRunningSum.GetValue();
//   // f3C12YQError=f3C12YQRunningSum.GetValueError();
//   // f3C12YQWidth=f3C12YQRunningSum.GetValueWidth();



  

  
//   // fTargetXDiffRunningSum.PrintInfo();
//   //fTargetXPDiffRunningSum.PrintInfo();
//   //fTargetYDiffRunningSum.PrintInfo();
//   //fTargetYPDiffRunningSum.PrintInfo();
  

//   return;

};



void QwHelicityCorrelatedFeedback::GetHCChargeStat(){

 fTargetHCChargeRunningSum.CalculateRunningAverage();

  if (fAsymmetry.RequestExternalValue("q_targC",&fTargetParameter)){
    QwMessage<<"Reading published HCcharge value stats"<<QwLog::endl;
     fTargetHCChargeRunningSum.PrintInfo();
    fTargetHCCharge=fTargetHCChargeRunningSum.GetValue();
    fTargetHCChargeError=fTargetHCChargeRunningSum.GetValueError();
    fTargetHCChargeWidth=fTargetHCChargeRunningSum.GetValueWidth();

    
    return ;

  }


  // QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fTargetHCCharge=-1;
  fTargetHCChargeError=-1;
  fTargetHCChargeWidth=-1;


   return; 
   
};


void QwHelicityCorrelatedFeedback::GetHAChargeStat(){

 fTargetHAChargeRunningSum.CalculateRunningAverage();

  if (fAsymmetry.RequestExternalValue("q_targA",&fTargetParameter)){
    QwMessage<<"Reading published HAcharge value stats"<<QwLog::endl;
     fTargetHAChargeRunningSum.PrintInfo();
    fTargetHACharge=fTargetHAChargeRunningSum.GetValue();
    fTargetHAChargeError=fTargetHAChargeRunningSum.GetValueError();
    fTargetHAChargeWidth=fTargetHAChargeRunningSum.GetValueWidth();

    
    return ;

  }


  // QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fTargetHACharge=-1;
  fTargetHAChargeError=-1;
  fTargetHAChargeWidth=-1;


   return; 
   
};



void QwHelicityCorrelatedFeedback::GetHBChargeStat(){

 fTargetHBChargeRunningSum.CalculateRunningAverage();

  if (fAsymmetry.RequestExternalValue("q_targB",&fTargetParameter)){
    QwMessage<<"Reading published HBcharge value stats"<<QwLog::endl;
     fTargetHBChargeRunningSum.PrintInfo();
    fTargetHBCharge=fTargetHBChargeRunningSum.GetValue();
    fTargetHBChargeError=fTargetHBChargeRunningSum.GetValueError();
    fTargetHBChargeWidth=fTargetHBChargeRunningSum.GetValueWidth();

    
    return ;

  }


  // QwError << " Could not get external value setting parameters to  q_targ" <<QwLog::endl;
  fTargetHBCharge=-1;
  fTargetHBChargeError=-1;
  fTargetHBChargeWidth=-1;


   return; 
   
};

  // fHCChargeRunningSum.CalculateRunningAverage();

  
  //   QwMessage<<"Reading published Hall C charge value stats"<<QwLog::endl;
  //   fHCChargeRunningSum.PrintInfo();
  //   fHCChargeAsymmetry=fHCChargeRunningSum.GetValue();
  //   fHCChargeAsymmetryError=fHCChargeRunningSum.GetValueError();
  //   fHCChargeAsymmetryWidth=fHCChargeRunningSum.GetValueWidth();

  
  //   return ;
 

//*****************************************************************
/**
/// \brief retrieves the Hall A charge asymmetry,asymmetry error ,asymmetry width
*/

//void QwHelicityCorrelatedFeedback::GetHAChargeStat(Int_t mode){
  // if (mode<0 ||  mode>3){
//     QwError << " Hall A Mode is out of bound " << mode <<" mode<0 ||  mode>3 "<<QwLog::endl;
//     return;
//   }
//   // fScalerChargeRunningSum.CalculateRunningAverage();
//   //  QwError<<"Reading published Hall A charge value stats "<<QwLog::endl;
//   // fScalerChargeRunningSum.PrintValue();
//   // fHAChargeAsym[mode]=fScalerChargeRunningSum.GetValue();
//   // fHAChargeAsymError[mode]=fScalerChargeRunningSum.GetValueError();
//   // fHAChargeAsymWidth[mode]=fScalerChargeRunningSum.GetValueWidth();
//   QwError<<" Scaler asym "<<fHAChargeAsym[mode]<<" error "<<fHAChargeAsymError[mode]<<" width "<<fHAChargeAsymWidth[mode]<<QwLog::endl;
//   return ;
//};

// //*****************************************************************
// /**
//  * Clear the running sums of yield, difference and asymmetry.
//  * Also clear the running burst sums if enabled
//  */
void  QwHelicityCorrelatedFeedback::ClearRunningSum()
{
  ClearEventData();
  fTargetCharge.ClearEventData();
  // //Clean bcm8 yield and bcm78 DD running sums
  // fAsymBCM78DDRunningSum.ClearEventData();
  // fYieldBCM8RunningSum.ClearEventData();
  // //clear US lumi running sum 
  // fAsymUSLumiSumRunningSum.ClearEventData();
}

void  QwHelicityCorrelatedFeedback::ClearRunningSum(Int_t mode)
{
  fFBRunningAsymmetry[mode].ClearEventData();
};

void  QwHelicityCorrelatedFeedback::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values){
  QwHelicityPattern::ConstructBranchAndVector(tree,prefix,values);
};

void  QwHelicityCorrelatedFeedback::FillTreeVector(std::vector<Double_t> &values) const{
  QwHelicityPattern::FillTreeVector(values);
};


TString  QwHelicityCorrelatedFeedback::GetHalfWavePlateState()
{
  TString plate_status = gSystem->GetFromPipe("caget -t -w 0.1 IGL1I00DI24_24M");
  return plate_status;
};

UInt_t QwHelicityCorrelatedFeedback::GetHalfWavePlate2State(){
  TString ihwp2_value = gSystem->GetFromPipe("caget -tf0 -w 0.1  IGL1I00DIOFLRD");
  UInt_t ihwp2 =ihwp2_value.Atoi();
  
  if (ihwp2>10000)
    return 1;//13056=IN
  else
    return 0; //8960=OUT
};

void QwHelicityCorrelatedFeedback::CheckFeedbackStatus()
{
  // Double_t old_status = 0.0;
  // old_status = fEPICSCtrl.Get_FeedbackStatus();
  // if(old_status == 1.0) { 
  //   // EPIC says, feedback is on, but we start it now. 
  //   // qwfeedback was killed by "kill -9 option" or was ended improperly.
  //   fEPICSCtrl.Set_FeedbackStatus(0.0);
  //   fFeedbackStatus=kFALSE;
  //   gSystem->Sleep(4000); // wait 4 secs
  // }
  // fEPICSCtrl.Set_FeedbackStatus(1.0);
  // fFeedbackStatus=kTRUE;
}
