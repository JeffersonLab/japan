#ifndef THaEtClient_
#define THaEtClient_

//////////////////////////////////////////////////////////////////////
//
//   THaEtClient
//   Data from ET Online System
//
//   THaEtClient contains normal CODA data obtained via
//   the ET (Event Transfer) online system invented
//   by the JLab DAQ group. 
//   This code works locally or remotely and uses the
//   ET system in a particular mode favored by  hall A.
//
//   Robert Michaels (rom@jlab.org)
//
/////////////////////////////////////////////////////////////////////

#include "THaCodaData.h"

#define ET_CHUNK_SIZE 150
#include "et.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>
#include <netdb.h>
#include "TString.h"

using namespace std;

// The ET memory file will have this prefix.  The suffix is $SESSION.
#define ETMEM_PREFIX "/tmp/et_sys_"

// Hall A computers that run CODA/ET
#define ADAQL1 "129.57.164.53"
#define ADAQL2 "129.57.164.59"
#define ADAQEP "129.57.164.78"
#define ADAQCP "129.57.164.79"
#define ADAQS2 "129.57.164.44"
#define ADAQS2 "129.57.164.44"
#define ADAQS3 "129.57.164.45"


class THaEtClient : public THaCodaData 
{

public:

    THaEtClient();                // Uses defaults
    THaEtClient(int mode);        // By default, gets data from ADAQS2
// find data on 'computer'.  e.g. computer="129.57.164.44"
    THaEtClient(TString computer, int mode);  
    THaEtClient(TString computer, TString session, int mode);  
    THaEtClient(TString computer, TString session, int mode, const TString stationname);  
    int codaOpen(TString computer);
    int codaOpen(TString computer, int mode);
    int codaOpen(TString computer, TString session, int mode);
    int codaClose();
    ~THaEtClient();
    int *getEvBuffer();        // Gets next event buffer after codaRead()
    int codaRead();            // codaRead() must be called once per event
    int getheartbeat();

private:

    THaEtClient(const THaEtClient &fn);
    THaEtClient& operator=(const THaEtClient &fn);
    int CHUNK;
    int DEBUG; 
    int FAST; 
    int SMALL_TIMEOUT; 
    int BIG_TIMEOUT; 
    int nread, nused, timeout;
    et_sys_id id;
    et_statconfig sconfig;
    et_stat_id my_stat;
    et_att_id my_att;
    et_openconfig openconfig;
    char *daqhost,*session,*etfile;
    int waitflag,initetfile,didclose,notopened,firstread;
    void initflags();
    int init();
    TString uniqueStation();
    int init(TString computer);

    TString fStationName;

    // use ClassDef if using rootcint
    #ifndef STANDALONE
       #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
          ClassDef(THaEtClient,0)   // ET client connection for online data
       #endif
    #endif

};

#endif








