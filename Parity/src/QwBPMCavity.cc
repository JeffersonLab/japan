/**********************************************************\
* File: QwBPMCavity.cc                                 *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBPMCavity.h"

// System headers
#include <stdexcept>

// Qweak headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif // __USE_DATABASE__

/* Position calibration factor, transform ADC counts in mm*/
const Double_t QwBPMCavity::kQwCavityCalibration = 1.0;
//The value of kQwCavityCalibration is made up so we have to replace it with an actual value when it is determined
//Josh Kaisen

const TString QwBPMCavity::subelement[QwBPMCavity::kNumElements]={"XI","YI","Q"};


Bool_t QwBPMCavity::ParseChannelName(const TString &channel,
				     TString &detname,
				     TString &subname,
				     UInt_t &localindex)
{
  localindex=kInvalidSubelementIndex;
  //QwMessage << "Channel Name: " << channel << QwLog::endl;
  for(size_t i=0;i<kNumElements;i++){
    if(channel.EndsWith(subelement[i],TString::kIgnoreCase)){
      localindex=i;
      subname = subelement[i];
      size_t detnamesize = channel.Sizeof() - subname.Sizeof();
      detname = channel(0,detnamesize);
      break;
    }
  }

  //QwMessage << "Detector Name: " << detname << QwLog::endl;
  //QwMessage << "Sub Name: " << subname << QwLog::endl;

  if(localindex==kInvalidSubelementIndex){
    detname = "";
    subname = "";
    QwWarning << "QwBPMCavity::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index" << QwLog::endl;
  }
  return (localindex!=kInvalidSubelementIndex);
}


void  QwBPMCavity::InitializeChannel(TString name)
{
  size_t i=0;
  Bool_t localdebug = kFALSE;

  VQwBPM::InitializeChannel(name);

  for(i=0;i<kNumElements;i++) {
    fElement[i].InitializeChannel(name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" Wire ["<<i<<"]="<<fElement[i].GetElementName()<<"\n";
  }

  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].InitializeChannel(name+"Rel"+subelement[i],"derived");
    fAbsPos[i].InitializeChannel(name+kAxisLabel[i],"derived");
  }

  bFullSave=kTRUE;

  return;
}

void  QwBPMCavity::InitializeChannel(TString subsystem, TString name)
{
  size_t i=0;
  Bool_t localdebug = kFALSE;
  
  VQwBPM::InitializeChannel(name);
  
  for(i=0;i<kNumElements;i++) {
    fElement[i].InitializeChannel(subsystem, "QwBPMCavity", name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" Wire ["<<i<<"]="<<fElement[i].GetElementName()<<"\n";
  }
  
  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].InitializeChannel(subsystem, "QwBPMCavity", name+"Rel"+subelement[i],"derived");
    fAbsPos[i].InitializeChannel(subsystem, "QwBPMCavity", name+kAxisLabel[i],"derived");
  }
  
  bFullSave=kTRUE;
  
  return;
}

void QwBPMCavity::ClearEventData()
{
  size_t i=0;

  for(i=0;i<kNumElements;i++){
    fElement[i].ClearEventData();
  }
  for(i=0;i<kNumAxes;i++){
    fAbsPos[i].ClearEventData();
    fRelPos[i].ClearEventData();
  }
}


Bool_t QwBPMCavity::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=0;
  for(size_t i=0;i<kNumElements;i++)
    {
      deviceerror|= fElement[i].ApplyHWChecks();  //OR the error code from each wire
      eventokay &= (deviceerror & 0x0);//AND with 0 since zero means HW is good.

      if (bDEBUG) std::cout<<" Inconsistent within BPM terminals wire[ "<<i<<" ] "<<std::endl;
      if (bDEBUG) std::cout<<" wire[ "<<i<<" ] sequence num "<<fElement[i].GetSequenceNumber()<<" sample size "<<fElement[i].GetNumberOfSamples()<<std::endl;
    }
  return eventokay;
}

void QwBPMCavity::IncrementErrorCounters()
{
  size_t i=0;

  for(i=0;i<kNumElements;i++){
    fElement[i].IncrementErrorCounters();
  }
  for(i=0;i<kNumAxes;i++){
    fRelPos[i].IncrementErrorCounters();
    fAbsPos[i].IncrementErrorCounters();
  }
}

