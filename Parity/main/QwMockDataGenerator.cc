/*------------------------------------------------------------------------*//*!

 \file QwMockDataGenerator.cc

 \brief Parity mock data generator, test code

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <iostream>

// Boost math library for random number generation
#include <boost/random.hpp>

// Qweak headers
#include "QwLog.h"
#include "QwBeamLine.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwMainCerenkovDetector.h"
#include "MollerMainDetector.h"
#include "QwLumi.h"
//#include "QwScanner.h"
#include "QwSubsystemArrayParity.h"


// Number of variables to correlate
#define NVARS 3


// Multiplet structure
static const int kMultiplet = 4;

// Beam trips on qwk_bcm0l03
static const bool kBeamTrips = true;

// Debug
static const bool kDebug = false;

// Stringify
inline std::string stringify(int i) {
  std::ostringstream stream;
  stream << i;
  return stream.str();
}

int main(int argc, char* argv[])
{
  // Define the command line options
  DefineOptionsParity(gQwOptions);

  ///  Without anything, print usage
  if (argc == 1) {
    gQwOptions.Usage();
    exit(0);
  }

  // Fill the search paths for the parameter files; this sets a static
  // variable within the QwParameterFile class which will be used by
  // all instances.
  // The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");

  // Set the command line arguments and the configuration filename,
  // and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwmockdataanalysis.conf");

  // Event buffer
  QwEventBuffer eventbuffer;

  // Detector array
  QwSubsystemArrayParity detectors(gQwOptions);
  detectors.ProcessOptions(gQwOptions);

  // Get the helicity
  QwHelicity* helicity = dynamic_cast<QwHelicity*>(detectors.GetSubsystemByName("Helicity Info"));
  if (! helicity) QwWarning << "No helicity subsystem defined!" << QwLog::endl;

  // Possible scenarios:
  // - everything is random, no correlations at all, no asymmetries at all
  // - variations in the mean of position, current, yield over the course of
  //   a run (linearly with run number, change mean/sigma as function of event
  //   number)
  // - one parameter has a helicity-correlated asymmetry, every other parameter
  //   is random and independently distributed
  // - two parameters have independent helicity-correlated asymmetries
  // - two parameters have correlated helicity-correlated asymmetries
  // - beam modulation

  // Get the beamline channels we want to correlate
  QwBeamLine* beamline = dynamic_cast<QwBeamLine*>(detectors.GetSubsystemByName("Injector BeamLine"));
  if (! beamline) QwWarning << "No beamline subsystem defined!" << QwLog::endl;
  beamline->LoadMockDataParameters("mock_data_parameters.map");



//-----------------------------------------------------------------------------------------------
  // Get the main detector channels we want to correlate
  MollerMainDetector* maindetector =
    dynamic_cast<MollerMainDetector*>(detectors.GetSubsystemByName("Main Detector"));
  if (! maindetector) QwWarning << "No main detector subsystem defined!" << QwLog::endl;

/*
if(1==2){
  Double_t bar_mean = 2.0e4;
  Double_t bar_sigma = 3.0e2;
  Double_t bar_asym = 4.0e-4;
  maindetector->SetRandomEventParameters(bar_mean, bar_sigma);
  maindetector->SetRandomEventAsymmetry(bar_asym);
  // Specific values
  // disabled, wdc, 2010-07-23
  maindetector->GetIntegrationPMT("MD2Neg")->SetRandomEventAsymmetry(2.0e-2);
  maindetector->GetIntegrationPMT("MD2Pos")->SetRandomEventAsymmetry(2.0e-2);
  maindetector->GetIntegrationPMT("MD3Neg")->SetRandomEventAsymmetry(3.0e-3);
  maindetector->GetIntegrationPMT("MD3Pos")->SetRandomEventAsymmetry(3.0e-3);
  maindetector->GetIntegrationPMT("MD4Neg")->SetRandomEventAsymmetry(4.0e-4);
  maindetector->GetIntegrationPMT("MD4Pos")->SetRandomEventAsymmetry(4.0e-4);
  maindetector->GetIntegrationPMT("MD5Neg")->SetRandomEventAsymmetry(5.0e-5);
  maindetector->GetIntegrationPMT("MD5Pos")->SetRandomEventAsymmetry(5.0e-5);
  maindetector->GetIntegrationPMT("MD6Neg")->SetRandomEventAsymmetry(6.0e-6);
  maindetector->GetIntegrationPMT("MD6Pos")->SetRandomEventAsymmetry(6.0e-6);
  maindetector->GetIntegrationPMT("MD7Neg")->SetRandomEventAsymmetry(7.0e-7);
  maindetector->GetIntegrationPMT("MD7Pos")->SetRandomEventAsymmetry(7.0e-7);
  maindetector->GetIntegrationPMT("MD8Neg")->SetRandomEventAsymmetry(8.0e-8);
  maindetector->GetIntegrationPMT("MD8Pos")->SetRandomEventAsymmetry(8.0e-8);

  // Set a asymmetric helicity asymmetry on one of the bars
  maindetector->GetIntegrationPMT("MD1Neg")->SetRandomEventAsymmetry(5.0e-5);
  maindetector->GetIntegrationPMT("MD1Pos")->SetRandomEventAsymmetry(-5.0e-5);

  // Set a drift component (amplitude, phase, frequency)
  maindetector->GetIntegrationPMT("MD3Neg")->AddRandomEventDriftParameters(3.0e6, 0, 60*Qw::Hz);
  maindetector->GetIntegrationPMT("MD3Neg")->AddRandomEventDriftParameters(6.0e5, 0, 120*Qw::Hz);
  maindetector->GetIntegrationPMT("MD3Neg")->AddRandomEventDriftParameters(4.5e5, 0, 240*Qw::Hz);
  
} //end if(1==2)
*/


  // Initialize randomness provider and distribution
  boost::mt19937 randomnessGenerator(999); // Mersenne twister with seed (see below)
  boost::normal_distribution<double> normalDistribution;
  boost::variate_generator
    < boost::mt19937, boost::normal_distribution<double> >
      normal(randomnessGenerator, normalDistribution);
  // WARNING: This variate_generator will return the SAME random values as the
  // variate_generator in QwVQWK_Channel when used with the same default seed!
  // Therefore you really should give an explicitly different seed for the
  // mt19937 randomness generator.



  // Loop over all runs
  UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
  UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
  for (UInt_t run  = runnumber_min;
              run <= runnumber_max;
              run++) {

     QwCombinedBCM<QwVQWK_Channel>::SetTripSeed(0x56781234 ^ (run*run));

    // Open new output file
    // (giving run number as argument to OpenDataFile confuses the segment search)
  
    
    TString filename = Form("%sQwMock_%u.log", eventbuffer.GetDataDirectory().Data(), run);
    if (eventbuffer.OpenDataFile(filename,"W") != CODA_OK) {
      std::cout << "Error: could not open file!" << std::endl;
      return 0;
    }
    eventbuffer.ResetControlParameters();
    eventbuffer.EncodePrestartEvent(run, 0); // prestart: runnumber, runtype
    eventbuffer.EncodeGoEvent();


    // Helicity initialization loop
    helicity->SetEventPatternPhase(-1, -1, -1);
    // 24-bit seed, should be larger than 0x1, 0x55 = 0101 0101
    // Consecutive runs should have no trivially related seeds:
    // e.g. with 0x2 * run, the first two files will be just 1 MPS offset...
    unsigned int seed = 0x1234 ^ run;
    helicity->SetFirstBits(24, seed & 0xFFFFFF);


    // Retrieve the requested range of event numbers
    if (kDebug) std::cout << "Starting event loop..." << std::endl;
    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");

    // Warn when only few events are requested, probably a problem in the input
    if (abs(eventnumber_max - eventnumber_min) < 10)
      QwWarning << "Only " << abs(eventnumber_max - eventnumber_min)
                << " events will be generated." << QwLog::endl;

    // Event generation loop
    for (Int_t event = eventnumber_min; event <= eventnumber_max; event++) {

      // First clear the event
      detectors.ClearEventData();

      // Set the event, pattern and phase number
      // - event number increments for every event
      // - pattern number increments for every multiplet
      // - phase number gives position in multiplet
      helicity->SetEventPatternPhase(event, event / kMultiplet, event % kMultiplet + 1);

      // Run the helicity predictor
      helicity->RunPredictor();
      // Concise helicity printout
      if (kDebug) {
        // - actual helicity
        if      (helicity->GetHelicityActual() == 0) std::cout << "-";
        else if (helicity->GetHelicityActual() == 1) std::cout << "+";
        else std::cout << "?";
        // - delayed helicity
        if      (helicity->GetHelicityDelayed() == 0) std::cout << "(-) ";
        else if (helicity->GetHelicityDelayed() == 1) std::cout << "(+) ";
        else std::cout << "(?) ";
        if (event % kMultiplet + 1 == 4) {
          std::cout << std::hex << helicity->GetRandomSeedActual()  << std::dec << ",  \t";
          std::cout << std::hex << helicity->GetRandomSeedDelayed() << std::dec << std::endl;
        }
      }

      // Calculate the time assuming one ms for every helicity window
      double helicity_window = Qw::ms;
      double time = event * helicity_window;

      // Fill the detectors with randomized data
      int myhelicity = helicity->GetHelicityActual() ? +1 : -1;


      // Secondly introduce correlations between variables
      //
      // N-dimensional correlated normal random variables:
      //   X = C' * Z
      // with
      //   X correlated and normally distributed,
      //   Z independent and normally distributed,
      //   C the Cholesky decomposition of the positive-definite covariance matrix
      //     (C should probably be calculated offline)
      //
      /* Sigma =
           1.00000   0.50000   0.50000
           0.50000   2.00000   0.30000
           0.50000   0.30000   1.50000

         C =
           1.00000   0.50000   0.50000
           0.00000   1.32288   0.03780
           0.00000   0.00000   1.11739

         Sigma = C' * C
       */
      double z[NVARS], x[NVARS];
      double C[NVARS][NVARS];
      for (int var = 0; var < NVARS; var++) {
        x[var] = 0.0;
        z[var] = normal();
        C[var][var] = 1.0;
      }
      C[0][0] = 1.0; C[0][1] = 0.5;     C[0][2] = 0.5;
      C[1][0] = 0.0; C[1][1] = 1.32288; C[1][2] = 0.03780;
      C[2][0] = 0.0; C[2][1] = 0.0;     C[2][2] = 1.11739;
      for (int i = 0; i < NVARS; i++)
        for (int j = 0; j < NVARS; j++)
          x[i] += C[j][i] * z[j];

      // Assign to data elements
      //maindetector->GetChannel("MD2Neg")->SetExternalRandomVariable(x[0]);
      //lumidetector->GetChannel("dlumi1")->SetExternalRandomVariable(x[1]);
      //beamline->GetBCM("qwk_bcm0l07")->SetExternalRandomVariable(x[2]);


      // Randomize data for this event
      detectors.RandomizeEventData(myhelicity, time);
//      detectors.ProcessEvent();
//      beamline-> ProcessEvent(); //Do we need to keep this line now?  Check the maindetector correlation with beamline devices with and without it.
      maindetector->ExchangeProcessedData();
      maindetector->RandomizeMollerEvent(myhelicity);

      // Write this event to file
      eventbuffer.EncodeSubsystemData(detectors);

      // Periodically print event number
      if ((kDebug && event % 1000 == 0)
                  || event % 10000 == 0)
        std::cout << "Generated " << event << " events." << std::endl;


    } // end of event loop


    eventbuffer.EncodeEndEvent();
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

    QwMessage << "Wrote mock data run " << filename << " successfully." << QwLog::endl;

  } // end of run loop

} // end of main
