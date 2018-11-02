/**********************************************************\
* File: QwBeamMod.cc                                      *
*                                                         *
* Author: Joshua Hoskins                                  *
* Time-stamp: 052510                                      *
\**********************************************************/

#include "QwBeamMod.h"

// System headers
#include <iostream>
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwParameterFile.h"
#include "QwHistogramHelper.h"
#define MYSQLPP_SSQLS_NO_STATICS
#ifdef __USE_DATABASE__
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__

// Root plotting headers
#include "TCanvas.h"
#include "TF1.h"
#include "TMath.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwBeamMod);


//*****************************************************************
void QwBeamMod::ProcessOptions(QwOptions &options){
      //Handle command line options
}

Int_t QwBeamMod::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t wordsofar=0;
  Int_t currentsubbankindex=-1;
  Int_t sample_size=0;

  TString varname, varvalue;

  // Open the file
  QwParameterFile mapstr(mapfile.Data());
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  mapstr.EnableGreediness();
  mapstr.SetCommentChars("!");
  while (mapstr.ReadNextLine() && mapstr.SkipSection("Monitors"))
  {
    UInt_t value = 0;
    if (mapstr.PopValue("roc",value)) {
      currentrocread = value;
      RegisterROCNumber(value,0);
      fWordsPerSubbank.push_back( std::pair<Int_t, Int_t>(fWord.size(),fWord.size()));
    }
    if (mapstr.PopValue("bank",value)) {
      currentbankread = value;
      RegisterSubbank(value);
      fWordsPerSubbank.push_back( std::pair<Int_t, Int_t>(fWord.size(),fWord.size()));
      //    std::cout<<"bank " <<  currentbankread <<std::endl;
    }
    if (mapstr.PopValue("sample_size",value)) {
      sample_size = value;
    }

    if (mapstr.HasVariablePair("=",varname,varvalue))
    {
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      QwWarning << "QwBeamMod::LoadChannelMap: Unrecognized declaration "
          << varname << " = " << varvalue << QwLog::endl;

    } else {
      Bool_t lineok=kTRUE;
      //  Break this line into tokens to process it.
      TString modtype = mapstr.GetTypedNextToken<TString>();	// module type
      /* Int_t modnum    = */ mapstr.GetTypedNextToken<Int_t>();	//slot number
      /* Int_t channum   = */ mapstr.GetTypedNextToken<Int_t>();	//channel number
      //	TString dettype   = mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
      //	dettype.ToLower();
      TString namech  = mapstr.GetTypedNextToken<TString>();  //name of the detector
      namech.ToLower();
      TString keyword = mapstr.GetTypedNextToken<TString>();
      keyword.ToLower();


      if (currentsubbankindex != GetSubbankIndex(currentrocread,currentbankread))
      {
        currentsubbankindex = GetSubbankIndex(currentrocread,currentbankread);
        wordsofar = 0;
      }


      QwModChannelID localModChannelID(currentsubbankindex, wordsofar,namech, modtype, this);


      if(modtype=="VQWK")
      {
        wordsofar+=6;

        if (lineok){
          QwVQWK_Channel localchan;
          localchan.InitializeChannel(GetSubsystemName(),"QwBeamMod",localModChannelID.fmodulename,"raw");
          fModChannel.push_back(localchan);
          fModChannel[fModChannel.size()-1].SetDefaultSampleSize(sample_size);
          localModChannelID.fIndex=fModChannel.size()-1;
          fModChannelID.push_back(localModChannelID);

          // Store reference to ramp channel
          if (localModChannelID.fmodulename == "ramp") {
            fRampChannelIndex = fModChannel.size() - 1;
          }
        }

        if(ldebug)
        {
          localModChannelID.Print();
          std::cout<<"line ok=";
          if(lineok) std::cout<<"TRUE"<<std::endl;
          else std::cout<<"FALSE"<<std::endl;
        }
      }


      if(modtype == "WORD")
      {
        //  std::cout << "Decoding QwWord :: " << namech << std::endl;


        QwWord localword;
        localword.fSubbankIndex=currentsubbankindex;
        localword.fWordInSubbank=wordsofar;
        wordsofar+=1;
        // I assume that one data = one word here. But it is not always the case, for
        // example the triumf adc gives 6 words per channel
        localword.fModuleType=modtype;
        localword.fWordName=namech;
        //localword.fWordType=dettype; // FIXME dettype is undefined so commented this out
        fWord.push_back(localword);

        // Store reference to pattern number
        if (localword.fWordName == "bm_pattern_number") {
          fPatternWordIndex = fWord.size() - 1;
        }

        if(namech=="ffb_status")//save the location of this word to access this later
          fFFB_Index=fWord.size()-1;


        fWordsPerSubbank[currentsubbankindex].second = fWord.size();
        QwDebug << "--" << namech << "--" << fWord.size()-1 << QwLog::endl;

      }
    }
  }
  if(ldebug)
  {
    std::cout<<"Done with Load map channel \n";
    for(size_t i=0;i<fModChannelID.size();i++)
      fModChannelID[i].Print();

    for(size_t i=0;i<fWord.size();i++)
      fWord[i].PrintID();
  }

  // Now load the variables to monitor
  mapstr.RewindToFileStart();
  while (QwParameterFile *section = mapstr.ReadNextSection(varvalue)) {
    if (varvalue == "Monitors") {
      fMonitorNames.clear();
      while (section->ReadNextLine()) {
        section->TrimComment();         // Remove everything after a comment character
        section->TrimWhitespace();      // Get rid of leading and trailing spaces
        varvalue = section->GetTypedNextToken<TString>();
	if (varvalue.Length() > 0) {
          // Add names of monitor channels for each degree of freedom
	  //
	  //
	  // Consider whether or not the input monitor is a bpm or not.  This should
	  // allow for usage of monitors other than bpms, .eg bcm{1,2}, lumi, pmtonl,..ect.
	  //

	  if(varvalue.Contains("qwk_bpm", TString::kExact)){
	    fMonitorNames.push_back(Form("%sX",varvalue.Data()));
	    fMonitorNames.push_back(Form("%sY",varvalue.Data()));
	  }
	  else if(varvalue.Contains("target", TString::kExact)){
	    fMonitorNames.push_back(Form("%sX",varvalue.Data()));
	    fMonitorNames.push_back(Form("%sY",varvalue.Data()));
	  }
	  else{
	    fMonitorNames.push_back(varvalue);
	  }
        }
      }
    }
    delete section;
  }
  // Resize local version of the BPMs
  QwVQWK_Channel dummy("dummy");
  fMonitors.resize(fMonitorNames.size(),dummy);
  ResizeOpticsDataContainers(fMonitorNames.size());

  // Debug output
  if (ldebug) {
    QwMessage << "Done with loading monitor channels:" << QwLog::endl;
    for (size_t i = 0; i < fMonitorNames.size(); i++)
      QwMessage << fMonitorNames[i] << QwLog::endl;
  }

  return 0;
}



