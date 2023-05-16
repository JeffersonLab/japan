/**********************************************************\
 * File: QwCombinedBPM.cc                                  *
 *                                                         *
 * Author: B. Waidyawansa                                  *
 * Time-stamp:                                             *
\**********************************************************/

#include "QwCombinedBPM.h"

// System headers
#include <stdexcept>

// Qweak headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif // __USE_DATABASE__
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"
#include "QwParameterFile.h"
#include "QwMollerADC_Channel.h"

template<typename T>
void  QwCombinedBPM<T>::InitializeChannel(TString name)
{

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(name+"WS","derived");

  for( Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].InitializeChannel(name+kAxisLabel[axis],"derived");
    fSlope[axis].InitializeChannel(name+kAxisLabel[axis]+"Slope","derived");
    fIntercept[axis].InitializeChannel(name+kAxisLabel[axis]+"Intercept","derived");
    fMinimumChiSquare[axis].InitializeChannel(name+kAxisLabel[axis]+"MinChiSquare","derived");
  }

  fixedParamCalculated = false;

  fElement.clear();
  fQWeights.clear();
  fXWeights.clear();
  fYWeights.clear();

  fSumQweights = 0.0;

  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    erra[axis]       = 0.0;
    errb[axis]       = 0.0;
    covab[axis]      = 0.0;
    A[axis]          = 0.0;
    B[axis]          = 0.0;
    D[axis]          = 0.0;
    m[axis]          = 0.0;
  }

  return;
}

template<typename T>
void  QwCombinedBPM<T>::InitializeChannel(TString subsystem, TString name)
{

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(subsystem, "QwCombinedBPM", name+"WS","derived");

  for( Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].InitializeChannel(subsystem, "QwCombinedBPM", name+kAxisLabel[axis],"derived");
    fSlope[axis].InitializeChannel(subsystem, "QwCombinedBPM", name+kAxisLabel[axis]+"Slope","derived");
    fIntercept[axis].InitializeChannel(subsystem, "QwCombinedBPM", name+kAxisLabel[axis]+"Intercept","derived");
    fMinimumChiSquare[axis].InitializeChannel(subsystem, "QwCombinedBPM",name+kAxisLabel[axis]+"MinChiSquare","derived");
  }
  
  fixedParamCalculated = false;

  fElement.clear();
  fQWeights.clear();
  fXWeights.clear();
  fYWeights.clear();

  fSumQweights = 0.0;

  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    erra[axis]       = 0.0;
    errb[axis]       = 0.0;
    covab[axis]      = 0.0;
    A[axis]          = 0.0;
    B[axis]          = 0.0;
    D[axis]          = 0.0;
    m[axis]          = 0.0;
  }

  return;
}


template<typename T>
void QwCombinedBPM<T>::ClearEventData()
{
  fEffectiveCharge.ClearEventData();

  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].ClearEventData();
    fSlope[axis].ClearEventData();
    fIntercept[axis].ClearEventData();
  }

  return;
}


template<typename T>
void QwCombinedBPM<T>::SetBPMForCombo(const VQwBPM* bpm, Double_t charge_weight,  Double_t x_weight, Double_t y_weight,
			Double_t sumqw)
{
  fElement.push_back(bpm);
  fQWeights.push_back(charge_weight);
  fXWeights.push_back(x_weight);
  fYWeights.push_back(y_weight);
  fSumQweights=sumqw;

  size_t i = fElement.size();
  if (i>=1){
    i--;
    //    std::cout << "+++++++++++++++++++++++++++\n+++++++++++++++++++++++++++\n" << std::endl;
    //     std::cout << "fElement.size()==" << fElement.size() << " " << i << " "
    // 	      << fElement.at(i)->GetElementName() << " "
    // 	      << fQWeights.at(i) << " "
    // 	      << fXWeights.at(i) << " "
    // 	      << fYWeights.at(i) << " "
    // 	      << "fElement.at(i)->GetEffectiveCharge()==" << fElement.at(i)->GetEffectiveCharge() << " "
    // 	      << std::endl;
    //     fElement.at(i)->GetEffectiveCharge()->PrintInfo();
    //     fElement.at(i)->PrintInfo();
  }
  return;

}


template<typename T>
Bool_t QwCombinedBPM<T>::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  return eventokay;
}


template<typename T>
void QwCombinedBPM<T>::IncrementErrorCounters()
{
  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].IncrementErrorCounters();
    fSlope[axis].IncrementErrorCounters();
    fIntercept[axis].IncrementErrorCounters();
    fMinimumChiSquare[axis].IncrementErrorCounters();
  }

  fEffectiveCharge.IncrementErrorCounters();
}

template<typename T>
void QwCombinedBPM<T>::PrintErrorCounters() const
{
  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].PrintErrorCounters();
    fSlope[axis].PrintErrorCounters();
    fIntercept[axis].PrintErrorCounters();
    fMinimumChiSquare[axis].PrintErrorCounters();
  }

  fEffectiveCharge.PrintErrorCounters();
}

template<typename T>
UInt_t QwCombinedBPM<T>::GetEventcutErrorFlag()
{
  UInt_t error=0;
  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    error|=fAbsPos[axis].GetEventcutErrorFlag();
    error|=fSlope[axis].GetEventcutErrorFlag();
    error|=fIntercept[axis].GetEventcutErrorFlag();
    error|=fMinimumChiSquare[axis].GetEventcutErrorFlag();
  }

  error|=fEffectiveCharge.GetEventcutErrorFlag();
  return error;
}



