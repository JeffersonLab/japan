/**********************************************************\
* File: QwHelicityPattern.cc                              *
*                                                         *
* Author:                                                 *
* Time-stamp:                                              *
\**********************************************************/

#include "QwHelicityPattern.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"
#include "QwHelicity.h"
#include "QwBlinder.h"
#include "VQwDataElement.h"


/*****************************************************************/
/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwHelicityPattern::DefineOptions(QwOptions &options)
{
  options.AddOptions("Helicity pattern")
    ("enable-burstsum", po::value<bool>()->default_bool_value(false),
     "enable burst sum calculation");
  options.AddOptions("Helicity pattern")
    ("enable-runningsum", po::value<bool>()->default_bool_value(true),
     "enable running sum calculation");
  options.AddOptions("Helicity pattern")
    ("enable-differences", po::value<bool>()->default_bool_value(false),
     "store pattern differences in tree");
  options.AddOptions("Helicity pattern")
    ("enable-alternateasym", po::value<bool>()->default_bool_value(false),
     "enable alternate asymmetries");

  options.AddOptions("Helicity pattern")
    ("print-burstsum", po::value<bool>()->default_bool_value(false),
     "print burst sum of subsystems");
  options.AddOptions("Helicity pattern")
    ("print-runningsum", po::value<bool>()->default_bool_value(false),
     "print running sum of subsystems");

  options.AddOptions("Helicity pattern")
    ("burstlength", po::value<int>()->default_value(240),
     "number of patterns per burst");

  QwBlinder::DefineOptions(options);
}

/*****************************************************************/
void QwHelicityPattern::ProcessOptions(QwOptions &options)
{
  fEnableBurstSum   = options.GetValue<bool>("enable-burstsum");
  fEnableRunningSum = options.GetValue<bool>("enable-runningsum");
  fPrintBurstSum    = options.GetValue<bool>("print-burstsum");
  fPrintRunningSum  = options.GetValue<bool>("print-runningsum");

  fEnableDifference    = options.GetValue<bool>("enable-differences");
  fEnableAlternateAsym = options.GetValue<bool>("enable-alternateasym");

  fBurstLength = options.GetValue<int>("burstlength");
  if (fBurstLength == 0) DisableBurstSum();

  if (fEnableAlternateAsym && fPatternSize <= 2){
    QwWarning << "QwHelicityPattern::ProcessOptions: "
	      << "The 'enable-alternateasym' flag is disabled for pair analysis."
	      << QwLog::endl;
    fEnableAlternateAsym = kFALSE;
  }

  fBlinder.ProcessOptions(options);
}

