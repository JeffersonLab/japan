#ifndef __MQWCODACONTROLEVENT__
#define __MQWCODACONTROLEVENT__

#include <vector>
#include <iostream>
#include <Rtypes.h>
#include <ctime>
#include "TDatime.h"
#include "TString.h"

///
/// \ingroup QwAnalysis
class MQwCodaControlEvent
{
 public:
  MQwCodaControlEvent();
  ~MQwCodaControlEvent();

  void ResetControlParameters();
  void ProcessControlEvent(UInt_t evtype, UInt_t* buffer);
  void ReportRunSummary();

  UInt_t GetStartTime() {return fStartTime;};
  UInt_t GetPrestartTime() {return fPrestartTime;};
  UInt_t GetPrestartRunNumber() {return fPrestartRunNumber;};
  UInt_t GetRunType() {return fRunType;};

  UInt_t GetGoTime(int index = 0);
  UInt_t GetGoEventCount(int index = 0);

  UInt_t GetPauseTime(int index = 0);
  UInt_t GetPauseEventCount(int index = 0);

  UInt_t GetEndTime() {return fEndTime;};
  UInt_t GetEndEventCount() {return fEndEventCount;};

  TString GetStartSQLTime();
  TString GetEndSQLTime();

  time_t  GetStartUnixTime();
  time_t  GetEndUnixTime();
    

 protected:
  void ProcessSync(UInt_t local_time, UInt_t statuscode);
  void ProcessPrestart(UInt_t local_time, UInt_t local_runnumber,
		       UInt_t local_runtype);
  void ProcessGo(UInt_t local_time, UInt_t evt_count);
  void ProcessPause(UInt_t local_time, UInt_t evt_count);
  void ProcessEnd(UInt_t local_time, UInt_t evt_count);

 protected:
  enum EventTypes{
    kSYNC_EVENT        =  16,
    kPRESTART_EVENT    =  17,
    kGO_EVENT          =  18,
    kPAUSE_EVENT       =  19,
    kEND_EVENT         =  20
  };

 protected:
  Bool_t fFoundControlEvents;

  UInt_t fPrestartTime;
  UInt_t fPrestartRunNumber;
  UInt_t fRunType;

  UInt_t fEndTime;
  UInt_t fEndEventCount;
  
  UInt_t fNumberPause;
  std::vector<UInt_t> fPauseEventCount;
  std::vector<UInt_t> fPauseTime;

  UInt_t fNumberGo;
  std::vector<UInt_t> fGoEventCount;
  std::vector<UInt_t> fGoTime;
  UInt_t fStartTime;

  TDatime fPrestartDatime;
  TDatime fStartDatime;
  TDatime fEndDatime;
};


#endif
