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

#include "THaCodaFile.h"

#ifndef STANDALONE
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(THaCodaFile)
#endif
#endif

//Constructors

  THaCodaFile::THaCodaFile() {       // do nothing (must open file separately)
       ffirst = 0;
       init(" no name ");
  }
  THaCodaFile::THaCodaFile(TString fname) {
       ffirst = 0;
       init(fname);
       codaOpen(fname.Data(),"r");       // read only
  }
  THaCodaFile::THaCodaFile(TString fname, TString readwrite) {
       ffirst = 0;
       init(fname);
       codaOpen(fname.Data(),readwrite.Data());  // pass read or write flag
  }

//Destructor
  THaCodaFile::~THaCodaFile () {
       codaClose();
       staterr("close",fStatus);
  };

  int THaCodaFile::codaOpen(TString fname) {
       init(fname);
       char rw[] ="r";
       // Introduce "char rw[]" to suppress  "warning: deprecated conversion from string constant to 'char*'"
       // due to the a bit new gcc version 4.3.2
       fStatus = evOpen((char*)fname.Data(),rw, &handle);
       staterr("open",fStatus);
       return fStatus;
  };

  int THaCodaFile::codaOpen(TString fname, TString readwrite) {
      init(fname);
      fStatus = evOpen((char*)fname.Data(),(char*)readwrite.Data(),&handle);
      staterr("open",fStatus);
      return fStatus;
  };


  int THaCodaFile::codaClose() {
// Close the file. Do nothing if file not opened.
    if( handle ) {
      fStatus = evClose(handle);
      handle = NULL;
      return fStatus;
    }
    fStatus = CODA_OK;
    return fStatus;
  }


  int THaCodaFile::codaRead() {
// codaRead: Reads data from file, stored in evbuffer.
// Must be called once per event.
    if ( handle ) {
       fStatus = evRead(handle, evbuffer, MAXEVLEN);
       staterr("read",fStatus);
       if (fStatus != S_SUCCESS) {
  	  if (fStatus == EOF) return fStatus;  // ok, end of file
          fStatus = CODA_ERROR;
       }
    } else {
      if(CODA_VERBOSE) {
         std::cout << "codaRead ERROR: tried to access a file with handle = NULL" << std::endl;
         std::cout << "You need to call codaOpen(filename)" << std::endl;
         std::cout << "or use the constructor with (filename) arg" << std::endl;
      }
      fStatus = CODA_ERROR;
    }
    return fStatus;
  };



  int THaCodaFile::codaWrite(int *evbuffer) {
// codaWrite: Writes data to file
     if ( handle ) {
       fStatus = evWrite(handle, evbuffer);
       staterr("write",fStatus);
     } else {
       std::cout << "codaWrite ERROR: tried to access file with handle = NULL" << std::endl;
       fStatus = CODA_ERROR;
     }
     return fStatus;
   };



  int* THaCodaFile::getEvBuffer() {
// Here's how to get raw event buffer, evbuffer, after codaRead call
      return evbuffer;
  }


  int THaCodaFile::filterToFile(TString output_file) {
// A call to filterToFile filters from present file to output_file
// using filter criteria defined by evtypes, evlist, and max_to_filt
// which are loaded by public methods of this class.  If no conditions
// were loaded, it makes a copy of the input file (i.e. no filtering).

       int i;
       if(output_file == filename) {
	 if(CODA_VERBOSE) {
           std::cout << "filterToFile: ERROR: ";
           std::cout << "Input and output files cannot be same " << std::endl;
           std::cout << "This is to protect you against overwriting data" << std::endl;
         }
         return CODA_ERROR;
       }
       FILE *fp;
       if ((fp = fopen(output_file.Data(),"r")) != NULL) {
          if(CODA_VERBOSE) {
  	    std::cout << "filterToFile:  ERROR:  ";
            std::cout << "Output file `" << output_file << "' exists " << std::endl;
            std::cout << "You must remove it by hand first. " << std::endl;
            std::cout << "This forces you to think and not overwrite data." << std::endl;
	  }
          fclose(fp);
          return CODA_ERROR;
       }
       THaCodaFile* fout = new THaCodaFile(output_file.Data(),"w");
       int nfilt = 0;

       while (codaRead() == S_SUCCESS) {
           int* rawbuff = getEvBuffer();
           int evtype = rawbuff[1]>>16;
           int evnum = rawbuff[4];
           int oktofilt = 1;
           if (CODA_DEBUG) {
	     std::cout << "Input evtype " << std::dec << evtype;
             std::cout << "  evnum " << evnum << std::endl;
             std::cout << "max_to_filt = " << max_to_filt << std::endl;
             std::cout << "evtype size = " << evtypes[0] << std::endl;
             std::cout << "evlist size = " << evlist[0] << std::endl;
	   }
           if ( evtypes[0] > 0 ) {
               oktofilt = 0;
               for (i=1; i<=evtypes[0]; i++) {
                   if (evtype == evtypes[i]) {
                       oktofilt = 1;
                       goto Cont1;
	 	   }
	       }
	   }
Cont1:
           if ( evlist[0] > 0 ) {
               oktofilt = 0;
               for (i=1; i<=evlist[0]; i++) {
                   if (evnum == evlist[i]) {
                       oktofilt = 1;
                       goto Cont2;
		   }
	       }
	   }
Cont2:
	   if (oktofilt) {
             nfilt++;
             if (CODA_DEBUG) {
	       std::cout << "Filtering event, nfilt " << std::dec << nfilt << std::endl;
	     }
	     fStatus = fout->codaWrite(getEvBuffer());
             if (fStatus != S_SUCCESS) {
	       if (CODA_VERBOSE) {
		 std::cout << "Error in filterToFile ! " << std::endl;
                 std::cout << "codaWrite returned status " << fStatus << std::endl;
	       }
               goto Finish;
	     }
             if (max_to_filt > 0) {
    	        if (nfilt == max_to_filt) {
                  goto Finish;
	        }
	     }
	   }
       }
Finish:
       delete fout;
       return S_SUCCESS;
  };



  void THaCodaFile::addEvTypeFilt(int evtype_to_filt)
