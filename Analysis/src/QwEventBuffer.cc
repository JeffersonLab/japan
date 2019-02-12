#include "QwEventBuffer.h"

#include "QwOptions.h"
#include "QwEPICSEvent.h"
#include "VQwSubsystem.h"
#include "QwSubsystemArray.h"

#include <TMath.h>

#include <vector>
#include <glob.h>

#include <csignal>
Bool_t globalEXIT;
void sigint_handler(int sig)
{
        std::cout << "handling signal no. " << sig << " ";
        std::cout << "(press ctrl-\\ to abort now)\n";
        globalEXIT=1;
}



#include "THaCodaFile.h"
#ifdef __CODA_ET
#include "THaEtClient.h"
#endif

std::string QwEventBuffer::fDefaultDataFileStem = "QwRun_";
std::string QwEventBuffer::fDefaultDataFileExtension = "log";

const Int_t QwEventBuffer::kRunNotSegmented = -20;
const Int_t QwEventBuffer::kNoNextDataFile  = -30;
const Int_t QwEventBuffer::kFileHandleNotConfigured  = -40;

/// This is the ASCII character array 'NULL', and is used by the
/// DAQ to indicate a known empty buffer.
const UInt_t QwEventBuffer::kNullDataWord = 0x4e554c4c;


/// Default constructor
QwEventBuffer::QwEventBuffer()
  :    fRunListFile(NULL),
       fDataFileStem(fDefaultDataFileStem),
       fDataFileExtension(fDefaultDataFileExtension),
       fEvStreamMode(fEvStreamNull),
       fEvStream(NULL),
       fCurrentRun(-1),
       fRunIsSegmented(kFALSE),
       fPhysicsEventFlag(kFALSE),
       fEvtNumber(0),
       fNumPhysicsEvents(0)
{
  //  Set up the signal handler.
  globalEXIT=0;
  signal(SIGINT,  sigint_handler);// ctrl+c
  signal(SIGTERM, sigint_handler);// kill in shell // 15
  //  signal(SIGTSTP, sigint_handler);// ctrl+z // 20

  fDataDirectory = getenv_safe("QW_DATA");
  if (fDataDirectory.Length() == 0){
    QwError << "ERROR:  Can't get the data directory in the QwEventBuffer creator."
	    << QwLog::endl;
  } else if (! fDataDirectory.EndsWith("/")) {
      fDataDirectory.Append("/");
  }

  fCleanParameter[0] = 0.0;
  fCleanParameter[1] = 0.0;
  fCleanParameter[2] = 0.0;
}

/**
 * Defines configuration options for QwEventBuffer class using QwOptions
 * functionality.
 *
 * @param options Options object
 */
void QwEventBuffer::DefineOptions(QwOptions &options)
{
  // Define the execution options
  options.AddDefaultOptions()
    ("online", po::value<bool>()->default_bool_value(false),
     "use online data stream");
  options.AddDefaultOptions()
    ("online.RunNumber", po::value<int>()->default_bool_value(0),
     "Effective run number to be used by online system to find the parameter files");
  options.AddDefaultOptions()
    ("run,r", po::value<string>()->default_value("0:0"),
     "run range in format #[:#]");
  options.AddDefaultOptions()
    ("runlist", po::value<string>()->default_value(""),
     "run list file example \n[5253]\n 234\n 246\n 256\n 345:456\n 567:789\n [5259]\n [5260]\n 0:10000\n [5261:5270]\n 9000:10000\n- for run 5253 it will analyze three individual events, and two event ranges \n- for run 5259 it will analyze the entire run (all segments) \n- for run 5260 it will analyze the first 10000 events \n- for runs 5261 through 5270 it will analyze the events 9000 through 10000)");
  options.AddDefaultOptions()
    ("event,e", po::value<string>()->default_value("0:"),
     "event range in format #[:#]");
  options.AddDefaultOptions()
    ("segment,s", po::value<string>()->default_value("0:"),
     "run segment range in format #[:#]");
  options.AddDefaultOptions()
    ("chainfiles", po::value<bool>()->default_bool_value(false),
     "chain file segments together, do not analyze them separately");
  options.AddDefaultOptions()
    ("codafile-stem", po::value<string>()->default_value(fDefaultDataFileStem),
     "stem of the input CODA filename");
  options.AddDefaultOptions()
    ("codafile-ext", po::value<string>()->default_value(fDefaultDataFileExtension),
     "extension of the input CODA filename");
  //  Special flag to allow sub-bank IDs less than 31
  options.AddDefaultOptions()
    ("allow-low-subbank-ids", po::value<bool>()->default_bool_value(false),
     "allow the sub-bank ids to be 31 or less, when using this flag, all ROCs must be sub-banked");
  //  Options specific to the ET clients
  options.AddOptions("ET system options")
    ("ET.hostname", po::value<string>(),
     "Name of the ET session's host machine --- Only used in online mode\nDefaults to the environment variable $HOSTNAME"); 
  options.AddOptions("ET system options")
    ("ET.session", po::value<string>(),
     "ET session name --- Only used in online mode\nDefaults to the environment variable $SESSION"); 
  options.AddOptions("ET system options")
    ("ET.station", po::value<string>(),
     "ET station name --- Only used in online mode"); 
}