template<typename T>
Bool_t QwCombinedBPM<T>::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t axis=0;
  UInt_t charge_error;
  UInt_t pos_error[2];
  charge_error = 0;
  pos_error[kXAxis]=0;
  pos_error[kYAxis]=0;

  for(size_t i=0;i<fElement.size();i++){
    ///  TODO:  The returned base class should be changed so
    ///         these casts aren't needed, but "GetErrorCode"
    ///         is not meaningful for every VQwDataElement.
    ///         Maybe the return should be a VQwHardwareChannel?

    //To update the event cut faliures in individual BPM devices
    charge_error      |= fElement[i]->GetEffectiveCharge()->GetErrorCode();
    pos_error[kXAxis] |= fElement[i]->GetPosition(kXAxis)->GetErrorCode();
    pos_error[kYAxis] |= fElement[i]->GetPosition(kYAxis)->GetErrorCode();
  }

  //Event cuts for  X & Y slopes
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fSlope[axis].UpdateErrorFlag(pos_error[axis]);
    if (fSlope[axis].ApplySingleEventCuts()){ //for X slope
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Slope event cut failed ";
    }
  }

  //Event cuts for  X & Y intercepts
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fIntercept[axis].UpdateErrorFlag(pos_error[axis]);
    if (fIntercept[axis].ApplySingleEventCuts()){ //for X slope
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Intercept event cut failed ";
    }
  }


  //Event cuts for  X & Y minimum chi square
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fMinimumChiSquare[axis].UpdateErrorFlag(pos_error[axis]);
    if (fMinimumChiSquare[axis].ApplySingleEventCuts()){ //for X slope
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" X Intercept event cut failed ";
    }
  }


  //Event cuts for  X & Y positions
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fAbsPos[axis].UpdateErrorFlag(pos_error[axis]);
    if (fAbsPos[axis].ApplySingleEventCuts()){ 
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
  }

  //Event cuts for four wire sum (EffectiveCharge)
  fEffectiveCharge.UpdateErrorFlag(charge_error);
  if (fEffectiveCharge.ApplySingleEventCuts()){
      status&=kTRUE;
  }
  else{
    status&=kFALSE;
    if (bDEBUG) std::cout<<"EffectiveCharge event cut failed ";
  }

  return status;
}

template<typename T>
UInt_t QwCombinedBPM<T>::UpdateErrorFlag()
{
  Int_t axis=0;
  UInt_t charge_error;
  UInt_t pos_error[2];
  charge_error = 0;
  pos_error[kXAxis]=0;
  pos_error[kYAxis]=0;

  UInt_t error = 0;
  
  for(size_t i=0;i<fElement.size();i++){
    //To update the event cut faliures in individual BPM devices
    charge_error      |= fElement[i]->GetEffectiveCharge()->GetErrorCode();
    pos_error[kXAxis] |= fElement[i]->GetPosition(kXAxis)->GetErrorCode();
    pos_error[kYAxis] |= fElement[i]->GetPosition(kYAxis)->GetErrorCode();
  }

  //Event cuts for  X & Y slopes
  for(axis=kXAxis;axis<kNumAxes;axis++){
    fIntercept[axis].UpdateErrorFlag(pos_error[axis]);
    fSlope[axis].UpdateErrorFlag(pos_error[axis]);
    fMinimumChiSquare[axis].UpdateErrorFlag(pos_error[axis]);
    fAbsPos[axis].UpdateErrorFlag(pos_error[axis]);

    //Get the Event cut error flag for SlopeX/Y
    error|=fSlope[axis].GetEventcutErrorFlag();
    error|=fIntercept[axis].GetEventcutErrorFlag();
    error|=fMinimumChiSquare[axis].GetEventcutErrorFlag();
    error|=fAbsPos[axis].GetEventcutErrorFlag();

  }

  //Event cuts for four wire sum (EffectiveCharge)
  fEffectiveCharge.UpdateErrorFlag(charge_error);
  //Get the Event cut error flag for EffectiveCharge
  error|=fEffectiveCharge.GetEventcutErrorFlag();

  return error;
}


template<typename T>
VQwHardwareChannel* QwCombinedBPM<T>::GetSubelementByName(TString ch_name)
{
  VQwHardwareChannel* tmpptr = NULL;
  ch_name.ToLower();
  if (ch_name=="xslope"){
    tmpptr = &fSlope[kXAxis];
  }else if (ch_name=="yslope"){
    tmpptr = &fSlope[kYAxis];
  }else if (ch_name=="xintercept"){
    tmpptr = &fIntercept[kXAxis];
  }else if (ch_name=="yintercept"){
    tmpptr = &fIntercept[kYAxis];
  }else if (ch_name=="xminchisquare"){
    tmpptr = &fMinimumChiSquare[kXAxis];
  }else if (ch_name=="yminchisquare"){
    tmpptr = &fMinimumChiSquare[kYAxis];
  }else  if (ch_name=="absx" || ch_name=="x" ){
    tmpptr = &fAbsPos[kXAxis];
  }else if (ch_name=="absy" || ch_name=="y"){
    tmpptr = &fAbsPos[kYAxis];
  }else if (ch_name=="effectivecharge" || ch_name=="charge"){
    tmpptr = &fEffectiveCharge;
  } else {
    TString loc="QwCombinedBPM::GetSubelementByName for"
      + this->GetElementName() + " was passed "
      + ch_name + ", which is an unrecognized subelement name.";
    throw std::invalid_argument(loc.Data());
  }
  return tmpptr;
}

