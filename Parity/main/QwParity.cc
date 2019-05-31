/*------------------------------------------------------------------------*//*!

 \file QwParity.cc

 \brief main(...) function for the qwanalysis executable

*//*-------------------------------------------------------------------------*/

// System headers
#include <iostream>
#include <fstream>
#include <vector>
#include <new>

// Boost headers
#include <boost/shared_ptr.hpp>

// ROOT headers
#include "Rtypes.h"
#include "TROOT.h"
#include "TFile.h"

// Qweak headers
#include "QwLog.h"
#include "QwRootFile.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#ifdef __USE_DATABASE__
#include "QwParityDB.h"
#endif //__USE_DATABASE__
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"
#include "QwHelicityPattern.h"
#include "QwEventRing.h"
#include "QwEPICSEvent.h"
#include "QwCombiner.h"
#include "QwCombinerSubsystem.h"
#include "QwPromptSummary.h"
#include "QwCorrelator.h"
#include "LRBCorrector.h"
#include "QwDataHandlerArrayHel.h"
#include "QwDataHandlerArrayEvt.h"
#include "EvtDatahandler.h"

// Qweak subsystems
// (for correct dependency generation)
#include "QwHelicity.h"
#include "QwFakeHelicity.h"
#include "QwBeamLine.h"
#include "QwBeamMod.h"
#include "QwIntegratedRaster.h"