void QwEventBuffer::ProcessOptions(QwOptions &options)
{
  fOnline     = options.GetValue<bool>("online");
  if (fOnline){
#ifndef __CODA_ET
    QwError << "Online mode will not work without the CODA libraries!"
	    << QwLog::endl;
    exit(EXIT_FAILURE);
#else
    if (options.HasValue("online.RunNumber")) {
      fCurrentRun = options.GetValue<int>("online.RunNumber");
    }
    if (options.HasValue("ET.station")) {
      fETStationName = options.GetValue<string>("ET.station");
    } else {
      fETStationName = "";
    }
    if (options.HasValue("ET.hostname")) {
      fETHostname = options.GetValue<string>("ET.hostname");
    } else {
      fETHostname = getenv("HOSTNAME");
    }
    if (options.HasValue("ET.session")) {
      fETSession = options.GetValue<string>("ET.session");
    } else {
      fETSession = getenv("SESSION");
    }
    if (fETHostname.Length() == 0 || fETSession.Length() == 0) {
      TString tmp = "";
      if (fETHostname == NULL || fETHostname.Length() == 0)
	tmp += " \"HOSTNAME\"";
      if (fETSession == NULL ||  fETSession.Length() == 0){
	if (tmp.Length() > 0)
	  tmp += " and";
	tmp += " ET \"SESSION\"";
      }
      QwError << "The" << tmp
	      << " variable(s) is(are) not defined in your environment.\n"
	      << "        This is needed to run the online analysis."
	      << QwLog::endl;
      exit(EXIT_FAILURE);
    }
#endif
  }
  fRunRange = options.GetIntValuePair("run");
  fEventRange = options.GetIntValuePair("event");
  fSegmentRange = options.GetIntValuePair("segment");
  fRunListFileName = options.GetValue<string>("runlist");
  fChainDataFiles = options.GetValue<bool>("chainfiles");
  fDataFileStem = options.GetValue<string>("codafile-stem");
  fDataFileExtension = options.GetValue<string>("codafile-ext");

  fAllowLowSubbankIDs = options.GetValue<bool>("allow-low-subbank-ids");

  // Open run list file
  if (fRunListFileName.size() > 0) {
    fRunListFile = new QwParameterFile(fRunListFileName);
    fEventListFile = 0;
    if (! GetNextRunRange()) {
      QwWarning << "No run range found in run list file: " << fRunListFile->GetLine() << QwLog::endl;
    }
  } else {
    fRunListFile = NULL;
    fEventListFile = NULL;
  }
}

void QwEventBuffer::PrintRunTimes()
{
  UInt_t nevents = fNumPhysicsEvents - fStartingPhysicsEvent;
  if (nevents==0) nevents=1;
  QwMessage << QwLog::endl
	    << "Analysis of run " << GetRunNumber() << QwLog::endl
	    << fNumPhysicsEvents << " physics events were processed"<< QwLog::endl
	    << "CPU time used:  " << fRunTimer.CpuTime() << " s "
	    << "(" << 1000.0 * fRunTimer.CpuTime() / nevents << " ms per event)" << QwLog::endl
	    << "Real time used: " << fRunTimer.RealTime() << " s "
	    << "(" << 1000.0 * fRunTimer.RealTime() / nevents << " ms per event)" << QwLog::endl
	    << QwLog::endl;
}



/// Read the next requested event range, return true if success
Bool_t QwEventBuffer::GetNextEventRange() {
  // If there is an event list, open the next section
  if (fEventListFile && !fEventListFile->IsEOF()) {
    std::string eventrange;
    // Find next non-whitespace, non-comment, non-empty line, before EOF
    do {
      fEventListFile->ReadNextLine(eventrange);
      fEventListFile->TrimWhitespace();
      fEventListFile->TrimComment('#');
    } while (fEventListFile->LineIsEmpty() && !fEventListFile->IsEOF());
    // If EOF return false; no next event range
    if (fEventListFile->IsEOF()) return kFALSE;
    // Parse the event range
    fEventRange = QwParameterFile::ParseIntRange(":",eventrange);
    QwMessage << "Next event range is " << eventrange << QwLog::endl;
    return kTRUE;
  }
  return kFALSE;
}

/// Read the next requested run range, return true if success
Bool_t QwEventBuffer::GetNextRunRange() {
  // Delete any open event list file before moving to next run
  if (fEventListFile) delete fEventListFile;
  // If there is a run list, open the next section
  std::string runrange;
  if (fRunListFile && !fRunListFile->IsEOF() &&
     (fEventListFile = fRunListFile->ReadNextSection(runrange))) {
    // Parse the run range
    fRunRange = QwParameterFile::ParseIntRange(":",runrange);
    QwMessage << "Next run range is " << runrange << QwLog::endl;
    // If there is no event range for this run range, set to default of 0:MAXINT
    if (! GetNextEventRange()) {
      QwWarning << "No valid event range in run list file: "
                << fEventListFile->GetLine() << ". "
                << "Assuming the full event range." << QwLog::endl;
      fEventRange = QwParameterFile::ParseIntRange(":","0:");
    }
    return kTRUE;
  }
  return kFALSE;
}

/// Get the next run in the active run range, proceed to next range if needed
Bool_t QwEventBuffer::GetNextRunNumber() {
  // First run
  if (fCurrentRun == -1) {
    fCurrentRun = fRunRange.first;
    return kTRUE;
  // Run is in range
  } else if (fCurrentRun < fRunRange.second) {
    fCurrentRun++;
    return kTRUE;
  // Run is not in range, get new range
  } else if (GetNextRunRange()) {
    fCurrentRun = fRunRange.first;
    return kTRUE;
  }
  return kFALSE;
}

TString QwEventBuffer::GetRunLabel() const
{
  TString runlabel = Form("%d",fCurrentRun);
  if (fRunIsSegmented && ! fChainDataFiles){
    runlabel += Form(".%03d",*fRunSegmentIterator);
  }
  return runlabel;
}

Int_t QwEventBuffer::ReOpenStream()
{
  Int_t status = CODA_ERROR;
  //  Reset the physics event counter
  fNumPhysicsEvents = fStartingPhysicsEvent;

  if (fOnline) {
    // Online stream
    status = OpenETStream(fETHostname, fETSession, 0, fETStationName);
  } else {
    // Offline data file
    if (fRunIsSegmented)
      // Segmented
      status = OpenNextSegment();
    else
      // Not segmented
      status = OpenDataFile(fCurrentRun);
  }
  return status;
}

Int_t QwEventBuffer::OpenNextStream()
{
  Int_t status = CODA_ERROR;
  if (globalEXIT==1) {
    //  We want to exit, so don't open the next stream.
    status = CODA_ERROR;
  } else if (fOnline) {
    /* Modify the call below for your ET system, if needed.
       OpenETStream( ET host name , $SESSION , mode)
       mode=0: wait forever
       mode=1: timeout quickly
    */
    QwMessage << "Try to open the ET station with HOSTNAME=="
	      << fETHostname
	      << ", SESSION==" << fETSession << "."
	      << QwLog::endl;
    status = OpenETStream(fETHostname, fETSession, 0, fETStationName);

  } else {
    //  Try to open the next data file for the current run,
    //  but only if we haven't hit the event limit.
    if (fCurrentRun != -1 && !fChainDataFiles
	&& fEvtNumber <= fEventRange.second) {
      status = OpenNextSegment();
    }
    while (status != CODA_OK && GetNextRunNumber()) {
      status = OpenDataFile(fCurrentRun);
      if (status == CODA_ERROR){
	//  The data file can't be opened.
	//  Get ready to process the next run.
	QwError << "ERROR:  Unable to find data files for run "
		<< fCurrentRun << ".  Moving to the next run.\n"
		<< QwLog::endl;
      }
    }

  }
  //  Grab the starting event counter
  fStartingPhysicsEvent = fNumPhysicsEvents;
  //  Start the timers.
  fRunTimer.Reset();
  fRunTimer.Start();
  fStopwatch.Start();
  return status;
}