/*
template<typename T>
void QwCombinedBPM<T>::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{

  if (ch_name=="xslope"){//cuts for the x slope
    QwMessage<<"XSlope LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fSlope[kXAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="yslope"){//cuts for the y slope
    QwMessage<<"YSlope LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fSlope[kYAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="xintercept"){//cuts for the x intercept
    QwMessage<<"XIntercept LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fIntercept[kXAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="yintercept"){//cuts for the y intercept
    QwMessage<<"YIntercept LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fIntercept[kYAxis].SetSingleEventCuts(minX,maxX);

  } else  if (ch_name=="absx"){
  //cuts for the absolute x and y
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[kXAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[kYAxis].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="effectivecharge"){ //cuts for the effective charge
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(minX,maxX);
  }
}

template<typename T>
void QwCombinedBPM<T>::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability){
  errorflag|=kBPMErrorFlag;//update the device flag
  if (ch_name=="xslope"){//cuts for the x slope
    QwMessage<<"XSlope LL " <<  minX <<" UL " << maxX << " kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fSlope[kXAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="yslope"){//cuts for the y slope
    QwMessage<<"YSlope LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fSlope[kYAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="xintercept"){//cuts for the x intercept
    QwMessage<<"XIntercept LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[kXAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="yintercept"){//cuts for the y intercept
    QwMessage<<"YIntercept LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[kYAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  } else  if (ch_name=="absx"){
  //cuts for the absolute x and y
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[kXAxis].SetSingleEventCuts(errorflag, 0,0,0);
    fAbsPos[kXAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fIntercept[kYAxis].SetSingleEventCuts(errorflag, 0,0,0);
    fAbsPos[kYAxis].SetSingleEventCuts(errorflag, minX,maxX, stability);

  }else if (ch_name=="effectivecharge"){ //cuts for the effective charge
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<" kGlobalCut  "<< (errorflag&kGlobalCut)<< QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(errorflag, minX,maxX, stability);
  }
}

*/

template<typename T>
void QwCombinedBPM<T>::UpdateErrorFlag(const VQwBPM *ev_error){
  Short_t i=0;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwBPMStripline::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
        const QwCombinedBPM<T>* value_bpm = dynamic_cast<const QwCombinedBPM<T>* >(ev_error);
	for(i=kXAxis;i<kNumAxes;i++) {
	  fAbsPos[i].UpdateErrorFlag(value_bpm->fAbsPos[i]);
	  fSlope[i].UpdateErrorFlag(value_bpm->fSlope[i]);
	  fIntercept[i].UpdateErrorFlag(value_bpm->fIntercept[i]);
	  fMinimumChiSquare[i].UpdateErrorFlag(value_bpm->fMinimumChiSquare[i]);
	}
	fEffectiveCharge.UpdateErrorFlag(value_bpm->fEffectiveCharge);
      }
    } else {
      TString loc="Standard exception from QwCombinedBPM::UpdateErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }  
};



template<typename T>
void  QwCombinedBPM<T>::ProcessEvent()
{
  Bool_t ldebug = kFALSE;

  static T  tmpQADC("tmpQADC"), tmpADC("tmpADC");

  this->ClearEventData();
  //check to see if the fixed parameters are calculated
  if(!fixedParamCalculated){
    if(ldebug) std::cout<<"QwCombinedBPM:Calculating fixed parameters..\n";
    CalculateFixedParameter(fXWeights,kXAxis); //for X
    CalculateFixedParameter(fYWeights,kYAxis); //for Y
    fixedParamCalculated = kTRUE;
  }

  for(size_t i=0;i<fElement.size();i++){
    if(ldebug){
      std::cout<<"*******************************\n";
      std::cout<<" QwCombinedBPM: Reading "<<fElement[i]->GetElementName()<<" with charge weight ="<<fQWeights[i]
	       <<" and  x weight ="<<fXWeights[i]
	       <<" and  y weight ="<<fYWeights[i]<<"\n"<<std::flush;

    }
    tmpQADC.AssignValueFrom(fElement[i]->GetEffectiveCharge());
    tmpQADC.Scale(fQWeights[i]);
    fEffectiveCharge+=tmpQADC;


    if(ldebug) {
      std::cout<<"fElement[" << i << "]->GetEffectiveCharge()=="
	       << fElement[i]->GetEffectiveCharge()
	       << std::endl << std::flush;
      fElement[i]->GetEffectiveCharge()->PrintInfo();
      std::cout<<"fElement[" << i << "]->GetPosition(kXAxis)=="
	       << fElement[i]->GetPosition(kXAxis)
	       << std::endl << std::flush;
      std::cout<<"fElement[" << i << "]->GetPosition(kYAxis)=="
	       << fElement[i]->GetPosition(kYAxis)
	       << std::endl << std::flush;

      if (fElement[i]->GetEffectiveCharge()==NULL){
	std::cout<<"fElement[" << i << "]->GetEffectiveCharge returns NULL"
		 << std::endl;
      } else
	std::cout<<"got 4-wire.hw_sum = "<<fEffectiveCharge.GetValue()
		 <<" vs     actual "
		 << fElement[i]->GetEffectiveCharge()->GetValue() 
		 << std::endl << std::flush;

      
      std::cout<<"copied absolute X position hw_sum from device "
	       << fElement[i]->GetPosition(kXAxis)->GetValue() <<std::endl;
      std::cout<<"copied absolute Y position hw_sum from device "
	       << fElement[i]->GetPosition(kYAxis)->GetValue() <<std::endl;

    }
  }

  fEffectiveCharge.Scale(1.0/fSumQweights);
  //fAbsPos[0].ResetErrorFlag(0x4000000);
  //Least squares fit for X
  LeastSquareFit(kXAxis, fXWeights );

  //Least squares fit for Y
  LeastSquareFit(kYAxis, fYWeights );


  if(ldebug){
    std::cout<<" QwCombinedBPM:: Projected target X position = "<<fAbsPos[kXAxis].GetValue()
	     <<" and target X slope = "<<fSlope[kXAxis].GetValue()
	     <<" and target X intercept = "<<fIntercept[kXAxis].GetValue()
	     <<" with mimimum chi square = "<< fMinimumChiSquare[kXAxis].GetValue()
	     <<" \nProjected target Y position = "<<fAbsPos[kYAxis].GetValue()
	     <<" and target Y slope = "<<fSlope[kYAxis].GetValue()
	     <<" and target Y intercept = "<<fIntercept[kYAxis].GetValue()
	     <<" with mimimum chi square = "<< fMinimumChiSquare[kYAxis].GetValue()<<std::endl;
		 
  }


  if (ldebug) {
    //fEffectiveCharge.PrintInfo();
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++) {
      fAbsPos[axis].PrintInfo();
      fSlope[axis].PrintInfo();
      fIntercept[axis].PrintInfo();
      //fMinimumChiSquare[axis].PrintInfo();
    }
  }

  return;

 }



