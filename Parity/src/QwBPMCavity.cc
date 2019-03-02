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
const Double_t QwBPMCavity::kQwCavityCalibration = 1e-8;
//The value of kQwCavityCalibration is made up so we have to replace it with an actual value when it is determined
//Josh Kaisen
const Double_t QwBPMCavity::kRotationCorrection = 1./1.414;
const TString QwBPMCavity::subelement[3]={"X","Y","Q"};

void  QwBPMCavity::InitializeChannel(TString name)
{
  Short_t i=0;
  Bool_t localdebug = kFALSE;

  VQwBPM::InitializeChannel(name);

  for(i=kXAxis;i<kNumAxes;i++)
    fAbsPos[i].InitializeChannel(name+"Abs"+kAxisLabel[i],"derived");

  fEffectiveCharge.InitializeChannel(name+"WS","raw");

  for(i=0;i<2;i++) {
    fWire[i].InitializeChannel(name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" Wire ["<<i<<"]="<<fWire[i].GetElementName()<<"\n";
  }

  for(i=kXAxis;i<kNumAxes;i++) fRelPos[i].InitializeChannel(name+"Rel"+subelement[i],"derived");

  bFullSave=kTRUE;

  return;
}

void  QwBPMCavity::InitializeChannel(TString subsystem, TString name)
{
  Short_t i=0;
  Bool_t localdebug = kFALSE;
  
  VQwBPM::InitializeChannel(name);
  
  for(i=kXAxis;i<kNumAxes;i++)
    fAbsPos[i].InitializeChannel(subsystem, "QwBPMCavity", name+"Abs"+kAxisLabel[i],"derived");
  
  fEffectiveCharge.InitializeChannel(subsystem, "QwBPMCavity", name+"WS","raw");
  
  for(i=0;i<2;i++) {
    fWire[i].InitializeChannel(subsystem, "QwBPMCavity", name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" Wire ["<<i<<"]="<<fWire[i].GetElementName()<<"\n";
  }
  
  for(i=kXAxis;i<kNumAxes;i++) fRelPos[i].InitializeChannel(subsystem, "QwBPMCavity", name+"Rel"+subelement[i],"derived");
  
  bFullSave=kTRUE;
  
  return;
}

void QwBPMCavity::ClearEventData()
{
  Short_t i=0;

  for(i=0;i<2;i++){
    fAbsPos[i].ClearEventData();
    fWire[i].ClearEventData();
    fRelPos[i].ClearEventData();
  }
  fEffectiveCharge.ClearEventData();

  return;
}


Bool_t QwBPMCavity::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=0;
  for(Short_t i=0;i<2;i++)
    {
      deviceerror|= fWire[i].ApplyHWChecks();  //OR the error code from each wire
      eventokay &= (deviceerror & 0x0);//AND with 0 since zero means HW is good.

      if (bDEBUG) std::cout<<" Inconsistent within BPM terminals wire[ "<<i<<" ] "<<std::endl;
      if (bDEBUG) std::cout<<" wire[ "<<i<<" ] sequence num "<<fWire[i].GetSequenceNumber()<<" sample size "<<fWire[i].GetNumberOfSamples()<<std::endl;
    }
  deviceerror = fEffectiveCharge.ApplyHWChecks();
  eventokay &= (deviceerror & 0x0);//AND with 0 
  return eventokay;
}

void QwBPMCavity::IncrementErrorCounters()
{
  Short_t i=0;

  for(i=0;i<2;i++) {
    fWire[i].IncrementErrorCounters();
    fRelPos[i].IncrementErrorCounters();
    fAbsPos[i].IncrementErrorCounters();
  }
  fEffectiveCharge.IncrementErrorCounters();
}

void QwBPMCavity::PrintErrorCounters() const
{
  Short_t i=0;

  for(i=0;i<2;i++) {
    fWire[i].PrintErrorCounters();
    fRelPos[i].PrintErrorCounters();
    fAbsPos[i].PrintErrorCounters();
  }
  fEffectiveCharge.PrintErrorCounters();
}

UInt_t QwBPMCavity::GetEventcutErrorFlag()
{
  Short_t i=0;
  UInt_t error=0;
  for(i=0;i<2;i++) {
    error|=fWire[i].GetEventcutErrorFlag();
    error|=fRelPos[i].GetEventcutErrorFlag();
    error|=fAbsPos[i].GetEventcutErrorFlag();
  }
  error|=fEffectiveCharge.GetEventcutErrorFlag();

  return error;
}

UInt_t QwBPMCavity::UpdateErrorFlag()
{
  Short_t i=0;
  UInt_t error1=0;
  UInt_t error2=0;  
  for(i=0;i<2;i++) {
    error1|=fWire[i].GetErrorCode();
    error2|=fWire[i].GetEventcutErrorFlag();
  }
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].UpdateErrorFlag(error1);
    fAbsPos[i].UpdateErrorFlag(error1);
    error2|=fRelPos[i].GetEventcutErrorFlag();
    error2|=fAbsPos[i].GetEventcutErrorFlag();
  }
  fEffectiveCharge.UpdateErrorFlag(error1);
  error2|=fEffectiveCharge.GetEventcutErrorFlag();
  return error2;
}