QwModChannelID::QwModChannelID(Int_t subbankid, Int_t wordssofar,
				   TString name,  TString modtype, QwBeamMod * obj):
  fSubbankIndex(subbankid),fWordInSubbank(wordssofar),
  fmoduletype(modtype),fmodulename(name),kUnknownDeviceType(-1)
{
  fTypeID = kQwUnknownDeviceType;
  //  for(size_t i=0;i<obj->fgModTypeNames.size();i++){
 //   if(dettype == obj->fgModTypeNames[i]){
 //     fTypeID = EBeamInstrumentType(i);
  //   std::cout << "Detector type not recognized" << std::endl;
   //   break;
   // }
  //  }
//   if (fTypeID == kUnknownDeviceType) {
//     std::cerr << "QwModChannelID::QwModChannelID:  Unknown detector type: "
//   	      << dettype <<", the detector "<<name<<" will not be decoded "
//   	      << std::endl;
//   }
}


//*****************************************************************

Int_t QwBeamMod::LoadEventCuts(TString  filename)
{
  Int_t eventcut_flag = 1;

  // Open the file
  QwParameterFile mapstr(filename.Data());
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  fFFB_holdoff=0;//Default holdoff for the FFB pause
  while (mapstr.ReadNextLine()){
    //std::cout<<"********* In the loop  *************"<<std::endl;
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname == "EVENTCUTS"){
	eventcut_flag = QwParameterFile::GetUInt(varvalue);
	//std::cout<<"EVENT CUT FLAG "<<eventcut_flag<<std::endl;
      }
    }
    else{
      TString device_type = mapstr.GetTypedNextToken<TString>();
      device_type.ToLower();
      TString device_name = mapstr.GetTypedNextToken<TString>();
      device_name.ToLower();


      if (device_type == "vqwk"){

	Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for BCM value
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for BCM value
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	varvalue.ToLower();
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	QwMessage<<"QwBeamMod Error Code  "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	Int_t det_index=GetDetectorIndex(GetDetectorTypeID(kQwUnknownDeviceType),device_name);
	QwMessage << "*****************************" << QwLog::endl;
	QwMessage << " Type " << device_type << " Name " << device_name << " Index [" << det_index << "] "
	          << " device flag " << eventcut_flag << QwLog::endl;
	fModChannel[det_index].SetSingleEventCuts((GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)|kBModErrorFlag),LLX,ULX,stabilitycut);
	QwMessage << "*****************************" << QwLog::endl;

      }
      else if (device_type == "word" && device_name== "ffb_status"){
	fFFB_holdoff=mapstr.GetTypedNextToken<UInt_t>();//Read the FFB OFF interval
      }

    }

  }
  //update the event cut ON/OFF for all the devices
  //std::cout<<"EVENT CUT FLAG"<<eventcut_flag<<std::endl;
  for (size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].SetEventCutMode(eventcut_flag);

  return 0;
}