Int_t QwEventBuffer::CloseStream()
{
  //  Stop the timers.
  fRunTimer.Stop();
  fStopwatch.Stop();
  QwWarning << "Starting CloseStream."
	    << QwLog::endl;
  Int_t status = kFileHandleNotConfigured;
  if (fEvStreamMode==fEvStreamFile
      && (fRunIsSegmented && !fChainDataFiles) ){
    //  The run is segmented and we are not chaining the
    //  segments together in the event loop, so close
    //  the current segment.
    status = CloseThisSegment();
  } else if (fEvStreamMode==fEvStreamFile) {
    status = CloseDataFile();
  } else if (fEvStreamMode==fEvStreamFile){
    status = CloseETStream();
  }
  return status;
}



Int_t QwEventBuffer::GetNextEvent()
{
  //  This will return for read errors,
  //  non-physics events, and for physics
  //  events that are within the event range.
  Int_t status = CODA_OK;
  do {
    status = GetEvent();
    if (globalEXIT == 1) {
      //  QUESTION:  Should we continue to loop once we've
      //  reached the maximum event, to allow access to
      //  non-physics events?
      //  For now, mock up EOF if we've reached the maximum event.
      status = EOF;
    }
    if (fEvtNumber > fEventRange.second) {
      do {
        if (GetNextEventRange()) status = CODA_OK;
        else status = EOF;
      } while (fEvtNumber < fEventRange.first);
    }
    //  While we're in a run segment which was not requested (which
    //  should happen only when reading the zeroth segment for startup
    //  information), pretend that there's an event cut causing us to
    //  ignore events.  Read configuration events only from the first
    //  part of the file.
    if (fRunIsSegmented && GetSegmentNumber() < fSegmentRange.first) {
      fEventRange.first = fEvtNumber + 1;
      if (fEvtNumber > 1000) status = EOF;
    }
  } while (status == CODA_OK  &&
           IsPhysicsEvent()   &&
           (fEvtNumber < fEventRange.first
         || fEvtNumber > fEventRange.second)
          );
  if (status == CODA_OK  && IsPhysicsEvent()) fNumPhysicsEvents++;

  //  Progress meter (this should probably produce less output in production)
  int nevents = 10000;
  if (IsPhysicsEvent() && fEvtNumber > 0 && fEvtNumber % nevents == 0) {
    QwMessage << "Processing event " << fEvtNumber << " ";
    fStopwatch.Stop();
    double efficiency = 100.0 * fStopwatch.CpuTime() / fStopwatch.RealTime();
    QwMessage << "(" << fStopwatch.CpuTime()*1e3/nevents << " ms per event with ";
    QwMessage << efficiency << "% efficiency)";
    fStopwatch.Reset();
    fStopwatch.Start();
    QwMessage << QwLog::endl;
  } else if (fEvtNumber > 0 && fEvtNumber % 100 == 0) {
    QwVerbose << "Processing event " << fEvtNumber << QwLog::endl;
  }

  return status;
}


Int_t QwEventBuffer::GetEvent()
{
  Int_t status = kFileHandleNotConfigured;
  ResetFlags();
  if (fEvStreamMode==fEvStreamFile){
    status = GetFileEvent();
  } else if (fEvStreamMode==fEvStreamET){
    status = GetEtEvent();
  }
  if (status == CODA_OK){
    DecodeEventIDBank((UInt_t*)(fEvStream->getEvBuffer()));
  }
  return status;
}

Int_t QwEventBuffer::GetFileEvent(){
  Int_t status = CODA_OK;
  //  Try to get a new event.  If the EOF occurs,
  //  and the run is segmented, try to open the
  //  next segment and read a new event; repeat
  //  if needed.
  do {
    status = fEvStream->codaRead();
    if (fChainDataFiles && status == EOF){
      CloseThisSegment();
      //  Crash out of the loop if we can't open the
      //  next segment!
      if (OpenNextSegment()!=CODA_OK) break;
    }
  } while (fChainDataFiles && status == EOF);
  return status;
}

Int_t QwEventBuffer::GetEtEvent(){
  Int_t status = CODA_OK;
  //  Do we want to have any loop here to wait for a bad
  //  read to be cleared?
  status = fEvStream->codaRead();
  if (status == CODA_EXIT)
    globalEXIT = 1;
  return status;
}


Int_t QwEventBuffer::WriteEvent(int* buffer)
{
  Int_t status = kFileHandleNotConfigured;
  ResetFlags();
  if (fEvStreamMode==fEvStreamFile){
    status = WriteFileEvent(buffer);
  } else if (fEvStreamMode==fEvStreamET) {
    QwMessage << "No support for writing to ET streams" << QwLog::endl;
    status = CODA_ERROR;
  }
  return status;
}

Int_t QwEventBuffer::WriteFileEvent(int* buffer)
{
  Int_t status = CODA_OK;
  //  fEvStream is of inherited type THaCodaData,
  //  but codaWrite is only defined for THaCodaFile.
  status = ((THaCodaFile*)fEvStream)->codaWrite(buffer);
  return status;
}


