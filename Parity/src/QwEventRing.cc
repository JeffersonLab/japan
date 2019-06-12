#include "QwEventRing.h"


QwEventRing::QwEventRing(QwOptions &options, QwSubsystemArrayParity &event)
: fRollingAvg(event)
{
  ProcessOptions(options);

  fEvent_Ring.resize(fRING_SIZE,event);

  bRING_READY = kFALSE;
  bEVENT_READY = kTRUE;

  fNextToBeFilled = 0;
  fNextToBeRead = 0;

  //open the log file
  if (bDEBUG_Write)
    out_file = fopen("Ring_log.txt", "wt");
}


void QwEventRing::DefineOptions(QwOptions &options)
{
  // Define the execution options
  options.AddOptions()("ring.size",
      po::value<int>()->default_value(4800),
      "QwEventRing: ring/buffer size");
  options.AddOptions()("ring.stability_cut",
      po::value<double>()->default_value(1),
      "QwEventRing: Stability ON/OFF");
}

void QwEventRing::ProcessOptions(QwOptions &options)
{
  // Reads Event Ring parameters from cmd
  Double_t stability = 0.0;
  if (gQwOptions.HasValue("ring.size"))
    fRING_SIZE=gQwOptions.GetValue<int>("ring.size");

  if (gQwOptions.HasValue("ring.stability_cut"))
    stability=gQwOptions.GetValue<double>("ring.stability_cut");

  if (stability>0.0)
    bStability=kTRUE;
  else
    bStability=kFALSE;
 
}
void QwEventRing::push(QwSubsystemArrayParity &event)
{
  if (bDEBUG) QwMessage << "QwEventRing::push:  BEGIN" <<QwLog::endl;

  

  if (bEVENT_READY){
    Int_t thisevent = fNextToBeFilled;
    Int_t prevevent = (thisevent+fRING_SIZE-1)%fRING_SIZE;
    fEvent_Ring[thisevent]=event;//copy the current good event to the ring 
    if (bStability){
      fRollingAvg.AccumulateAllRunningSum(event);
    }


    if (bDEBUG) QwMessage<<" Filled at "<<thisevent;//<<"Ring count "<<fRing_Count<<QwLog::endl; 
    if (bDEBUG_Write) fprintf(out_file," Filled at %d ",thisevent);


    fNextToBeFilled=(thisevent+1)%fRING_SIZE;
    
    if(fNextToBeFilled == 0){
      //then we have RING_SIZE events to process
      if (bDEBUG) QwMessage<<" RING FILLED "<<thisevent; //<<QwLog::endl; 
      if (bDEBUG_Write) fprintf(out_file," RING FILLED ");
      bRING_READY=kTRUE;//ring is filled with good multiplets
      fNextToBeFilled=0;//next event to be filled is the first element  
    }
      //check for current ramps
    if (bRING_READY && bStability){
	fRollingAvg.CalculateRunningAverage();
	/*
	//The fRollingAvg dose not contain any regular errorcodes since it only accumulate rolling sum for errorflag==0 event.
	//The only errorflag it generates is the stability cut faliure error when the rolling avg is computed. 
	//Therefore when fRollingAvg.GetEventcutErrorFlag() is called it will return non-zero error code only if a global stability cut has failed
	//When fRollingAvg.GetEventcutErrorFlag() is called the fErrorFlag of the subsystemarrayparity object will be updated with any global
	//stability cut faliures
	*/
	fRollingAvg.UpdateErrorFlag(); //to update the global error code in the fRollingAvg
	if ( fRollingAvg.GetEventcutErrorFlag() != 0 ) {
	  //  This test really needs to determine in any of the subelements
	  //  might have a local stability cut failure, instead of just this
	  //  global stability cut failure.
	  for(Int_t i=0;i<fRING_SIZE;i++){
	    fEvent_Ring[i].UpdateErrorFlag(fRollingAvg);
	    fEvent_Ring[i].UpdateErrorFlag();
	  }
	}
	if ((fEvent_Ring[thisevent].GetEventcutErrorFlag() & kBCMErrorFlag)!=0 &&
	    (fEvent_Ring[prevevent].GetEventcutErrorFlag() & kBCMErrorFlag)!=0){
	  for(Int_t i=0;i<fRING_SIZE;i++){
	    fEvent_Ring[i].UpdateErrorFlag(kBeamTripError);
	  }
	}
    }
    //ring processing is done at a separate location
  }
}


QwSubsystemArrayParity& QwEventRing::pop(){
  Int_t tempIndex;
  tempIndex=fNextToBeRead;  
  if (bDEBUG) QwMessage<<" Read at "<<fNextToBeRead<<QwLog::endl; 
  if (bDEBUG_Write) fprintf(out_file," Read at %d \n",fNextToBeRead);
  
  if (fNextToBeRead==(fRING_SIZE-1)){
    bRING_READY=kFALSE;//setting to false is an extra measure of security to prevent reading a NULL value. 
  }
  if (bStability){
     fRollingAvg.DeaccumulateRunningSum(fEvent_Ring[tempIndex]);
  }
  fNextToBeRead=(fNextToBeRead+1)%fRING_SIZE;  
  return fEvent_Ring[tempIndex];  
}


Bool_t QwEventRing::IsReady(){ //Check for readyness to read data from the ring using the pop() routine   
  return bRING_READY;
}