//*****************************************************************
Int_t QwBeamMod::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;
  TString varname;

  Int_t lineread=1;

  if(ldebug)std::cout<<"QwBeamMod::LoadInputParameters("<< pedestalfile<<")\n";

  QwParameterFile mapstr(pedestalfile.Data());  //Open the file
  while (mapstr.ReadNextLine())
    {
      lineread+=1;
      if(ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;
      else
	{
	  TString varname = mapstr.GetTypedNextToken<TString>();	//name of the channel
	  varname.ToLower();
	  varname.Remove(TString::kBoth,' ');
	  Double_t varped = mapstr.GetTypedNextToken<Double_t>(); // value of the pedestal
	  Double_t varcal = mapstr.GetTypedNextToken<Double_t>(); // value of the calibration factor
	  /* Double_t varweight = */ mapstr.GetTypedNextToken<Double_t>(); // value of the statistical weight

	  //if(ldebug) std::cout<<"inputs for channel "<<varname
	  //	      <<": ped="<<varped<<": cal="<<varcal<<": weight="<<varweight<<"\n";
	  for(size_t i=0;i<fModChannel.size();i++)
	    if(fModChannel[i].GetElementName()==varname)
		{
		  fModChannel[i].SetPedestal(varped);
		  fModChannel[i].SetCalibrationFactor(varcal);
		  break;
		}
	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
}


Int_t QwBeamMod::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    if (lkDEBUG)
      QwMessage << "QwBeamMod::ProcessEvBuffer:  "
		<< "Begin processing ROC" << roc_id
		<< " and subbank "<<bank_id
		<< " number of words="<<num_words<<QwLog::endl;
    if (buffer[0]==0xf0f0f0f0 && num_words%2==1){
      buffer++;
      if (lkDEBUG)
	QwMessage << "QwBeamMod::ProcessEvBuffer:  "
		  << "Skipped padding word 0xf0f0f0f0 at beginning of buffer."
		  << QwLog::endl;
    }

    for(size_t i=0;i<fModChannelID.size();i++)
      {
	if(fModChannelID[i].fSubbankIndex==index)
	  {

		if (lkDEBUG)
		  {
		    std::cout<<"found modulation data for "<<fModChannelID[i].fmodulename<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fModChannelID[i].fWordInSubbank<<std::endl;
		  }
		fModChannel[fModChannelID[i].fIndex].  // Instead of BCM this will become our data word.If we use detectorID we need to add a specific ID
		  ProcessEvBuffer(&(buffer[fModChannelID[i].fWordInSubbank]),
				  num_words-fModChannelID[i].fWordInSubbank);

	  }
      }

     for(Int_t i=fWordsPerSubbank[index].first; i<fWordsPerSubbank[index].second; i++) {
      if(fWord[i].fWordInSubbank+1<= (Int_t) num_words) {
	fWord[i].fValue=buffer[fWord[i].fWordInSubbank];
      } else {
	QwWarning << "QwBeamMod::ProcessEvBuffer:  There is not enough word in the buffer to read data for "
		  << fWord[i].fWordName << QwLog::endl;
	QwWarning << "QwBeamMod::ProcessEvBuffer:  Words in this buffer:" << num_words
		  << " trying to read word number =" << fWord[i].fWordInSubbank << QwLog::endl;
      }
    }
    if(lkDEBUG) {
      QwDebug << "QwBeamMod::ProcessEvBuffer:  Done with Processing this event" << QwLog::endl;
      for(size_t i=0;i<fWord.size();i++) {
	std::cout << " word number = " << i << " ";
	fWord[i].Print();
      }
    }

  }
  lkDEBUG=kFALSE;

  SetDataLoaded(kTRUE);

  return 0;
}

