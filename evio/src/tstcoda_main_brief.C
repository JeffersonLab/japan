// Test of THaCodaData abstract class
// THaCodaFile and THaEtClient both inherit from THaCodaData.
// R. Michaels, March 2001.

#include <iostream.h>
#include "THaCodaFile.h"
#include "THaEtClient.h"
#include "TString.h"

void usage();
void do_something(int* data);

int main(int argc, char* argv[])
{

  THaCodaData *coda;      // THaCodaData is abstract

  if (argc < 2) {
     usage();
     return 1;
  }

  int choice1 = atoi(argv[1]);

  if (choice1 == 1) {  // CODA File

// CODA file "snippet.dat" is a disk file of CODA data.  
     TString filename("snippet.dat");
     coda = new THaCodaFile();
     if (coda->codaOpen(filename) != 0) {
        cout << "ERROR:  Cannot open CODA data" << endl;
        return 1;
     }

  } else {         // Online ET connection

     int mymode = 1;  // 1=time-out mode (recommended)
     TString mycomputer("adaqcp");
     TString mysession("par1");

     coda = new THaEtClient();
     if (coda->codaOpen(mycomputer, mysession, mymode) != 0) {
        cout << "ERROR:  Cannot open ET connection" << endl;
        return 1;
     }
  }

// Loop over events
  int NUMEVT=200;
  int status;
  for (int iev = 0; iev < NUMEVT; iev++) {

     status = coda->codaRead();  

     if (status != 0) {
       if ( status == -1) {
          cout << "Normal end of CODA data. Bye bye." << endl;
       } else {
          cout << "ERROR: codaRread status = "<<hex<<status<<endl;
       }
       coda->codaClose();
       return 0;

     } else {

        do_something( coda->getEvBuffer() );

     }
  }
  coda->codaClose();

};


void usage() {  
  cout << "Usage:  'tstcoda choice' " << endl;
  cout << "\n where choice = " << endl;
  cout << "   1. open a CODA file and print data" << endl;
  cout << "   2. open an ET connection and print data "<<endl;
};

void do_something (int* data) {
 // Crude event dump
  int len = data[0] + 1;
  int evtype = data[1]>>16;
  int evnum = data[4];
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
};





