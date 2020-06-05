/**********************************************************\
* File: QwLinearDiodeArray.cc                                          *
*                                                         *
* Author: B. Waidyawansa                                  *
* Time-stamp:  09-14-2010                                 *
\**********************************************************/

#include "QwLinearDiodeArray.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwParameterFile.h"
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif // __USE_DATABASE__

const size_t QwLinearDiodeArray::kMaxElements = 8;

/*Individual pads of the linear array*/
const TString QwLinearDiodeArray::subelement[8]={"p1","p2","p3","p4","p5","p6","p7","p8"};

/* Pad size in mm*/
const Double_t QwLinearDiodeArray::kQwLinearDiodeArrayPadSize = 1.57;

void  QwLinearDiodeArray::InitializeChannel(TString name)
{

  Int_t i=0;
  Bool_t localdebug = kFALSE;
  VQwBPM::InitializeChannel(name);

  for(i=0;i<8;i++) {
    fPhotodiode[i].InitializeChannel(name+subelement[i],"raw");
    
    if(localdebug)
      std::cout<<" photodiode ["<<i<<"]="<<fPhotodiode[i].GetElementName()<<"\n";
  }
  

  fEffectiveCharge.InitializeChannel(name+"WS","derived");

  //  We don't initialize the photodiode channels yet.
  fRelPos[0].InitializeChannel(name+"RelMean","derived");
  fRelPos[1].InitializeChannel(name+"RelVariance","derived");

  bFullSave=kTRUE;

  return;
}

void  QwLinearDiodeArray::InitializeChannel(TString subsystem, TString name)
{

  Int_t i=0;
  Bool_t localdebug = kFALSE;

  VQwBPM::InitializeChannel(name);

  for(i=0;i<8;i++) {
    fPhotodiode[i].InitializeChannel(subsystem, "QwLinearDiodeArray", name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" photodiode ["<<i<<"]="<<fPhotodiode[i].GetElementName()<<"\n";
  }

  fEffectiveCharge.InitializeChannel(subsystem, "QwLinearDiodeArray", name+"WS","derived");

  //  We don't initialize the photodiode channels yet.

  fRelPos[0].InitializeChannel(subsystem, "QwLinearDiodeArray", name+"RelMean","derived");
  fRelPos[1].InitializeChannel(subsystem, "QwLinearDiodeArray", name+"RelVariance","derived");

  bFullSave=kTRUE;

  return;
}

void QwLinearDiodeArray::ClearEventData()
{
  size_t i=0;

  for(i=0;i<8;i++) fPhotodiode[i].ClearEventData();

  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].ClearEventData();
  }
  fEffectiveCharge.ClearEventData();

 return;
}


Bool_t QwLinearDiodeArray::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=0;
  for(size_t i=0;i<8;i++)
    {
      deviceerror|= fPhotodiode[i].ApplyHWChecks();  //OR the error code from each wire
      eventokay &= (deviceerror & 0x0);//AND with 0 since zero means HW is good.

      if (bDEBUG) std::cout<<" Inconsistent within LinearArray terminals photodiode[ "<<i<<" ] "<<std::endl;
      if (bDEBUG) std::cout<<" photodiode[ "<<i<<" ] sequence num "<<fPhotodiode[i].GetSequenceNumber()<<" sample size "<<fPhotodiode[i].GetNumberOfSamples()<<std::endl;
    }

  return eventokay;
}

void QwLinearDiodeArray::IncrementErrorCounters()
{
  size_t i=0;
  for(i=0;i<8;i++) fPhotodiode[i].IncrementErrorCounters();
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].IncrementErrorCounters();
  }
  fEffectiveCharge.IncrementErrorCounters();
}

void QwLinearDiodeArray::PrintErrorCounters() const
{
  size_t i=0;
  for(i=0;i<8;i++) fPhotodiode[i].PrintErrorCounters();
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].PrintErrorCounters();
  }
  fEffectiveCharge.PrintErrorCounters();
}

UInt_t QwLinearDiodeArray::GetEventcutErrorFlag()
{
  size_t i=0;
  UInt_t error=0;
  for(i=0;i<8;i++) error|=fPhotodiode[i].GetEventcutErrorFlag();
  for(i=kXAxis;i<kNumAxes;i++) {
    error|=fRelPos[i].GetEventcutErrorFlag();
  }
  error|=fEffectiveCharge.GetEventcutErrorFlag();

  return error;
}

