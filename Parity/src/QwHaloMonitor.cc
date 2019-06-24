/**********************************************************\
* File: QwHaloMonitor.cc                                  *
*                                                         *
* Author:B. Waidyawansa                                   *
* Time-stamp:24-june-2010                                 *
\**********************************************************/

#include "QwHaloMonitor.h"

// System headers
#include <stdexcept>

// Qweak headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif // __USE_DATABASE__

void  QwHaloMonitor::InitializeChannel(TString subsystem, TString name){
  fHalo_Counter.InitializeChannel(name);
  SetElementName(name);
}

void  QwHaloMonitor::InitializeChannel(TString name){
  fHalo_Counter.InitializeChannel(name);
  SetElementName(name);
}

void QwHaloMonitor::ClearEventData()
{
  fHalo_Counter.ClearEventData();
}

void QwHaloMonitor::ProcessEvent()
{
  // no processing required for the halos as they are just counters(?).
  fHalo_Counter.ProcessEvent();
}

Int_t QwHaloMonitor::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,UInt_t index)
{
  return fHalo_Counter.ProcessEvBuffer(buffer,num_words_left);
}


Bool_t QwHaloMonitor::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;
  return eventokay;
}


Bool_t QwHaloMonitor::ApplySingleEventCuts()
{
  return fHalo_Counter.ApplySingleEventCuts();
}


void QwHaloMonitor::PrintErrorCounters() const
{
  fHalo_Counter.PrintErrorCounters();
}


QwHaloMonitor& QwHaloMonitor::operator= (const QwHaloMonitor &value)
{
  if (GetElementName()!=""){
    this->fHalo_Counter=value.fHalo_Counter;
  }
  return *this;
}

QwHaloMonitor& QwHaloMonitor::operator+= (const QwHaloMonitor &value)
{
  if (GetElementName()!=""){
    this->fHalo_Counter+=value.fHalo_Counter;
  }
  return *this;
}

QwHaloMonitor& QwHaloMonitor::operator-= (const QwHaloMonitor &value)
{
  if (GetElementName()!=""){
    this->fHalo_Counter-=value.fHalo_Counter;
  }
  return *this;
}


void QwHaloMonitor::Sum(QwHaloMonitor &value1, QwHaloMonitor &value2){
  *this =  value1;
  *this += value2;
}

void QwHaloMonitor::Difference(QwHaloMonitor &value1, QwHaloMonitor &value2){
  *this =  value1;
  *this -= value2;
}

void QwHaloMonitor::Ratio(QwHaloMonitor &numer, QwHaloMonitor &denom)
{
  if (GetElementName()!=""){
      this->fHalo_Counter.Ratio(numer.fHalo_Counter,denom.fHalo_Counter);
  }
  return;
}

void QwHaloMonitor::Scale(Double_t factor)
{
  fHalo_Counter.Scale(factor);
}

void QwHaloMonitor::AccumulateRunningSum(const QwHaloMonitor& value, Int_t count, Int_t ErrorMask) {
  fHalo_Counter.AccumulateRunningSum(value.fHalo_Counter, count, ErrorMask);
}

void QwHaloMonitor::DeaccumulateRunningSum(QwHaloMonitor& value, Int_t ErrorMask) {
  fHalo_Counter.DeaccumulateRunningSum(value.fHalo_Counter, ErrorMask);
}

void QwHaloMonitor::CalculateRunningAverage(){
  fHalo_Counter.CalculateRunningAverage();
}


void QwHaloMonitor::PrintValue() const
{
  fHalo_Counter.PrintValue();
}

void QwHaloMonitor::PrintInfo() const
{
  std::cout << "QwVQWK_Channel Info " << std::endl;
  fHalo_Counter.PrintInfo();
}

Bool_t QwHaloMonitor::CheckForBurpFail(const VQwDataElement *ev_error){
  Short_t i=0;
  Bool_t burpstatus = kFALSE;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      //std::cout<<" Here in QwHaloMonitor::CheckForBurpFail \n";
      if (this->GetElementName()!="") {
        const QwHaloMonitor* value_halo = dynamic_cast<const QwHaloMonitor* >(ev_error);
        burpstatus |= fHalo_Counter.CheckForBurpFail(&(value_halo->fHalo_Counter)); 
      }
    } else {
      TString loc="Standard exception from QwHaloMonitor::CheckForBurpFail :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }
  return burpstatus;
}

void  QwHaloMonitor::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
      fHalo_Counter.ConstructHistograms(folder, prefix);
  }
}

void  QwHaloMonitor::FillHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.FillHistograms();
  }
}

void  QwHaloMonitor::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.ConstructBranchAndVector(tree, prefix,values);
    // this functions doesn't do anything yet
  }
}

void  QwHaloMonitor::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.ConstructBranch(tree, prefix);
    // this functions doesn't do anything yet
  }
}



void  QwHaloMonitor::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {

    //QwMessage <<" QwHaloMonitor "<<devicename<<QwLog::endl;
    if (modulelist.HasValue(devicename)){
      fHalo_Counter.ConstructBranch(tree, prefix);
      QwMessage <<" Tree leaf added to "<<devicename<<QwLog::endl;
    }
    // this functions doesn't do anything yet
  }
}




void  QwHaloMonitor::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.FillTreeVector(values);
    // this functions doesn't do anything yet
  }
}

#ifdef __USE_DATABASE__
std::vector<QwDBInterface> QwHaloMonitor::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  fHalo_Counter.AddEntriesToList(row_list);
  return row_list;
}


std::vector<QwErrDBInterface> QwHaloMonitor::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  fHalo_Counter.AddErrEntriesToList(row_list);
  return row_list;
}
#endif // __USE_DATABASE__