Bool_t QwBeamMod::ApplySingleEventCuts(){

  Bool_t test_Mod=kTRUE;
  Bool_t test_BCM1=kTRUE;


  for(size_t i=0;i<fModChannel.size();i++){
    //std::cout<<"  BCM ["<<i<<"] "<<std::endl;
    test_BCM1 = fModChannel[i].ApplySingleEventCuts();
    test_Mod &= test_BCM1;
    if(!test_BCM1 && bDEBUG) std::cout<<"******* QwBeamMod::SingleEventCuts()->BCM[ "<<i<<" , "<<fModChannel[i].GetElementName()<<" ] ******\n";
  }
  
  if (fWord[fFFB_Index].fValue==8 && fFFB_Flag && fFFB_holdoff_Counter==0){
    fFFB_holdoff_Counter=fFFB_holdoff;
    fFFB_Flag=kFALSE;
    
  } 
  if (!fFFB_Flag &&  (fFFB_holdoff_Counter>0 && fFFB_holdoff_Counter<=fFFB_holdoff) ){
    fFFB_ErrorFlag = (kGlobalCut+kBModErrorFlag+kBModFFBErrorFlag+kEventCutMode3);
    fFFB_holdoff_Counter--;
    if(fFFB_holdoff_Counter==0)
      fFFB_Flag=kTRUE;
  }else
    fFFB_ErrorFlag=0;



  return test_Mod;

}

void QwBeamMod::IncrementErrorCounters()
{
  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i].IncrementErrorCounters();
  }
}

void QwBeamMod::PrintErrorCounters() const{//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwBeamMod Error Summary****************"<<std::endl;
  QwVQWK_Channel::PrintErrorCounterHead();
  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i].PrintErrorCounters();
  }
  QwVQWK_Channel::PrintErrorCounterTail();
}

void QwBeamMod::UpdateErrorFlag(const VQwSubsystem *ev_error)
{
  const QwBeamMod* input = dynamic_cast<const QwBeamMod*> (ev_error);

  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i].UpdateErrorFlag(input->fModChannel[i]);
  }
}

UInt_t QwBeamMod::GetEventcutErrorFlag(){//return the error flag
  UInt_t ErrorFlag;
  ErrorFlag=0;
  for(size_t i=0;i<fModChannel.size();i++){
    ErrorFlag |= fModChannel[i].GetEventcutErrorFlag();
  }
  ErrorFlag |= fFFB_ErrorFlag;
  return ErrorFlag;

}



void  QwBeamMod::ProcessEvent()
{
  for (size_t i = 0; i < fModChannel.size(); i++) {
    // First apply HW checks and update HW  error flags.
    fModChannel[i].ApplyHWChecks();
    fModChannel[i].ProcessEvent();
  }
}

void  QwBeamMod::ExchangeProcessedData()
{
  // Make sure sizes are equal
  if (fMonitorNames.size() != fMonitors.size())
    QwError << "QwBeamMod: Sizes of fBPMnames and fBPMs do not match!" << QwLog::endl;
  // Loop over BPMs
  for (size_t bpm = 0; bpm < fMonitorNames.size(); bpm++) {
    // Get references to external values
    if (! RequestExternalValue(fMonitorNames[bpm],&fMonitors[bpm]))
      QwError << "QwBeamMod: RequestExternalValue for " << fMonitorNames[bpm]
              << " failed!" << QwLog::endl;
  }
}

void  QwBeamMod::ProcessEvent_2()
{
  // Fill histograms here to bypass event cuts
  FillHistograms();
}

Int_t QwBeamMod::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
}

//*****************************************************************

void QwBeamMod::ClearEventData()
{
   for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].ClearEventData();

   for (size_t i=0;i<fWord.size();i++)
    fWord[i].ClearEventData();
}