UInt_t QwLinearDiodeArray::UpdateErrorFlag()
{
  size_t i=0;
  UInt_t error1=0;
  UInt_t error2=0;  
  for(i=0;i<8;i++){
    error1|=fPhotodiode[i].GetErrorCode();
    error2|=fPhotodiode[i].GetEventcutErrorFlag();
  }
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].UpdateErrorFlag(error1);
    error2|=fRelPos[i].GetEventcutErrorFlag();
  }
  fEffectiveCharge.UpdateErrorFlag(error1);
  error2|=fEffectiveCharge.GetEventcutErrorFlag();
  return error2;
}

Bool_t QwLinearDiodeArray::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  size_t i=0;
  UInt_t error_code = 0;
  //Event cuts for four wires
  for(i=0;i<8;i++){
    if (fPhotodiode[i].ApplySingleEventCuts()){ 
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" array ["<<i<<"] event cut failed ";
    }
    //Get the Event cut error flag for wires
    error_code|=fPhotodiode[i].GetErrorCode();
  }

   //Event cuts for Relative X & Y
  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].UpdateErrorFlag(error_code);//To update the event cut failed error code from the channels/wires error codes
    if (fRelPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }
  }

 //Event cuts for four wire sum (EffectiveCharge)
  fEffectiveCharge.UpdateErrorFlag(error_code);//To update the eff-charge error code from the channels/wires event cut error codes
  if (fEffectiveCharge.ApplySingleEventCuts()){
      status&=kTRUE;
  }
  else{
    status&=kFALSE;
    if (bDEBUG) std::cout<<"EffectiveCharge event cut failed ";
  }
  return status;
}


VQwHardwareChannel* QwLinearDiodeArray::GetSubelementByName(TString ch_name)
{
  VQwHardwareChannel* tmpptr = NULL;
  ch_name.ToLower();
  if (ch_name=="relx"){
    tmpptr = &fRelPos[0];
  }else if (ch_name=="rely"){
    tmpptr = &fRelPos[1];
  }else if (ch_name=="absx" || ch_name=="x" ){
    tmpptr = &fAbsPos[0];
  }else if (ch_name=="absy" || ch_name=="y"){
    tmpptr = &fAbsPos[1];
  }else if (ch_name=="effectivecharge" || ch_name=="charge"){
    tmpptr = &fEffectiveCharge;
  } else {
    TString loc="QwLinearDiodeArray::GetSubelementByName for"
      + this->GetElementName() + " was passed "
      + ch_name + ", which is an unrecognized subelement name.";
    throw std::invalid_argument(loc.Data());
  }
  return tmpptr;
}

/*
void QwLinearDiodeArray::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{
  QwWarning << "QwLinearDiodeArray::SetSingleEventCuts:  "
	    << "Does not do anything yet." << QwLog::endl;
}*/

void QwLinearDiodeArray::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability, Double_t burplevel){
  errorflag|=kBPMErrorFlag;//update the device flag (Do not have a error flag yet)
  //  QwWarning << "QwLinearDiodeArray::SetSingleEventCuts:  " << "Does not do anything yet." << QwLog::endl;
  if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[0].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel);

  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel);

  } else if (ch_name=="effectivecharge"){
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel);

  } else if (ch_name=="photodiode"){
    QwMessage<<"photodiode LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[1].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel);
  }
}

Bool_t QwLinearDiodeArray::CheckForBurpFail(const VQwDataElement *ev_error){
  Short_t i=0;
  Bool_t burpstatus = kFALSE;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      //std::cout<<" Here in QwLinearDiodeArray::CheckForBurpFail \n";
      if (this->GetElementName()!="") {
        const QwLinearDiodeArray* value_lin = dynamic_cast<const QwLinearDiodeArray* >(ev_error);
        for(i=kXAxis;i<kNumAxes;i++) {
          burpstatus |= fRelPos[i].CheckForBurpFail(&(value_lin->fRelPos[i]));
        }
        for(i=0;i<8;i++){
          burpstatus |= fPhotodiode[i].CheckForBurpFail(&(value_lin->fPhotodiode[i]));
        }
        burpstatus |= fEffectiveCharge.CheckForBurpFail(&(value_lin->fEffectiveCharge)); 
      }
    } else {
      TString loc="Standard exception from QwLinearDiodeArray::CheckForBurpFail :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }
  return burpstatus;
};

