/*------------------------------------------------------------------------*//*!

 \file QwMockDataAnalysis.cc

 \brief Parity mock data analysis, test code

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <iostream>

// Boost math library for random number generation
#include <boost/random.hpp>

// ROOT headers
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>


// Qweak headers
#include "QwBeamLine.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwParityDB.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwMainCerenkovDetector.h"
#include "QwLumi.h"
#include "QwSubsystemArrayParity.h"


// Number of events to generate
#define NEVENTS 1000
// Number of variables to correlate
#define NVARS 4


// Multiplet structure
static const int kMultiplet = 4;

// Debug level
static bool bDebug = false;

// Activate components
static bool bTree = true;
static bool bHisto = true;
static bool bHelicity = true;
static bool bDatabase = false;

int main(int argc, char* argv[])
{
  // First, we set the command line arguments and the configuration filename,
  // and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  //  gQwOptions.SetConfigFile("qwmockdataanalysis.conf");
  gQwOptions.SetConfigFile("Parity/prminput/qwanalysis_beamline.conf");
  gQwOptions.SetConfigFile("Parity/prminput/qweak_mysql.conf");
  // Define the command line options
  DefineOptionsParity(gQwOptions);

  ///  Fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS")+"/Parity/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  // Load histogram definitions
  gQwHists.LoadHistParamsFromFile("qweak_parity_hists.in");

  // Detector array
  QwSubsystemArrayParity detectors(gQwOptions);
  detectors.ProcessOptions(gQwOptions);

  // Helicity pattern
  QwHelicityPattern helicitypattern(detectors);
  helicitypattern.ProcessOptions(gQwOptions);

  // Running sum
  QwSubsystemArrayParity runningsum(detectors);

  // Get the helicity
  QwHelicity* helicity = dynamic_cast<QwHelicity*>(detectors.GetSubsystemByName("Helicity info"));

  // Event buffer
  QwEventBuffer eventbuffer;


  // Loop over all runs
  UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
  UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
  for (UInt_t run  = runnumber_min;
              run <= runnumber_max;
              run++) {

    // Data file (input)
    TString datafilename = TString("QwMock_") + Form("%d.log",run);
    if (eventbuffer.OpenDataFile(datafilename,"R") != CODA_OK) {
      std::cout << "Error: could not open file!" << std::endl;
      return 0;
    }


    // ROOT file output (histograms)
    TString rootfilename = getenv_safe_TString("QW_ROOTFILES") + TString("/QwMock_") + Form("%d.root",run);
    TFile rootfile(rootfilename, "RECREATE", "QWeak ROOT file");
    if (bHisto) {
      rootfile.cd();
      detectors.ConstructHistograms(rootfile.mkdir("mps_histo"));
      if (bHelicity) {
        rootfile.cd();
        helicitypattern.ConstructHistograms(rootfile.mkdir("hel_histo"));
      }
    }

    // ROOT file output (trees)
    TTree *mpstree;
    TTree *heltree;
    Int_t eventnumber;
    std::vector <Double_t> mpsvector;
    std::vector <Double_t> helvector;
    if (bTree) {
      rootfile.cd();
      mpstree = new TTree("MPS_Tree","MPS event data tree");
      mpsvector.reserve(6000);
      mpstree->Branch("eventnumber",&eventnumber,"eventnumber/F");
      TString dummystr="";
      detectors.ConstructBranchAndVector(mpstree, dummystr, mpsvector);
      rootfile.cd();
      if (bHelicity) {
        rootfile.cd();
        heltree = new TTree("HEL_Tree","Helicity event data tree");
        helvector.reserve(6000);
        TString dummystr="";
        helicitypattern.ConstructBranchAndVector(heltree, dummystr, helvector);
      }
    }


    // Loop over events in this CODA file
    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
    while (eventbuffer.GetEvent() == CODA_OK) {

      // First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
        // Send ROC configuration event data to the subsystem objects.
        eventbuffer.FillSubsystemConfigurationData(detectors);
      }

      // Now, if this is not a physics event, go back and get a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;

      //  Check to see if we want to process this event.
      eventnumber = eventbuffer.GetEventNumber();
      if      (eventnumber < eventnumber_min) continue;
      else if (eventnumber > eventnumber_max) break;

      // Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      // Process this events
      detectors.ProcessEvent();
      detectors.IncrementErrorCounters();

      // Helicity pattern
      if (bHelicity)
        helicitypattern.LoadEventData(detectors);

      // Print the helicity information
      if (bHelicity && bDebug) {
        // - actual helicity
        if      (helicity->GetHelicityActual() == 0) std::cout << "-";
        else if (helicity->GetHelicityActual() == 1) std::cout << "+";
        else std::cout << "?";
        // - delayed helicity
        if      (helicity->GetHelicityDelayed() == 0) std::cout << "(-) ";
        else if (helicity->GetHelicityDelayed() == 1) std::cout << "(+) ";
        else std::cout << "(?) ";
        if (helicity->GetPhaseNumber() == kMultiplet) {
          std::cout << std::hex << helicity->GetRandomSeedActual() << std::dec << ",  \t";
          std::cout << std::hex << helicity->GetRandomSeedDelayed() << std::dec << std::endl;
        }
      }

      // Check for helicity validity (TODO I'd prefer to use kUndefinedHelicity)
      if (bHelicity && helicity->GetHelicityDelayed() == -9999) continue;

      // Fill the histograms
      if (bHisto) detectors.FillHistograms();

      // Accumulate the running sum to calculate the event based running average
      runningsum.AccumulateRunningSum(detectors);

      // Fill the MPS tree
      if (bTree) {
        eventnumber = eventbuffer.GetEventNumber();
        detectors.FillTreeVector(mpsvector);
        mpstree->Fill();
      }
      // Fill the helicity tree
      if (bHelicity && helicitypattern.IsCompletePattern()) {
        helicitypattern.CalculateAsymmetry();
        if (bHisto) helicitypattern.FillHistograms();
        if (bTree) {
          helicitypattern.FillTreeVector(helvector);
          heltree->Fill();
        }
        helicitypattern.ClearEventData();
      }

      // Periodically print event number
      if ((bDebug && eventbuffer.GetEventNumber() % 1000 == 0)
                  || eventbuffer.GetEventNumber() % 10000 == 0)
        std::cout << "Number of events processed so far: "
                  << eventbuffer.GetEventNumber() << std::endl;

      // TODO (wdc) QwEventBuffer should have Bool_t AtEndOfBurst()
      //if (QwEvt.AtEndOfBurst()){
      if (eventbuffer.GetEventNumber() % 1000 == 0) {
        {
          helicitypattern.AccumulateRunningBurstSum();
          helicitypattern.CalculateBurstAverage();
          helicitypattern.ClearBurstSum();
        }
      }

    } // end of loop over events

    // Calculate the running averages
    helicitypattern.CalculateRunningAverage();
    helicitypattern.CalculateRunningBurstAverage();
    runningsum.CalculateRunningAverage();

    // Close ROOT file
    rootfile.Write(0,TObject::kOverwrite);

    // Close data file and print run summary
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

    // Write to database
    if (bDatabase) {
      QwParityDB* qweak_database  = new QwParityDB();
      QwMessage << "GetMonitorID(qwk_batext2) = " << qweak_database->GetMonitorID("qwk_batext2") << QwLog::endl;
      QwMessage << "GetMonitorID(phasemonitor) = " << qweak_database->GetMonitorID("phasemonitor") << QwLog::endl;
      QwMessage << "GetMonitorID(qwk_junk) = " << qweak_database->GetMonitorID("qwk_junk") << QwLog::endl;
      QwMessage << "GetMainDetectorID(md1neg) = " << qweak_database->GetMainDetectorID("md1neg") << QwLog::endl;
      QwMessage << "GetMainDetectorID(spare3) = " << qweak_database->GetMainDetectorID("spare3") << QwLog::endl;
      QwMessage << "GetMainDetectorID(combinationallmd) = " << qweak_database->GetMainDetectorID("combinationallmd") << QwLog::endl;
      QwMessage << "GetLumiDetectorID(dlumi8) = " << qweak_database->GetLumiDetectorID("dlumi8") << QwLog::endl;
      QwMessage << "GetVersion() = " << qweak_database->GetVersion() << QwLog::endl;
      // GetRunID() and GetAnalysisID have their own Connect() and Disconnect() functions.
      UInt_t run_id      = qweak_database->GetRunID(eventbuffer);
      UInt_t analysis_id = qweak_database->GetAnalysisID(eventbuffer);

      QwMessage << "QwMockDataAnalysis.cc::"
                << " Run Number "  << QwColor(Qw::kBoldMagenta) << eventbuffer.GetRunNumber() << QwColor(Qw::kNormal)
                << " Run ID "      << QwColor(Qw::kBoldMagenta) << run_id<< QwColor(Qw::kNormal)
                << " Analysis ID " << QwColor(Qw::kBoldMagenta) << analysis_id
                << QwLog::endl;

      // Each sussystem has its own Connect() and Disconnect() functions.
      helicitypattern.FillDB(qweak_database);

      delete qweak_database; qweak_database = NULL;

    } // end of database write


  } // end of loop over runs


}