Bool_t QwBPMCavity::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;
  UInt_t error_code = 0;
  //Event cuts for X & Y
  for(i=0;i<2;i++){

    if (fWire[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }
    error_code |= fWire[i].GetErrorCode();//this to be updated in the rel and abp pos channels
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

  //Event cuts for four wire sum (EffectiveCharge)
  fEffectiveCharge.UpdateErrorFlag(error_code);
  if (fEffectiveCharge.ApplySingleEventCuts()){
    status&=kTRUE;
  }
  else{
    status&=kFALSE;
    if (bDEBUG) std::cout<<"EffectiveCharge event cut failed ";
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
     fEffectiveCharge.SetSingleEventCuts(minX,maxX);

  }

}

void QwBPMCavity::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability){
  errorflag|=kBPMErrorFlag;//update the device flag
  if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
     fRelPos[0].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  } else  if (ch_name=="absx"){
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[0].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
      fAbsPos[1].SetSingleEventCuts(errorflag,minX,maxX,stability);

  }else if (ch_name=="effectivecharge"){
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
     fEffectiveCharge.SetSingleEventCuts(errorflag,minX,maxX,stability);

  }

}

*/

void QwBPMCavity::UpdateErrorFlag(const VQwBPM *ev_error){
  Short_t i=0;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwBPMStripline::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
        const QwBPMCavity* value_bpm = dynamic_cast<const QwBPMCavity* >(ev_error);
	for(i=0;i<2;i++){
	  fWire[i].UpdateErrorFlag(value_bpm->fWire[i]);
	}
	for(i=kXAxis;i<kNumAxes;i++) {
	  fRelPos[i].UpdateErrorFlag(value_bpm->fRelPos[i]);
	  fAbsPos[i].UpdateErrorFlag(value_bpm->fAbsPos[i]);
	}
	fEffectiveCharge.UpdateErrorFlag(value_bpm->fEffectiveCharge);
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
  Bool_t localdebug = kFALSE;
  Short_t i = 0;


  ApplyHWChecks();
  /**First apply HW checks and update HW  error flags. 
     Calling this routine here and not in ApplySingleEventCuts  
     makes a difference for a BPMs because they have derrived devices.
  */
  for(i=0;i<2;i++)
    {
      fWire[i].ProcessEvent();
    }
  fEffectiveCharge.ProcessEvent();

  if (localdebug) {
    fEffectiveCharge.PrintInfo();
    fWire[i].PrintInfo();
  }

  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i]= fWire[i];
    fRelPos[i].Scale(kQwCavityCalibration);
    fAbsPos[i]= fRelPos[i];
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
  }

  return;
}


Int_t QwBPMCavity::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<2)
    {
      fWire[index].ProcessEvBuffer(buffer,word_position_in_buffer);
    }
  else if(index==2)
    {
      fEffectiveCharge.ProcessEvBuffer(buffer,word_position_in_buffer);
    }
  else
    {
      std::cerr <<
	"QwBPMCavity::ProcessEvBuffer(): attempt to fill in raw date for a wire that doesn't exist \n";
    }
  return word_position_in_buffer;
}



void QwBPMCavity::PrintValue() const
{
  for (Short_t i = 0; i < 2; i++) {
    fAbsPos[i].PrintValue();
    fWire[i].PrintValue();
    fRelPos[i].PrintValue();
  }
  return;
}