void QwLinearDiodeArray::UpdateErrorFlag(const VQwBPM *ev_error){
  Short_t i=0;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwQPD::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
        const QwLinearDiodeArray* value_bpm = dynamic_cast<const QwLinearDiodeArray* >(ev_error);
	for(i=0;i<4;i++){
	  fPhotodiode[i].UpdateErrorFlag(value_bpm->fPhotodiode[i]);
	}
	for(i=kXAxis;i<kNumAxes;i++) {
	  fRelPos[i].UpdateErrorFlag(value_bpm->fRelPos[i]);
	}
	fEffectiveCharge.UpdateErrorFlag(value_bpm->fEffectiveCharge);
      }
    } else {
      TString loc="Standard exception from QwLinearDiodeArray::UpdateErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }  
};

void  QwLinearDiodeArray::ProcessEvent()
{
  Bool_t localdebug = kFALSE;
  static QwVQWK_Channel mean, meansqr;
  static QwVQWK_Channel tmp("tmp");
  static QwVQWK_Channel tmp2("tmp2");

  mean.InitializeChannel("mean","raw");
  meansqr.InitializeChannel("meansqr","raw");


  size_t i = 0;


  ApplyHWChecks();
  //first apply HW checks and update HW  error flags. 
  // Calling this routine here and not in ApplySingleEventCuts  
  //makes a difference for a LinearArrays because they have derrived devices.

  fEffectiveCharge.ClearEventData();
  for(i=0;i<8;i++){
    fPhotodiode[i].ProcessEvent();
    fEffectiveCharge+=fPhotodiode[i];
  }
  
  
  //  First calculate the mean pad position and mean of squared pad position
  //  with respect to the center of the array, in units of pad spacing.
  mean.ClearEventData();
  meansqr.ClearEventData();
  for (i=0;i<8;i++){
    Double_t pos = kQwLinearDiodeArrayPadSize*i*0.5;
    tmp = fPhotodiode[i];
    tmp.Scale(pos);  // Scale for S(i)*pos
    mean+=tmp;
    tmp.Scale(pos);  // Scale again for S(i)*(pos**2)
    meansqr+=tmp;
  }
  fRelPos[0].Ratio(mean,fEffectiveCharge);
  tmp = meansqr;
  meansqr.Ratio(tmp,fEffectiveCharge);
  tmp2.Product(fRelPos[0], fRelPos[0]);

  //  Now calculate the variance
  fRelPos[1].Difference(meansqr,tmp2);

  if(localdebug){
    std::cout<<"\n#################"<<std::endl;
    std::cout<<" LinearArray name="<<fElementName<<std::endl;
    std::cout<<" Size of the linear array = "<<8<<std::endl;
    std::cout<<" event number= "<<fPhotodiode[0].GetSequenceNumber()<<std::endl;
    for(Int_t i = 0; i<8; i++)
      std::cout<<" pad"<<i<<" ="<<fPhotodiode[i].GetValue()<<std::endl;
    std::cout<<" mean ="<<fRelPos[0].GetValue()<<std::endl;
    std::cout<<" varaiance ="<<fRelPos[1].GetValue()<<std::endl;
    std::cout<<" total charge ="<<fEffectiveCharge.GetValue()<<std::endl;

  }
  
  return;
}


Int_t QwLinearDiodeArray::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<8)
    {
      fPhotodiode[index].ProcessEvBuffer(buffer,word_position_in_buffer);
    }
  else
    {
    std::cerr <<
      "QwLinearDiodeArray::ProcessEvBuffer(): attemp to fill in raw data for a pad that doesn't exist \n";
    }
  return word_position_in_buffer;
}



void QwLinearDiodeArray::PrintValue() const
{
  for (size_t i = 0; i < 2; i++) {
    fRelPos[i].PrintValue();
  }
  return;
}