//*****************************************************************
Int_t QwBeamMod::GetDetectorTypeID(TString name)
{
  Int_t result=-1;
  for(size_t i=0;i<fgModTypeNames.size();i++)
    if(name==fgModTypeNames[i])
      {result=i;i=fgModTypeNames.size()+1;}
  return result;
}

//*****************************************************************
Int_t QwBeamMod::GetDetectorIndex(Int_t type_id, TString name)
{
  Bool_t ldebug=kFALSE;
  if(ldebug)
    {
      std::cout<<"QwBeamMod::GetDetectorIndex\n";
      std::cout<<"type_id=="<<type_id<<" name="<<name<<"\n";
      std::cout<<fModChannelID.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for(size_t i=0;i<fModChannelID.size();i++)
    {

	if(fModChannelID[i].fmodulename==name)
	  result=fModChannelID[i].fIndex;
      if(ldebug)
	{
	  std::cout<<"testing against ("<<fModChannelID[i].fTypeID
		   <<","<<fModChannelID[i].fmodulename<<")=>"<<result<<"\n";
	}
    }

  return result;
}
//*****************************************************************
VQwSubsystem&  QwBeamMod::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwBeamMod::operator= \n";
  if(Compare(value))
    {

      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);
      
      for(size_t i=0;i<input->fModChannel.size();i++)
	this->fModChannel[i]=input->fModChannel[i];
      for(size_t i=0;i<input->fWord.size();i++)
  	this->fWord[i].fValue=input->fWord[i].fValue;
      this->fFFB_ErrorFlag=input->fFFB_ErrorFlag;
    }
  return *this;
}

VQwSubsystem&  QwBeamMod::operator+=  (VQwSubsystem *value)
{
  if(Compare(value))
    {
      //QwBeamMod* input= (QwBeamMod*)value ;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

      for(size_t i=0;i<input->fModChannel.size();i++)
	this->fModChannel[i]+=input->fModChannel[i];
//         for(size_t i=0;i<input->fWord.size();i++)
//    	this->fWord[i]+=input->fWord[i];
      this->fFFB_ErrorFlag |= input->fFFB_ErrorFlag;
    }
  return *this;
}

VQwSubsystem&  QwBeamMod::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      //QwBeamMod* input= (QwBeamMod*)value;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

      for(size_t i=0;i<input->fModChannel.size();i++)
	this->fModChannel[i]-=input->fModChannel[i];
//       for(size_t i=0;i<input->fWord.size();i++)
//         this->fWord[i]-=input->fWord[i];
      this->fFFB_ErrorFlag |= input->fFFB_ErrorFlag;

    }
  return *this;
}

void  QwBeamMod::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
}

void  QwBeamMod::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
}

void QwBeamMod::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if(Compare(numer)&&Compare(denom))
    {
      //QwBeamMod* innumer= (QwBeamMod*)numer ;
      QwBeamMod* innumer = dynamic_cast<QwBeamMod*>(numer);
      //QwBeamMod* indenom= (QwBeamMod*)denom ;
      QwBeamMod* indenom = dynamic_cast<QwBeamMod*>(denom);

      for(size_t i=0;i<innumer->fModChannel.size();i++)
	this->fModChannel[i].Ratio(innumer->fModChannel[i],indenom->fModChannel[i]);
      for(size_t i=0;i<innumer->fWord.size();i++)
	this->fWord[i].fValue=innumer->fWord[i].fValue;

    }
}


void QwBeamMod::Scale(Double_t factor)
{
  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].Scale(factor);
}

void QwBeamMod::CalculateRunningAverage() { }

void QwBeamMod::AccumulateRunningSum(VQwSubsystem*) { }

Bool_t QwBeamMod::Compare(VQwSubsystem *value)
{
  Bool_t res = kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
      //      std::cout<<" types are not ok \n";
      //      std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    }
  else
    {
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

	if(input->fModChannel.size()!=fModChannel.size())
	  {
	    res=kFALSE;
	  //	  std::cout<<" not the same number of bcms \n";
	  }
    }
  return res;
}


//*****************************************************************