Int_t QwEventBuffer::EncodeSubsystemData(QwSubsystemArray &subsystems)
{
  // Encode the data in the elements of the subsystem array
  std::vector<UInt_t> buffer;
   subsystems.EncodeEventData(buffer);

  // Add CODA event header
  std::vector<UInt_t> header;
  header.push_back((0x0001 << 16) | (0x10 << 8) | 0xCC);
		// event type | event data type | event ID (0xCC for CODA event)
  header.push_back(4);	// size of header field
  header.push_back((0xC000 << 16) | (0x01 << 8) | 0x00);
		// bank type | bank data type (0x01 for uint32) | bank ID (0x00 for header event)
  header.push_back(++fEvtNumber); // event number (initialized to 0,
		// so increment before use to agree with CODA number)
  header.push_back(1);	// event class
  header.push_back(0);	// status summary

  // Copy the encoded event buffer into an array of integers,
  // as expected by the CODA routines.
  // Size of the event buffer in long words
  int* codabuffer = new int[header.size() + buffer.size() + 1];
  // First entry contains the buffer size
  int k = 0;
  codabuffer[k++] = header.size() + buffer.size();
  for (size_t i = 0; i < header.size(); i++)
    codabuffer[k++] = header.at(i);
  for (size_t i = 0; i < buffer.size(); i++)
    codabuffer[k++] = buffer.at(i);

  // Now write the buffer to the stream
  Int_t status = WriteEvent(codabuffer);
  // delete the buffer
  delete[] codabuffer;
  // and report success or fail
  return status;
}


Int_t QwEventBuffer::EncodePrestartEvent(int runnumber, int runtype)
{
  int buffer[5];
  int localtime = (int) time(0);
  buffer[0] = 4; // length
  buffer[1] = ((kPRESTART_EVENT << 16) | (0x01 << 8) | 0xCC);
  buffer[2] = localtime;
  buffer[3] = runnumber;
  buffer[4] = runtype;
  ProcessPrestart(localtime, runnumber, runtype);
  return WriteEvent(buffer);
}
Int_t QwEventBuffer::EncodeGoEvent()
{
  int buffer[5];
  int localtime = (int) time(0);
  int eventcount = 0;
  buffer[0] = 4; // length
  buffer[1] = ((kGO_EVENT << 16) | (0x01 << 8) | 0xCC);
  buffer[2] = localtime;
  buffer[3] = 0; // (unused)
  buffer[4] = eventcount;
  ProcessGo(localtime, eventcount);
  return WriteEvent(buffer);
}
Int_t QwEventBuffer::EncodePauseEvent()
{
  int buffer[5];
  int localtime = (int) time(0);
  int eventcount = 0;
  buffer[0] = 4; // length
  buffer[1] = ((kPAUSE_EVENT << 16) | (0x01 << 8) | 0xCC);
  buffer[2] = localtime;
  buffer[3] = 0; // (unused)
  buffer[4] = eventcount;
  ProcessPause(localtime, eventcount);
  return WriteEvent(buffer);
}
Int_t QwEventBuffer::EncodeEndEvent()
{
  int buffer[5];
  int localtime = (int) time(0);
  int eventcount = 0;
  buffer[0] = 4; // length
  buffer[1] = ((kEND_EVENT << 16) | (0x01 << 8) | 0xCC);
  buffer[2] = localtime;
  buffer[3] = 0; // (unused)
  buffer[4] = eventcount;
  ProcessEnd(localtime, eventcount);
  return WriteEvent(buffer);
}


void QwEventBuffer::ResetFlags(){
}


void QwEventBuffer::DecodeEventIDBank(UInt_t *buffer)
{
  UInt_t local_datatype;
  UInt_t local_eventtype;

  fPhysicsEventFlag = kFALSE;

  QwDebug << "QwEventBuffer::DecodeEventIDBank: " <<  std::hex
	  << buffer[0] << " "
	  << buffer[1] << " "
	  << buffer[2] << " "
	  << buffer[3] << " "
	  << buffer[4] << std::dec << " "
	  << QwLog::endl;
  
  if ( buffer[0] == 0 ){
    /*****************************************************************
     *  This buffer is empty.                                        *
     *****************************************************************/
    fEvtLength = (1);     //  Pretend that there is one word.
    fWordsSoFar = (1);      //  Mark that we've read the word already.
    fEvtType = (0);
    fEvtTag       = 0;
    fBankDataType = 0;
    fIDBankNum    = 0;
    fEvtNumber    = 0;
    fEvtClass     = 0;
    fStatSum      = 0;
  } else {
    /*****************************************************************
     *  This buffer contains data; fill the event ID parameters.     *
     *****************************************************************/
    //  First word is the number of long-words in the buffer.
    fEvtLength = (buffer[0]+1);

    // Second word contains the event type, for CODA events.
    fEvtTag   = (buffer[1] & 0xFFFF0000) >> 16;  // (bits(31-16));
    local_datatype = (buffer[1] & 0xFF00) >> 8;  // (bits(15-8));
    fIDBankNum = (buffer[1] & 0xFF);             // (bits(7-0));
    if ( fIDBankNum == 0xCC) {
      //  This is a CODA event bank; the event type is equal to
      //  the event tag.
      local_eventtype = fEvtTag;
      fEvtType = (local_eventtype);
      fBankDataType = local_datatype;

      // local_eventtype is unsigned int and always positive
      if (/* local_eventtype >= 0 && */ local_eventtype <= 15) {
        //  This is a physics event; record the event number, event
        //  classification, and status summary.
        fEvtNumber = buffer[4];
        fEvtClass  = buffer[5];
        fStatSum   = buffer[6];
	fPhysicsEventFlag = kTRUE;
        //  Now skip to the first ROC data bank.
        fWordsSoFar = (7);
      } else {
        //  This is not a physics event, but is still in the CODA
        //  event format.  The first two words have been examined.
        fEvtNumber = 0;
        fEvtClass  = 0;
        fStatSum   = 0;
        fWordsSoFar = (2);
	//  Run this event through the Control event processing.
	//  If it is not a control event, nothing will happen.
	ProcessControlEvent(fEvtType, &buffer[fWordsSoFar]);
      }
    } else {
      //  This is not an event in the CODA event bank format,
      //  but it still follows the CEBAF common event format.
      //  Arbitrarily set the event type to "fEvtTag".
      //  The first two words have been examined.
      fEvtType = (fEvtTag);
      fBankDataType = local_datatype;
      fEvtNumber = 0;
      fEvtClass  = 0;
      fStatSum   = 0;
      fWordsSoFar = (2);
    }
  }
  //  Initialize the fragment size to the event size, in case the 
  //  event is not subbanked.
  fFragLength = fEvtLength-fWordsSoFar;
  QwDebug << Form("buffer[0-1] 0x%x 0x%x ; ",
   		  buffer[0], buffer[1])
	  << Form("Length: %d; Tag: 0x%x; Bank data type: 0x%x; Bank ID num: 0x%x; ",
   		  fEvtLength, fEvtTag, fBankDataType, fIDBankNum)
   	  << Form("Evt type: 0x%x; Evt number %d; Evt Class 0x%.8x; ",
   		  fEvtType, fEvtNumber, fEvtClass)
   	  << Form("Status Summary: 0x%.8x; Words so far %d",
   		  fStatSum, fWordsSoFar)
	  << QwLog::endl;
}


