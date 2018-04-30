// An ET Client that retreives the ET's heartbeat.
// B. Moffit


#include "THaEtClient.h"
#include <iostream>
#include <string>
#include <fstream>
#include <signal.h>

extern "C" void signalhandler (int s);
int stop_it = 0;

int main(int argc, char *argv[]) 
{
  
  signal(31, signalhandler);  // kill -31 to end this

  int mymode = 1;   // prefered mode for ET
  THaEtClient *et;
  et = new THaEtClient("adaql1", mymode);  // opens connection to adaqcp computer.

  string HBenv = "HEARTBEATFILE";
  char *cHBenv = getenv(HBenv.c_str());
  TString HBfile;
  if(cHBenv == 0) {
    cHBenv = "/adaqfs/halla/apar/feedback/etHeartbeat.dat";
  }

  ofstream fileout;
  while(stop_it==0) {
    fileout.open(cHBenv);
    int heartbeat = et->getheartbeat();
    cout << "ET Heartbeat: " << heartbeat << endl;
    fileout << heartbeat << endl << flush;
    fileout.close();
    sleep(5);
  }
  et->codaClose();
  
  return 1;
}

void signalhandler(int sig)
{  // To deal with the signal "kill -31 pid"
  cout << "Ending etHeartbeat"<<endl<<flush;
  stop_it = 1;
}
