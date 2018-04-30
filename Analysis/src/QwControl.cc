#include "QwControl.h"

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>
using std::cout; using std::endl;

// ROOT headers
#include "TROOT.h"
#include "TBenchmark.h"

// Qweak headers
#include "VQwSystem.h"

// Qweak analyzers
#include "VQwAnalyzer.h"
#include "QwTrackingAnalyzer.h"

// Qweak dataservers
#include "VQwDataserver.h"
#include "QwTrackingDataserver.h"


#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwControl)
#endif


void* QwRunThread (void* arg)
{
  // Threaded running of the analyzer
  // so that other tasks may be performed

  if (! ((TObject*) arg)->InheritsFrom("QwControl")) {
    TThread::Printf(" Error...QwControl base class not supplied\n");
    return NULL;;
  }
  QwControl* qr = (QwControl*) arg;
  qr->Run();
  return NULL;
}

//---------------------------------------------------------------------------
void QwControl::Run()
{
  // Run online or offline analysis
  // Offline handles ascii file(s)

  gBenchmark = new TBenchmark();
  gBenchmark->Start("QwControlRunThread");

  // Set up analysis
  //fAnalysis->FileConfig (fAnalysisSetup);

  // Online
  if (fIsOnline) {
    // Connect to data stream
    // if (data stream) data loop;

  // Offline
  } else {
    OfflineLoop();
  }
  gBenchmark->Show("QwControlRunThread");
  fIsFinished = true;
}

//-----------------------------------------------------------------------------
void QwControl::Start()
{
  // Start the analysis thread. This will run in the background
  // but will produce to occasional warning or info message

  if (fRunThread) {
    printf(" Warning...deleting old RunThread and starting new one,\n");
    fRunThread->Delete();
  }
  printf(" Starting new RunThread.\n");
  fRunThread = new TThread("QwRunThread",
			   (void(*) (void*))&(QwRunThread),
			   (void*) this);
  fRunThread->Run();
  return;
}

//-----------------------------------------------------------------------------
void QwControl::OnlineLoop()
{
  // Loop while online data

    // Wait until buffer ready

    // Process event

  // Finish
}

//-----------------------------------------------------------------------------
void QwControl::OfflineLoop()
{
  // Loop over the offline data sources (ascii files, ntuples from geant)

  // Setup file name

  int nEvent = 0;	// number of processed events
  int nEventMax = 10;	// maximum number of events

  while (nEvent < nEventMax) {
    // Get next event from dataserver
    fDataserver->NextEvent();
    fAnalyzer->SetHitList (fDataserver->GetHitList());

    // Process this event
    fAnalyzer->Process();

    nEvent++;
  }
}

//-----------------------------------------------------------------------------
VQwAnalyzer* QwControl::CreateAnalyzer (const char* name)
{
  if (strcmp (name, "QwTrackingAnalyzer") == 0) {
    VQwAnalyzer* analyzer = new QwTrackingAnalyzer (name);
    return analyzer;
  }
  return 0;
}
//-----------------------------------------------------------------------------
void QwControl::SetAnalyzer (VQwAnalyzer* analyzer)
{
// TODO (wdc) Why doesn't this work? (and in SetDataserver below)
//  if (! analyzer->InheritsFrom("VQwAnalyzer")) {
//    std::cout << "Error: analysis has to inherit from VQwAnalyzer!" << std::endl;
//    return;
//  }
  fAnalyzer = analyzer;
  return;
}

//-----------------------------------------------------------------------------
VQwDataserver* QwControl::CreateDataserver (const char* name)
{
  if (strcmp (name, "QwTrackingDataserver") == 0) {
    VQwDataserver* dataserver = new QwTrackingDataserver (name);
    return dataserver;
  }
  return 0;
}
//-----------------------------------------------------------------------------
void QwControl::SetDataserver (VQwDataserver* dataserver)
{
//  if (! dataserver->InheritsFrom("VQwDataserver")) {
//    std::cout << "Error: dataserver has to inherit from VQwDataserver!" << std::endl;
//    return;
//  }
  fDataserver = dataserver;
  return;
}
