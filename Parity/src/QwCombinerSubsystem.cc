/*
 * QwCombinerSubsystem.cc
 *
 *  Created on: Aug 11, 2011
 *      Author: meeker
 */

#include "QwCombinerSubsystem.h"
#include "VQwSubsystemParity.h"
#include "QwCombiner.h"
#include "QwSubsystemArrayParity.h"
#include "QwParameterFile.h"

RegisterSubsystemFactory(QwCombinerSubsystem);


QwCombinerSubsystem::~QwCombinerSubsystem()
{
}

struct null_deleter { 
  void operator()(void const *) const { }
};

boost::shared_ptr<VQwSubsystem> QwCombinerSubsystem::GetSharedPointerToStaticObject(){
  boost::shared_ptr<VQwSubsystem> px(this, null_deleter());
  return px;
}

VQwSubsystem& QwCombinerSubsystem::operator=(VQwSubsystem* value)
{
  QwCombinerSubsystem* input= dynamic_cast<QwCombinerSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i < input->fDependentVar.size(); i++) {
      this->fOutputVar.at(i)->AssignValueFrom(input->fOutputVar.at(i));
    }
  }
  return *this;
}

VQwSubsystem& QwCombinerSubsystem::operator+=(VQwSubsystem* value)
{
  QwCombinerSubsystem* input = dynamic_cast<QwCombinerSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i < input->fDependentVar.size(); i++) {
      this->fOutputVar.at(i)->AddValueFrom(input->fOutputVar.at(i));
    }
  }
  return *this;
}

VQwSubsystem& QwCombinerSubsystem:: operator-=(VQwSubsystem* value)
{
  QwCombinerSubsystem* input = dynamic_cast<QwCombinerSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i<input->fDependentVar.size(); i++) {
      this->fOutputVar.at(i)->SubtractValueFrom(input->fOutputVar.at(i));
    }
  }
  return *this;
}

VQwSubsystem& QwCombinerSubsystem:: operator*=(VQwSubsystem* value)
{
  QwCombinerSubsystem* input = dynamic_cast<QwCombinerSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i<input->fDependentVar.size(); i++) {
      this->fOutputVar.at(i)->MultiplyBy(input->fOutputVar.at(i));
    }
  }
  return *this;
}

VQwSubsystem& QwCombinerSubsystem:: operator/=(VQwSubsystem* value)
{
  QwCombinerSubsystem* input = dynamic_cast<QwCombinerSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i<input->fDependentVar.size(); i++) {
      this->fOutputVar.at(i)->DivideBy(input->fOutputVar.at(i));
    }
  }
  return *this;
}


void QwCombinerSubsystem::Ratio(VQwSubsystem* value1, VQwSubsystem* value2)
{
  *this = value1;
  *this /= value2;
}


void QwCombinerSubsystem::Scale(Double_t value)
{ 
  for(size_t i = 0; i < this->fDependentVar.size(); i++)
  {
    this->fOutputVar.at(i)->Scale(value);
  }
  
};

void QwCombinerSubsystem::AccumulateRunningSum(VQwSubsystem* input, Int_t count, Int_t ErrorMask)
{
  QwCombinerSubsystem* value = dynamic_cast<QwCombinerSubsystem*> (input);
  if (value!=NULL){
    QwCombiner::AccumulateRunningSum(*value, count, ErrorMask);
  }
}

void QwCombinerSubsystem::DeaccumulateRunningSum(VQwSubsystem* input, Int_t ErrorMask)
{
  QwCombinerSubsystem* value = dynamic_cast<QwCombinerSubsystem*> (input);
  if (value!=NULL) {
    for (size_t i = 0; i < value-> fDependentVar.size(); i++) {
      fOutputVar.at(i)->DeaccumulateRunningSum(value->fOutputVar.at(i), ErrorMask);
    }
  }
}

void QwCombinerSubsystem::CalculateRunningAverage()
{
  QwCombiner::CalculateRunningAverage();
}

void QwCombinerSubsystem:: PrintValue() const{
  QwCombiner::PrintValue();
}


void QwCombinerSubsystem::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fOutputVar.at(i)->ConstructHistograms(folder,prefix);
  }
};

void QwCombinerSubsystem::FillHistograms()
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fOutputVar.at(i)->FillHistograms();
  }
};

void QwCombinerSubsystem::DeleteHistograms()
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    //    fOutputVar.at(i)->DeleteHistograms();
  }
};

void QwCombinerSubsystem::ConstructBranch(TTree *tree, TString & prefix)
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fOutputVar.at(i)->ConstructBranch(tree, prefix);
  }
};

void QwCombinerSubsystem::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp;
  QwParameterFile* nextmodule;
  trim_file.RewindToFileStart();
  tmp="Combiner";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for (size_t i = 0; i < fDependentVar.size(); i++){
      fOutputVar.at(i)->ConstructBranch(tree, prefix, *nextmodule);
    }
  }
};


void QwCombinerSubsystem::IncrementErrorCounters()
{
  /// TODO:  Write QwCombinerSubsystem::IncrementErrorCounters
}

void QwCombinerSubsystem::UpdateErrorFlag(const VQwSubsystem *ev_error){
  /// TODO:  Write QwCombinerSubsystem::UpdateErrorFlag
  //if (Compare(ev_error)){
  //QwCombinerSubsystem* input = dynamic_cast<QwCombinerSubsystem*> (ev_error);
  //}  
};


/// DERIVED FUNCTIONS /// 


/*  All of the functions below are using generic
 * returns for testing purposes. 
 */



Int_t QwCombinerSubsystem::LoadChannelMap(TString)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwCombinerSubsystem::LoadInputParameters(TString)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwCombinerSubsystem::LoadEventCuts(TString)
{
  Int_t sample = 0;
  return sample;
  
}

Int_t QwCombinerSubsystem::ProcessConfigurationBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwCombinerSubsystem::ProcessEvBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Int_t sample = 0;
  return sample;
}


Bool_t QwCombinerSubsystem::ApplySingleEventCuts()
{
  return true;
}

void QwCombinerSubsystem::PrintErrorCounters() const
{
}

UInt_t QwCombinerSubsystem::GetEventcutErrorFlag()
{
    return 0;
  
}