Int_t main(Int_t argc, Char_t* argv[])
{
  ///  Define the command line options
  DefineOptionsParity(gQwOptions);

  ///  Define additional command line arguments and the configuration filename,
  ///  and we define the options that can be used in them (using QwOptions).
  gQwOptions.AddOptions()("single-output-file", po::value<bool>()->default_bool_value(false), "Write a single output file");
  gQwOptions.AddOptions()("print-errorcounters", po::value<bool>()->default_bool_value(true), "Print summary of error counters");
  gQwOptions.AddOptions()("write-promptsummary", po::value<bool>()->default_bool_value(false), "Write PromptSummary");

  ///  Without anything, print usage
  if (argc == 1) {
    gQwOptions.Usage();
    exit(0);
  }

  ///  First, fill the search paths for the parameter files; this sets a
  ///  static variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.AddConfigFile("qweak_mysql.conf");

  gQwOptions.ListConfigFiles();

  /// Load command line options for the histogram/tree helper class
  gQwHists.ProcessOptions(gQwOptions);
  /// Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);


  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);

  ///  Create the database connection
  #ifdef __USE_DATABASE__
  QwParityDB database(gQwOptions);
  #endif //__USE_DATABASE__

  //  QwPromptSummary promptsummary;

  ///  Start loop over all runs
  while (eventbuffer.OpenNextStream() == CODA_OK) {

    ///  Begin processing for the first run

    Int_t run_number = eventbuffer.GetRunNumber();
    TString run_label = eventbuffer.GetRunLabel();

    ///  Set the current event number for parameter file lookup
    QwParameterFile::SetCurrentRunNumber(run_number);
    //  Parse the options again, in case there are run-ranged config files
    gQwOptions.Parse(kTRUE);
    eventbuffer.ProcessOptions(gQwOptions);

    //    if (gQwOptions.GetValue<bool>("write-promptsummary")) {
    QwPromptSummary promptsummary(run_number, eventbuffer.GetSegmentNumber());
    //    }
    ///  Create an EPICS event
    QwEPICSEvent epicsevent;
    epicsevent.ProcessOptions(gQwOptions);
    epicsevent.LoadChannelMap("EpicsTable.map");


    ///  Load the detectors from file
    QwSubsystemArrayParity detectors(gQwOptions);
    detectors.ProcessOptions(gQwOptions);
    detectors.ListPublishedValues();

    /// Create event-based correction subsystem
    //    TString name = "EvtCorrector";
    //    QwCombinerSubsystem corrector_sub(gQwOptions, detectors, name);
    //    detectors.push_back(corrector_sub.GetSharedPointerToStaticObject());
    
    /// Create the helicity pattern
    //    Instead of having run_label in the constructor of helicitypattern, it might
    //    make since to have it be an option for use globally
    QwHelicityPattern helicitypattern(detectors,run_label);
    helicitypattern.ProcessOptions(gQwOptions);
    
    /// Create the data handler array
    QwDataHandlerArrayHel datahandlerarray(gQwOptions,helicitypattern,run_label);
    //QwDataHandlerArrayEvt datahandlerarray(gQwOptions,BeamMod,run_label);

    
    ///  Create the event ring with the subsystem array
    QwEventRing eventring(gQwOptions,detectors);
    //  Make a copy of the detectors object to hold the
    //  events which pass through the ring.
    QwSubsystemArrayParity ringoutput(detectors);

    ///  Create the running sum
    QwSubsystemArrayParity runningsum(detectors);
    QwHelicityPattern patternsum(helicitypattern);

    //  Initialize the database connection.
    #ifdef __USE_DATABASE__
    database.SetupOneRun(eventbuffer);
    #endif // __USE_DATABASE__

    //  Open the ROOT file (close when scope ends)
    QwRootFile *treerootfile  = NULL;
    QwRootFile *burstrootfile = NULL;
    QwRootFile *historootfile = NULL;


    if (gQwOptions.GetValue<bool>("single-output-file")) {

      treerootfile  = new QwRootFile(run_label);
      burstrootfile = historootfile = treerootfile;
      //  Construct a tree which contains map file names which are used to analyze data
      treerootfile->WriteParamFileList("mapfiles", detectors);

    } else {

      treerootfile  = new QwRootFile(run_label + ".trees");
      burstrootfile = new QwRootFile(run_label + ".bursts");
      historootfile = new QwRootFile(run_label + ".histos");

      //  Construct a tree which contains map file names which are used to analyze data
      detectors.PrintParamFileList();
      treerootfile->WriteParamFileList("mapfiles", detectors);
      burstrootfile->WriteParamFileList("mapfiles", detectors);
      historootfile->WriteParamFileList("mapfiles", detectors);
    }
    #ifdef __USE_DATABASE__
    if (database.AllowsWriteAccess()) {
      database.FillParameterFiles(detectors);
    }
    #endif // __USE_DATABASE__
    //  Construct histograms
    historootfile->ConstructHistograms("evt_histo", ringoutput);
    historootfile->ConstructHistograms("mul_histo", helicitypattern);
    detectors.ShareHistograms(ringoutput);

    //  Construct tree branches
    treerootfile->ConstructTreeBranches("evt", "MPS event data tree", ringoutput);
    treerootfile->ConstructTreeBranches("mul", "Helicity event data tree", helicitypattern);
    burstrootfile->ConstructTreeBranches("pr", "Pair tree", helicitypattern.GetPairYield(),"yield_");
    burstrootfile->ConstructTreeBranches("pr", "Pair tree", helicitypattern.GetPairAsymmetry(),"asym_");
    treerootfile->ConstructTreeBranches("slow", "EPICS and slow control tree", epicsevent);

    datahandlerarray.ConstructTreeBranches(treerootfile);

    burstrootfile->ConstructTreeBranches("burst", "Burst level data tree", helicitypattern.GetBurstYield(),"yield_");
    burstrootfile->ConstructTreeBranches("burst", "Burst level data tree", helicitypattern.GetBurstAsymmetry(),"asym_");
    burstrootfile->ConstructTreeBranches("burst", "Burst level data tree", helicitypattern.GetBurstDifference(),"diff_");

    // Summarize the ROOT file structure
    //treerootfile->PrintTrees();
    //treerootfile->PrintDirs();


    //  Clear the single-event running sum at the beginning of the runlet
    runningsum.ClearEventData();
    patternsum.ClearRunningSum();
    //  Clear the running sum of the burst values at the beginning of the runlet
    helicitypattern.ClearBurstSum();


    //  Load the blinder seed from the database for this runlet.
    #ifdef __USE_DATABASE__
    helicitypattern.UpdateBlinder(&database);
    #endif // __USE_DATABASE__

    //  Find the first EPICS event and try to initialize
    //  the blinder, but only for disk files, not online.
    if (! eventbuffer.IsOnline() ){
      QwMessage << "Finding first EPICS event" << QwLog::endl;
      while (eventbuffer.GetNextEvent() == CODA_OK) {
	if (eventbuffer.IsEPICSEvent()) {
	  eventbuffer.FillEPICSData(epicsevent);
	  if (epicsevent.HasDataLoaded()) {
	    helicitypattern.UpdateBlinder(epicsevent);
	    // and break out of this event loop
	    break;
	  }
	}
      }
      epicsevent.ResetCounters();
      //  Rewind stream
      QwMessage << "Rewinding stream" << QwLog::endl;
      eventbuffer.ReOpenStream();
    }

    ///  Start loop over events
    while (eventbuffer.GetNextEvent() == CODA_OK) {

      //  First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
        //  Send ROC configuration event data to the subsystem objects.
        eventbuffer.FillSubsystemConfigurationData(detectors);
      }

      //  Secondly, process EPICS events
      if (eventbuffer.IsEPICSEvent()) {
        eventbuffer.FillEPICSData(epicsevent);
	if (epicsevent.HasDataLoaded()){
	  epicsevent.CalculateRunningValues();
	  helicitypattern.UpdateBlinder(epicsevent);
	
	  treerootfile->FillTreeBranches(epicsevent);
	  treerootfile->FillTree("slow");
	}
      }


      //  Now, if this is not a physics event, go back and get a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;


      //  Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      //  Process the subsystem data
      detectors.ProcessEvent();


      // The event pass the event cut constraints
      if (detectors.ApplySingleEventCuts()) {
	
	//	// TEST 
	// 	combiner_sub.ProcessData();

        // Add event to the ring
        eventring.push(detectors);

        // Check to see ring is ready
        if (eventring.IsReady()) {
	  ringoutput = eventring.pop();
	  ringoutput.IncrementErrorCounters();


	  // Accumulate the running sum to calculate the event based running average
	  runningsum.AccumulateRunningSum(ringoutput);

	  // Fill the histograms
	  historootfile->FillHistograms(ringoutput);

	  // Fill mps tree branches
	  treerootfile->FillTreeBranches(ringoutput);
	  treerootfile->FillTree("evt");

          // Load the event into the helicity pattern
          helicitypattern.LoadEventData(ringoutput);

	  if (helicitypattern.PairAsymmetryIsGood()) {
	    patternsum.AccumulatePairRunningSum(helicitypattern);
	    // Fill pair tree branches
	    treerootfile->FillTreeBranches(helicitypattern.GetPairYield());
	    treerootfile->FillTreeBranches(helicitypattern.GetPairAsymmetry());
	    treerootfile->FillTreeBranches(helicitypattern.GetPairDifference());
	    treerootfile->FillTree("pr");
	    
	    // Clear the data
	    helicitypattern.ClearPairData();
	  }
          // Check to see if we can calculate helicity pattern asymmetry, do so, and report if it worked
          if (helicitypattern.IsGoodAsymmetry()) {
	    patternsum.AccumulateRunningSum(helicitypattern);

              // Fill histograms
              historootfile->FillHistograms(helicitypattern);

              // Fill helicity tree branches
              treerootfile->FillTreeBranches(helicitypattern);
              treerootfile->FillTree("mul");

              // Burst mode
              if (helicitypattern.IsEndOfBurst()) {
                helicitypattern.AccumulateRunningBurstSum();
                helicitypattern.CalculateBurstAverage();

                // Fill burst tree branches
                burstrootfile->FillTreeBranches(helicitypattern.GetBurstYield());
                burstrootfile->FillTreeBranches(helicitypattern.GetBurstAsymmetry());
                burstrootfile->FillTreeBranches(helicitypattern.GetBurstDifference());
                burstrootfile->FillTree("burst");

                // Clear the data
                helicitypattern.ClearBurstSum();
              }

              // Process data handlers
              datahandlerarray.ProcessDataHandlerEntry();
	      
              // Fill regressed tree branches
	      datahandlerarray.FillTreeBranches(treerootfile);

              // Clear the data
              helicitypattern.ClearEventData();

	  } // helicitypattern.IsGoodAsymmetry()

        } // eventring.IsReady()

      } // detectors.ApplySingleEventCuts()

    } // end of loop over events
    
    //  Perform actions at the end of the event loop on the
    //  detectors object, which ought to have handles for the
    //  MPS based histograms.
    ringoutput.AtEndOfEventLoop();

    QwMessage << "Number of events processed at end of run: "
              << eventbuffer.GetEventNumber() << QwLog::endl;


    // Calculate running averages over helicity patterns
    if (patternsum.IsRunningSumEnabled()) {
      patternsum.CalculateRunningAverage();
      if (helicitypattern.IsBurstSumEnabled()) {
        helicitypattern.CalculateRunningBurstAverage();
      }
    }

    datahandlerarray.FinishDataHandler();

    // This will calculate running averages over single helicity events
    runningsum.CalculateRunningAverage();
    if (gQwOptions.GetValue<bool>("print-runningsum")) {
      QwMessage << " Running average of events" << QwLog::endl;
      QwMessage << " =========================" << QwLog::endl;
      runningsum.PrintValue();
    }
   
  
    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          *
     *                                                               *
     *  Then, we need to delete the histograms here.                 *
     *  If we wait until the subsystem destructors, we get a         *
     *  segfault; but in addition to that we should delete them      *
     *  here, in case we run over multiple runs at a time.           */
    if (treerootfile == historootfile) {
      treerootfile->Write(0,TObject::kOverwrite);
      delete treerootfile; treerootfile = 0; burstrootfile = 0; historootfile = 0;
    } else {
      treerootfile->Write(0,TObject::kOverwrite);
      burstrootfile->Write(0,TObject::kOverwrite);
      historootfile->Write(0,TObject::kOverwrite);
      delete treerootfile; treerootfile = 0;
      delete burstrootfile; burstrootfile = 0;
      delete historootfile; historootfile = 0;
    }

    //  Print the event cut error summary for each subsystem
    if (gQwOptions.GetValue<bool>("print-errorcounters")) {
      QwMessage << " ------------ error counters ------------------ " << QwLog::endl;
      ringoutput.PrintErrorCounters();
    }
    
    if (gQwOptions.GetValue<bool>("write-promptsummary")) {
      //      runningsum.WritePromptSummary(&promptsummary, "yield");
      // runningsum.WritePromptSummary(&promptsummary, "asymmetry");
      //      runningsum.WritePromptSummary(&promptsummary, "difference");
      datahandlerarray.WritePromptSummary(&promptsummary, "asymmetry");
      patternsum.WritePromptSummary(&promptsummary);
      promptsummary.PrintCSV();
    }
    //  Read from the database
    #ifdef __USE_DATABASE__
    database.SetupOneRun(eventbuffer);

    // Each subsystem has its own Connect() and Disconnect() functions.
    if (database.AllowsWriteAccess()) {
      patternsum.FillDB(&database);
      patternsum.FillErrDB(&database);
      epicsevent.FillDB(&database);
      helicitypattern.return_running_combiner().FillDB(&database,"asymmetry");
      ringoutput.FillDB_MPS(&database, "optics");
    }
    #endif // __USE_DATABASE__    
  
    //epicsevent.WriteEPICSStringValues();

    //  Close event buffer stream
    eventbuffer.CloseStream();



    //  Report run summary
    eventbuffer.ReportRunSummary();
    eventbuffer.PrintRunTimes();
  } // end of loop over runs

  QwMessage << "I have done everything I can do..." << QwLog::endl;

  return 0;
}