void QwLinearDiodeArray::PrintInfo() const
{
  size_t i = 0;
  for (i = 0; i < 8; i++) fPhotodiode[i].PrintInfo();
  for (i = 0; i < 2; i++) {
    fAbsPos[i].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();
}


TString QwLinearDiodeArray::GetSubElementName(Int_t subindex)
{
  TString thisname;
  size_t localindex=kInvalidSubelementIndex;
  if (subindex>-1) localindex = subindex;

  if(localindex<8)
    thisname=fPhotodiode[subindex].GetElementName();
  else
    std::cerr<< "QwLinearDiodeArray::GetSubElementName for "
	     << GetElementName()<<" this subindex, "
	     << subindex << ", doesn't exist \n";

  return thisname;
}

UInt_t QwLinearDiodeArray::GetSubElementIndex(TString subname)
{
  size_t localindex=kInvalidSubelementIndex;
  TString padindex;

  padindex = subname(subname.Sizeof()-2,1);
  //  Interpret the subname as the pad index.
  if (padindex.IsDigit()){
    Int_t tmpval = padindex.Atoi();
    if (tmpval>-1) localindex = tmpval-1;
  }

  // localindex is unsigned int and always positive
  if (localindex > kMaxElements){
    std::cerr << "QwLinearDiodeArray::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;
    localindex=kInvalidSubelementIndex;
  }
  return localindex;
}

void  QwLinearDiodeArray::GetAbsolutePosition()
{
  for(size_t i=kXAxis;i<kNumAxes;i++){
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
  }

}


VQwBPM& QwLinearDiodeArray::operator= (const VQwBPM &value)
{
  *(dynamic_cast<QwLinearDiodeArray*>(this)) =
      *(dynamic_cast<const QwLinearDiodeArray*>(&value));
  return *this;
}

QwLinearDiodeArray& QwLinearDiodeArray::operator= (const QwLinearDiodeArray &value)
{
  VQwBPM::operator= (value);

  if (GetElementName()!=""){
    size_t i = 0;
    this->fEffectiveCharge=value.fEffectiveCharge;
    for(i=0;i<8;i++) this->fPhotodiode[i]=value.fPhotodiode[i];
    for(i=kXAxis;i<kNumAxes;i++) {
      this->fRelPos[i]=value.fRelPos[i];
    }
  }
  return *this;
}

VQwBPM& QwLinearDiodeArray::operator+= (const VQwBPM &value)
{
  *(dynamic_cast<QwLinearDiodeArray*>(this)) +=
      *(dynamic_cast<const QwLinearDiodeArray*>(&value));
  return *this;
}

QwLinearDiodeArray& QwLinearDiodeArray::operator+= (const QwLinearDiodeArray &value)
{

  if (GetElementName()!=""){
    size_t i = 0;
    this->fEffectiveCharge+=value.fEffectiveCharge;
    for(i=0;i<8;i++) this->fPhotodiode[i]+=value.fPhotodiode[i];
    for(i=kXAxis;i<kNumAxes;i++) {
      this->fRelPos[i]+=value.fRelPos[i];
    }
  }
  return *this;
}

VQwBPM& QwLinearDiodeArray::operator-= (const VQwBPM &value)
{
  *(dynamic_cast<QwLinearDiodeArray*>(this)) -=
      *(dynamic_cast<const QwLinearDiodeArray*>(&value));
  return *this;
}

QwLinearDiodeArray& QwLinearDiodeArray::operator-= (const QwLinearDiodeArray &value)
{

  if (GetElementName()!=""){
    size_t i = 0;
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(i=0;i<8;i++) this->fPhotodiode[i]-=value.fPhotodiode[i];
    for(i=kXAxis;i<kNumAxes;i++) {
      this->fRelPos[i]-=value.fRelPos[i];
    }
  }
  return *this;
}


void QwLinearDiodeArray::Ratio(QwLinearDiodeArray &numer, QwLinearDiodeArray &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // LinearArray is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  return;
}



void QwLinearDiodeArray::Scale(Double_t factor)
{
  size_t i = 0;
  fEffectiveCharge.Scale(factor);

  for(i=0;i<8;i++) fPhotodiode[i].Scale(factor);
  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].Scale(factor);
  }
  return;
}


