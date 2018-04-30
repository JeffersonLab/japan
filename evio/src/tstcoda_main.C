// Test of THaCodaData abstract class
// THaCodaFile and THaEtClient both inherit from THaCodaData.
// R. Michaels, March 2001.

#include <iostream>
#include "THaCodaFile.h"
#include "THaEtClient.h"
#include "TString.h"
#include <signal.h>

//#define DUMPIT
//#define SPEEDTEST

void usage();
void do_something(int* data);
extern "C" void signalhandler (int s);
THaCodaData *coda;
int stop_it = 0;

int main(int argc, char* argv[])
{

  if (argc < 3) {  
     usage();
     return 1;
  }

  signal(31, signalhandler);  // kill -31 to end this

  int choice1 = atoi(argv[1]);
  int choice2 = atoi(argv[2]);
  int NUMEVT;
  NUMEVT =200;
  if (argc > 3) NUMEVT = atoi(argv[3]);

  if (choice1 == 1) {  // CODA File

// CODA file "snippet.dat" is a disk file of CODA data.  
      TString filename("snippet.dat");

      if (choice2 == 1) { // Two types of THaCodaFile constructors 
         coda = new THaCodaFile();
         if (coda->codaOpen(filename) != 0) {
            cout << "ERROR:  Cannot open CODA data" << endl;
            return 1;
         }
      } else {  // 2nd type of c'tor
         coda = new THaCodaFile(filename);
      }

  } else {         // Online ET connection

      int mymode = 1;
      TString mycomputer("adaql1");
      TString mysession("par1");

      if (choice2 == 1) {   // Three different types of constructor 
         coda = new THaEtClient();
         if (coda->codaOpen(mycomputer, mysession, mymode) != 0) {
            cout << "ERROR:  Cannot open ET connection" << endl;
            return 1;
          }
      } else if (choice2 == 2) {
         coda = new THaEtClient(mycomputer, mymode); 
      } else {
         coda = new THaEtClient(mycomputer, mysession, mymode);  
      }

  }

// Loop over events
  int status;

  for (int iev = 0; iev < NUMEVT; iev++) {

      status = coda->codaRead();  

      if (status != 0 || stop_it == 1) {
        if (stop_it) cout << "External kill signal seen " <<endl;
        if ( status == -1) {
           if (choice1 == 1) cout << "End of CODA file. Bye bye." << endl;
           if (choice1 == 2) cout << "CODA/ET not running. Bye bye." << endl;
        } else {
	   if (!stop_it) cout << "ERROR: codaRread status = " << hex << status << endl;
        }
        coda->codaClose();
        return 0;

      } else {
        
        do_something( coda->getEvBuffer() );

      }
  }

  cout << "closing CODA connection"<<endl<<flush;
  coda->codaClose();
  return 0;
};


void usage() {  
  cout << "Usage:  'tstcoda choice1 choice2' " << endl;
  cout << "\n where choice1 = " << endl;
  cout << "   1. open a CODA file and print data" << endl;
  cout << "   2. open an ET connection and print data "<<endl;
  cout << "\n and choice2 = " << endl;
  cout << "1 - 3 are different ways to open connection "<<endl;
  cout << "If CODA file, you have 2 choices "<<endl;
  cout << "If ET connection, you have 3 choices "<<endl;
};

void do_something (int* data) {
  int len = 0;
  len = data[0] + 1;
  int evtype = 0;
  evtype = data[1]>>16;
  int evnum = 0;
  evnum = data[4];
#ifdef DUMPIT
 // Crude event dump
  cout << "\n\n Event number " << dec << evnum;
  cout << " length " << len << " type " << evtype << endl;
  int ipt = 0;
  for (int j = 0; j < (len/5); j++) {
      cout << dec << "\n evbuffer[" << ipt << "] = ";
      for (int k=j; k<j+5; k++) {
    	 cout << hex << data[ipt++] << " ";
      }
      cout << endl;
  }
  if (ipt < len) {
      cout << dec << "\n evbuffer[" << ipt << "] = ";
      for (int k=ipt; k<len; k++) {
	 cout << hex << data[ipt++] << " ";
      }
      cout << endl;
  }
#endif
#ifdef SPEEDTEST
  static int dummy;
  dummy += evnum;
  if (evnum % 600 == 0) cout << "Event "<<evnum<<endl;
  if (evnum % 2400 == 0) cout << "dummy sum "<<dummy<<endl;
#endif
};

void signalhandler(int sig)
{  // To deal with the signal "kill -31 pid"
  cout << "Ending the online analysis"<<endl<<flush;
  stop_it = 1;

// No !  You don't want to do this here.  Instead send global signal.
//  int status = 0;
//  status = coda->codaRead();  
//  cout << "Read status "<<dec<<status<<endl<<flush;
//  coda->codaClose();
//  exit(1);

}