template<typename T>
 void QwCombinedBPM<T>::CalculateFixedParameter(std::vector<Double_t> fWeights, Int_t pos)
 {

   Bool_t ldebug = kFALSE;
   static Double_t zpos = 0.0;

   for(size_t i=0;i<fElement.size();i++){
     zpos = fElement[i]->GetPositionInZ();
     A[pos] += zpos*fWeights[i]; //zw
     B[pos] += fWeights[i]; //w
     D[pos] += zpos*zpos*fWeights[i]; //z^2w
   }

   m[pos]     = D[pos]*B[pos]-A[pos]*A[pos];
   erra[pos]  = B[pos]/m[pos];
   errb[pos]  = D[pos]/m[pos];
   covab[pos] = -A[pos]/m[pos];

  // Divvy
  if (m[pos] == 0)
    QwWarning << "Angry Divvy: Division by zero in " << this->GetElementName() << QwLog::endl;

   if(ldebug){
     std::cout<<" A = "<<A[pos]<<", B = "<<B[pos]<<", D = "<<D[pos]<<", m = "<<m[pos]<<std::endl;
     std::cout<<"For least square fit, errors are  "<<erra[pos]
	      <<"\ncovariance  = "<<covab[pos]<<"\n\n";
   }

   return;
 }


template<typename T>
 Double_t QwCombinedBPM<T>::SumOver(std::vector<Double_t> weight,std::vector <T> val)
 {
   Double_t sum = 0.0;
   if(weight.size()!=fElement.size()){
     std::cout
       <<"QwCombinedBPM:: Number of devices doesnt match the number of weights."
       <<" Exiting calculating parameters for the least squares fit"
       <<std::endl;
   }
   else{
     for(size_t i=0;i<weight.size();i++){
       val[i].Scale(weight[i]);
       sum+=val[i].GetValue();
     }
   }
   return sum;
 }

template<typename T>
 void QwCombinedBPM<T>::LeastSquareFit(VQwBPM::EBeamPositionMonitorAxis axis, std::vector<Double_t> fWeights)
 {

   /**
      REF : W.R Leo

      For Y = aX +b

      A = sigma(X * Wy)     B = sigma(Wy)    C = sigma(Y*Wy)    D = sigma(X *X * Wy)     E = sigma(X*Y*Wy)   F = sigma(Y * Y *Wy)

      then
      a = (EB-CA)/(DB-AA)      b =(DC-EA)/(DB-AA)   
   **/

   Bool_t ldebug = kFALSE;
   static Double_t zpos = 0;
   static T tmp1("tmp1","derived");
   static T tmp2("tmp2","derived");
   static T tmp3("tmp3","derived");
   static T C[kNumAxes];
   static T E[kNumAxes];

   // initialize the VQWK_Channel arrays
   C[kXAxis].InitializeChannel("cx","derived");
   C[kYAxis].InitializeChannel("cy","derived");
   E[kXAxis].InitializeChannel("ex","derived");
   E[kYAxis].InitializeChannel("ey","derived");

   C[axis].ClearEventData();
   E[axis].ClearEventData();
   for(size_t i=0;i<fElement.size();i++){
     zpos = fElement[i]->GetPositionInZ();
     tmp1.ClearEventData();
     tmp1.AssignValueFrom(fElement[i]->GetPosition(axis));
     tmp1.Scale(fWeights[i]);
     C[axis] += tmp1; //xw or yw
     tmp1.Scale(zpos);//xzw or yzw
     E[axis] += tmp1;
   }

   if(ldebug) std::cout<<"\n A ="<<A[axis]
		       <<" -- B ="<<B[axis]
		       <<" --C ="<<C[axis].GetValue()
		       <<" --D ="<<D[axis]
		       <<" --E ="<<E[axis].GetValue()<<"\n";

   // calculate the slope  a = E*erra + C*covab
   fSlope[axis].AssignScaledValue(E[axis], erra[axis]);
   tmp2.AssignScaledValue(C[axis], covab[axis]);
   fSlope[axis] += tmp2;

   // calculate the intercept  b = C*errb + E*covab
   fIntercept[axis].AssignScaledValue(C[axis], errb[axis]);
   tmp2.AssignScaledValue(E[axis], covab[axis]);
   fIntercept[axis] += tmp2;

   if(ldebug)    std::cout<<" Least Squares Fit Parameters for "<< axis
			  <<" are: \n slope = "<< fSlope[axis].GetValue()
			  <<" \n intercept = " << fIntercept[axis].GetValue()<<"\n\n";


   // absolute positions at target  using X = Za + b
   tmp1.ClearEventData();
   // Absolute position of the combined bpm is not a physical position but a derived one.


   zpos = this->GetPositionInZ();
   //UInt_t err_flag=fAbsPos[axis].GetEventcutErrorFlag();    
   fAbsPos[axis] = fIntercept[axis]; // X =  b
   //fAbsPos[axis].ResetErrorFlag(err_flag);
   tmp1.AssignScaledValue(fSlope[axis],zpos); //az
   fAbsPos[axis] += tmp1;  //X = az+b


   // to perform the minimul chi-square test
   // We want to calculte (X-az-b)^2 for each bpm in the combination and sum over the values
   tmp3.ClearEventData();
   fMinimumChiSquare[axis].ClearEventData();

   for(size_t i=0;i<fElement.size();i++){
     tmp1.ClearEventData();
     tmp2.ClearEventData();
     //std::cout<<"\nName -------- ="<<(fElement[i]->GetElementName())<<std::endl;
     //std::cout<<"\nRead value ="<<(fElement[i]->GetPosition(axis))->GetValue()<<std::endl;

     tmp1.AssignValueFrom(fElement[i]->GetPosition(axis)); // = X
     //std::cout<<"Read value ="<<tmp1.GetValue()<<std::endl;

     tmp2.AssignScaledValue(fSlope[axis],fElement[i]->GetPositionInZ());
     tmp2+=fIntercept[axis];
     //std::cout<<"Calculated abs value ="<<tmp2.GetValue()<<std::endl;

     tmp1 -= tmp2;      // = X-Za-b
     //std::cout<<"Read-calculated ="<<tmp1.GetValue()<<std::endl;
     tmp1.Product(tmp1,tmp1); // = (X-Za-b)^2
     //std::cout<<"(Read-calculated)^2 ="<<tmp1.GetValue()<<std::endl;
     tmp1.Scale(fWeights[i]*fWeights[i]); // = [(X-Za-b)^2]W
     //std::cout<<"(Read-calculated)^2/weight ="<<tmp1.GetValue()<<std::endl;
     tmp3+=tmp1; //sum over
     //std::cout<<"Sum (Read-calculated)^2/weight +="<<tmp3.GetValue()<<std::endl;
   }

   if (fElement.size()>2){
     fMinimumChiSquare[axis].AssignScaledValue(tmp3,1.0/(fElement.size()-2)); //minimul chi-square
   } else {
     fMinimumChiSquare[axis].AssignScaledValue(tmp3,0.0);
   }

   //std::cout << "1.0/fElement.size() = " << 1.0/fElement.size() << std::endl;
   
   return;
 }