Bool_t QwEventBuffer::FillSubsystemConfigurationData(QwSubsystemArray &subsystems)
{
  ///  Passes the data for the configuration events into each subsystem
  ///  object.  Each object is responsible for recognizing the configuration
  ///  data which it ought to decode.
  ///  NOTE TO DAQ PROGRAMMERS:
  ///      The configuration event for a ROC must have the same
  ///      subbank structure as the physics events for that ROC.
  Bool_t okay = kTRUE;
  UInt_t rocnum = fEvtType - 0x90;
  QwMessage << "QwEventBuffer::FillSubsystemConfigurationData:  "
	    << "Found configuration event for ROC"
	    << rocnum
	    << QwLog::endl;
  QwMessage << Form("Length: %d; Tag: 0x%x; Bank data type: 0x%x; Bank ID num: 0x%x; ",
		    fEvtLength, fEvtTag, fBankDataType, fIDBankNum)
	    << Form("Evt type: 0x%x; Evt number %d; Evt Class 0x%.8x; ",
		    fEvtType, fEvtNumber, fEvtClass)
	    << QwLog::endl;
  //  Loop through the data buffer in this event.
  UInt_t *localbuff = (UInt_t*)(fEvStream->getEvBuffer());
  DecodeEventIDBank(localbuff);
  while ((okay = DecodeSubbankHeader(&localbuff[fWordsSoFar]))){
    //  If this bank has further subbanks, restart the loop.
    if (fSubbankType == 0x10) {
      QwMessage << "This bank has further subbanks, restart the loop" << QwLog::endl;
      continue;
    }
    //  If this bank only contains the word 'NULL' then skip
    //  this bank.
    if (fFragLength==1 && localbuff[fWordsSoFar]==kNullDataWord){
      fWordsSoFar += fFragLength;
      QwMessage << "Skip this bank" << QwLog::endl;
      continue;
    }

    //  Subsystems may be configured to accept data in formats
    //  other than 32 bit integer (banktype==1), but the
    //  bank type is not provided.  Subsystems must be able
    //  to process their data knowing only the ROC and bank tags.
    //
    //  After trying the data in each subsystem, bump the
    //  fWordsSoFar to move to the next bank.

    subsystems.ProcessConfigurationBuffer(rocnum, fSubbankTag,
					  &localbuff[fWordsSoFar],
					  fFragLength);
    fWordsSoFar += fFragLength;
    QwDebug << "QwEventBuffer::FillSubsystemConfigurationData:  "
	    << "Ending loop: fWordsSoFar=="<<fWordsSoFar
	    <<QwLog::endl;
  }

  return okay;
}

Bool_t QwEventBuffer::FillSubsystemData(QwSubsystemArray &subsystems)
{
  //  Initialize local flag
  Bool_t okay = kTRUE;

  //  Reload the data buffer and decode the header again, this allows
  //  multiple calls to this function for different subsystem arrays.
  UInt_t *localbuff = (UInt_t*)(fEvStream->getEvBuffer());
  DecodeEventIDBank(localbuff);

  //  Clear the old event information from the subsystems.
  subsystems.ClearEventData();

  //  Pass CODA run, segment, event number and type to the subsystem array.
  subsystems.SetCodaRunNumber(fCurrentRun);
  subsystems.SetCodaSegmentNumber(fRunIsSegmented? *fRunSegmentIterator: 0);
  subsystems.SetCodaEventNumber(fEvtNumber);
  subsystems.SetCodaEventType(fEvtType);



  // If this event type is masked for the subsystem array, return right away
  if (((0x1 << (fEvtType - 1)) & subsystems.GetEventTypeMask()) == 0) {
    return kTRUE;
  }

  UInt_t offset;

  //  Loop through the data buffer in this event.
  while ((okay = DecodeSubbankHeader(&localbuff[fWordsSoFar]))){

    //  If this bank has further subbanks, restart the loop.
    if (fSubbankType == 0x10) continue;

    //  If this bank only contains the word 'NULL' then skip
    //  this bank.
    if (fFragLength == 1 && localbuff[fWordsSoFar]==kNullDataWord) {
      fWordsSoFar += fFragLength;
      continue;
    }

    // if (fSubbankType == 0x85) {
    //   std::cout << "ProcessEventBuffer: , SubbankTag= "<< fSubbankTag<<" FragLength="<<fFragLength <<std::endl;
    // }

//     QwDebug << "QwEventBuffer::FillSubsystemData:  "
// 	    << "Beginning loop: fWordsSoFar=="<<fWordsSoFar
// 	    <<QwLog::endl;

    //  Loop through the subsystems and try to store the data
    //  from this bank in each subsystem.
    //
    //  Subsystems may be configured to accept data in formats
    //  other than 32 bit integer (banktype==1), but the
    //  bank type is not provided.  Subsystems must be able
    //  to process their data knowing only the ROC and bank tags.
    //
    //  After trying the data in each subsystem, bump the
    //  fWordsSoFar to move to the next bank.

    if( fROC == 0 && fSubbankTag==0x6101) {
      //std::cout << "ProcessEventBuffer: ROC="<<fROC<<", SubbankTag="<< fSubbankTag<<", FragLength="<<fFragLength <<std::endl;
      fCleanParameter[0]=localbuff[fWordsSoFar+fFragLength-4];//clean data
      fCleanParameter[1]=localbuff[fWordsSoFar+fFragLength-3];//scan data 1
      fCleanParameter[2]=localbuff[fWordsSoFar+fFragLength-2];//scan data 2
      //std::cout << "ProcessEventBuffer: ROC="<<fROC<<", SubbankTag="<< fSubbankTag
      //		<<", FragLength="<<fFragLength << " " <<fCleanParameter[0]<< " " <<fCleanParameter[1]<< " " <<fCleanParameter[2]<<std::endl;

    }
    
    subsystems.SetCleanParameters(fCleanParameter);

    Int_t nmarkers = CheckForMarkerWords(subsystems);
    if (nmarkers>0) {
      //  There are markerwords for this ROC/Bank
      for (size_t i=0; i<nmarkers; i++){
	offset = FindMarkerWord(i,&localbuff[fWordsSoFar],fFragLength);
	BankID_t tmpbank = GetMarkerWord(i);
	tmpbank = ((tmpbank)<<32) + fSubbankTag;
	if (offset != -1){
	  offset++; //  Skip the marker word
	  subsystems.ProcessEvBuffer(fEvtType, fROC, tmpbank,
				     &localbuff[fWordsSoFar+offset],
				     fFragLength-offset);
	}
      }
    } else {
      QwDebug << "QwEventBuffer::FillSubsystemData:  "
	      << "fROC=="<<fROC << ", fSubbankTag==" << fSubbankTag
	      << QwLog::endl;	
      subsystems.ProcessEvBuffer(fEvtType, fROC, fSubbankTag,
				 &localbuff[fWordsSoFar],
				 fFragLength);
    }
    fWordsSoFar += fFragLength;
//     QwDebug << "QwEventBuffer::FillSubsystemData:  "
// 	    << "Ending loop: fWordsSoFar=="<<fWordsSoFar
// 	    <<QwLog::endl;
  }
  return okay;
}


