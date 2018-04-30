/*
 * QwRegressionSubsystem.cc
 *
 *  Created on: Aug 11, 2011
 *      Author: meeker
 */

#include "QwRegressionSubsystem.h"
#include "VQwSubsystemParity.h"
#include "QwRegression.h"
#include "QwSubsystemArrayParity.h"
#include "QwParameterFile.h"

RegisterSubsystemFactory(QwRegressionSubsystem);


QwRegressionSubsystem::~QwRegressionSubsystem()
{
}

struct null_deleter { 
  void operator()(void const *) const { }
};

boost::shared_ptr<VQwSubsystem> QwRegressionSubsystem::GetSharedPointerToStaticObject(){
  boost::shared_ptr<VQwSubsystem> px(this, null_deleter());
  return px;
}

VQwSubsystem& QwRegressionSubsystem::operator=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input= dynamic_cast<QwRegressionSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i < input->fDependentVar.size(); i++) {
      this->fDependentVar.at(i).second->AssignValueFrom(input->fDependentVar.at(i).second);
    }
  }
  return *this;
}

VQwSubsystem& QwRegressionSubsystem::operator+=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i < input->fDependentVar.size(); i++) {
      this->fDependentVar.at(i).second->AddValueFrom(input->fDependentVar.at(i).second);
    }
  }
  return *this;
}

VQwSubsystem& QwRegressionSubsystem:: operator-=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i<input->fDependentVar.size(); i++) {
      this->fDependentVar.at(i).second->SubtractValueFrom(input->fDependentVar.at(i).second);
    }
  }
  return *this;
}

VQwSubsystem& QwRegressionSubsystem:: operator*=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i<input->fDependentVar.size(); i++) {
      this->fDependentVar.at(i).second->MultiplyBy(input->fDependentVar.at(i).second);
    }
  }
  return *this;
}

VQwSubsystem& QwRegressionSubsystem:: operator/=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(value);
  if (input!=NULL) {
    for(size_t i = 0; i<input->fDependentVar.size(); i++) {
      this->fDependentVar.at(i).second->DivideBy(input->fDependentVar.at(i).second);
    }
  }
  return *this;
}


void QwRegressionSubsystem::Sum(VQwSubsystem* value1, VQwSubsystem* value2)
{
  *this = value1;
  *this += value2;
}


void QwRegressionSubsystem::Difference(VQwSubsystem* value1, VQwSubsystem* value2)
{
  *this = value1;
  *this -= value2;
}


void QwRegressionSubsystem::Ratio(VQwSubsystem* value1, VQwSubsystem* value2)
{
  *this = value1;
  *this /= value2;
}


void QwRegressionSubsystem::Scale(Double_t value)
{ 
  for(size_t i = 0; i < this->fDependentVar.size(); i++)
  {
    this->fDependentVar.at(i).second->Scale(value);
  }
  
};

void QwRegressionSubsystem::AccumulateRunningSum(VQwSubsystem* input)
{
  QwRegressionSubsystem* value = dynamic_cast<QwRegressionSubsystem*> (input);
  if (value!=NULL){
    QwRegression::AccumulateRunningSum(*value);
  }
}

void QwRegressionSubsystem::DeaccumulateRunningSum(VQwSubsystem* input)
{
  QwRegressionSubsystem* value = dynamic_cast<QwRegressionSubsystem*> (input);
  if (value!=NULL) {
    for (size_t i = 0; i < value-> fDependentVar.size(); i++) {
      fDependentVar.at(i).second->DeaccumulateRunningSum(value->fDependentVar.at(i).second);
    }
  }
}

void QwRegressionSubsystem::CalculateRunningAverage()
{
  QwRegression::CalculateRunningAverage();
}

void QwRegressionSubsystem:: PrintValue() const{
  QwRegression::PrintValue();
}


void QwRegressionSubsystem::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fDependentVar.at(i).second->ConstructHistograms(folder,prefix);
  }
};

void QwRegressionSubsystem::FillHistograms()
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fDependentVar.at(i).second->FillHistograms();
  }
};

void QwRegressionSubsystem::DeleteHistograms()
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    //    fDependentVar.at(i).second->DeleteHistograms();
  }
};

void QwRegressionSubsystem::ConstructBranch(TTree *tree, TString & prefix)
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fDependentVar.at(i).second->ConstructBranch(tree, prefix);
  }
};

void QwRegressionSubsystem::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp;
  QwParameterFile* nextmodule;
  trim_file.RewindToFileStart();
  tmp="Regression";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for (size_t i = 0; i < fDependentVar.size(); i++){
      fDependentVar.at(i).second->ConstructBranch(tree, prefix, *nextmodule);
    }
  }
};


void QwRegressionSubsystem::IncrementErrorCounters()
{
  /// TODO:  Write QwRegressionSubsystem::IncrementErrorCounters
}

void QwRegressionSubsystem::UpdateErrorFlag(const VQwSubsystem *ev_error){
  /// TODO:  Write QwRegressionSubsystem::UpdateErrorFlag
  //if (Compare(ev_error)){
  //QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*> (ev_error);
  //}  
};


/// DERIVED FUNCTIONS /// 


/*  All of the functions below are using generic
 * returns for testing purposes. 
 */



Int_t QwRegressionSubsystem::LoadChannelMap(TString)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwRegressionSubsystem::LoadInputParameters(TString)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwRegressionSubsystem::LoadEventCuts(TString)
{
  Int_t sample = 0;
  return sample;
  
}

Int_t QwRegressionSubsystem::ProcessConfigurationBuffer(UInt_t, UInt_t, UInt_t*, UInt_t)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwRegressionSubsystem::ProcessEvBuffer(UInt_t, UInt_t, UInt_t*, UInt_t)
{
  Int_t sample = 0;
  return sample;
}


Bool_t QwRegressionSubsystem::ApplySingleEventCuts()
{
  return true;
}

void QwRegressionSubsystem::PrintErrorCounters() const
{
}

UInt_t QwRegressionSubsystem::GetEventcutErrorFlag()
{
    return 0;
  
}