void QwLinearDiodeArray::CalculateRunningAverage()
{
  size_t i = 0;
  for(i=0;i<8;i++) fPhotodiode[i].CalculateRunningAverage();
  for (i = 0; i < 2; i++) fRelPos[i].CalculateRunningAverage();
  fEffectiveCharge.CalculateRunningAverage();
  return;
}

void QwLinearDiodeArray::AccumulateRunningSum(const VQwBPM& value, Int_t count, Int_t ErrorMask){
  AccumulateRunningSum(*dynamic_cast<const QwLinearDiodeArray* >(&value), count, ErrorMask);
}

void QwLinearDiodeArray::AccumulateRunningSum(const QwLinearDiodeArray& value, Int_t count, Int_t ErrorMask)
{
  size_t i = 0;
  for(i=0;i<8;i++) fPhotodiode[i].AccumulateRunningSum(value.fPhotodiode[i], count, ErrorMask);
  for (i = 0; i < 2; i++) fRelPos[i].AccumulateRunningSum(value.fRelPos[i], count, ErrorMask);
  fEffectiveCharge.AccumulateRunningSum(value.fEffectiveCharge, count, ErrorMask);
  return;
}

void QwLinearDiodeArray::DeaccumulateRunningSum(VQwBPM& value, Int_t ErrorMask){
  DeaccumulateRunningSum(*dynamic_cast<QwLinearDiodeArray* >(&value), ErrorMask);
}

void QwLinearDiodeArray::DeaccumulateRunningSum(QwLinearDiodeArray& value, Int_t ErrorMask)
{
  size_t i = 0;
  for(i=0;i<8;i++) fPhotodiode[i].DeaccumulateRunningSum(value.fPhotodiode[i], ErrorMask);
  for (i = 0; i < 2; i++) fRelPos[i].DeaccumulateRunningSum(value.fRelPos[i], ErrorMask);
  fEffectiveCharge.DeaccumulateRunningSum(value.fEffectiveCharge, ErrorMask);
  return;
}



void  QwLinearDiodeArray::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }  else {
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;

    if(prefix.Contains("asym_"))
      thisprefix.ReplaceAll("asym_","diff_");
    SetRootSaveStatus(prefix);
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<8;i++) fPhotodiode[i].ConstructHistograms(folder, thisprefix);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fRelPos[i].ConstructHistograms(folder, thisprefix);
    }
  }
  return;
}

void  QwLinearDiodeArray::FillHistograms()
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else {
    fEffectiveCharge.FillHistograms();
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<8;i++) fPhotodiode[i].FillHistograms();
    }
    for(i=kXAxis;i<kNumAxes;i++){
      fRelPos[i].FillHistograms();
    }
  }
  return;
}

void  QwLinearDiodeArray::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix.Contains("asym_"))
      thisprefix.ReplaceAll("asym_","diff_");

    SetRootSaveStatus(prefix);

    fEffectiveCharge.ConstructBranchAndVector(tree,prefix,values);
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<8;i++) fPhotodiode[i].ConstructBranchAndVector(tree,thisprefix,values);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    }

  }
  return;
}

void  QwLinearDiodeArray::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix.Contains("asym_"))
      thisprefix.ReplaceAll("asym_","diff_");

    SetRootSaveStatus(prefix);

    fEffectiveCharge.ConstructBranch(tree,prefix);
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<8;i++) fPhotodiode[i].ConstructBranch(tree,thisprefix);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fRelPos[i].ConstructBranch(tree,thisprefix);
    }

  }
  return;
}

void  QwLinearDiodeArray::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      if (modulelist.HasValue(devicename)){
	TString thisprefix=prefix;
	if(prefix.Contains("asym_"))
	  thisprefix.ReplaceAll("asym_","diff_");

	SetRootSaveStatus(prefix);

	fEffectiveCharge.ConstructBranch(tree,prefix);
	size_t i = 0;
	if(bFullSave) {
	  for(i=0;i<8;i++) fPhotodiode[i].ConstructBranch(tree,thisprefix);
	}
	for(i=kXAxis;i<kNumAxes;i++) {
	  fRelPos[i].ConstructBranch(tree,thisprefix);
	}

	QwMessage <<" Tree leaves added to "<<devicename<<" Corresponding channels"<<QwLog::endl;
      }
      // this functions doesn't do anything yet
    }





  return;
}