template<typename T>
 Int_t QwCombinedBPM<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
 {
   return word_position_in_buffer;
 }

//-----------------------------------------------------------------------------------------------------------

template<typename T>
void QwCombinedBPM<T>::GetProjectedPosition(VQwBPM *device)
 {
   //  For now, go ahead and do the projection for any BPM, not just for ones which are used by this
   //  CombinedBPM object.

   EBeamPositionMonitorAxis axis;

   for(Short_t iaxis=kXAxis; iaxis<kNumAxes; iaxis++) {
      //std::cout << "iaxis= " << iaxis << std::endl;
      //std::cout << "kNumAxes= " << kNumAxes << std::endl;
      if (iaxis==kXAxis) {
	axis=kXAxis;
      //std::cout << "kXAxis= " << kXAxis << std::endl;
      //std::cout << "axis= " << axis << std::endl;
	}
      else if (iaxis==kYAxis) {
	axis=kYAxis;
      //std::cout << "kYAxis= " << kYAxis << std::endl;
      //std::cout << "axis= " << axis << std::endl;
	}
      else continue;
      (device->GetPosition(axis))->ClearEventData();
      (device->GetPosition(axis))->AssignScaledValue(fSlope[axis],device->GetPositionInZ());
      (device->GetPosition(axis))->operator+=(&fIntercept[axis]);
   }
   // Maybe we should apply resolution smearing to the stripline BPMs?
   // device->PrintInfo();
   device->ApplyResolutionSmearing();
    //device->PrintInfo();
   device->FillRawEventData();
    //std::cout << "Device " << device->GetElementName() << " X = " << std::setprecision(15) << device->GetPosition(kXAxis)->GetValue() 
    //<< "\t Y = " << std::setprecision(15) << device->GetPosition(kYAxis)->GetValue() << std::endl;
}

//-----------------------------------------------------------------------------------------------------------


template<typename T>
void QwCombinedBPM<T>::PrintValue() const
{
   Short_t axis;

   for(axis = kXAxis; axis < kNumAxes; axis++){
     fAbsPos[axis].PrintValue();
   }
   ///  TODO:  To print the Z position, we need to use GetPositionInZ()
   for(axis = kXAxis; axis < kNumAxes; axis++) {
     fSlope[axis].PrintValue();
     fIntercept[axis].PrintValue();
     fMinimumChiSquare[axis].PrintValue();
   }
   fEffectiveCharge.PrintValue();

   return;
 }


template<typename T>
void QwCombinedBPM<T>::PrintInfo() const
{

  Short_t axis;

  for(axis = kXAxis; axis < kNumAxes; axis++){
    fAbsPos[axis].PrintInfo();
  }
  ///  TODO:  To print the Z position, we need to use GetPositionInZ()
  for(axis = kXAxis; axis < kNumAxes; axis++) {
    fSlope[axis].PrintInfo();
    fIntercept[axis].PrintInfo();
    fMinimumChiSquare[axis].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();

  return;
}


template<typename T>
VQwBPM& QwCombinedBPM<T>::operator= (const VQwBPM &value)
{
  *(dynamic_cast<QwCombinedBPM<T>*>(this))=
    *(dynamic_cast<const QwCombinedBPM<T>*>(&value));
  return *this;
}

template<typename T>
QwCombinedBPM<T>& QwCombinedBPM<T>::operator= (const QwCombinedBPM<T> &value)
{
  VQwBPM::operator= (value);
  if (this->GetElementName()!=""){
    this->fEffectiveCharge=value.fEffectiveCharge;
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
      this->fSlope[axis]=value.fSlope[axis];
      this->fIntercept[axis] = value.fIntercept[axis];
      this->fAbsPos[axis]=value.fAbsPos[axis];
      this->fMinimumChiSquare[axis]=value.fMinimumChiSquare[axis];
    }
  }
  return *this;
}


template<typename T>
VQwBPM& QwCombinedBPM<T>::operator+= (const VQwBPM &value)
{
  *(dynamic_cast<QwCombinedBPM<T>*>(this))+=
    *(dynamic_cast<const QwCombinedBPM<T>*>(&value));
  return *this;
}

template<typename T>
QwCombinedBPM<T>& QwCombinedBPM<T>::operator+= (const QwCombinedBPM<T> &value)
{

     if (this->GetElementName()!=""){
       this->fEffectiveCharge+=value.fEffectiveCharge;
       for(Short_t axis=kXAxis;axis<kNumAxes;axis++) 	{
	 this->fSlope[axis]+=value.fSlope[axis];
	 this->fIntercept[axis]+=value.fIntercept[axis];
	 this->fAbsPos[axis]+=value.fAbsPos[axis];
	 this->fMinimumChiSquare[axis]+=value.fMinimumChiSquare[axis];
       }
     }
     return *this;
}