// added all this method for QwEPICSEvent class
Bool_t QwEventBuffer::FillEPICSData(QwEPICSEvent &epics)
{
  // QwVerbose << "QwEventBuffer::FillEPICSData:  "
// 	  << Form("Length: %d; Tag: 0x%x; Bank ID num: 0x%x; ",
// 		  fEvtLength, fEvtTag, fIDBankNum)
// 	  << Form("Evt type: 0x%x; Evt number %d; Evt Class 0x%.8x; ",
// 		  fEvtType, fEvtNumber, fEvtClass)
// 	  << Form("Status Summary: 0x%.8x; Words so far %d",
// 		  fStatSum, fWordsSoFar)
// 	  << QwLog::endl;


  ///
  Bool_t okay = kTRUE;
  if (! IsEPICSEvent()){
    okay = kFALSE;
    return okay;
  }
  QwVerbose << "QwEventBuffer::FillEPICSData:  "
	    << QwLog::endl;
  //  Loop through the data buffer in this event.
  UInt_t *localbuff = (UInt_t*)(fEvStream->getEvBuffer());
  if (fBankDataType==0x10){
    while ((okay = DecodeSubbankHeader(&localbuff[fWordsSoFar]))){
      //  If this bank has further subbanks, restart the loop.
      if (fSubbankType == 0x10) continue;
      //  If this bank only contains the word 'NULL' then skip
      //  this bank.
      if (fFragLength==1 && localbuff[fWordsSoFar]==kNullDataWord){
	fWordsSoFar += fFragLength;
	continue;
      }

      if (fSubbankType == 0x3){
	//  This is an ASCII string bank.  Try to decode it and
	//  pass it to the EPICS class.
	char* tmpchar = (Char_t*)&localbuff[fWordsSoFar];
	
	epics.ExtractEPICSValues(string(tmpchar), GetEventNumber());
	QwVerbose << "test for GetEventNumber =" << GetEventNumber() << QwLog::endl;// always zero, wrong.
	
      }


      fWordsSoFar += fFragLength;

//     QwDebug << "QwEventBuffer::FillEPICSData:  "
// 	    << "Ending loop: fWordsSoFar=="<<fWordsSoFar
// 	    <<QwLog::endl;
//     QwMessage<<"\nQwEventBuffer::FillEPICSData: fWordsSoFar = "<<fWordsSoFar<<QwLog::endl;


    }
  } else {
    // Single bank in the event, use event headers.
    if (fBankDataType == 0x3){
      //  This is an ASCII string bank.  Try to decode it and
      //  pass it to the EPICS class.
      Char_t* tmpchar = (Char_t*)&localbuff[fWordsSoFar];
      
      QwError << tmpchar << QwLog::endl;
      
      epics.ExtractEPICSValues(string(tmpchar), GetEventNumber());
      
    }

  }

  //std::cout<<"\nEpics data coming!! "<<fWordsSoFar<<std::endl;
  QwVerbose << "QwEventBuffer::FillEPICSData:  End of routine"
	    << QwLog::endl;
  return okay;
}


Bool_t QwEventBuffer::DecodeSubbankHeader(UInt_t *buffer){
  //  This function will decode the header information from
  //  either a ROC bank or a subbank.  It will also bump
  //  fWordsSoFar to be referring to the first word of
  //  the subbank's data.
  //
  //  NOTE TO DAQ PROGRAMMERS:
  //      All internal subbank tags MUST be defined to
  //      be greater than 31.
  Bool_t okay = kTRUE;
  if (fWordsSoFar >= fEvtLength){
    //  We have reached the end of this event.
    okay = kFALSE;
  } else if (fBankDataType == 0x10) {
    //  This bank has subbanks, so decode the subbank header.
    fFragLength   = buffer[0] - 1;  // This is the number of words in the data block
    fSubbankTag   = (buffer[1]&0xFFFF0000)>>16; // Bits 16-31
    fSubbankType  = (buffer[1]&0xFF00)>>8;      // Bits 8-15
    fSubbankNum   = (buffer[1]&0xFF);           // Bits 0-7

    QwDebug << "QwEventBuffer::DecodeSubbankHeader: "
	    << "fROC=="<<fROC << ", fSubbankTag==" << fSubbankTag
	    << ", fSubbankType=="<<fSubbankType << ", fSubbankNum==" <<fSubbankNum
	    << ", fAllowLowSubbankIDs==" << fAllowLowSubbankIDs
	    << QwLog::endl;

    if (fSubbankTag<=31 
	&& ( (fAllowLowSubbankIDs==kFALSE)
	     || (fAllowLowSubbankIDs==kTRUE && fSubbankType==0x10) ) ){
      //  Subbank tags between 0 and 31 indicate this is
      //  a ROC bank.
      fROC        = fSubbankTag;
      fSubbankTag = 0;
    }
    if (fWordsSoFar+2+fFragLength > fEvtLength){
      //  Trouble, because we'll have too many words!
      QwError << "fWordsSoFar+2+fFragLength=="<<fWordsSoFar+2+fFragLength
		<< " and fEvtLength==" << fEvtLength
		<< QwLog::endl;
      okay = kFALSE;
    }
    fWordsSoFar   += 2;
  }
  QwDebug << "QwEventBuffer::DecodeSubbankHeader: " 
	  << "fROC=="<<fROC << ", fSubbankTag==" << fSubbankTag <<": "
	  <<  std::hex
	  << buffer[0] << " "
	  << buffer[1] << " "
	  << buffer[2] << " "
	  << buffer[3] << " "
	  << buffer[4] << std::dec << " "
	  << fWordsSoFar << " "<< fEvtLength
	  << QwLog::endl;
  //  There is no final else, because any bank type other than 
  //  0x10 should just return okay.
  return okay;
}