void  QwLinearDiodeArray::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the tree.
  }
  else {
    fEffectiveCharge.FillTreeVector(values);
    size_t i = 0;
    if(bFullSave) {
      for(i=0;i<8;i++) fPhotodiode[i].FillTreeVector(values);
    }
    for(i=kXAxis;i<kNumAxes;i++){
      fRelPos[i].FillTreeVector(values);
    }
  }
  return;
}


void QwLinearDiodeArray::SetEventCutMode(Int_t bcuts)
{
  size_t i = 0;
  //  bEVENTCUTMODE=bcuts;
  for (i=0;i<8;i++) fPhotodiode[i].SetEventCutMode(bcuts);
  for (i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].SetEventCutMode(bcuts);
    fAbsPos[i].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);
}


void QwLinearDiodeArray::MakeLinearArrayList()
{
  for (size_t i = kXAxis; i < kNumAxes; i++) {
	QwVQWK_Channel relpos(fRelPos[i]);
	relpos = fRelPos[i];
    fLinearArrayElementList.push_back(relpos);
  }
  QwVQWK_Channel effectivecharge(fEffectiveCharge);
  effectivecharge = fEffectiveCharge;
  fLinearArrayElementList.push_back(effectivecharge);
}

#ifdef __USE_DATABASE__
std::vector<QwDBInterface> QwLinearDiodeArray::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();

  for(size_t i=0;i<2;i++) {
    fRelPos[i].AddEntriesToList(row_list);
  }
  fEffectiveCharge.AddEntriesToList(row_list);
  return row_list;
}

std::vector<QwErrDBInterface> QwLinearDiodeArray::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  for(size_t i=0;i<2;i++) {
    fRelPos[i].AddErrEntriesToList(row_list);
  }
  fEffectiveCharge.AddErrEntriesToList(row_list);
  return row_list;
}
#endif // __USE_DATABASE__


/**********************************
 * Mock data generation routines
 **********************************/

void  QwLinearDiodeArray::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
  Double_t sumX = 1.1e8; // These are just guesses, but I made X and Y different
  Double_t sumY = 0.9e8; // to make it more interesting for the analyzer...

  
  // Determine the asymmetry from the position
  Double_t meanXP = (1.0 + meanX) * sumX / 2.0;
  Double_t meanXM = (1.0 - meanX) * sumX / 2.0; // = sumX - meanXP;
  Double_t meanYP = (1.0 + meanY) * sumY / 2.0;
  Double_t meanYM = (1.0 - meanY) * sumY / 2.0; // = sumY - meanYP;

  // Determine the spread of the asymmetry (this is not tested yet)
  // (negative sigma should work in the QwVQWK_Channel, but still using fabs)
  Double_t sigmaXP = fabs(sumX * sigmaX / meanX);
  Double_t sigmaXM = sigmaXP;
  Double_t sigmaYP = fabs(sumY * sigmaY / meanY);
  Double_t sigmaYM = sigmaYP;

  // Propagate these parameters to the ADCs
  fPhotodiode[0].SetRandomEventParameters(meanXP, sigmaXP);
  fPhotodiode[1].SetRandomEventParameters(meanXM, sigmaXM);
  fPhotodiode[2].SetRandomEventParameters(meanYP, sigmaYP);
  fPhotodiode[3].SetRandomEventParameters(meanYM, sigmaYM);
}


void QwLinearDiodeArray::RandomizeEventData(int helicity, double time)
{
  for (size_t i=0; i<8; i++) fPhotodiode[i].RandomizeEventData(helicity, time);

  return;
}


void QwLinearDiodeArray::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  for (size_t i=0; i<2; i++)
    {
      fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
}


void QwLinearDiodeArray::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (size_t i=0; i<8; i++) fPhotodiode[i].EncodeEventData(buffer);
}


void QwLinearDiodeArray::SetDefaultSampleSize(Int_t sample_size)
{
  for(size_t i=0;i<8;i++) fPhotodiode[i].SetDefaultSampleSize((size_t)sample_size);
  return;
}


void QwLinearDiodeArray::SetSubElementPedestal(Int_t j, Double_t value)
{
  fPhotodiode[j].SetPedestal(value);
  return;
}

void QwLinearDiodeArray::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fPhotodiode[j].SetCalibrationFactor(value);
  return;
}

