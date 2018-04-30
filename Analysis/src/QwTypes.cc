#include "QwTypes.h"
#include "TString.h"

/**
 * Determine the EQwPMTInstrumentType value corresponding to a
 * text name of the type, as used in the channel map files.
 * The text comparison is not case sensitive.
 * @param  name Text name of an instrument type, such as "IntegrationPMT"
 * @return      EQwPMTInstrumentType value corresponding to the name
 */
EQwPMTInstrumentType GetQwPMTInstrumentType(TString name){
  //  The name is passed by value because we will
  //  change it to lower case.
  name.ToLower();
  EQwPMTInstrumentType result;
  result = kQwUnknownPMT;
  if (name=="integrationpmt"){
    result = kQwIntegrationPMT;
  } else if (name=="scalerpmt"){
    result = kQwScalerPMT;
  }
  else if (name=="combinationpmt"){
    result = kQwCombinedPMT;
  }
  return result;
}


EQwBeamInstrumentType GetQwBeamInstrumentType(TString name){
  //  The name is passed by value because we will
  //  change it to lower case.
  name.ToLower();
  EQwBeamInstrumentType result;
  result = kQwUnknownDeviceType;
  if (name=="bpmstripline"){
    result = kQwBPMStripline;
  } 
  else if (name=="bcm"){
    result = kQwBCM;
  }
  else if (name=="combinedbcm"){
    result = kQwCombinedBCM;
  }
  else if (name=="combinedbpm"){
    result = kQwCombinedBPM;
  }
  else if (name=="energycalculator"){
    result = kQwEnergyCalculator;
  }
  else if (name=="halomonitor"){
    result = kQwHaloMonitor;
  }
  else if (name=="bpmcavity"){
    result = kQwBPMCavity;
  }
  else if (name=="qpd"){
    result = kQwQPD;
  }
  else if (name=="lineararray"){
    result = kQwLinearArray;
  }
  else if (name=="clock") {
    result = kQwClock;
  }
  return result;
}


/**
 * Get the text name of a EQwPMTInstrumentType, as it would be
 * used in the channel map files.
 * @param  type   EQwPMTInstrumentType value for which the name should be 
 *                returned, such as kQwPMTIntegrationPMT
 * @return        Text name corresponding to the type
 */
TString GetQwPMTInstrumentTypeName(EQwPMTInstrumentType type){
  TString result;
  result = "UnknownPMT";
  if (type==kQwIntegrationPMT){
    result = "IntegrationPMT";
    result.ToLower();
  } 
  else if (type==kQwScalerPMT){
    result = "ScalerPMT";
    result.ToLower();
  } 
  else if (type==kQwCombinedPMT){
    result = "CombinationPMT";
    result.ToLower();
  }
  return result;
}


TString GetQwBeamInstrumentTypeName(EQwBeamInstrumentType type){
  TString result;
  result = "kQwUnknownDeviceType";
  if (type==kQwBPMStripline){
    result = "bpmstripline";
  } 
  else if (type==kQwBCM){
    result = "bcm";
  }
  else if (type==kQwQPD){
    result = "qpd";
  }
  else if (type==kQwLinearArray){
    result = "lineararray";
  }
  else if (type==kQwCombinedBCM){
    result = "combinedbcm";
  }
  else if (type==kQwCombinedBPM){
    result = "combinedbpm";
  }
  else if (type==kQwEnergyCalculator){
    result = "energycalculator";
  }
  else if (type==kQwHaloMonitor){
    result = "halomonitor";
  }
  else if (type==kQwBPMCavity)
    result = "bpmcavity";
  return result;
}

UInt_t GetGlobalErrorFlag(TString evtype,Int_t evMode,Double_t stabilitycut){
  UInt_t evntMode;
  if (evMode==3)
    evntMode=kEventCutMode3;
  else
    evntMode=0;
  if (evtype=="g" && stabilitycut>0)
    return (kGlobalCut|kStabilityCut|evntMode);
  else if (evtype=="g" && stabilitycut<=0)
    return (kGlobalCut|evntMode);  
  else if (evtype=="l" && stabilitycut>0)
    return (kLocalCut|evntMode|kStabilityCut);
  else if (evtype=="l" && stabilitycut<=0)
    return (kLocalCut|evntMode); 
  
  return 0;
}

std::string WienModeName(EQwWienMode type){
  const std::string wienname[5] = {"Indeterminate", "Forward", "Backward",
			       "Vertical", "Horizontal"};
  return wienname[type];
};

EQwWienMode WienModeIndex(TString name){
  EQwWienMode retval = kWienIndeterminate;
  if (name=="Forward"){
    retval = kWienForward;
  } else if (name=="Backward"){
    retval = kWienBackward;
  } else if (name=="Vertical"){
    retval = kWienVertTrans;
  } else if (name=="Horizontal"){
    retval = kWienHorizTrans;
  }
  return retval;
}