void QwBPMCavity::PrintErrorCounters() const
{
  size_t i=0;
  for(i=0;i<kNumElements;i++) {
    fElement[i].PrintErrorCounters();
  }
  for(i=0;i<kNumAxes;i++){
    //    fRelPos[i].PrintErrorCounters();
    fAbsPos[i].PrintErrorCounters();
  }
}

UInt_t QwBPMCavity::GetEventcutErrorFlag()
{
  size_t i=0;
  UInt_t error=0;
  for(i=0;i<kNumElements;i++) {
    error|=fElement[i].GetEventcutErrorFlag();
  }
  for(i=0;i<kNumAxes;i++){
    error|=fRelPos[i].GetEventcutErrorFlag();
    error|=fAbsPos[i].GetEventcutErrorFlag();
  }
  return error;
}

UInt_t QwBPMCavity::UpdateErrorFlag()
{
  size_t i=0;
  UInt_t error1=0;
  UInt_t error2=0;  
  for(i=0;i<kNumElements;i++) {
    error1|=fElement[i].GetErrorCode();
    error2|=fElement[i].GetEventcutErrorFlag();
  }
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].UpdateErrorFlag(error1);
    fAbsPos[i].UpdateErrorFlag(error1);
    error2|=fRelPos[i].GetEventcutErrorFlag();
    error2|=fAbsPos[i].GetEventcutErrorFlag();
  }
  return error2;
}

Bool_t QwBPMCavity::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;
  UInt_t error_code = 0;
  //Event cuts for elements
  for(i=0;i<kNumElements;i++){
    if (fElement[i].ApplySingleEventCuts()){
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Element "<< fElement[i].GetElementName()
			   << " event cut failed ";
    }
    error_code |= fElement[i].GetErrorCode();
  }
  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].UpdateErrorFlag(error_code);
    if (fRelPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }
  }

  for(i=kXAxis;i<kNumAxes;i++){
    fAbsPos[i].UpdateErrorFlag(error_code);
    if (fAbsPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
  }
  return status;
}

VQwHardwareChannel* QwBPMCavity::GetSubelementByName(TString ch_name)
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
  }else if (ch_name=="effectivecharge" || ch_name=="charge" || ch_name=="q"){
    tmpptr = &fElement[kQElem];
  } else {
    TString loc="QwLinearDiodeArray::GetSubelementByName for"
      + this->GetElementName() + " was passed "
      + ch_name + ", which is an unrecognized subelement name.";
    throw std::invalid_argument(loc.Data());
  }
  return tmpptr;
}


/*
void QwBPMCavity::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{

  if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
     fRelPos[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(minX,maxX);

  } else  if (ch_name=="absx"){
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
      fAbsPos[1].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="effectivecharge"){
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
     fElement[kQElem].SetSingleEventCuts(minX,maxX);

  }

}*/

void QwBPMCavity::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability, Double_t burplevel){
  errorflag|=kBPMErrorFlag;//update the device flag
  if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
     fRelPos[0].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel); 

  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel); 

  } else  if (ch_name=="absx"){
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[0].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel); 

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[1].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel);

  }else if (ch_name=="effectivecharge"){
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fElement[kQElem].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel);

  }else if (ch_name=="xi"){
    QwMessage<<"XI " <<  minX <<" UL " << maxX <<QwLog::endl;
    fElement[0].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel);

  }else if (ch_name=="yi"){
    QwMessage<<"YI " <<  minX <<" UL " << maxX <<QwLog::endl;
    fElement[1].SetSingleEventCuts(errorflag,minX,maxX,stability,burplevel);

  }

}

Bool_t QwBPMCavity::CheckForBurpFail(const VQwDataElement *ev_error){
  Short_t i=0;
  Bool_t burpstatus = kFALSE;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      //std::cout<<" Here in QwBPMCavity::CheckForBurpFail \n";
      if (this->GetElementName()!="") {
        const QwBPMCavity* value_bpm = dynamic_cast<const QwBPMCavity* >(ev_error);
        for(i=0;i<2;i++){
          burpstatus |= fElement[i].CheckForBurpFail(&(value_bpm->fElement[i]));
          burpstatus |= fRelPos[i].CheckForBurpFail(&(value_bpm->fRelPos[i]));
          burpstatus |= fAbsPos[i].CheckForBurpFail(&(value_bpm->fAbsPos[i]));
        }
        burpstatus |= fElement[kQElem].CheckForBurpFail(&(value_bpm->fElement[kQElem]));
      }
    } else {
      TString loc="Standard exception from QwBPMCavity::CheckForBurpFail :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }
  return burpstatus;
}