// Function to set up filtering by event type
  {
     int i;
     initFilter();
     if (evtypes[0] >= maxftype-1) {
        TArrayI temp = evtypes;
        maxftype = maxftype + 100;
        evtypes.Set(maxftype);
        for (i=0; i<=temp[0]; i++) evtypes[i]=temp[i];
        temp.~TArrayI();
     }
     evtypes[0] = evtypes[0] + 1;  // 0th element = num elements in list
     int n = evtypes[0];
     evtypes[n] = evtype_to_filt;
     return;
  };


  void THaCodaFile::addEvListFilt(int event_num_to_filt)
// Function to set up filtering by list of event numbers
  {
     int i;
     initFilter();
     if (evlist[0] >= maxflist-1) {
        TArrayI temp = evlist;
        maxflist = maxflist + 100;
        evlist.Set(maxflist);
        for (i=0; i<=temp[0]; i++) evlist[i]=temp[i];
        temp.~TArrayI();
     }
     evlist[0] = evlist[0] + 1;  // 0th element = num elements in list
     int n = evlist[0];
     evlist[n] = event_num_to_filt;
     return;
  };

  void THaCodaFile::setMaxEvFilt(int max_event)
// Function to set up the max number of events to filter
  {
     max_to_filt = max_event;
     return;
  };



void THaCodaFile::staterr(TString tried_to, unsigned int status) {
// staterr gives the non-expert user a reasonable clue
// of what the status returns from evio mean.
// Note: severe errors can cause job to exit(0)
// and the user has to pay attention to why.
    if (status == S_SUCCESS) return;  // everything is fine.
    if (tried_to == "open") {
       std::cout << "THaCodaFile: ERROR opening file = " << filename << std::endl;
       std::cout << "Most likely errors are: " << std::endl;
       std::cout << "   1.  You mistyped the name of file ?" << std::endl;
       std::cout << "   2.  The file has length zero ? " << std::endl;
    }
    switch (static_cast<Long64_t>(status)) {
      case S_EVFILE_TRUNC :
	 std::cout << "THaCodaFile ERROR:  Truncated event on file read" << std::endl;
         std::cout << "Evbuffer size is too small.  Job aborted." << std::endl;
         exit(0);  //  If this ever happens, recompile with MAXEVLEN
	           //  bigger, and mutter under your breath at the author.
      case S_EVFILE_BADBLOCK :
        std::cout << "Bad block number encountered " << std::endl;
        break;
      case S_EVFILE_BADHANDLE :
        std::cout << "Bad handle (file/stream not open) " << std::endl;
        break;
      case S_EVFILE_ALLOCFAIL :
        std::cout << "Failed to allocate event I/O" << std::endl;
        break;
      case S_EVFILE_BADFILE :
        std::cout << "File format error" << std::endl;
        break;
      case S_EVFILE_UNKOPTION :
        std::cout << "Unknown option specified" << std::endl;
        break;
      case S_EVFILE_UNXPTDEOF :
        std::cout << "Unexpected end of file while reading event" << std::endl;
        break;
      case EOF:
        if(CODA_VERBOSE) {
          std::cout << "Normal end of file " << filename << " encountered" << std::endl;
	}
        break;
      default:
        std::cout << "Error status  0x" << std::hex << status << std::endl;
      }
  };

  void THaCodaFile::init(TString fname) {
    handle = NULL;
    filename = fname;
  };

  void THaCodaFile::initFilter() {
    if (!ffirst) {
       ffirst = 1;
       maxflist = 100;
       maxftype = 100;
       evlist.Set(maxflist);
       evtypes.Set(maxftype);
       evlist[0] = 0;
       evtypes[0] = 0;
    }
  };