template<typename T>
VQwBPM& QwCombinedBPM<T>::operator-= (const VQwBPM &value)
{
  *(dynamic_cast<QwCombinedBPM<T>*>(this))-=
    *(dynamic_cast<const QwCombinedBPM<T>*>(&value));
  return *this;
}


template<typename T>
QwCombinedBPM<T>& QwCombinedBPM<T>::operator-= (const QwCombinedBPM<T> &value)
{

  if (this->GetElementName()!=""){
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
      this->fSlope[axis]-=value.fSlope[axis];
      this->fIntercept[axis]-=value.fIntercept[axis];
      this->fAbsPos[axis]-=value.fAbsPos[axis];
      this->fMinimumChiSquare[axis]-=value.fMinimumChiSquare[axis];
    }
  }
  return *this;
}


template<typename T>
void QwCombinedBPM<T>::Ratio(VQwBPM &numer, VQwBPM &denom)
{
  Ratio(*dynamic_cast<QwCombinedBPM<T>*>(&numer),
      *dynamic_cast<QwCombinedBPM<T>*>(&denom));
}

template<typename T>
void QwCombinedBPM<T>::Ratio(QwCombinedBPM<T> &numer,
    QwCombinedBPM<T> &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // combined bpm is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  if (this->GetElementName()!=""){
    //    The slope, intercept and absolute positions should all be differences, not asymmetries.
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++) {
      this->fSlope[axis]     = numer.fSlope[axis];
      this->fIntercept[axis] = numer.fIntercept[axis];
      this->fAbsPos[axis]    = numer.fAbsPos[axis];
      this->fMinimumChiSquare[axis]    = numer.fMinimumChiSquare[axis];
    }
  }

  return;
}


template<typename T>
void QwCombinedBPM<T>::Scale(Double_t factor)
{
  fEffectiveCharge.Scale(factor);
  for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fSlope[axis].Scale(factor);
    fIntercept[axis].Scale(factor);
    fAbsPos[axis].Scale(factor);
    fMinimumChiSquare[axis].Scale(factor);
  }
  return;
}

template<typename T>
void QwCombinedBPM<T>::CalculateRunningAverage()
{
  fEffectiveCharge.CalculateRunningAverage();

  for (Short_t axis = kXAxis; axis < kNumAxes; axis++) {
    fSlope[axis].CalculateRunningAverage();
    fIntercept[axis].CalculateRunningAverage();
    fAbsPos[axis].CalculateRunningAverage();
    fMinimumChiSquare[axis].CalculateRunningAverage();
  }
}


template<typename T>
void QwCombinedBPM<T>::AccumulateRunningSum(const VQwBPM& value, Int_t count, Int_t ErrorMask)
{
  AccumulateRunningSum(*dynamic_cast<const QwCombinedBPM<T>* >(&value), count, ErrorMask);
}

template<typename T>
void QwCombinedBPM<T>::AccumulateRunningSum(const QwCombinedBPM<T>& value, Int_t count, Int_t ErrorMask)
{
  for (Short_t axis = kXAxis; axis < kNumAxes; axis++){
    fSlope[axis].AccumulateRunningSum(value.fSlope[axis], count, ErrorMask);
    fIntercept[axis].AccumulateRunningSum(value.fIntercept[axis], count, ErrorMask);
    fAbsPos[axis].AccumulateRunningSum(value.fAbsPos[axis], count, ErrorMask);
    fMinimumChiSquare[axis].AccumulateRunningSum(value.fMinimumChiSquare[axis], count, ErrorMask);
  }
  fEffectiveCharge.AccumulateRunningSum(value.fEffectiveCharge, count, ErrorMask);
}

template<typename T>
void QwCombinedBPM<T>::DeaccumulateRunningSum(VQwBPM& value, Int_t ErrorMask)
{
  DeaccumulateRunningSum(*dynamic_cast<QwCombinedBPM<T>* >(&value), ErrorMask);
}

template<typename T>
void QwCombinedBPM<T>::DeaccumulateRunningSum(QwCombinedBPM<T>& value, Int_t ErrorMask)
{

  for (Short_t axis = kXAxis; axis < kNumAxes; axis++){
    fSlope[axis].DeaccumulateRunningSum(value.fSlope[axis], ErrorMask);
    fIntercept[axis].DeaccumulateRunningSum(value.fIntercept[axis], ErrorMask);
    fAbsPos[axis].DeaccumulateRunningSum(value.fAbsPos[axis], ErrorMask);
    fMinimumChiSquare[axis].DeaccumulateRunningSum(value.fMinimumChiSquare[axis], ErrorMask);
  }
  fEffectiveCharge.DeaccumulateRunningSum(value.fEffectiveCharge, ErrorMask);

}





template<typename T>
void  QwCombinedBPM<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    //we calculate the asym_ for the fEffectiveCharge becasue its an asymmetry and not a difference.
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;
    if(prefix.Contains("asym_"))
      thisprefix.ReplaceAll("asym_","diff_");
    this->SetRootSaveStatus(prefix);

    for(Short_t axis=kXAxis;axis<kNumAxes;axis++) {
	fSlope[axis].ConstructHistograms(folder, thisprefix);
	fIntercept[axis].ConstructHistograms(folder, thisprefix);
	fAbsPos[axis].ConstructHistograms(folder, thisprefix);
	fMinimumChiSquare[axis].ConstructHistograms(folder, thisprefix);
    }

  }
  return;
}

template<typename T>
void  QwCombinedBPM<T>::FillHistograms()
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fEffectiveCharge.FillHistograms();
    for(Short_t axis=kXAxis;axis<kNumAxes;axis++) {
      fSlope[axis].FillHistograms();
      fIntercept[axis].FillHistograms();
      fAbsPos[axis].FillHistograms();
      fMinimumChiSquare[axis].FillHistograms();
    }
  }
  return;
}