void QwBPMCavity::UpdateErrorFlag(const VQwBPM *ev_error){
  size_t i=0;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwBPMStripline::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
        const QwBPMCavity* value_bpm = dynamic_cast<const QwBPMCavity* >(ev_error);
	for(i=0;i<kNumElements;i++){
	  fElement[i].UpdateErrorFlag(value_bpm->fElement[i]);
	}
	for(i=kXAxis;i<kNumAxes;i++) {
	  fRelPos[i].UpdateErrorFlag(value_bpm->fRelPos[i]);
	  fAbsPos[i].UpdateErrorFlag(value_bpm->fAbsPos[i]);
	}
      }
    } else {
      TString loc="Standard exception from QwBPMCavity::UpdateErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }   
};



void  QwBPMCavity::ProcessEvent()
{
  size_t i = 0;

  ApplyHWChecks();
  /**First apply HW checks and update HW  error flags. 
     Calling this routine here and not in ApplySingleEventCuts  
     makes a difference for a BPMs because they have derrived devices.
  */
  for(i=0;i<kNumElements;i++) {
    fElement[i].ProcessEvent();
  }

  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].Ratio(fElement[i],fElement[kQElem]);
    fRelPos[i].Scale(kQwCavityCalibration);
    fAbsPos[i]= fRelPos[i];
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
  }

  return;
}


Int_t QwBPMCavity::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<kNumElements) {
    fElement[index].ProcessEvBuffer(buffer,word_position_in_buffer);
  }
  else {
    std::cerr <<
      "QwBPMCavity::ProcessEvBuffer(): attempt to fill in raw date for a wire that doesn't exist \n";
  }
  return word_position_in_buffer;
}



void QwBPMCavity::PrintValue() const
{
  for (size_t i = 0; i < kNumElements; i++) {
    fElement[i].PrintValue();
  }
  for (size_t i=0;i<kNumAxes;i++){
    //    fRelPos[i].PrintValue();
    fAbsPos[i].PrintValue();
  }
}

void QwBPMCavity::PrintInfo() const
{
  size_t i = 0;
  for (i = 0; i < kNumElements; i++) {
    fElement[i].PrintInfo();
  }
  for(i=0;i<kNumAxes;i++){
    //    fRelPos[i].PrintInfo();
    fAbsPos[i].PrintInfo();
  }
}


TString QwBPMCavity::GetSubElementName(Int_t subindex)
{
  TString thisname;
  if(subindex<kNumElements&&subindex>-1)
    thisname=fElement[subindex].GetElementName();
  else
    std::cerr<<"QwBPMCavity::GetSubElementName for "<<
      GetElementName()<<" this subindex doesn't exists \n";
  return thisname;
}

UInt_t QwBPMCavity::GetSubElementIndex(TString subname)
{
  subname.ToUpper();
  UInt_t localindex=kInvalidSubelementIndex;
  for(size_t i=0;i<kNumElements;i++)
    if(subname==subelement[i])localindex=i;

  if(localindex==kInvalidSubelementIndex)
    std::cerr << "QwBPMCavity::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;

  return localindex;
}

void  QwBPMCavity::GetAbsolutePosition()
{
  for(size_t i=0;i<kNumAxes;i++){
    fRelPos[i].Ratio(fElement[i],fElement[kQElem]);
    fAbsPos[i]= fRelPos[i];
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
  }
  // For Z, the absolute position will be the offset we are reading from the
  // geometry map file. Since we are not putting that to the tree it is not
  // treated as a vqwk channel.
}


VQwBPM& QwBPMCavity::operator= (const VQwBPM &value)
{
  *(dynamic_cast<QwBPMCavity*>(this)) =
      *(dynamic_cast<const QwBPMCavity*>(&value));
  return *this;
}