void  QwBeamMod::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  // Go to the specified folder
  if (folder != NULL) folder->cd();

  // No histogram creation for asym, yield, diff, etc
  if (prefix != "") return;

  TString basename;
  for (size_t bpm = 0; bpm < fMonitorNames.size(); bpm++) {
    // Find histogram with correct name
    basename = TString("bmod_") + prefix + fMonitorNames[bpm];
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_X"));
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_Y"));
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_E"));
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_XP"));
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_YP"));
  }

  // Beam modulation correlations
  for (size_t chan = 0; chan < fModChannel.size(); chan++) {
    // Find histogram with correct name
    basename = TString("bmod_") + prefix + fModChannel[chan].GetElementName();
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_X"));
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_Y"));
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_E"));
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_XP"));
    fHistograms.push_back(gQwHists.Construct1DProf(basename + "_YP"));
  }

  // Beam modulation channels
  //for (size_t i = 0; i < fModChannel.size(); i++)
  //  fModChannel[i].ConstructHistograms(folder,prefix);
}

void  QwBeamMod::FillHistograms()
{
  // No data loaded
  if (! HasDataLoaded()) return;

  // No histograms defined
  if (fHistograms.size() == 0) return;

  // Beam modulation channels
  //for (size_t i = 0; i < fModChannel.size(); i++)
  //  fModChannel[i].FillHistograms();

  // Do we have a ramp channel?
  if (fRampChannelIndex < 0 || fRampChannelIndex > Int_t(fModChannel.size())) return;

  // Do we have a pattern word?
  if (fPatternWordIndex < 0 || fPatternWordIndex > Int_t(fWord.size())) return;

  // Determine the ramp/phase
  Double_t ramp = fModChannel[fRampChannelIndex].GetValue();
  if (ramp < 0) return;

  // Determine the pattern number -- the pattern number for single coil is
  // between [0, 4] so we need to check for this.
  Int_t pattern = -1;
  
  if(fWord[fPatternWordIndex].fValue < 11 && fWord[fPatternWordIndex].fValue > 0)
    pattern = fWord[fPatternWordIndex].fValue;
  else
    pattern = fWord[fPatternWordIndex].fValue - 11;
  
  if (pattern < 0 || pattern > 4) return;

  // Fill histograms for all BPMs and each of the modulation patterns
  //
  // Due to the the way the ADC averages the ramp signal we want to filter
  // out events at the edged of the signal.
  //
  // Seperated the ramp cut here because it is ridiculously long... 
  //

  Double_t ramp_block_41 = fModChannel[fRampChannelIndex].GetValue(4) + fModChannel[fRampChannelIndex].GetValue(1); 
  Double_t ramp_block_32 = fModChannel[fRampChannelIndex].GetValue(3) + fModChannel[fRampChannelIndex].GetValue(2);
  Double_t ramp_block    = ramp_block_41 - ramp_block_32;  

  //  Require the difference between the block1/4 average and 
  //  the block2/3 average to be within 5 degrees.
  //  In run 9636, the distribution of ramp_block has a sigma of 0.5 degrees.
  if( ramp_block < 5.0 && ramp_block > -5.0 ){
    for (size_t bpm = 0; bpm < fMonitors.size(); bpm++){
      fHistograms[5 * bpm + pattern]->Fill(ramp,fMonitors[bpm].GetValue());
    }
  
    // Beam modulation correlations
    for (size_t chan = 0; chan < fModChannel.size(); chan++){
      fHistograms[5 * (fMonitors.size() + chan) + pattern]->Fill(ramp,fModChannel[chan].GetValue());
    }
  }
}

void QwBeamMod::AtEndOfEventLoop()
{

  AnalyzeOpticsPlots();

}

void QwBeamMod::ClearVectors()
{
    fOffset.clear();
    fAmplitude.clear();
    fPhase.clear();
    fOffsetError.clear();
    fAmplitudeError.clear();
    fPhaseError.clear();
    fChisquare.clear();
    fNFitPoints.clear();
    
}

void QwBeamMod::ResizeOpticsDataContainers(Int_t size)
{
  fOffset.resize(size);
  fAmplitude.resize(size);
  fPhase.resize(size);
  fOffsetError.resize(size);
  fAmplitudeError.resize(size);
  fPhaseError.resize(size);
  fChisquare.resize(size);
  fNFitPoints.resize(size);

  for(Int_t i = 0; i < size; i++){
    fOffset[i].resize(fNumberPatterns);
    fAmplitude[i].resize(fNumberPatterns);
    fPhase[i].resize(fNumberPatterns);
    fOffsetError[i].resize(fNumberPatterns);
    fAmplitudeError[i].resize(fNumberPatterns);
    fPhaseError[i].resize(fNumberPatterns);
    fChisquare[i].resize(fNumberPatterns);
    fNFitPoints[i].resize(fNumberPatterns);
  }
}

