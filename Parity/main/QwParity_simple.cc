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
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"
#include "QwHelicityPattern.h"
#include "QwEventRing.h"
//#include "QwEPICSEvent.h"
#include "QwPromptSummary.h"

// Qweak subsystems
// (for correct dependency generation)
#include "QwHelicity.h"
#include "QwFakeHelicity.h"
#include "QwBeamLine.h"



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
#ifdef  __USE_DATABASE__
  gQwOptions.AddConfigFile("qweak_mysql.conf");
#endif

  gQwOptions.ListConfigFiles();

  /// Load command line options for the histogram/tree helper class
  gQwHists.ProcessOptions(gQwOptions);
  /// Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);


  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);

  ///  Start loop over all runs
  while (eventbuffer.OpenNextStream() == CODA_OK) {

    ///  Begin processing for the first run

    Int_t run_number = eventbuffer.GetRunNumber();

    ///  Set the current event number for parameter file lookup
    QwParameterFile::SetCurrentRunNumber(run_number);

    //    if (gQwOptions.GetValue<bool>("write-promptsummary")) {
    QwPromptSummary promptsummary(run_number, eventbuffer.GetSegmentNumber());
    //    }
    // ///  Create an EPICS event
    // QwEPICSEvent epicsevent;
    // epicsevent.ProcessOptions(gQwOptions);
    // epicsevent.LoadChannelMap("EpicsTable.map");

    ///  Load the detectors from file
    QwSubsystemArrayParity detectors(gQwOptions);
    detectors.ProcessOptions(gQwOptions);
    detectors.ListPublishedValues();

    ///  Create the helicity pattern
    QwHelicityPattern helicitypattern(detectors);
    helicitypattern.ProcessOptions(gQwOptions);
      
    ///  Create the event ring with the subsystem array
    QwEventRing eventring(gQwOptions,detectors);
    //  Make a copy of the detectors object to hold the
    //  events which pass through the ring.
    QwSubsystemArrayParity ringoutput(detectors);

    ///  Create the running sum
    QwSubsystemArrayParity runningsum(detectors);

    //  Open the ROOT file (close when scope ends)
    TString run_label = eventbuffer.GetRunLabel();
    QwRootFile *rootfile = new QwRootFile(run_label);

    //  Construct a tree which contains map file names which are used to analyze data
    rootfile->WriteParamFileList("mapfiles", detectors);

    //  Construct histograms
    rootfile->ConstructHistograms("evt_histo", ringoutput);
    rootfile->ConstructHistograms("mul_histo", helicitypattern);
    detectors.ShareHistograms(ringoutput);

    //  Construct tree branches
    rootfile->ConstructTreeBranches("evt", "MPS event data tree", ringoutput);
    rootfile->ConstructTreeBranches("mul", "Helicity event data tree", helicitypattern);

    //  Clear the single-event running sum at the beginning of the runlet
    runningsum.ClearEventData();

    // //  Find the first EPICS event and try to initialize
    // //  the blinder.
    // QwMessage << "Finding first EPICS event" << QwLog::endl;
    // while (eventbuffer.GetNextEvent() == CODA_OK) {
    //   if (eventbuffer.IsEPICSEvent()) {
    // 	eventbuffer.FillEPICSData(epicsevent);
    // 	if (epicsevent.HasDataLoaded()) {
    // 	  helicitypattern.UpdateBlinder(epicsevent);
    // 	  // and break out of this event loop
    // 	  break;
    // 	}
    //   }
    // }
    // epicsevent.ResetCounters();
    // //  Rewind stream
    // QwMessage << "Rewinding stream" << QwLog::endl;
    // eventbuffer.ReOpenStream();

    ///  Start loop over events
    while (eventbuffer.GetNextEvent() == CODA_OK) {

      //  First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
        //  Send ROC configuration event data to the subsystem objects.
        eventbuffer.FillSubsystemConfigurationData(detectors);
      }

      // //  Secondly, process EPICS events
      // if (eventbuffer.IsEPICSEvent()) {
      //   eventbuffer.FillEPICSData(epicsevent);
      // 	if (epicsevent.HasDataLoaded()){
      // 	  epicsevent.CalculateRunningValues();
      // 	  helicitypattern.UpdateBlinder(epicsevent);
	
      // 	  rootfile->FillTreeBranches(epicsevent);
      // 	  rootfile->FillTree("slow");
      // 	}
      // }


      //  Now, if this is not a physics event, go back and get a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;



      //  Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      //  Process the subsystem data
      detectors.ProcessEvent();



      // The event pass the event cut constraints
      if (detectors.ApplySingleEventCuts()) {
	
        // Add event to the ring
        eventring.push(detectors);

        // Check to see ring is ready
        if (eventring.IsReady()) {
	  ringoutput = eventring.pop();
	  ringoutput.IncrementErrorCounters();


	  // Accumulate the running sum to calculate the event based running average
	  runningsum.AccumulateRunningSum(ringoutput);

	  // Fill the histograms
	  rootfile->FillHistograms(ringoutput);

	  // Fill mps tree branches
	  rootfile->FillTreeBranches(ringoutput);
	  rootfile->FillTree("evt");

          // Load the event into the helicity pattern
          helicitypattern.LoadEventData(ringoutput);

          // Calculate helicity pattern asymmetry
          if (helicitypattern.IsCompletePattern()) {

            // Update the blinder if conditions have changed
            helicitypattern.UpdateBlinder(ringoutput);

            // Calculate the asymmetry
            helicitypattern.CalculateAsymmetry();
            if (helicitypattern.IsGoodAsymmetry()) {

              // Fill histograms
              rootfile->FillHistograms(helicitypattern);

              // Fill helicity tree branches
              rootfile->FillTreeBranches(helicitypattern);
              rootfile->FillTree("mul");

              // // Linear regression on asymmetries
	      // regression.LinearRegression(QwRegression::kRegTypeAsym);
	      // running_regression.AccumulateRunningSum(regression);

              // // Fill corrected tree branches
	      // rootfile->FillTreeBranches(regression);
	      // rootfile->FillTree("mulc");

              // Clear the data
              helicitypattern.ClearEventData();

            } // helicitypattern.IsGoodAsymmetry()

          } // helicitypattern.IsCompletePattern()

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
    if (helicitypattern.IsRunningSumEnabled()) {
      helicitypattern.CalculateRunningAverage();
    }

    // This will calculate running averages over single helicity events
    runningsum.CalculateRunningAverage();
    if (gQwOptions.GetValue<bool>("print-runningsum")) {
      QwMessage << " Running average of events" << QwLog::endl;
      QwMessage << " =========================" << QwLog::endl;
      runningsum.PrintValue();
    }


    //  Construct objects
    rootfile->ConstructObjects("objects", helicitypattern);

    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          *
     *                                                               *
     *  Then, we need to delete the histograms here.                 *
     *  If we wait until the subsystem destructors, we get a         *
     *  segfault; but in addition to that we should delete them     *
     *  here, in case we run over multiple runs at a time.           */
    rootfile->Write(0,TObject::kOverwrite);
    delete rootfile; rootfile = 0;

    //  Print the event cut error summary for each subsystem
    if (gQwOptions.GetValue<bool>("print-errorcounters")) {
      QwMessage << " ------------ error counters ------------------ " << QwLog::endl;
      ringoutput.PrintErrorCounters();
    }

    if (gQwOptions.GetValue<bool>("write-promptsummary")) {
      //      runningsum.WritePromptSummary(&promptsummary, "yield");
      // runningsum.WritePromptSummary(&promptsummary, "asymmetry");
      //      runningsum.WritePromptSummary(&promptsummary, "difference");
      helicitypattern.WritePromptSummary(&promptsummary);
      promptsummary.PrintCSV();
    }

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