template<typename T>
void  QwCombinedBPM<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {

      TString thisprefix=prefix;
      if(prefix.Contains("asym_"))
	thisprefix.ReplaceAll("asym_","diff_");

      this->SetRootSaveStatus(prefix);

      fEffectiveCharge.ConstructBranchAndVector(tree,prefix,values);
      for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
	fSlope[axis].ConstructBranchAndVector(tree,thisprefix,values);
	fIntercept[axis].ConstructBranchAndVector(tree,thisprefix,values);
	fAbsPos[axis].ConstructBranchAndVector(tree,thisprefix,values);
	fMinimumChiSquare[axis].ConstructBranchAndVector(tree,thisprefix,values);
      }

    }

  return;
}

template<typename T>
void  QwCombinedBPM<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      TString thisprefix=prefix;
      if(prefix.Contains("asym_"))
	thisprefix.ReplaceAll("asym_","diff_");


      fEffectiveCharge.ConstructBranch(tree,prefix);

      for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
	fSlope[axis].ConstructBranch(tree,thisprefix);
	fIntercept[axis].ConstructBranch(tree,thisprefix);
	fAbsPos[axis].ConstructBranch(tree,thisprefix);
	fMinimumChiSquare[axis].ConstructBranch(tree,thisprefix);
      }

    }
  return;
}

template<typename T>
void  QwCombinedBPM<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;
  devicename=this->GetElementName();
  devicename.ToLower();

  if (this->GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      if (modulelist.HasValue(devicename)){
	TString thisprefix=prefix;
	if(prefix.Contains("asym_"))
	  thisprefix.ReplaceAll("asym_","diff_");


	fEffectiveCharge.ConstructBranch(tree,prefix);

	for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
	  fSlope[axis].ConstructBranch(tree,thisprefix);
	  fIntercept[axis].ConstructBranch(tree,thisprefix);
	  fAbsPos[axis].ConstructBranch(tree,thisprefix);
	  fMinimumChiSquare[axis].ConstructBranch(tree,thisprefix);
	}
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
    }

    }
  return;
}


template<typename T>
void  QwCombinedBPM<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the tree.
  }
  else{
    fEffectiveCharge.FillTreeVector(values);

    for(Short_t axis=kXAxis;axis<kNumAxes;axis++){
      fSlope[axis].FillTreeVector(values);
      fIntercept[axis].FillTreeVector(values);
      fAbsPos[axis].FillTreeVector(values);
      fMinimumChiSquare[axis].FillTreeVector(values);
    }
  }
  return;
}

template<typename T>
void QwCombinedBPM<T>::SetEventCutMode(Int_t bcuts)
{

  //  bEVENTCUTMODE=bcuts;
  for (Short_t axis=kXAxis;axis<kNumAxes;axis++){
    fSlope[axis].SetEventCutMode(bcuts);
    fIntercept[axis].SetEventCutMode(bcuts);
    fAbsPos[axis].SetEventCutMode(bcuts);
    fMinimumChiSquare[axis].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);

  return;
}


template<typename T>
void QwCombinedBPM<T>::MakeBPMComboList()
{
  for (size_t axis = kXAxis; axis < kNumAxes; axis++) {
    T abspos(fAbsPos[axis]);
    abspos = fAbsPos[axis];
    fBPMComboElementList.push_back(abspos);
    T slope(fSlope[axis]);
    slope = fSlope[axis];
    fBPMComboElementList.push_back(slope);
    T intercept(fIntercept[axis]);
    intercept = fIntercept[axis];
    fBPMComboElementList.push_back(intercept);
    T minimumchisquare(fMinimumChiSquare[axis]);
    minimumchisquare = fMinimumChiSquare[axis];
    fBPMComboElementList.push_back(minimumchisquare);
  }
  T effectivecharge(fEffectiveCharge);
  effectivecharge = fEffectiveCharge;
  fBPMComboElementList.push_back(effectivecharge);
}

//-----------------------------------------------------------------------

template<typename T>
void QwCombinedBPM<T>::RandomizeEventData(int helicity, double time)
{
  static Double_t zpos = 0;
  static T tmp1("tmp1","derived");
  // Randomize the abs position and angle.
  for (size_t axis=kXAxis; axis<kNumAxes; axis++) 
  {
    //std::cout << "In QwCombinedBPM::RandomizeEventData-> Slope(before): " << fSlope[axis].GetValue() << std::endl;
    //std::cout << "BEFORE RANDOMIZING POS & SLOPE: " << std::endl;
    //fSlope[axis].PrintInfo();
    //fAbsPos[axis].PrintInfo();

    fAbsPos[axis].RandomizeEventData(helicity, time);
    fSlope[axis].RandomizeEventData(helicity, time);

    //std::cout << "AFTER RANDOMIZING POS & SLOPE: " << std::endl;
    //fSlope[axis].PrintInfo();
    //fAbsPos[axis].PrintInfo();
    //fSlope[axis].PrintValue();
    //std::cout << "In QwCombinedBPM::RandomizeEventData-> Slope(after): " << fSlope[axis].GetValue() << std::endl;

    zpos = this->GetPositionInZ();
    //std::cout << "In QwCombinedBPM: zpos= " << zpos << std::endl;
    //UInt_t err_flag=fAbsPos[axis].GetEventcutErrorFlag();    
    fIntercept[axis] = fAbsPos[axis]; // b =  X
    //fAbsPos[axis].ResetErrorFlag(err_flag);
    tmp1.AssignScaledValue(fSlope[axis],zpos); //az
    fIntercept[axis] -= tmp1;  //b = X - az
    //    std::cout << axis << " " << fAbsPos[axis].GetValue() << "-" << fSlope[axis].GetValue() <<"*"<<zpos <<"=?" << fIntercept[axis].GetValue() << std::endl;
   }
 return;
}