QwBPMCavity& QwBPMCavity::operator= (const QwBPMCavity &value)
{
  VQwBPM::operator= (value);

  if (GetElementName()!=""){
    size_t i = 0;
    for(i=0;i<kNumElements;i++) {
      this->fElement[i]=value.fElement[i];
    }
    for(i=0;i<kNumAxes;i++){
      this->fRelPos[i]=value.fRelPos[i];
      this->fAbsPos[i]=value.fAbsPos[i];
    }
  }
  return *this;
}


QwBPMCavity& QwBPMCavity::operator+= (const QwBPMCavity &value)
{

  if (GetElementName()!=""){
    size_t i = 0;
    for(i=0;i<kNumElements;i++) {
      this->fElement[i]+=value.fElement[i];
    }
    for(i=0;i<kNumAxes;i++){
      this->fRelPos[i]+=value.fRelPos[i];
      this->fAbsPos[i]+=value.fAbsPos[i];
    }
  }
  return *this;
}

VQwBPM& QwBPMCavity::operator+= (const VQwBPM &value)
{
  *(dynamic_cast<QwBPMCavity*>(this)) +=
      *(dynamic_cast<const QwBPMCavity*>(&value));
  return *this;
}



QwBPMCavity& QwBPMCavity::operator-= (const QwBPMCavity &value)
{

  if (GetElementName()!=""){
    size_t i = 0;
    for(i=0;i<kNumElements;i++) {
      this->fElement[i]-=value.fElement[i];
    }
    for(i=0;i<kNumAxes;i++){
      this->fRelPos[i]-=value.fRelPos[i];
      this->fAbsPos[i]-=value.fAbsPos[i];
    }
  }
  return *this;
}

VQwBPM& QwBPMCavity::operator-= (const VQwBPM &value)
{
  *(dynamic_cast<QwBPMCavity*>(this)) -=
      *(dynamic_cast<const QwBPMCavity*>(&value));
  return *this;
}



void QwBPMCavity::Ratio(QwBPMCavity &numer, QwBPMCavity &denom)
{
  // this function is called when forming asymmetries. In this case what we actually want for the
  // stripline is the difference only not the asymmetries

  *this=numer;
  this->fElement[kQElem].Ratio(numer.fElement[kQElem],denom.fElement[kQElem]);
  return;
}



void QwBPMCavity::Scale(Double_t factor)
{
  for(size_t i=0;i<kNumElements;i++){
    fElement[i].Scale(factor);
  }
  for(size_t i=0;i<kNumAxes;i++){
    fRelPos[i].Scale(factor);
    fAbsPos[i].Scale(factor);
  }
  return;
}


void QwBPMCavity::CalculateRunningAverage()
{
  size_t i = 0;
  for(i=0;i<kNumElements;i++)
    fElement[i].CalculateRunningAverage();
  for (i = 0; i < kNumAxes; i++){
    fRelPos[i].CalculateRunningAverage();
    fAbsPos[i].CalculateRunningAverage();
  }
  return;
}

void QwBPMCavity::AccumulateRunningSum(const VQwBPM &value, Int_t count, Int_t ErrorMask){
  AccumulateRunningSum(*dynamic_cast<const QwBPMCavity* >(&value), count, ErrorMask);
};

void QwBPMCavity::AccumulateRunningSum(const QwBPMCavity& value, Int_t count, Int_t ErrorMask)
{

  size_t i = 0;
  for(i=0;i<kNumElements;i++)
    fElement[i].AccumulateRunningSum(value.fElement[i], count, ErrorMask);
  for (i = 0; i < 2; i++){
    fRelPos[i].AccumulateRunningSum(value.fRelPos[i], count, ErrorMask);
    fAbsPos[i].AccumulateRunningSum(value.fAbsPos[i], count, ErrorMask);
  }
  return;
}

void QwBPMCavity::DeaccumulateRunningSum(VQwBPM &value, Int_t ErrorMask){
  DeaccumulateRunningSum(*dynamic_cast<QwBPMCavity* >(&value), ErrorMask);
};

