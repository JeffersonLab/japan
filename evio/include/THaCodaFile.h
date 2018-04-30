#ifndef THaCodaFile_h
#define THaCodaFile_h

/////////////////////////////////////////////////////////////////////
//
//  THaCodaFile
//  File of CODA data
//
//  CODA data file, and facilities to open, close, read,
//  write, filter CODA data to disk files, etc.
//  A lot of this relies on the "evio.cpp" code from
//  DAQ group which is a C++ rendition of evio.c that
//  we have used for years, but here are some useful
//  added features.
//
//  author  Robert Michaels (rom@jlab.org)
//
/////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "THaCodaData.h"
#include <stdlib.h>
#include "evio.h"
#include "TString.h"
#include "TArrayI.h"
#include <iostream>

using namespace std;

class THaCodaFile : public THaCodaData
{

public:

  THaCodaFile();
  THaCodaFile(TString filename);
  THaCodaFile(TString filename, TString rw);
  ~THaCodaFile();
  using THaCodaData::codaOpen;
  int codaOpen(TString filename);
  int codaOpen(TString filename, TString rw);
  int codaClose();
  int codaRead();
  int codaWrite(int* evbuffer);
  int *getEvBuffer();
  int filterToFile(TString output_file);     // filter to an output file
  void addEvTypeFilt(int evtype_to_filt);    // add an event type to list
  void addEvListFilt(int event_to_filt);     // add an event num to list
  void setMaxEvFilt(int max_event);          // max num events to filter

private:

  THaCodaFile(const THaCodaFile &fn);
  THaCodaFile& operator=(const THaCodaFile &fn);
  void init(TString fname);
  void initFilter();
  void staterr(TString tried_to, int status);  // Can cause job to exit(0)
  int ffirst;
  int max_to_filt;
  EVFILE *handle;
  int maxflist,maxftype;
  TArrayI evlist, evtypes;

  #ifndef STANDALONE
     #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
        ClassDef(THaCodaFile,0)   //  File of CODA data
     #endif
  #endif

};

#endif