const TString&  QwEventBuffer::DataFile(const UInt_t run, const Short_t seg = -1)
{
  TString basename = fDataFileStem + Form("%u.",run) + fDataFileExtension;
  if(seg == -1){
    fDataFile = fDataDirectory + basename;
  } else {
    fDataFile = fDataDirectory + basename + Form(".%d",seg);
  }
  return fDataFile;
}


Bool_t QwEventBuffer::DataFileIsSegmented()
{
  glob_t globbuf;

  TString searchpath;
  TString scanvalue;
  Int_t   local_segment;

  std::vector<Int_t> tmp_segments;
  std::vector<Int_t> local_index;

  /*  Clear and set up the fRunSegments vector. */
  tmp_segments.clear();
  fRunSegments.clear();
  fRunSegments.resize(0);
  fRunIsSegmented = kFALSE;

  searchpath = fDataFile;
  glob(searchpath.Data(), GLOB_ERR, NULL, &globbuf);

  if (globbuf.gl_pathc == 1){
    /* The base file name exists.                               *
     * Do not look for file segments.                           */
    fRunIsSegmented = kFALSE;

  } else {
    /* The base file name does not exist.                       *
     * Look for file segments.                                  */
    QwWarning << "File " << fDataFile << " does not exist!\n"
	      << "      Trying to find run segments for run "
	      << fCurrentRun << "...  ";

    searchpath.Append(".[0-9]*");
    glob(searchpath.Data(), GLOB_ERR, NULL, &globbuf);

    if (globbuf.gl_pathc == 0){
      /* There are no file segments and no base file            *
       * Produce and error message and exit.                    */
      QwError << "\n      There are no file segments either!!" << QwLog::endl;

      // This could mean a single gzipped file!
      fRunIsSegmented = kFALSE;

    } else {
      /* There are file segments.                               *
       * Determine the segment numbers and fill fRunSegments    *
       * to indicate the existing file segments.                */

      QwMessage << "OK" << QwLog::endl;
      scanvalue = fDataFile + ".%d";

      /*  Get the list of segment numbers in file listing       *
       *  order.                                                */
      for (size_t iloop=0;iloop<globbuf.gl_pathc;++iloop){
        /*  Extract the segment numbers from the file name.     */
        sscanf(globbuf.gl_pathv[iloop], scanvalue.Data(), &local_segment);
        tmp_segments.push_back(local_segment);
      }
      local_index.resize(tmp_segments.size(),0);
      /*  Get the list of segments sorted numerically in        *
       *  increasing order.                                     */
      TMath::Sort(static_cast<int>(tmp_segments.size()),&(tmp_segments[0]),&(local_index[0]),
                  kFALSE);
      /*  Put the segments into numerical order in fRunSegments.  Add  *
       *  only those segments requested (though always add segment 0). */
      QwMessage << "      Found the segment(s): ";
      size_t printed = 0;
      for (size_t iloop=0; iloop<tmp_segments.size(); ++iloop){
        local_segment = tmp_segments[local_index[iloop]];
        if (printed++) QwMessage << ", ";
	QwMessage << local_segment ;
	if (local_segment == 0 ||
	    ( fSegmentRange.first <= local_segment &&
	      local_segment <= fSegmentRange.second ) ) {
	  fRunSegments.push_back(local_segment);
	} else {
	  QwMessage << " (skipped)" ;
	}
      }
      QwMessage << "." << QwLog::endl;
      fRunSegmentIterator = fRunSegments.begin();

      fRunIsSegmented = kTRUE;

      /* If the first requested segment hasn't been found,
	 forget everything. */
      if ( local_segment < fSegmentRange.first ) {
	QwError << "First requested run segment "
		<< fSegmentRange.first << " not found.\n";
	fRunSegments.pop_back();
	fRunSegmentIterator = fRunSegments.begin();
	fRunIsSegmented = kTRUE; // well, it is true.
      }
    }
  }
  globfree(&globbuf);
  return fRunIsSegmented;
}

//------------------------------------------------------------

Int_t QwEventBuffer::CloseThisSegment()
{
  Int_t status = kFileHandleNotConfigured;
  Int_t last_runsegment;
  if (fRunIsSegmented){
    last_runsegment = *fRunSegmentIterator;
    fRunSegmentIterator++;
    if (fRunSegmentIterator <= fRunSegments.end()){
      QwMessage << "Closing run segment " << last_runsegment <<"."
		<< QwLog::endl;
      status = CloseDataFile();
    }
  } else {
    //  Don't try to close a nonsegmented file; we will explicitly
    //  use CloseDataFile() later.
  }
  return status;
}

//------------------------------------------------------------

Int_t QwEventBuffer::OpenNextSegment()
{
  Int_t status;
  if (! fRunIsSegmented){
    /*  We are processing a non-segmented run.            *
     *  We should not have entered this routine, but      *
     *  since we are here, don't do anything.             */
    status = kRunNotSegmented;

  } else if (fRunSegments.size()==0){
    /*  There are actually no file segments located.      *
     *  Return "kNoNextDataFile", but don't print an      *
     *  error message.                                    */
    status = kNoNextDataFile;

  } else if (fRunSegmentIterator >= fRunSegments.begin() &&
             fRunSegmentIterator <  fRunSegments.end() ) {
    QwMessage << "Trying to open run segment " << *fRunSegmentIterator << QwLog::endl;
    status = OpenDataFile(DataFile(fCurrentRun,*fRunSegmentIterator),"R");

  } else if (fRunSegmentIterator == fRunSegments.end() ) {
    /*  We have reached the last run segment. */
    QwMessage << "There are no run segments remaining." << QwLog::endl;
    status = kNoNextDataFile;

  } else {
    QwError << "QwEventBuffer::OpenNextSegment(): Unrecognized error" << QwLog::endl;
    status = CODA_ERROR;
  }
  return status;
}