void QwBeamMod::AnalyzeOpticsPlots()
{
  //   How to get the run info:
  //   UInt_t runnum = this->GetParent()->GetCodaRunNumber();
  //   UInt_t segnum = this->GetParent()->GetCodaSegmentNumber();

  TF1 *sine = new TF1("sine", "[0] + [1]*sin(TMath::DegToRad()*x + [2])", 5, 350);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 5);

  //Double_t mean; // unused
  Double_t amplitude;
  Double_t phase;

  ResizeOpticsDataContainers(fMonitorNames.size());

  canvas->cd();
  for(size_t bpm = 0; bpm < fMonitors.size(); bpm++){

    for(size_t pattern = 0; pattern < 5; pattern++){
      //  Only do the fits if there are more than three entries.
      if (fHistograms[5*bpm + pattern]->GetEntries()>3){
	sine->SetParameters(fHistograms[5*bpm + pattern]->GetMean(), 0.10, 0);
	sine->SetLineColor(2);
	sine->SetParLimits(2, 0, TMath::Pi()*2 );
	fHistograms[5*bpm + pattern]->Fit("sine","R B");
	
	//mean = sine->GetParameter(0); // unused
	amplitude = TMath::Abs(sine->GetParameter(1));
	phase = sine->GetParameter(2) * TMath::RadToDeg();
	
	amplitude *= GetAmplitudeSign(sine->Derivative(10), 
				      sine->Derivative2(10), 
				      sine->Derivative3(10), 
				      sine->GetParameter(0));

	if(phase >= 180){
	  phase -= 180;
// 	  amplitude = -amplitude;
	} else if (phase < 0){
	  phase += 180;
// 	  amplitude = -amplitude;
	} 
	fOffset[bpm][pattern] = sine->GetParameter(0);
	fAmplitude[bpm][pattern] = amplitude;
	fPhase[bpm][pattern] = phase;
	fOffsetError[bpm][pattern] = sine->GetParError(0);
	fAmplitudeError[bpm][pattern] = sine->GetParError(1);
	fPhaseError[bpm][pattern] = sine->GetParError(2);
	fChisquare[bpm][pattern] = sine->GetChisquare();
	fNFitPoints[bpm][pattern] = sine->GetNumberFitPoints();
      } else {
	QwDebug << "QwBeamMod can't fit [" << bpm << "][" << pattern
		<< "] because there are only " 
		<< fHistograms[5*bpm + pattern]->GetEntries()
		<< " entries, and we need at least 3."
		<< QwLog::endl;
	//  No events in this histogram.  Zero-out the fit results
	fOffset[bpm][pattern]         = 0.0;
	fAmplitude[bpm][pattern]      = 0.0;
	fPhase[bpm][pattern]          = 0.0;
	fOffsetError[bpm][pattern]    = 0.0;
	fAmplitudeError[bpm][pattern] = 0.0;
	fPhaseError[bpm][pattern]     = 0.0;
	fChisquare[bpm][pattern]      = 0.0;
	fNFitPoints[bpm][pattern]     = 0;
      }
    }
  }
  delete canvas;
  delete sine;
}

Double_t QwBeamMod::GetAmplitudeSign(Double_t d1, Double_t d2, Double_t d3, Double_t fmean)
{

  Double_t sign = 0.0;

  if(d1 > 0.0 && d2 < 0.0)          sign =  1.0;
  else if(d1 == 0.0 && fmean > 0.0) sign =  1.0;
  else if(d1 < 0.0 && d2 < 0.0)     sign =  1.0;

  else if(d1 < 0.0 && d3 < 0)       sign =  1.0;

  else if(d1 < 0.0 && d2 > 0.0)     sign = -1.0;
  else if(d1 == 0.0 && fmean < 0.0) sign = -1.0;
  else if(d1 > 0.0 && d2 > 0.0)     sign = -1.0;

  else if(d1 < 0.0 && d3 > 0)       sign = -1.0;

  else
    sign = 1.0;

  return(sign);

}

void QwBeamMod::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  TString basename;
  
  for(size_t i = 0; i < fModChannel.size(); i++)
    fModChannel[i].ConstructBranchAndVector(tree, prefix, values);