template<typename T>
void QwCombinedBPM<T>::SetMockDataAsDiff() {
  this->SetMockDataAsDiff();
}


template<typename T>
void QwCombinedBPM<T>::LoadMockDataParameters(QwParameterFile &paramfile){
/*
  Bool_t   ldebug=kFALSE;
  Double_t meanX=0.0, sigmaX=0.0, meanY=0.0, sigmaY=0.0;
  Double_t meanXslope=0.0, sigmaXslope=0.0, meanYslope=0.0, sigmaYslope=0.0;
*/
  Double_t xres=0.0, yres=0.0; //  Temporary variables for the resoluton.

  if (paramfile.GetLine().find("resolution")!=std::string::npos){
    paramfile.GetNextToken();
    xres = paramfile.GetTypedNextToken<Double_t>();
    yres = paramfile.GetTypedNextToken<Double_t>();
    this->SetResolution(xres, yres);
  } else {  
    //  If we have asym, mean, sigma for each coorindate, then we can do:

    TString value = paramfile.GetNextToken();
    if (value=="xpos") {
      fAbsPos[kXAxis].SetMockDataAsDiff();
      fAbsPos[kXAxis].LoadMockDataParameters(paramfile);
    }
    else if (value=="ypos") {
      fAbsPos[kYAxis].SetMockDataAsDiff();
      fAbsPos[kYAxis].LoadMockDataParameters(paramfile);
    }
    else if (value=="xslope") {
      fSlope[kXAxis].SetMockDataAsDiff();
      fSlope[kXAxis].LoadMockDataParameters(paramfile);
    }
    else if (value=="yslope") {
      fSlope[kYAxis].SetMockDataAsDiff();
      fSlope[kYAxis].LoadMockDataParameters(paramfile);
    }
/*
    for(size_t i=kXAxis;i<kNumAxes;i++){
      //std::cout << "In QwCombinedBPM: ChannelName = " << GetElementName() << std::endl;
      fAbsPos[i].SetMockDataAsDiff();
      fAbsPos[i].LoadMockDataParameters(paramfile);
    }
    for(size_t i=kXAxis;i<kNumAxes;i++){
      //std::cout << "In QwCombinedBPM: ChannelName = " << GetElementName() << std::endl;
      fSlope[i].SetMockDataAsDiff();
      fSlope[i].LoadMockDataParameters(paramfile);
    }
*/
//  and so on....
/*
    meanX       = paramfile.GetTypedNextToken<Double_t>();
    sigmaX      = paramfile.GetTypedNextToken<Double_t>();
    meanY       = paramfile.GetTypedNextToken<Double_t>();
    sigmaY      = paramfile.GetTypedNextToken<Double_t>();
    meanXslope  = paramfile.GetTypedNextToken<Double_t>();
    sigmaXslope = paramfile.GetTypedNextToken<Double_t>();
    meanYslope  = paramfile.GetTypedNextToken<Double_t>();
    sigmaYslope = paramfile.GetTypedNextToken<Double_t>();

    if (ldebug==1) {
      std::cout << "#################### \n";
      std::cout << "meanX, sigmaX, meanY, sigmaY, meanXslope, sigmaXslope, meanYslope, sigmaYslope \n" << std::endl;
      std::cout << meanX                            << " / "
	        << sigmaX                           << " / "
	        << meanY                            << " / "
	        << sigmaY                           << " / "
	        << meanXslope                       << " / "
                << sigmaXslope                      << " / "
	        << meanYslope                       << " / "
	        << sigmaYslope                      << " / "
	        << std::endl;
    }
    this->SetRandomEventParameters(meanX, sigmaX, meanY, sigmaY, meanXslope, sigmaXslope, meanYslope, sigmaYslope);
*/
  }
}


template<typename T>
void QwCombinedBPM<T>::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY,
                                   Double_t meanXslope, Double_t sigmaXslope, Double_t meanYslope, Double_t sigmaYslope)
{
/*
  fAbsPos[kXAxis].SetMockDataAsDiff();
  fAbsPos[kYAxis].SetMockDataAsDiff();
  fSlope[kXAxis].SetMockDataAsDiff();
  fSlope[kYAxis].SetMockDataAsDiff();
*/
  fAbsPos[kXAxis].SetRandomEventParameters(meanX, sigmaX);
  fAbsPos[kYAxis].SetRandomEventParameters(meanY, sigmaY);
  fSlope[kXAxis].SetRandomEventParameters(meanXslope, sigmaXslope);
  fSlope[kYAxis].SetRandomEventParameters(meanYslope, sigmaYslope);

 return;
}

//-----------------------------------------------------------------------


#ifdef __USE_DATABASE__
template<typename T>
std::vector<QwDBInterface> QwCombinedBPM<T>::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  for(size_t axis=kXAxis;axis<kNumAxes;axis++) {
    fAbsPos[axis].AddEntriesToList(row_list);
    fSlope[axis].AddEntriesToList(row_list);
    fIntercept[axis].AddEntriesToList(row_list);
    fMinimumChiSquare[axis].AddEntriesToList(row_list);
  }
  fEffectiveCharge.AddEntriesToList(row_list);
  return row_list;
}



template<typename T>
std::vector<QwErrDBInterface> QwCombinedBPM<T>::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  for(size_t axis=kXAxis;axis<kNumAxes;axis++) {
    fAbsPos[axis].AddErrEntriesToList(row_list);
    fSlope[axis].AddErrEntriesToList(row_list);
    fIntercept[axis].AddErrEntriesToList(row_list);
    fMinimumChiSquare[axis].AddErrEntriesToList(row_list);
  }
  //  fEffectiveCharge.AddErrEntriesToList(row_list);
  return row_list;
}





#endif // __USE_DATABASE__

template class QwCombinedBPM<QwVQWK_Channel>; 
template class QwCombinedBPM<QwSIS3801_Channel>; 
template class QwCombinedBPM<QwSIS3801D24_Channel>;
template class QwCombinedBPM<QwMollerADC_Channel>;