void QwBPMCavity::DeaccumulateRunningSum(QwBPMCavity& value, Int_t ErrorMask)
{
  size_t i = 0;
  for(i=0;i<kNumElements;i++)
    fElement[i].DeaccumulateRunningSum(value.fElement[i], ErrorMask);
  for (i = 0; i < kNumAxes; i++){
    fRelPos[i].DeaccumulateRunningSum(value.fRelPos[i], ErrorMask);
    fAbsPos[i].DeaccumulateRunningSum(value.fAbsPos[i], ErrorMask);
  }
  return;
}




void  QwBPMCavity::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }  else {
    fElement[kQElem].ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;

    if(prefix.Contains("asym_"))
      thisprefix.ReplaceAll("asym_","diff_");
    SetRootSaveStatus(prefix);
    size_t i = 0;
    for(i=kXAxis;i<kNumAxes;i++) {
      if(bFullSave) fElement[i].ConstructHistograms(folder, thisprefix);
      // fRelPos[i].ConstructHistograms(folder, thisprefix);
      fAbsPos[i].ConstructHistograms(folder, thisprefix);
    }
  }
  return;
}

void  QwBPMCavity::FillHistograms()
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else {
    fElement[kQElem].FillHistograms();
    size_t i = 0;
    for(i=kXAxis;i<kNumAxes;i++){
      if (bFullSave) fElement[i].FillHistograms();
      //      fRelPos[i].FillHistograms();
      fAbsPos[i].FillHistograms();
    }
    //No data for z position
  }
  return;
}

void  QwBPMCavity::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix.Contains("asym_"))
      thisprefix.ReplaceAll("asym_","diff_");
    SetRootSaveStatus(prefix);

    fElement[kQElem].ConstructBranchAndVector(tree,prefix,values);
    size_t i = 0;
    for(i=kXAxis;i<kNumAxes;i++) {
      if (bFullSave) fElement[i].ConstructBranchAndVector(tree,thisprefix,values);
      //      fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
      fAbsPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    }

  }
  return;
}

 void  QwBPMCavity::ConstructBranch(TTree *tree, TString &prefix)
 {
   if (GetElementName()==""){
     //  This channel is not used, so skip constructing trees.
   }
   else {
     TString thisprefix=prefix;
     if(prefix.Contains("asym_"))
       thisprefix.ReplaceAll("asym_","diff_");
     SetRootSaveStatus(prefix);

     fElement[kQElem].ConstructBranch(tree,prefix);
     size_t i = 0;
     for(i=kXAxis;i<kNumAxes;i++) {
       if (bFullSave) fElement[i].ConstructBranch(tree,thisprefix);
       //       fRelPos[i].ConstructBranch(tree,thisprefix);
       fAbsPos[i].ConstructBranch(tree,thisprefix);
     }

   }
   return;
 }

 void  QwBPMCavity::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
 {
   TString devicename;
   /*
   QwMessage <<" QwBCM::ConstructBranch "<<QwLog::endl;
   modulelist.RewindToFileStart();
   while (modulelist.ReadNextLine()){
       modulelist.TrimComment('!');   // Remove everything after a '!' character.
       modulelist.TrimWhitespace();   // Get rid of leading and trailing spaces
       QwMessage <<" "<<modulelist.GetLine()<<" ";
   }
   QwMessage <<QwLog::endl;
   */
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

       fElement[kQElem].ConstructBranch(tree,prefix);
       size_t i = 0;
       for(i=kXAxis;i<kNumAxes;i++) {
	 if (bFullSave) fElement[i].ConstructBranch(tree,thisprefix);
	 //	 fRelPos[i].ConstructBranch(tree,thisprefix);
         fAbsPos[i].ConstructBranch(tree,thisprefix);
       }

       QwMessage <<" Tree leaves added to "<<devicename<<" Corresponding channels"<<QwLog::endl;
       }
       // this functions doesn't do anything yet
     }





   return;
 }


void  QwBPMCavity::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the tree.
  }
  else {
    fElement[kQElem].FillTreeVector(values);
    size_t i = 0;
    for(i=kXAxis;i<kNumAxes;i++){
      if (bFullSave) fElement[i].FillTreeVector(values);
      //      fRelPos[i].FillTreeVector(values);
      fAbsPos[i].FillTreeVector(values);
    }
  }
  return;
}