//   for (size_t i=0;i<fWord.size();i++)
//     fWord[i].ConstructBranchAndVector(tree, prefix, values);
  fTreeArrayIndex  = values.size();
  for (size_t i=0; i<fWord.size(); i++)
	{
// 	  basename = fWord[i].fWordName;
 	  basename = prefix+fWord[i].fWordName;
	  values.push_back(0.0);
	  tree->Branch(basename, &(values.back()), basename+"/D");
	}
}

void QwBeamMod::FillTreeVector(std::vector<Double_t> &values) const
{
  size_t index = fTreeArrayIndex;

  for (size_t i = 0; i < fModChannel.size(); i++)
    fModChannel[i].FillTreeVector(values);
  for (size_t i = 0; i < fWord.size(); i++)
    values[index++] = fWord[i].fValue;
}


//*****************************************************************
void  QwBeamMod::Print()
{
  QwMessage << "Name of the subsystem =" << fSystemName << QwLog::endl;

  QwMessage << "there are " << fModChannel.size() << " mods" << QwLog::endl;

  QwMessage << " Printing Running AVG and other channel info for fModChannel" << QwLog::endl;
  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i].PrintValue();
  for(size_t i=0;i<fWord.size();i++)
    fWord[i].Print();
}

void  QwBeamMod::PrintModChannelID()
{
  for (size_t i=0;i<fModChannelID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fModChannelID[i].Print();
    }
}


void  QwModChannelID::Print()
{
  QwMessage <<std::endl<<"Detector name= "<<fmodulename<<QwLog::endl;
  QwMessage <<"SubbankkIndex= "<<fSubbankIndex<<QwLog::endl;
  QwMessage <<"word index in subbank= "<<fWordInSubbank<<QwLog::endl;
  QwMessage <<"module type= "<<fmoduletype<<QwLog::endl;
// QwMessage <<"detector type=  "<<fdetectortype<<" that is index="<<fTypeID<<QwLog::endl;
  QwMessage <<"Index of this detector in the vector of similar detector= "<<
    fIndex << QwLog::endl;
//QwMessage <<"Subelement index= "<< fSubelement<<QwLog::endl;


  QwMessage << "---------------------------------------------------" << QwLog::endl;
  QwMessage << QwLog::endl;
}

//*****************************************************************
#ifdef __USE_DATABASE__
void QwBeamMod::FillDB_MPS(QwParityDB *db, TString datatype)
{
  Bool_t local_print_flag = false;

  if(local_print_flag) {
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "            QwBeamMod::FillDB_MPS                       " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwParitySSQLS::beam_optics> entrylist;

  QwParitySSQLS::beam_optics row;

  UInt_t analysis_id = db->GetAnalysisID();

  for(size_t bpm = 0; bpm < fMonitors.size(); bpm++){
    for(size_t pattern = 0; pattern < 5; pattern++){
      //  Explicitly zero the beam optics ID to ensure a non-sensical default
      //  is not picked up.
      row.beam_optics_id = 0;
      row.analysis_id = analysis_id;
      row.monitor_id = db->GetMonitorID(fMonitorNames[bpm].Data());
      row.modulation_type_id = pattern;
      row.n = fNFitPoints[bpm][pattern];
      row.offset = fOffset[bpm][pattern];
      row.amplitude = fAmplitude[bpm][pattern];
      row.phase = fPhase[bpm][pattern];
      row.o_error = fOffsetError[bpm][pattern];
      row.a_error = fAmplitudeError[bpm][pattern];
      row.p_error = fPhaseError[bpm][pattern];
      row.gof_para = fChisquare[bpm][pattern];

      entrylist.push_back(row);
    }
  }

  if(local_print_flag) {
    QwMessage << QwColor(Qw::kGreen) << "Entrylist Size : "
	      << QwColor(Qw::kBoldRed) << entrylist.size()
              << QwColor(Qw::kNormal) << QwLog::endl;
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(entrylist.begin(), entrylist.end());
    query.execute();
  }
  else {
    QwMessage << "QwBeamMod::FillDB_MPS :: Nothing to insert in database." << QwLog::endl;
  }
  db->Disconnect();

  return;
}

void QwBeamMod::FillDB(QwParityDB *db, TString datatype)
{
  return;
}

void QwBeamMod::FillErrDB(QwParityDB *db, TString datatype)
{
  return;
}
#endif // __USE_DATABASE__

void QwBeamMod::WritePromptSummary(QwPromptSummary *ps, TString datatype)
{
  return;
};
