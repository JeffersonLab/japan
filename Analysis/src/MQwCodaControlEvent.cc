#include "MQwCodaControlEvent.h"

#include "QwLog.h"

MQwCodaControlEvent::MQwCodaControlEvent(){
  ResetControlParameters();
}

MQwCodaControlEvent::~MQwCodaControlEvent() { }

void MQwCodaControlEvent::ResetControlParameters()
{
  fFoundControlEvents = kFALSE;
  fPrestartTime  = 0;
  fPrestartRunNumber = 0;
  fRunType       = 0;
  fEndTime       = 0;
  fEndEventCount = 0;
  fNumberPause   = 0;
  fPauseTime.clear();
  fPauseEventCount.clear();
  fNumberGo      = 0;
  fGoTime.clear();
  fGoEventCount.clear();
  fStartTime     = 0;
  fPrestartDatime.Set(UInt_t(0));
  fStartDatime.Set(UInt_t(0));
  fEndDatime.Set(UInt_t(0));
}

void MQwCodaControlEvent::ProcessControlEvent(UInt_t evtype, UInt_t* buffer){
  UInt_t local_time;
  UInt_t local_evcount;

  local_time      = buffer[0];
  local_evcount   = buffer[2];
  if (evtype==kSYNC_EVENT){
    UInt_t local_status = buffer[1];
    ProcessSync(local_time, local_status);
  } else if (evtype==kPRESTART_EVENT){
    UInt_t local_runnumber = buffer[1];
    UInt_t local_runtype   = buffer[2];
    ProcessPrestart(local_time, local_runnumber, local_runtype);
  } else if (evtype==kGO_EVENT){
    ProcessGo(local_time, local_evcount);
  } else if (evtype==kPAUSE_EVENT){
    ProcessPause(local_time, local_evcount);
  } else if (evtype==kEND_EVENT){
    ProcessEnd(local_time, local_evcount);
  } else {
    //  This isn't a control event.
    //  Do nothing.
  }
}


void MQwCodaControlEvent::ProcessSync(UInt_t local_time, UInt_t statuscode)
{
  fFoundControlEvents = kTRUE;
  // To be implemented...
}


void MQwCodaControlEvent::ProcessPrestart(UInt_t local_time, UInt_t local_runnumber,
				     UInt_t local_runtype)
{
  fFoundControlEvents = kTRUE;
  //
  ResetControlParameters();
  //
  fPrestartTime = local_time;
  fPrestartRunNumber    = local_runnumber;
  fRunType      = local_runtype;
  fPrestartDatime.Set(fPrestartTime);
}

void MQwCodaControlEvent::ProcessPause(UInt_t local_time, UInt_t evt_count)
{
  fFoundControlEvents = kTRUE;
  //
  fNumberPause++;
  fPauseEventCount.push_back(evt_count);
  fPauseTime.push_back(local_time);
}

void MQwCodaControlEvent::ProcessGo(UInt_t local_time, UInt_t evt_count)
{
  fFoundControlEvents = kTRUE;
  //
  fNumberGo++;
  fGoEventCount.push_back(evt_count);
  fGoTime.push_back(local_time);
  if (fNumberGo == 1){
    fStartTime = fGoTime[0];
    fStartDatime.Set(fStartTime);
  }
}

void MQwCodaControlEvent::ProcessEnd(UInt_t local_time, UInt_t evt_count)
{
  fFoundControlEvents = kTRUE;
  //
  fEndTime       = local_time;
  fEndEventCount = evt_count;
  fEndDatime.Set(fEndTime);
}


UInt_t MQwCodaControlEvent::GetGoTime(int index)
{
  if (index>=0 && index<(Int_t)fNumberGo) return fGoTime[index];
  return 0;
}

UInt_t MQwCodaControlEvent::GetGoEventCount(int index)
{
  if (index>=0 && index<(Int_t)fNumberGo) return fGoEventCount[index];
  return 0;
}

UInt_t MQwCodaControlEvent::GetPauseTime(int index)
{
  if (index>=0 && index<(Int_t)fNumberPause) return fPauseTime[index];
  return 0;
}

UInt_t MQwCodaControlEvent::GetPauseEventCount(int index)
{
  if (index>=0 && index<(Int_t)fNumberPause) return fPauseEventCount[index];
  return 0;
}


TString MQwCodaControlEvent::GetStartSQLTime()
{
  return fStartDatime.AsSQLString();
}

TString MQwCodaControlEvent::GetEndSQLTime()
{
  return fEndDatime.AsSQLString();
}


void MQwCodaControlEvent::ReportRunSummary()
{
  if (fFoundControlEvents){
    //  At least one control event has been found.
    //  Report the control event data we did find.
    Int_t i;
    QwMessage << "Run Number:         " << fPrestartRunNumber << QwLog::endl;
    QwMessage << "Run Type:           " << fRunType << QwLog::endl;
    QwMessage << "PreStart Time:      " << fPrestartTime << QwLog::endl;
    QwMessage << "Start Time:         " << fStartTime << QwLog::endl;
    QwMessage << "End Time:           " << fEndTime << QwLog::endl;
    QwMessage << "End Event Counter:  " << fEndEventCount << QwLog::endl;
    if (fEndTime>0 && fStartTime>0)
      QwMessage << "Run Duration (sec): " << fEndTime-fStartTime << QwLog::endl;
    else
      QwMessage << "Run Duration (sec): " << "n/a" << QwLog::endl;
    QwMessage << "SQL-Formatted Start Time: " << GetStartSQLTime()
	      << QwLog::endl; 
    QwMessage << "SQL-Formatted End Time:   " << GetEndSQLTime()
	      << QwLog::endl; 
    QwMessage << "Number of Pauses during this run: " << fNumberPause 
	      << QwLog::endl;
    for (i=0; i<(Int_t)fNumberPause; i++) {
      QwMessage << "Pause Number: " << i
		<<"; Events so far: " << fPauseEventCount[i] 
		<< "; Runtime since start (sec): " 
		<< fPauseTime[i]-fStartTime;
      if ((Int_t)fNumberGo > i+1){
	QwMessage << "; Duration of Pause (sec): " 
		  << fGoTime[i+1]-fPauseTime[i]
		  << QwLog::endl;
      }else {
	QwMessage << QwLog::endl;
      }
    }
  }
}


time_t MQwCodaControlEvent::GetStartUnixTime()
{
  return (time_t) GetStartTime();
}


time_t MQwCodaControlEvent::GetEndUnixTime()
{
  // if GetEndTime is equal to 0
  // we wiil see the end time as 
  //
  UInt_t end_time       = GetEndTime();
  UInt_t qweak_end_time = 1338523199;
  // 2012-05-31 23:59:59 in the MySQL database
  // $date -d@1338523199 
  // Thu May 31 23:59:59 EDT 2012 in a xterm (Linux)

  if(end_time) return (time_t) end_time;
  else         return (time_t) qweak_end_time;
}