void QwBPMCavity::SetEventCutMode(Int_t bcuts)
{
  size_t i = 0;
  //  bEVENTCUTMODE=bcuts;
  for (i=0;i<kNumElements;i++) {
    fElement[i].SetEventCutMode(bcuts);
  }
  for(i=0;i<kNumAxes;i++){
    fRelPos[i].SetEventCutMode(bcuts);
    fAbsPos[i].SetEventCutMode(bcuts);
  }
}


void QwBPMCavity::MakeBPMCavityList()
{
  for (size_t i = kXAxis; i < kNumAxes; i++) {
	QwVQWK_Channel relpos(fRelPos[i]);
	relpos = fRelPos[i];
	fBPMElementList.push_back(relpos);
  }
  QwVQWK_Channel effectivecharge(fElement[kQElem]);
  effectivecharge = fElement[kQElem];
  fBPMElementList.push_back(effectivecharge);
}

#ifdef __USE_DATABASE__
std::vector<QwDBInterface> QwBPMCavity::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  for(size_t i=0;i<kNumAxes;i++) {
    fRelPos[i].AddEntriesToList(row_list);
    fAbsPos[i].AddEntriesToList(row_list);
  }
  fElement[kQElem].AddEntriesToList(row_list);
  return row_list;
}


std::vector<QwErrDBInterface> QwBPMCavity::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  for(size_t i=0;i<kNumAxes;i++) {
    fRelPos[i].AddErrEntriesToList(row_list);
    fAbsPos[i].AddErrEntriesToList(row_list);
  }
  fElement[kQElem].AddErrEntriesToList(row_list);
  return row_list;
}
#endif // __USE_DATABASE__

/**********************************
 * Mock data generation routines
 **********************************/

void  QwBPMCavity::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
  //Double_t sumX = 1.1e8; // These are just guesses, but I made X and Y different
  //Double_t sumY = 0.9e8; // to make it more interesting for the analyzer...

  // Rotate the requested position if necessary (this is not tested yet)
  /* if (bRotated) {
    Double_t rotated_meanX = (meanX + meanY) / kRotationCorrection;
    Double_t rotated_meanY = (meanX - meanY) / kRotationCorrection;
    meanX = rotated_meanX;
    meanY = rotated_meanY;
    }*/

  // Determine the asymmetry from the position
  //Double_t meanXP = (1.0 + meanX / kQwCavityCalibration) * sumX / 2.0;
  //Double_t meanXM = (1.0 - meanX / kQwCavityCalibration) * sumX / 2.0; // = sumX - meanXP;
  //Double_t meanYP = (1.0 + meanY / kQwCavityCalibration) * sumY / 2.0;
  //Double_t meanYM = (1.0 - meanY / kQwCavityCalibration) * sumY / 2.0; // = sumY - meanYP;

  // Determine the spread of the asymmetry (this is not tested yet)
  // (negative sigma should work in the QwVQWK_Channel, but still using fabs)
  //Double_t sigmaXP = fabs(sumX * sigmaX / meanX);
  //Double_t sigmaXM = sigmaXP;
  //Double_t sigmaYP = fabs(sumY * sigmaY / meanY);
  //Double_t sigmaYM = sigmaYP;

  // Propagate these parameters to the ADCs
  //fElement[0].SetRandomEventParameters(meanXP, sigmaXM);
  //fElement[1].SetRandomEventParameters(meanXM, sigmaYM);
  //fElement[2].SetRandomEventParameters(meanYP, sigmaYP);
}


void QwBPMCavity::RandomizeEventData(int helicity, double time)
{
  for (size_t i=0; i<kNumElements; i++) fElement[i].RandomizeEventData(helicity, time);

  return;
}


void QwBPMCavity::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  for (size_t i=0; i<kNumElements; i++)
    {
      fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
}


void QwBPMCavity::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (size_t i=0; i<kNumElements; i++)
    fElement[i].EncodeEventData(buffer);
}


void QwBPMCavity::SetDefaultSampleSize(Int_t sample_size)
{
  for(size_t i=0;i<kNumElements;i++)
    fElement[i].SetDefaultSampleSize((size_t)sample_size);
}


void QwBPMCavity::SetSubElementPedestal(Int_t j, Double_t value)
{
  fElement[j].SetPedestal(value);
}

void QwBPMCavity::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fElement[j].SetCalibrationFactor(value);
}