//------------------------------------------------------------
//call this routine if we've selected the run segment by hand
Int_t QwEventBuffer::OpenDataFile(UInt_t current_run, Short_t seg)
{
  fCurrentRun = current_run;

  fRunSegments.clear();
  fRunIsSegmented = kTRUE;

  fRunSegments.push_back(seg);
  fRunSegmentIterator = fRunSegments.begin();
  return OpenNextSegment();
}

//------------------------------------------------------------
//call this routine if the run is not segmented
Int_t QwEventBuffer::OpenDataFile(UInt_t current_run, const TString rw)
{
  Int_t status;
  fCurrentRun = current_run;
  DataFile(fCurrentRun);
  if (DataFileIsSegmented()){
    status = OpenNextSegment();
  } else {
    status = OpenDataFile(DataFile(fCurrentRun),rw);
  }
  return status;
}



//------------------------------------------------------------
Int_t QwEventBuffer::OpenDataFile(const TString filename, const TString rw)
{
  if (fEvStreamMode==fEvStreamNull){
    QwDebug << "QwEventBuffer::OpenDataFile:  File handle doesn't exist.\n"
	    << "                              Try to open a new file handle!"
	    << QwLog::endl;
    fEvStream = new THaCodaFile();
    fEvStreamMode = fEvStreamFile;
  } else if (fEvStreamMode!=fEvStreamFile){
    QwError << "QwEventBuffer::OpenDataFile:  The stream is not configured as an input\n"
	    << "                              file stream!  Can't deal with this!\n"
	    << QwLog::endl;
    exit(1);
  }
  fDataFile = filename;

  if (rw.Contains("w",TString::kIgnoreCase)) {
    // If we open a file for write access, let's suppose
    // we've given the path we want to use.
    QwMessage << "Opening data file:  " << fDataFile << QwLog::endl;
  } else {
    //  Let's try to find the data file for read access.
    glob_t globbuf;
    glob(fDataFile.Data(), GLOB_ERR, NULL, &globbuf);
    if (globbuf.gl_pathc == 0){
      //  Can't find the file; try in the "fDataDirectory".
      fDataFile = fDataDirectory + filename;
      glob(fDataFile.Data(), GLOB_ERR, NULL, &globbuf);
    }
    if (globbuf.gl_pathc == 0){
      //  Can't find the file; try gzipped.
      fDataFile = filename + ".gz";
      glob(fDataFile.Data(), GLOB_ERR, NULL, &globbuf);
    }
    if (globbuf.gl_pathc == 0){
      //  Can't find the file; try gzipped in the "fDataDirectory".
      fDataFile = fDataDirectory + filename + ".gz";
      glob(fDataFile.Data(), GLOB_ERR, NULL, &globbuf);
    }
    if (globbuf.gl_pathc == 1){
      QwMessage << "Opening data file:  " << fDataFile << QwLog::endl;
    } else {
      fDataFile = filename;
      QwError << "Unable to find "
              << filename.Data()  << " or "
              << (fDataDirectory + filename).Data()  << QwLog::endl;
    }
    globfree(&globbuf);
  }
  return fEvStream->codaOpen(fDataFile, rw);
}


//------------------------------------------------------------
Int_t QwEventBuffer::CloseDataFile()
{
  Int_t status = kFileHandleNotConfigured;
  if (fEvStreamMode==fEvStreamFile){
    status = fEvStream->codaClose();
  }
  return status;
}

//------------------------------------------------------------
Int_t QwEventBuffer::OpenETStream(TString computer, TString session, int mode,
				  const TString stationname)
{
  Int_t status = CODA_OK;
  if (fEvStreamMode==fEvStreamNull){
#ifdef __CODA_ET
    if (stationname != ""){
      fEvStream = new THaEtClient(computer, session, mode, stationname);
    } else {
      fEvStream = new THaEtClient(computer, session, mode);
    }
    fEvStreamMode = fEvStreamET;
#endif
  }
  return status;
}

//------------------------------------------------------------
Int_t QwEventBuffer::CloseETStream()
{
  Int_t status = kFileHandleNotConfigured;
  if (fEvStreamMode==fEvStreamFile){
    status = fEvStream->codaClose();
  }
  return status;
}

//------------------------------------------------------------
Int_t QwEventBuffer::CheckForMarkerWords(QwSubsystemArray &subsystems)
{
  QwDebug << "QwEventBuffer::GetMarkerWordList:  start function" <<QwLog::endl;
  fThisRocBankLabel = fROC;
  fThisRocBankLabel = fThisRocBankLabel<<32;
  fThisRocBankLabel += fSubbankTag;
  if (fMarkerList.count(fThisRocBankLabel)==0){
    std::vector<UInt_t> tmpvec;
    subsystems.GetMarkerWordList(fROC, fSubbankTag, tmpvec);
    fMarkerList.emplace(fThisRocBankLabel, tmpvec);
    fOffsetList.emplace(fThisRocBankLabel, std::vector<UInt_t>(tmpvec.size(),0));
  }
  QwDebug << "QwEventBuffer::GetMarkerWordList:  fMarkerList.count(fThisRocBankLabel)==" 
	  << fMarkerList.count(fThisRocBankLabel) 
	  << " fMarkerList.at(fThisRocBankLabel).size()==" 
	  << fMarkerList.at(fThisRocBankLabel).size()
	  << QwLog::endl;
  return fMarkerList.at(fThisRocBankLabel).size();
}

UInt_t QwEventBuffer::GetMarkerWord(UInt_t markerID){
  return fMarkerList.at(fThisRocBankLabel).at(markerID);
};


UInt_t QwEventBuffer::FindMarkerWord(UInt_t markerindex, UInt_t* buffer, UInt_t num_words){
  UInt_t markerpos  = fOffsetList.at(fThisRocBankLabel).at(markerindex);
  UInt_t markerval  = fMarkerList.at(fThisRocBankLabel).at(markerindex);
  if (markerpos < num_words && buffer[markerpos] == markerval){
    // The marker word is where it was last time
    return markerpos;
  } else {
    for (size_t i=0; i<num_words; i++){
      if (buffer[i] == markerval){
	fOffsetList.at(fThisRocBankLabel).at(markerindex) = i;
	markerpos = i;
	break;
      }
    }
  }
  return markerpos;
}