void QwBPMCavity::PrintInfo() const
{
  Short_t i = 0;
  for (i = 0; i < 2; i++) {
    fWire[i].PrintInfo();
    fAbsPos[i].PrintInfo();
    fRelPos[i].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();
}


TString QwBPMCavity::GetSubElementName(Int_t subindex)
{
  TString thisname;
  if(subindex<2&&subindex>-1)
    thisname=fWire[subindex].GetElementName();
  else
    std::cerr<<"QwBPMCavity::GetSubElementName for "<<
      GetElementName()<<" this subindex doesn't exists \n";

  return thisname;
}

UInt_t QwBPMCavity::GetSubElementIndex(TString subname)
{
  subname.ToUpper();
  UInt_t localindex=kInvalidSubelementIndex;
  for(Short_t i=0;i<3;i++) if(subname==subelement[i])localindex=i;

  if(localindex==kInvalidSubelementIndex)
    std::cerr << "QwBPMCavity::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;

  return localindex;
}

void  QwBPMCavity::GetAbsolutePosition()
{
  for(Short_t i=0;i<2;i++){
    fRelPos[i] = fWire[i];
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

  this->bRotated=value.bRotated;
  if (GetElementName()!=""){
    Short_t i = 0;
    this->fEffectiveCharge=value.fEffectiveCharge;
    for(i=0;i<2;i++) {
      this->fWire[i]=value.fWire[i];
      this->fRelPos[i]=value.fRelPos[i];
      this->fAbsPos[i]=value.fAbsPos[i];
    }
  }
  return *this;
}


QwBPMCavity& QwBPMCavity::operator+= (const QwBPMCavity &value)
{

  if (GetElementName()!=""){
    Short_t i = 0;
    this->fEffectiveCharge+=value.fEffectiveCharge;
    for(i=0;i<2;i++) {
      this->fWire[i]+=value.fWire[i];
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
    Short_t i = 0;
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(i=0;i<2;i++) {
      this->fWire[i]-=value.fWire[i];
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
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  return;
}



void QwBPMCavity::Scale(Double_t factor)
{
  fEffectiveCharge.Scale(factor);

  for(Short_t i=0;i<2;i++){
    fWire[i].Scale(factor);
    fRelPos[i].Scale(factor);
    fAbsPos[i].Scale(factor);
  }
  return;
}


void QwBPMCavity::CalculateRunningAverage()
{
  Short_t i = 0;
  for(i=0;i<2;i++) fWire[i].CalculateRunningAverage();
  for (i = 0; i < 2; i++) fRelPos[i].CalculateRunningAverage();
  for (i = 0; i < 2; i++) fAbsPos[i].CalculateRunningAverage();
  fEffectiveCharge.CalculateRunningAverage();
  return;
}

void QwBPMCavity::AccumulateRunningSum(const VQwBPM &value){
  AccumulateRunningSum(*dynamic_cast<const QwBPMCavity* >(&value));
};

void QwBPMCavity::AccumulateRunningSum(const QwBPMCavity& value)
{

  Short_t i = 0;
  for(i=0;i<2;i++) fWire[i].AccumulateRunningSum(value.fWire[i]);
  fEffectiveCharge.AccumulateRunningSum(value.fEffectiveCharge);
  for (i = 0; i < 2; i++) fRelPos[i].AccumulateRunningSum(value.fRelPos[i]);
  for (i = 0; i < 2; i++) fAbsPos[i].AccumulateRunningSum(value.fAbsPos[i]);
  return;
}

void QwBPMCavity::DeaccumulateRunningSum(VQwBPM &value){
  DeaccumulateRunningSum(*dynamic_cast<QwBPMCavity* >(&value));
};

void QwBPMCavity::DeaccumulateRunningSum(QwBPMCavity& value)
{
  Short_t i = 0;
  for(i=0;i<2;i++) fWire[i].DeaccumulateRunningSum(value.fWire[i]);
  fEffectiveCharge.DeaccumulateRunningSum(value.fEffectiveCharge);
  for (i = 0; i < 2; i++) fRelPos[i].DeaccumulateRunningSum(value.fRelPos[i]);
  for (i = 0; i < 2; i++) fAbsPos[i].DeaccumulateRunningSum(value.fAbsPos[i]);
  return;
}




void  QwBPMCavity::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }  else {
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;

    if(prefix=="asym_")
      thisprefix="diff_";
    SetRootSaveStatus(prefix);
    Short_t i = 0;
    for(i=kXAxis;i<kNumAxes;i++) {
      if(bFullSave) fWire[i].ConstructHistograms(folder, thisprefix);
      fRelPos[i].ConstructHistograms(folder, thisprefix);
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
    fEffectiveCharge.FillHistograms();
    Short_t i = 0;
    for(i=kXAxis;i<kNumAxes;i++){
      if (bFullSave) fWire[i].FillHistograms();
      fRelPos[i].FillHistograms();
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
    if(prefix=="asym_")
      thisprefix="diff_";

    SetRootSaveStatus(prefix);

    fEffectiveCharge.ConstructBranchAndVector(tree,prefix,values);
    Short_t i = 0;
    for(i=kXAxis;i<kNumAxes;i++) {
      if (bFullSave) fWire[i].ConstructBranchAndVector(tree,thisprefix,values);
      fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
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
     if(prefix=="asym_")
       thisprefix="diff_";

     SetRootSaveStatus(prefix);

     fEffectiveCharge.ConstructBranch(tree,prefix);
     Short_t i = 0;
     for(i=kXAxis;i<kNumAxes;i++) {
       if (bFullSave) fWire[i].ConstructBranch(tree,thisprefix);
       fRelPos[i].ConstructBranch(tree,thisprefix);
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
       if(prefix=="asym_")
         thisprefix="diff_";

       SetRootSaveStatus(prefix);

       fEffectiveCharge.ConstructBranch(tree,prefix);
       Short_t i = 0;
       for(i=kXAxis;i<kNumAxes;i++) {
	 if (bFullSave) fWire[i].ConstructBranch(tree,thisprefix);
	 fRelPos[i].ConstructBranch(tree,thisprefix);
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
    fEffectiveCharge.FillTreeVector(values);
    Short_t i = 0;
    for(i=kXAxis;i<kNumAxes;i++){
      if (bFullSave) fWire[i].FillTreeVector(values);
      fRelPos[i].FillTreeVector(values);
      fAbsPos[i].FillTreeVector(values);
    }
  }
  return;
}

void QwBPMCavity::SetEventCutMode(Int_t bcuts)
{
  Short_t i = 0;
  //  bEVENTCUTMODE=bcuts;
  for (i=0;i<2;i++) {
    fWire[i].SetEventCutMode(bcuts);
    fRelPos[i].SetEventCutMode(bcuts);
    fAbsPos[i].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);
}


void QwBPMCavity::MakeBPMCavityList()
{
  for (size_t i = kXAxis; i < kNumAxes; i++) {
	QwVQWK_Channel relpos(fRelPos[i]);
	relpos = fRelPos[i];
	fBPMElementList.push_back(relpos);
  }
  QwVQWK_Channel effectivecharge(fEffectiveCharge);
  effectivecharge = fEffectiveCharge;
  fBPMElementList.push_back(effectivecharge);
}

#ifdef __USE_DATABASE__
std::vector<QwDBInterface> QwBPMCavity::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  for(size_t i=0;i<2;i++) {
    fRelPos[i].AddEntriesToList(row_list);
    fAbsPos[i].AddEntriesToList(row_list);
  }
  fEffectiveCharge.AddEntriesToList(row_list);
  return row_list;
}


std::vector<QwErrDBInterface> QwBPMCavity::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  for(size_t i=0;i<2;i++) {
    fRelPos[i].AddErrEntriesToList(row_list);
    fAbsPos[i].AddErrEntriesToList(row_list);
  }
  fEffectiveCharge.AddErrEntriesToList(row_list);
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
  //fWire[0].SetRandomEventParameters(meanXP, sigmaXM);
  //fWire[1].SetRandomEventParameters(meanXM, sigmaYM);
  //fWire[2].SetRandomEventParameters(meanYP, sigmaYP);
}


void QwBPMCavity::RandomizeEventData(int helicity, double time)
{
  for (Short_t i=0; i<2; i++) fWire[i].RandomizeEventData(helicity, time);

  return;
}


void QwBPMCavity::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  for (Short_t i=0; i<2; i++)
    {
      fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
}


void QwBPMCavity::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (Short_t i=0; i<2; i++) fWire[i].EncodeEventData(buffer);
}


void QwBPMCavity::SetDefaultSampleSize(Int_t sample_size)
{
  for(Short_t i=0;i<2;i++) fWire[i].SetDefaultSampleSize((size_t)sample_size);
  return;
}


void QwBPMCavity::SetSubElementPedestal(Int_t j, Double_t value)
{
  fWire[j].SetPedestal(value);
  return;
}

void QwBPMCavity::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fWire[j].SetCalibrationFactor(value);
  return;
}