/*****************************************************************/
QwHelicityPattern::QwHelicityPattern(QwSubsystemArrayParity &event, const TString &run)
  : fBlinder(),
    fHelicityIsMissing(kFALSE),   
    fIgnoreHelicity(kFALSE),
    fYield(event), 
    fAsymmetry(event),   
    fEnableAlternateAsym(kFALSE), 
    fAsymmetry1(event), 
    fAsymmetry2(event),
    fEnableBurstSum(kFALSE),      
    fPrintBurstSum(kFALSE),
    fPairYield(event), 
    fPairDifference(event), 
    fPairAsymmetry(event),
    fBurstYield(event), 
    fBurstDifference(event), 
    fBurstAsymmetry(event),
    fRunningBurstYield(event), 
    fRunningBurstDifference(event), 
    fRunningBurstAsymmetry(event),
    fEnableRunningSum(kTRUE),     
    fPrintRunningSum(kFALSE),
    fEnableDifference(kFALSE),
    fDifference(event), 
    fAlternateDiff(event),
    fPositiveHelicitySum(event), 
    fNegativeHelicitySum(event),
    fLastWindowNumber(0),
    fLastPatternNumber(0),
    fLastPhaseNumber(0),
    fNextPair(0),
    correlator(gQwOptions,*this, run),
    regress_from_LRB(gQwOptions,*this, run),
    regression(gQwOptions,*this),
    running_regression(regression)
{
  // Retrieve the helicity subsystem to query for
  std::vector<VQwSubsystem*> subsys_helicity = event.GetSubsystemByType("QwHelicity");
  if (subsys_helicity.size() > 0) {

    // Take the first helicity subsystem
    //    fHelicitySubsystem = dynamic_cast<QwHelicity*>(subsys_helicity.at(0));
    QwHelicity* helicity = dynamic_cast<QwHelicity*>(subsys_helicity.at(0));

    // Get the maximum pattern phase (i.e. pattern size)
    fPatternSize = helicity->GetMaxPatternPhase();

    // Warn if more than one helicity subsystem defined
    if (subsys_helicity.size() > 1)
      QwWarning << "Multiple helicity subsystems defined! "
                << "Using " << helicity->GetSubsystemName() << "."
                << QwLog::endl;

  } else {
    // We are not using any helicity subsystem
    QwError << "No helicity subsystem defined!  " 
	    << "Calculate asymmetries based on (+--+) quartets!"
	    << QwLog::endl;
    fHelicityIsMissing = kTRUE;
    fPatternSize = 4; // default to quartets
  }
  QwMessage << "QwHelicity::MaxPatternPhase = " << fPatternSize << QwLog::endl;

  try
    {
      if(fPatternSize%2 == 0)
        {
          fEvents.resize(fPatternSize,event);
          fHelicity.resize(fPatternSize,-9999);
          fEventNumber.resize(fPatternSize,-1);
          fEventLoaded.resize(fPatternSize,kFALSE);

          // Initialize the pattern number
          fQuartetNumber = 0;
          fCurrentPatternNumber = -1;

          // Clear the burst sum
          ClearBurstSum();

          // Clear the running sum
          ClearRunningSum();

        }
      else
        {
          TString loc=
            "Standard exception from QwHelicityPattern : the pattern size has to be even;  right now pattern_size=";
          loc+=Form("%d",fPatternSize);
          throw std::invalid_argument(loc.Data());
        }
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

}

/*****************************************************************/
QwHelicityPattern::QwHelicityPattern(const QwHelicityPattern &source)
  : 
    fYield(source.fYield), 
    fAsymmetry(source.fAsymmetry),   
    fEnableAlternateAsym(source.fEnableAlternateAsym), 
    fAsymmetry1(source.fAsymmetry1), 
    fAsymmetry2(source.fAsymmetry2),
    fEnableRunningSum(source.fEnableRunningSum),
    fPrintRunningSum(source.fPrintRunningSum),
    fEnableDifference(source.fEnableDifference),
    fDifference(source.fDifference),
    fPairYield(source.fYield), 
    fPairDifference(source.fYield),
    fPairAsymmetry(source.fYield),
    fBurstYield(source.fYield), 
    fBurstDifference(source.fYield),
    fBurstAsymmetry(source.fYield),
    fRunningBurstYield(source.fYield), 
    fRunningBurstDifference(source.fYield), 
    fRunningBurstAsymmetry(source.fYield),
    fAlternateDiff(source.fYield),
    fPositiveHelicitySum(source.fYield), 
    fNegativeHelicitySum(source.fYield),
    fNextPair(source.fNextPair),
    correlator(gQwOptions,*this, 999999),
    regress_from_LRB(gQwOptions,*this,999999),
    regression(gQwOptions,*this),
    running_regression(regression)
{
};



//*****************************************************************
/**
 * Load event data corresponding to the current pattern from the
 * subsystems.
 */
void QwHelicityPattern::LoadEventData(QwSubsystemArrayParity &event)
{

  Bool_t localdebug = kFALSE;
  fPatternIsGood = kFALSE;

  Long_t localEventNumber = -1;
  Long_t localPatternNumber = -1;
  Int_t  localPhaseNumber = -1;
  Int_t  localHelicityActual = -1;
  Bool_t localIgnoreHelicity = kFALSE;
  
  
  // Get the list of helicity subsystems
  if (! fHelicityIsMissing){
    std::vector<VQwSubsystem*> subsys_helicity = event.GetSubsystemByType("QwHelicity");
    QwHelicity* helicity = 0;
    
    if (subsys_helicity.size() > 0) {
      // Take the first helicity subsystem
      helicity = dynamic_cast<QwHelicity*>(subsys_helicity.at(0));
      if (helicity->HasDataLoaded()){
	localIgnoreHelicity = helicity->IsHelicityIgnored();
	// Get the event, pattern, phase number and helicity
	localEventNumber    = helicity->GetEventNumber();
	localPatternNumber  = helicity->GetPatternNumber();
	localPhaseNumber    = helicity->GetPhaseNumber() - helicity->GetMinPatternPhase(); // Use "reduced pattern phase", counts from 0.
	localHelicityActual = helicity->GetHelicityActual();
      } else {
	QwError << "QwHelicityPattern::LoadEventData:  The helicity subsystem does not have valid data!"
		<< QwLog::endl;
      }
    } else {
      // We are not usng any helicity subsystem
      static Bool_t user_has_been_warned = kFALSE;
      if (! user_has_been_warned) {
	QwError << "No helicity subsystem found!  Dropping to \"Missing Helicity\" mode!" << QwLog::endl;
	user_has_been_warned = kTRUE;
	fHelicityIsMissing = kTRUE;
      }
    }
  }
  if (fHelicityIsMissing){
    localIgnoreHelicity = kTRUE;
    localPatternNumber = fLastPatternNumber;
    localEventNumber   = fLastWindowNumber + 1;
    localPhaseNumber   = fLastPhaseNumber + 1;
    if(localPhaseNumber>=fPatternSize){
      localPatternNumber++;
      localPhaseNumber = 0; // Use "reduced pattern phase", counts from 0.
    }
    fLastWindowNumber  = localEventNumber;
    fLastPhaseNumber   = localPhaseNumber;
    fLastPatternNumber = localPatternNumber; 
  }
  if(localdebug) {
    std::cout<<"\n ###################################\n";
    std::cout<<"QwHelicityPattern::LoadEventData :: ";
    std::cout<<" event, pattern, phase # "<<localEventNumber<<" "<<localPatternNumber<<" "<<localPhaseNumber<<"\n";
    std::cout<<" helicity ="<< localHelicityActual<<"\n";
    for(size_t i=0; i<fEvents.size(); i++)
      std::cout<<i<<":"<<fEventLoaded[i]<<"  ";
    std::cout<<"\n";
  }
  if(fCurrentPatternNumber!=localPatternNumber){
    // new pattern
    ClearEventData();
    fCurrentPatternNumber=localPatternNumber;
  }
  if(localPhaseNumber<0){
    QwWarning << "QwHelicityPattern::LoadEventData:  "
	    << "Reduced event phase number is less than zero; ignore this event."
	    << QwLog::endl;
    ClearEventData();
  } else if(localPhaseNumber>=fPatternSize){
    QwWarning<<" In QwHelicityPattern::LoadEventData trying upload an event with a phase larger than expected \n"
	   <<" phase ="<<localPhaseNumber+1<<" maximum expected phase="<<fPatternSize<<"\n"
	   <<" operation impossible, pattern reset to 0: no asymmetries will be computed "<<QwLog::endl;
    ClearEventData();
  } else {
    if(localdebug){
      std::cout<<"QwHelicityPattern::LoadEventData local i="
	       <<localPhaseNumber<<"\n";
    }
    fEvents[localPhaseNumber]      = event;
    fEventLoaded[localPhaseNumber] = kTRUE;
    fHelicity[localPhaseNumber]    = localHelicityActual;
    fEventNumber[localPhaseNumber] = localEventNumber;
    // Check to see if we should ignore the helicity; this is
    // reset to false in ClearEventData.
    fIgnoreHelicity |= localIgnoreHelicity;
    SetDataLoaded(kTRUE);
  }
  if(localdebug){
    Print();
  }
  return;
}

Bool_t QwHelicityPattern::PairAsymmetryIsGood()
{
  Bool_t complete_and_good = kFALSE;
  if (NextPairIsComplete()){
    CalculatePairAsymmetry();  /*  Uses the same calculational variables as the pattern */
    complete_and_good = fPairIsGood;
  }
  return complete_and_good;
};

Bool_t QwHelicityPattern::NextPairIsComplete()
{
  Bool_t filled=kFALSE;
  // Potentially this function could check all possible pairs to see which are complete, if the initial
  // fNextPair is not complete.  The function must end with fNextPair equal to the index of the lowest pair
  // that is complete.
  if (fNextPair<fPatternSize/2){
    size_t firstevt  = fNextPair*2;
    size_t secondevt = firstevt + 1;
    filled = fEventLoaded.at(firstevt) && fEventLoaded.at(secondevt);
  }
  return (filled);
}

void  QwHelicityPattern::CalculatePairAsymmetry()
{

  Bool_t localdebug=kFALSE;

  if(localdebug)  std::cout<<"Entering QwHelicityPattern::CalculateAsymmetry \n";

  Int_t plushel  = 1;
  Int_t minushel = 0;
  Int_t checkhel = 0;

  if (fNextPair<fPatternSize/2){
    size_t firstevt  = fNextPair*2;
    size_t secondevt = firstevt + 1;
    fPairIsGood = kTRUE;
    fNextPair++;

    fPairYield.Sum(fEvents.at(firstevt), fEvents.at(secondevt));
    fPairYield.Scale(0.5);
  
    if (fIgnoreHelicity){
      fPairDifference.Difference(fEvents.at(firstevt), fEvents.at(secondevt));
      fPairDifference.Scale(0.5);
    } else {
      if (fHelicity[firstevt] == plushel && fHelicity[firstevt]!=fHelicity[secondevt]) {
	fPairDifference.Difference(fEvents.at(firstevt), fEvents.at(secondevt));
	fPairDifference.Scale(0.5);
      } else if (fHelicity[firstevt] == minushel && fHelicity[firstevt]!=fHelicity[secondevt]) {
	fPairDifference.Difference(fEvents.at(secondevt),fEvents.at(firstevt));
	fPairDifference.Scale(0.5);
      } else {
	QwDebug << "QwHelicityPattern::CalculatePairAsymmetry:  "
		<< "Helicity should be "<<plushel<<" or "<<minushel
		<<" but is "<< fHelicity[firstevt] << " and " << fHelicity[secondevt]
		<< "; Asymmetry computation aborted!"<<QwLog::endl;
	fPairIsGood = kFALSE;
	// there is a different number of plus and minus helicity window.
	QwError<<" QwHelicityPattern::CalculatePairAsymmetry == \n"
	       <<" you do not have the same number of positive and negative \n"
	       <<" impossible to compute assymetry \n"
	       <<" dropping every thing -- pattern number ="<<fCurrentPatternNumber<<QwLog::endl;
	// This is an unknown helicity event.
      }
    }
  }

  if (fPairIsGood){
    if (! fIgnoreHelicity){
      //      // Update the blinder if conditions have changed
      //      UpdateBlinder(fPairYield);
      //  Only blind the difference if we're using the real helicity.
      fBlinder.Blind(fPairDifference,fPairYield);
      //  Update the global error code in fDifference, and use it
      //  to update the errors in fYield, in case blinder errors
      //  can propagate to the global error.
      fPairDifference.UpdateErrorFlag();
      fPairYield.UpdateErrorFlag(fPairDifference);
    }
    fPairAsymmetry.Ratio(fPairDifference,fPairYield);
    //    fAsymmetry.IncrementErrorCounters();
  }
}


Bool_t QwHelicityPattern::IsGoodAsymmetry()
{
  Bool_t complete_and_good = kFALSE;
  if (IsCompletePattern()){
    CalculateAsymmetry();
    complete_and_good = fPatternIsGood;
  }
  return complete_and_good;
};

//*****************************************************************
/**
 * Check to see if the pattern is complete.
 */
Bool_t  QwHelicityPattern::IsCompletePattern() const
{
  Bool_t localdebug=kFALSE;
  Bool_t filled=kTRUE;
  Int_t i=fPatternSize-1;
  while(filled && i>-1)
    {
      if (localdebug){
        std::cout<<" i="<<i<<" is loaded ?"
                 <<fEventLoaded[fEvents.size()-i-1]<<"\n";
      }
      if(!fEventLoaded[i])
        filled=kFALSE;
      i--;
    }



  return filled;
}


//*****************************************************************
/**
 * Calculate asymmetries for the current pattern.
 */
void  QwHelicityPattern::CalculateAsymmetry()
{

  Bool_t localdebug=kFALSE;

  if(localdebug)  std::cout<<"Entering QwHelicityPattern::CalculateAsymmetry \n";

  Int_t plushel  = 1;
  Int_t minushel = 0;
  Int_t checkhel = 0;
  Bool_t firstplushel=kTRUE;
  Bool_t firstminushel=kTRUE;

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();

  if (fIgnoreHelicity){
    //  Don't check to see if we have equal numbers of even and odd helicity states in this pattern.
    //  Build an asymmetry with even-parity phases as "+" and odd-parity phases as "-"
    for (size_t i = 0; i < (size_t) fPatternSize; i++) {
      Int_t localhel = 1;
      for (size_t j = 0; j < (size_t) fPatternSize/2; j++) {
	localhel ^= ((i >> j)&0x1);
      }
      if (localhel == plushel) {
	if (firstplushel) {
	  fPositiveHelicitySum = fEvents.at(i);
	  firstplushel = kFALSE;
	} else {
	  fPositiveHelicitySum += fEvents.at(i);
	}
      } else if (localhel == minushel) {
	if (firstminushel) {
	  fNegativeHelicitySum = fEvents.at(i);
	  firstminushel = kFALSE;
	} else {
	  fNegativeHelicitySum += fEvents.at(i);
	}
      }
    }
  } else {
    //  
    for (size_t i = 0; i < (size_t) fPatternSize; i++) {
      if (fHelicity[i] == plushel) {
	if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  here filling fPositiveHelicitySum \n";
	if (firstplushel) {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  with = \n";
	  fPositiveHelicitySum = fEvents.at(i);
	  firstplushel = kFALSE;
	} else {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  with += \n";
	  fPositiveHelicitySum += fEvents.at(i);
	}
	checkhel += 1;
      } else if (fHelicity[i] == minushel) {
	if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  here filling fNegativeHelicitySum \n";
	if (firstminushel) {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  with = \n";
	  fNegativeHelicitySum = fEvents.at(i);
	  firstminushel = kFALSE;
	} else {
	  if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:  with += \n";
	  fNegativeHelicitySum += fEvents.at(i);
	}
	checkhel -= 1;
      } else {
	QwDebug << "QwHelicityPattern::CalculateAsymmetry:  "
		<< "Helicity should be "<<plushel<<" or "<<minushel
		<<" but is "<< fHelicity[i]
		<< "; Asymmetry computation aborted!"<<QwLog::endl;
	ClearEventData();
	i = fPatternSize;
	checkhel = -9999;
	// This is an unknown helicity event.
      }
    }
  }

  if (checkhel == -9999) {
    //do nothing the asymmetry computation has been aborted earlier in this function
    fPatternIsGood = kFALSE;
  } else if (checkhel!=0) {
    fPatternIsGood = kFALSE;
    // there is a different number of plus and minus helicity window.
    QwError<<" QwHelicityPattern::CalculateAsymmetry == \n"
	   <<" you do not have the same number of positive and negative \n"
	   <<" impossible to compute assymetry \n"
	   <<" dropping every thing -- pattern number ="<<fCurrentPatternNumber<<QwLog::endl;
  } else {
    //  This is a good pattern.
    //  Calculate the asymmetry.
    fPatternIsGood = kTRUE;
    fQuartetNumber++;//Then increment the quartet number
    //std::cout<<" quartet count ="<<fQuartetNumber<<"\n";

    fYield.Sum(fPositiveHelicitySum,fNegativeHelicitySum);
    fYield.Scale(1.0/fPatternSize);
    fDifference.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
    fDifference.Scale(1.0/fPatternSize);

    
    
    if (! fIgnoreHelicity){
      // Update the blinder if conditions have changed
      UpdateBlinder(fYield);
      //  Only blind the difference if we're using the real helicity.
      fBlinder.Blind(fDifference,fYield);
      //  Update the global error code in fDifference, and use it
      //  to update the errors in fYield, in case blinder errors
      //  can propagate to the global error.
      fDifference.UpdateErrorFlag();
      fYield.UpdateErrorFlag(fDifference);
    }
    fAsymmetry.Ratio(fDifference,fYield);
    fAsymmetry.IncrementErrorCounters();

    /*
      With additional two asymmetry calculations
      Don't blind them!

      quartet pattern + - - +
                      1 2 3 4
                      fAsymmetry  = (1+4)-(2+3)/(1+2+3+4)
                      fAsymmetry1 = (1+2)-(3+4)/(1+2+3+4)
                      fAsymmetry2 = (1+3)-(2+4)/(1+2+3+4)
    */
    if (fEnableAlternateAsym) {
      //  fAsymmetry1:  (first 1/2 pattern - second 1/2 pattern)/fYield
      fPositiveHelicitySum.ClearEventData();
      fNegativeHelicitySum.ClearEventData();
      fPositiveHelicitySum = fEvents.at(0);
      fNegativeHelicitySum = fEvents.at(fPatternSize/2);
      if (fPatternSize/2 > 1){
	for (size_t i = 1; i < (size_t) fPatternSize/2 ; i++){
	  fPositiveHelicitySum += fEvents.at(i);
	  fNegativeHelicitySum += fEvents.at(fPatternSize/2 +i);
	}
      }
      fAlternateDiff.ClearEventData();
      fAlternateDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
      fAlternateDiff.Scale(1.0/fPatternSize);
      //  Do not blind this helicity-uncorrelated difference.
      fAsymmetry1.Ratio(fAlternateDiff,fYield);
      //  fAsymmetry2:  (even events - odd events)/fYield
      //  Only build fAsymmetry2 if fPatternSize>2.
      if (fPatternSize > 2) {
	fPositiveHelicitySum.ClearEventData();
	fNegativeHelicitySum.ClearEventData();
	fPositiveHelicitySum = fEvents.at(0);
	fNegativeHelicitySum = fEvents.at(1);
	if (fPatternSize/2 > 1){
	  for (size_t i = 1; i < (size_t) fPatternSize/2 ; i++){
	    fPositiveHelicitySum += fEvents.at(2*i);
	    fNegativeHelicitySum += fEvents.at(2*i + 1);
	  }
	}
	fAlternateDiff.ClearEventData();
	fAlternateDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
	fAlternateDiff.Scale(1.0/fPatternSize);
	//  Do not blind this helicity-uncorrelated difference.
	fAsymmetry2.Ratio(fAlternateDiff,fYield);
      }
    }

    // Accumulate the burst and running sums
    if (fEnableBurstSum)   AccumulateBurstSum();
    
    if (localdebug) QwDebug << " pattern number =" << fQuartetNumber << QwLog::endl;
  }

  return;
}

//*****************************************************************
/**
 * Clear event data and the vectors used for the calculation of.
 * yields and asymmetries.
 */
void QwHelicityPattern::ClearEventData()
{
  fIgnoreHelicity = kFALSE;
  for(size_t i=0; i<fEvents.size(); i++)
    {
      fEvents[i].ClearEventData();
      fEventLoaded[i]=kFALSE;
      fHelicity[i]=-999;
    }
  fBlinder.ClearEventData();

  // Primary yield and asymmetry
  fYield.ClearEventData();
  fAsymmetry.ClearEventData();
  // Alternate asymmetries
  if (fEnableAlternateAsym){
    fAsymmetry1.ClearEventData();
    fAsymmetry2.ClearEventData();
  }

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();
  fDifference.ClearEventData();
  fAlternateDiff.ClearEventData();

  fPairIsGood = kFALSE;
  fNextPair   = 0;

  fPatternIsGood = kFALSE;
  SetDataLoaded(kFALSE);
}

//*****************************************************************
/**
 * Clear the running sums of yield, difference and asymmetry.
 * Also clear the running burst sums if enabled.
 */
void  QwHelicityPattern::ClearRunningSum()
{
  fYield.ClearEventData();
  fDifference.ClearEventData();
  fAsymmetry.ClearEventData();
  // Running alternate asymmetries
  if (fEnableAlternateAsym) {
    fAsymmetry1.ClearEventData();
    fAsymmetry2.ClearEventData();
  }
  // Running burst sums
  if (fEnableBurstSum) {
    fRunningBurstYield.ClearEventData();
    fRunningBurstDifference.ClearEventData();
    fRunningBurstAsymmetry.ClearEventData();
  }
}

//*****************************************************************
/**
 * Clear the burst sums of yield and difference.  No asymmetry
 * burst sum is used.
 */
void  QwHelicityPattern::ClearBurstSum()
{
  if (fEnableBurstSum) {
    fBurstYield.ClearEventData();
    fBurstDifference.ClearEventData();
    fBurstAsymmetry.ClearEventData();
  }
}

//*****************************************************************
/**
 * Accumulate the burst sum by adding this helicity pattern to the
 * burst sums of yield and difference.  There is no burst sum of
 * asymmetry, because that can only be calculated with meaningful
 * moments at the end of a burst.
 */
void  QwHelicityPattern::AccumulateBurstSum()
{
  if (fPatternIsGood){
    fBurstYield.AccumulateRunningSum(fYield);
    fBurstDifference.AccumulateRunningSum(fDifference);
    // The difference is blinded, so the burst difference is also blinded.
  }
}

//*****************************************************************
/**
 * Accumulate the running sum by adding this helicity pattern to the
 * running sums of yield, difference and asymmetry.
 */
void  QwHelicityPattern::AccumulateRunningSum(QwHelicityPattern &entry)
{
  if (entry.fPatternIsGood){
    fYield.AccumulateRunningSum(entry.fYield);
    fAsymmetry.AccumulateRunningSum(entry.fAsymmetry);
    if (fEnableDifference){
      fDifference.AccumulateRunningSum(entry.fDifference);
      // The difference is blinded, so the running difference is also blinded.
    }
    if (fEnableAlternateAsym) {
      fAsymmetry1.AccumulateRunningSum(entry.fAsymmetry1);
      fAsymmetry2.AccumulateRunningSum(entry.fAsymmetry2);
    }
  }
}

//*****************************************************************
/**
 * Accumulate the running burst sum by adding the current burst sum
 * to the running sums of burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::AccumulateRunningBurstSum()
{
  // Accumulate the burst yield and difference
  if (fEnableRunningSum) {
    fRunningBurstYield.AccumulateRunningSum(fBurstYield);
    fRunningBurstDifference.AccumulateRunningSum(fBurstDifference);
    // The burst difference is blinded, so the running burst difference is also blinded.
  }

  // Calculate asymmetry over this entire burst
  fBurstAsymmetry.Ratio(fBurstDifference, fBurstYield);
  // Accumulate this burst asymmetry
  fRunningBurstAsymmetry.AccumulateRunningSum(fBurstAsymmetry);

  // Be sure to clear the burst sums after this function!
}

//*****************************************************************
/**
 * Calculate the average burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::CalculateBurstAverage()
{
  fBurstAsymmetry.CalculateRunningAverage();
  fBurstDifference.CalculateRunningAverage();
  fBurstYield.CalculateRunningAverage();

  if (fPrintBurstSum) PrintBurstAverage();
}

//*****************************************************************
/**
 * Calculate the average running burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::CalculateRunningBurstAverage()
{
  fRunningBurstAsymmetry.CalculateRunningAverage();
  fRunningBurstDifference.CalculateRunningAverage();
  fRunningBurstYield.CalculateRunningAverage();

  if (fPrintBurstSum) PrintRunningBurstAverage();
}

//*****************************************************************
void  QwHelicityPattern::CalculateRunningAverage()
{
  fAsymmetry.CalculateRunningAverage();
  if (fEnableDifference){
    fDifference.CalculateRunningAverage();
  }
  fYield.CalculateRunningAverage();
  if (fEnableAlternateAsym) {
    fAsymmetry1.CalculateRunningAverage();
    fAsymmetry2.CalculateRunningAverage();
  }
  if (fPrintRunningSum) PrintRunningAverage();
}

//*****************************************************************
void  QwHelicityPattern::PrintRunningBurstAverage() const
{
  QwMessage << " Running burst average of asymmetry" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstAsymmetry.PrintValue();

  QwMessage << " Running burst average of difference" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstDifference.PrintValue();

  QwMessage << " Running burst average of yields" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstYield.PrintValue();
}

//*****************************************************************
void  QwHelicityPattern::PrintRunningAverage() const
{
  QwMessage << "QwHelicityPattern::PrintRunningAverage() const " << QwLog::endl;
  //  QwMessage << " Running average of yields     " << QwLog::endl;
  fYield.PrintValue();
  //  QwMessage << " Running average of asymmetry  " << QwLog::endl;
  fAsymmetry.PrintValue();
  if (fEnableAlternateAsym) {
    //    QwMessage << " Running average of first half/second half asymmetry  "  << QwLog::endl;
    fAsymmetry1.PrintValue();
    //    QwMessage << " Running average of even/odd asymmetry  " << QwLog::endl;
    fAsymmetry2.PrintValue();
  }
  if (fEnableDifference){
    //    QwMessage << " Running average of difference " << QwLog::endl;
    fDifference.PrintValue();
  }

}

//*****************************************************************
void  QwHelicityPattern::PrintBurstAverage() const
{
  QwMessage << " Burst average of asymmetry  " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fBurstAsymmetry.PrintValue();

  QwMessage << " Burst average of difference " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fBurstDifference.PrintValue();

  QwMessage << " Burst average of yields     " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fBurstYield.PrintValue();
}

//*****************************************************************
void  QwHelicityPattern::ConstructHistograms(TDirectory *folder)
{
  TString prefix = "yield_";
  fYield.ConstructHistograms(folder,prefix);
  prefix = "asym_";
  fAsymmetry.ConstructHistograms(folder,prefix);

  if (fEnableDifference) {
    prefix = "diff_";
    fDifference.ConstructHistograms(folder,prefix);
  }
  if (fEnableAlternateAsym) {
    prefix = "asym1_";
    fAsymmetry1.ConstructHistograms(folder,prefix);
    prefix = "asym2_";
    fAsymmetry2.ConstructHistograms(folder,prefix);
  }
}

void  QwHelicityPattern::FillHistograms()
{
  if (fPatternIsGood) {
    fYield.FillHistograms();
    fAsymmetry.FillHistograms();
    if (fEnableDifference) {
      fDifference.FillHistograms();
    }
    if (fEnableAlternateAsym){
      fAsymmetry1.FillHistograms();
      fAsymmetry2.FillHistograms();
    }
  }
}

void QwHelicityPattern::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  TString newprefix = "yield_" + prefix;
  fYield.ConstructBranchAndVector(tree, newprefix, values);
  newprefix = "asym_" + prefix;
  fAsymmetry.ConstructBranchAndVector(tree, newprefix, values);

  if (fEnableDifference) {
    newprefix = "diff_" + prefix;
    fDifference.ConstructBranchAndVector(tree, newprefix, values);
  }
  if (fEnableAlternateAsym) {
    newprefix = "asym1_" + prefix;
    fAsymmetry1.ConstructBranchAndVector(tree, newprefix, values);
    newprefix = "asym2_" + prefix;
    fAsymmetry2.ConstructBranchAndVector(tree, newprefix, values);
  }
}

void QwHelicityPattern::ConstructBranch(TTree *tree, TString & prefix)
{
  TString newprefix = "yield_" + prefix;
  fYield.ConstructBranch(tree, newprefix);
  newprefix = "asym_" + prefix;
  fAsymmetry.ConstructBranch(tree, newprefix);

  if (fEnableDifference) {
    newprefix = "diff_" + prefix;
    fDifference.ConstructBranch(tree, newprefix);
  }
  if (fEnableAlternateAsym) {
    newprefix = "asym1_" + prefix;
    fAsymmetry1.ConstructBranch(tree, newprefix);
    newprefix = "asym2_" + prefix;
    fAsymmetry2.ConstructBranch(tree, newprefix);
  }
}

void QwHelicityPattern::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile &trim_tree)
{
  TString newprefix = "yield_" + prefix;
  fYield.ConstructBranch(tree, newprefix, trim_tree);
  newprefix = "asym_" + prefix;
  fAsymmetry.ConstructBranch(tree, newprefix, trim_tree);

  if (fEnableDifference) {
    newprefix = "diff_" + prefix;
    fDifference.ConstructBranch(tree, newprefix);
  }
  if (fEnableAlternateAsym) {
    newprefix = "asym1_" + prefix;
    fAsymmetry1.ConstructBranch(tree, newprefix, trim_tree);
    newprefix = "asym2_" + prefix;
    fAsymmetry2.ConstructBranch(tree, newprefix, trim_tree);
  }
}

void QwHelicityPattern::FillTreeVector(std::vector<Double_t> &values) const
{
  if (fPatternIsGood) {
    fYield.FillTreeVector(values);
    fAsymmetry.FillTreeVector(values);
    if (fEnableDifference) {
      fDifference.FillTreeVector(values);
    }
    if (fEnableAlternateAsym) {
      fAsymmetry1.FillTreeVector(values);
      fAsymmetry2.FillTreeVector(values);
    }
  }
}

#ifdef __USE_DATABASE__
void QwHelicityPattern::FillDB(QwParityDB *db)
{
  fBlinder.FillDB(db,"");

  fYield.FillDB(db, "yield");
  fAsymmetry.FillDB(db, "asymmetry");
  if (fEnableDifference) {
    fDifference.FillDB(db, "difference");
  }
  if (fEnableAlternateAsym) {
    fAsymmetry1.FillDB(db, "asymmetry1");
    fAsymmetry2.FillDB(db, "asymmetry2");
  }
}


void QwHelicityPattern::FillErrDB(QwParityDB *db)
{
  fBlinder.FillErrDB(db,"");
  fAsymmetry.FillErrDB(db, "");
  return;
};
#endif // __USE_DATABASE__

void QwHelicityPattern::WritePromptSummary(QwPromptSummary *ps)
{
  fYield.WritePromptSummary(ps, "yield");
  fAsymmetry.WritePromptSummary(ps, "asymmetry");
  
  return;
};


//*****************************************************************

void QwHelicityPattern::Print() const
{
  QwOut << "Pattern number = " << fCurrentPatternNumber << QwLog::endl;
  for (Int_t i = 0; i < fPatternSize; i++)
    QwOut << "Event " << fEventNumber[i] << ": "
          << fEventLoaded[i] << ", " << fHelicity[i] << QwLog::endl;
  QwOut << "Is a complete pattern? (n/y:0/1) " << IsCompletePattern() << QwLog::endl;
}

void QwHelicityPattern::ProcessDataHandlerEntry() {

	regression.LinearRegression(QwCombiner::kRegTypeAsym);
	running_regression.AccumulateRunningSum(regression);
  regress_from_LRB.LinearRegression(QwCombiner::kRegTypeAsym);
  correlator.FillCorrelator();

}

void QwHelicityPattern::FinishDataHandler() {

  correlator.CalcCorrelations();
  running_regression.CalculateRunningAverage();
  running_regression.PrintValue();

}





