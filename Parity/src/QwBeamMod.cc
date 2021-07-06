/**********************************************************\
* File: QwBeamMod.cc                                      *
*                                                         *
* Author: Joshua Hoskins                                  *
* Time-stamp: 052510                                      *
*                                                         *
* Updated for PREX by Ezekiel Wertz and Paul King         *
* Time-Stamp: 06/17/19                                    *
*                                                         *
*                                                         *
\**********************************************************/

#include "QwBeamMod.h"

// System headers
#include <iostream>
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwParameterFile.h"
#include "QwHistogramHelper.h"
#ifdef __USE_DATABASE__
#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__

#include "QwScaler_Channel.h"

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

Int_t QwBeamMod::LoadChannelMap(TString mapfile){
  //  std::cout <<"Here in LoadChannelMap" << std::endl;
  Bool_t ldebug=kFALSE;

  Int_t wordsofar=0;
  Int_t bankindex=-1;
 

  TString varname, varvalue;

  // Open the file
  QwParameterFile mapstr(mapfile.Data());
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  mapstr.EnableGreediness();
  mapstr.SetCommentChars("!");
  while (mapstr.ReadNextLine())  {
    RegisterRocBankMarker(mapstr);

    if(bankindex!=GetSubbankIndex(fCurrentROC_ID,fCurrentBank_ID)) { 
      bankindex=GetSubbankIndex(fCurrentROC_ID,fCurrentBank_ID);
      if ((bankindex+1)>0){
	UInt_t numbanks = UInt_t(bankindex+1);
	if (fWordsPerSubbank.size()<numbanks){
	  fWordsPerSubbank.resize(numbanks,
				  std::pair<Int_t, Int_t>(fWord.size(),fWord.size()));
	}
      }
      wordsofar=0;
    }
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

   
    if (mapstr.HasVariablePair("=",varname,varvalue))
    {
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      QwWarning << "QwBeamMod::LoadChannelMap: Unrecognized declaration "
          << varname << " = " << varvalue << QwLog::endl;

    } else {
      Bool_t lineok=kTRUE;
      // QwModChannelID localModChannelID(bankindex, wordsofar,namech, modtype, this);
      QwModChannelID localModChannelID(bankindex, mapstr);
      TString namech  = localModChannelID.fmodulename; 
       if(localModChannelID.fmoduletype=="VQWK") {
        wordsofar+=6;

        if (lineok){
         VQwHardwareChannel* localchan1 = new QwVQWK_Channel();
	
	 localchan1->InitializeChannel(GetName(),"QwBeamMod",localModChannelID.fmodulename,"raw");
          fModChannel.push_back(localchan1);
	  fModChannel[fModChannel.size()-1]->LoadChannelParameters(mapstr);
          localModChannelID.fIndex=fModChannel.size()-1;
          fModChannelID.push_back(localModChannelID);
	  /*  localchan1->PrintInfo();
	  localchan1->PrintValue();
	  localchan1->PrintErrorCounters();*/
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

      if(localModChannelID.fmoduletype=="SCALER" || localModChannelID.fmoduletype == "SIS3801D24") {
        wordsofar+=1;
	
        if (lineok){
	  VQwHardwareChannel* localchan2 = new QwSIS3801D24_Channel();
	  
          localchan2->InitializeChannel(GetName(),"QwBeamMod",localModChannelID.fmodulename,"raw");
          fModChannel.push_back(localchan2);
	  fModChannel[fModChannel.size()-1]->LoadChannelParameters(mapstr);
          localModChannelID.fIndex=fModChannel.size()-1;
          fModChannelID.push_back(localModChannelID);
	  /* localchan2->PrintInfo();
	   localchan2->PrintValue();
	   localchan2->PrintErrorCounters();*/
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
      
      if (localModChannelID.fmoduletype == "SIS3801D32") {
        wordsofar+=1;
	
        if (lineok){
	  VQwHardwareChannel* localchan3 = new QwSIS3801D32_Channel();
       
          localchan3->InitializeChannel(GetName(),"QwBeamMod",localModChannelID.fmodulename,"raw");
          fModChannel.push_back(localchan3);
	  fModChannel[fModChannel.size()-1]->LoadChannelParameters(mapstr);
          localModChannelID.fIndex=fModChannel.size()-1;
          fModChannelID.push_back(localModChannelID);
	  /* localchan3->PrintInfo();
	  localchan3->PrintValue();
	  localchan3->PrintErrorCounters();*/
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

      if(localModChannelID.fmoduletype=="SKIP"){
	if (localModChannelID.modnum<=0) wordsofar+=1;
	else           wordsofar+= localModChannelID.modnum;
      
       }

      if(localModChannelID.fmoduletype == "WORD") {
        //  std::cout << "Decoding QwWord :: " << namech << std::endl;
        QwWord localword;
        localword.fSubbankIndex=bankindex;
        localword.fWordInSubbank=wordsofar;
        wordsofar+=1;
        // I assume that one data = one word here. But it is not always the case, for
        // example the triumf adc gives 6 words per channel
        localword.fModuleType=localModChannelID.fmoduletype;
        localword.fWordName=namech;
        //localword.fWordType=dettype; // FIXME dettype is undefined so commented this out
        fWord.push_back(localword);

        // Store reference to pattern number
        if (localword.fWordName == "bm_pattern_number") {
          fPatternWordIndex = fWord.size() - 1;
        }

        if(namech=="ffb_status")//save the location of this word to access this later
          fFFB_Index=fWord.size()-1;
	//store reference to bmwobj
	if(namech == "bmwobj"){
	  fBmwObj_Index = fWord.size() - 1;
	}

        fWordsPerSubbank[bankindex].second = fWord.size();
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
  return 0;
}



//QwModChannelID::QwModChannelID(Int_t subbankid, Int_t wordssofar,
//				   TString name,  TString modtype, QwBeamMod * obj):
//  fSubbankIndex(subbankid),fWordInSubbank(wordssofar),
//  fmoduletype(modtype),fmodulename(name),kUnknownDeviceType(-1)
//{
//  fTypeID = kQwUnknownDeviceType;
//}

QwModChannelID::QwModChannelID(Int_t subbankid,
				   QwParameterFile &paramfile):
  fSubbankIndex(subbankid), fIndex(-1)
{
  
  fmoduletype   = paramfile.GetTypedNextToken<TString>();
  modnum        = paramfile.GetTypedNextToken<Int_t>();    //slot number
  channum       = paramfile.GetTypedNextToken<Int_t>();    //channel number
  //  fdetectortype = paramfile.GetTypedNextToken<TString>();  //type-purpose of the detector
  fmodulename  = paramfile.GetTypedNextToken<TString>();  //name of the detector
  fmoduletype.ToUpper();
  //  fdetectortype.ToLower();
  fmodulename.ToLower();

  Int_t offset;
  if (fmoduletype == "VQWK") {
    fWordInSubbank = QwVQWK_Channel::GetBufferOffset(modnum, channum);
    if (paramfile.ReturnValue("vqwk_buffer_offset",offset)) {
      fWordInSubbank += offset;
    }
    // } else if (fmoduletype == "ADC18") {
    //fWordInSubbank = QwADC18_Channel::GetBufferOffset(modnum, channum);
    //if (paramfile.ReturnValue("adc18_buffer_offset",offset)) {
    //  fWordInSubbank += offset;
    // }
  } else if (fmoduletype == "SCALER"||  fmoduletype == "SIS3801D24" ||  fmoduletype == "SIS3801D32" ) {
    fWordInSubbank = VQwScaler_Channel::GetBufferOffset(modnum, channum);
    if (paramfile.ReturnValue("scaler_buffer_offset",offset)) {
      fWordInSubbank += offset;
    }
  } else {
    fWordInSubbank = -1;
  }
}





//*****************************************************************

void QwBeamMod::LoadEventCuts_Init(){
  fFFB_holdoff=0;//Default holdoff for the FFB pause
}

void QwBeamMod::LoadEventCuts_Line(QwParameterFile &mapstr, TString &varvalue, Int_t &eventcut_flag){
  TString device_type = mapstr.GetTypedNextToken<TString>();
  device_type.ToUpper();
  TString device_name = mapstr.GetTypedNextToken<TString>();
  device_name.ToUpper();

  if (device_type == "VQWK"||device_type=="SCALER" ||device_type=="SIS3801D24" ||device_type=="SIS3801D32"){
	  device_name.ToLower();
	  Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for BCM value
	  Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for BCM value
	  varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
    Double_t burplevel = mapstr.GetTypedNextToken<Double_t>();
	  varvalue.ToLower();
	  Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
  	QwMessage<<"QwBeamMod Error Code  "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	  Int_t det_index=GetDetectorIndex(device_name);
	  QwMessage << "*****************************" << QwLog::endl;
	  QwMessage << " Type " << device_type << " Name " << device_name << " Index [" << det_index << "] "
	            << " device flag " << eventcut_flag << QwLog::endl;
	  fModChannel[det_index]->SetSingleEventCuts((GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)|kBModErrorFlag),LLX,ULX,stabilitycut,burplevel);
	  QwMessage << "*****************************" << QwLog::endl;

  } else if (device_type == "WORD" && device_name== "FFB_STATUS"){
	  fFFB_holdoff=mapstr.GetTypedNextToken<UInt_t>();//Read the FFB OFF interval
  } else if (device_type == "WORD" && device_name== "BMWOBJ"){
	  fBMWObj_LL=mapstr.GetTypedNextToken<Int_t>();
	  fBMWObj_UL=mapstr.GetTypedNextToken<Int_t>();
	  QwMessage << "bmwobj error cuts"
	            << "LowerLimit=" << fBMWObj_LL
	            << "UpperLimit=" << fBMWObj_UL <<QwLog::endl;
  }
}

void QwBeamMod::LoadEventCuts_Fin(Int_t &eventcut_flag){
  //update the event cut ON/OFF for all the devices
  for (size_t i=0;i<fModChannel.size();i++)
    fModChannel[i]->SetEventCutMode(eventcut_flag);
}

//*****************************************************************
Bool_t QwBeamMod::CheckForBurpFail(const VQwSubsystem *subsys){
  Bool_t burpstatus = kFALSE;
  VQwSubsystem* tmp = const_cast<VQwSubsystem *>(subsys);
  if(Compare(tmp)) {
    const QwBeamMod* input = dynamic_cast<const QwBeamMod*>(subsys);
    for(size_t i=0;i<input->fModChannel.size();i++){
      //QwError << "************* test Clock *****************" << QwLog::endl;
      burpstatus |= (this->fModChannel[i])->CheckForBurpFail(input->fModChannel[i]);
    }
  }
  return burpstatus;
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
	    if(fModChannel[i]->GetElementName()==varname)
		{
		  fModChannel[i]->SetPedestal(varped);
		  fModChannel[i]->SetCalibrationFactor(varcal);
		  break;
		}
	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
}


Int_t QwBeamMod::ProcessEvBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words)
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

    for(size_t i=0;i<fModChannelID.size();i++) {
      if(fModChannelID[i].fSubbankIndex==index)	{
	if (lkDEBUG) {
	  std::cout<<"found modulation data for "<<fModChannelID[i].fmodulename<<std::endl;
	  std::cout<<"word left to read in this buffer:"<<num_words-fModChannelID[i].fWordInSubbank<<std::endl;
	}
	fModChannel[fModChannelID[i].fIndex]->ProcessEvBuffer(&(buffer[fModChannelID[i].fWordInSubbank]),
							      num_words-fModChannelID[i].fWordInSubbank);
	//fModChannel[fModChannelID[i].fIndex]->PrintInfo();
	//fModChannel[fModChannelID[i].fIndex]->PrintValue();
	//fModChannel[fModChannelID[i].fIndex]->PrintErrorCounters();
      }
    }

    for(Int_t i=fWordsPerSubbank[index].first; i<fWordsPerSubbank[index].second; i++) {
      if(fWord[i].fWordInSubbank+1<= (Int_t) num_words) {
	fWord[i].fValue=buffer[fWord[i].fWordInSubbank];
	//fWord[i].Print();
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
  //std::cout << "Here in ApplySingleEventCuts() " << std::endl;
  // std::cout << "ErrorFlag: " << fFFB_ErrorFlag << std::endl;
  Bool_t test_Mod=kTRUE;
  Bool_t test_BCM1=kTRUE;
  
  for(size_t i=0;i<fModChannel.size();i++){
    //std::cout<<"  BCM ["<<i<<"] "<<std::endl;
   //fModChannel[i]->PrintValue();
   //if(fModChannel[i]->GetEventcutErrorFlag() >0){
     //  fModChannel[i]->PrintValue();
     // }
    test_BCM1 = fModChannel[i]->ApplySingleEventCuts();
    test_Mod &= test_BCM1;
   
    // fModChannel[i]->PrintValue();
    //std::cout << "ErrorFlag: "<< fModChannel[i]->GetEventcutErrorFlag()<< std::endl;
    
    //if(fModChannel[i]->GetEventcutErrorFlag() >0){
    // fModChannel[i]->PrintValue();
    //}
    

    if(!test_BCM1 &&  bDEBUG) std::cout<<"******* QwBeamMod::SingleEventCuts()->BCM[ "<<i<<" , "<<fModChannel[i]->GetElementName()<<" ] ******" << std::endl;
  }
  
  
  /*  if (fWord[fFFB_Index].fValue==8 && fFFB_Flag && fFFB_holdoff_Counter==0){
    fFFB_holdoff_Counter=fFFB_holdoff;
    fFFB_Flag=kFALSE;
    
    } 
  */
  // std::cout << "Upper Limit: " << fBMWObj_UL << " , " << "Lower Limit: " << fBMWObj_LL << std::endl;
  //copy for fBmwObj
  if (fBMWObj_LL < fBMWObj_UL) {
    //std::cout << "Upper Limit: " << fBMWObj_UL << " , " << "Lower Limit: " << fBMWObj_LL << std::endl;
    
    // Cuts are valid
    if (fWord[fBmwObj_Index].GetValue()>fBMWObj_UL
	|| fWord[fBmwObj_Index].GetValue()<fBMWObj_LL){
      // Value is outside of cuts range.
     
      //std::cout << "bmwobj value: "<< fWord[fBmwObj_Index].GetValue() << " , " << "ErrorFlag: "<< fFFB_ErrorFlag << std::endl;
      fFFB_ErrorFlag = (kGlobalCut+kBModErrorFlag+kBModFFBErrorFlag+kEventCutMode3);
      //std::cout << "ErrorFlag: " << fFFB_ErrorFlag << std::endl;
    }
  }
  
  /*if (!fFFB_Flag &&  (fFFB_holdoff_Counter>0 && fFFB_holdoff_Counter<=fFFB_holdoff) ){
    fFFB_ErrorFlag = (kGlobalCut+kBModErrorFlag+kBModFFBErrorFlag+kEventCutMode3);
    fFFB_holdoff_Counter--;
    if(fFFB_holdoff_Counter==0)
      fFFB_Flag=kTRUE;
  }
  else
  fFFB_ErrorFlag=0;*/
     
  //std::cout << "WordValue: " << fWord[fBmwObj_Index].GetValue() << std::endl;
  //std::cout << "ErrorFlag: " << fFFB_ErrorFlag << std::endl;
  return test_Mod;

}

void QwBeamMod::IncrementErrorCounters()
{
  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i]->IncrementErrorCounters();
  }
}

void QwBeamMod::PrintErrorCounters() const{//inherited from the VQwSubsystemParity; this will display the error summary

  std::cout<<"*********QwBeamMod Error Summary****************"<<std::endl;
  QwVQWK_Channel::PrintErrorCounterHead();
  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i]->PrintErrorCounters();
  }
  QwVQWK_Channel::PrintErrorCounterTail();
}

void QwBeamMod::UpdateErrorFlag(const VQwSubsystem *ev_error)
{
  const QwBeamMod* input = dynamic_cast<const QwBeamMod*> (ev_error);

  for(size_t i=0;i<fModChannel.size();i++){
    fModChannel[i]->UpdateErrorFlag(input->fModChannel[i]->GetEventcutErrorFlag());
  }
  //  fFFB_ErrorFlag = input->fFFB_ErrorFlag;

}

UInt_t QwBeamMod::GetEventcutErrorFlag(){//return the error flag
  UInt_t ErrorFlag;
  ErrorFlag=0;
  for(size_t i=0;i<fModChannel.size();i++){
    ErrorFlag |= fModChannel[i]->GetEventcutErrorFlag();
  }
  ErrorFlag |= fFFB_ErrorFlag;
  
  return ErrorFlag;
}




void  QwBeamMod::ProcessEvent(){
  //std::cout << "Here in ProcessEvent() " << std::endl;
  for (size_t i = 0; i < fModChannel.size(); i++) {
    // First apply HW checks and update HW  error flags.
    fModChannel[i]->ApplyHWChecks();
    fModChannel[i]->ProcessEvent();
    //fModChannel[i]->PrintValue();
  }
}

void  QwBeamMod::ProcessEvent_2()
{
  // Fill histograms here to bypass event cuts
  FillHistograms();
}

Int_t QwBeamMod::ProcessConfigurationBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
}

//*****************************************************************

 void QwBeamMod::ClearEventData(){
   // std::cout << "Here in ClearEventData " << std::endl;
   for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i]->ClearEventData();

   for (size_t i=0;i<fWord.size();i++)
    fWord[i].ClearEventData();
   //std::cout << "Error Flag: " << fFFB_ErrorFlag << std::endl;
   fFFB_ErrorFlag=0;
}

//*****************************************************************
Int_t QwBeamMod::GetDetectorIndex(TString name)
{
  Bool_t ldebug=kFALSE;
  if(ldebug)
    {
      std::cout<<"QwBeamMod::GetDetectorIndex\n";
      std::cout<<fModChannelID.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for(size_t i=0;i<fModChannelID.size();i++)
    {

      if(fModChannelID[i].fmodulename==name){
	  result=fModChannelID[i].fIndex;
      }
   
      if(ldebug)
	{
	  std::cout<<"testing against ("<<fModChannelID[i].fTypeID
		   <<","<<fModChannelID[i].fmodulename<<")=>"<<result<<"\n";
	}
    }
  //do it over the words, fWords
   for(size_t i=0;i<fWord.size();i++) {

      if(fWord[i].fWordName==name){
	  result=i;
      }    
    }

  return result;
}
//*****************************************************************
VQwSubsystem&  QwBeamMod::operator=  (VQwSubsystem *value)
{
 
  if(Compare(value))
    {

      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);
      
      for(size_t i=0;i<input->fModChannel.size();i++){
	//input->fModChannel[i]->PrintValue();
	this->fModChannel[i]->AssignValueFrom(input->fModChannel[i]);
	//fModChannel[i]->PrintValue();
      }
      for(size_t i=0;i<input->fWord.size();i++)
  	this->fWord[i].fValue=input->fWord[i].fValue;
      this->fFFB_ErrorFlag=input->fFFB_ErrorFlag;
     
    }
  return *this;
}

VQwSubsystem&  QwBeamMod::operator+=  (VQwSubsystem *value)
{
  if(Compare(value)){
    // std::cout << "Here in operator+= " << std::endl;
      //QwBeamMod* input= (QwBeamMod*)value ;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

      for(size_t i=0;i<input->fModChannel.size();i++)
	*(this->fModChannel[i]) += input->fModChannel[i];
//         for(size_t i=0;i<input->fWord.size();i++)
//    	this->fWord[i]+=input->fWord[i];
      this->fFFB_ErrorFlag |= input->fFFB_ErrorFlag;
      // std::cout <<"Error Flag: " << this->fFFB_ErrorFlag <<std::endl;
    }
  return *this;
}

VQwSubsystem&  QwBeamMod::operator-=  (VQwSubsystem *value){
  
  if(Compare(value))
    {
      //QwBeamMod* input= (QwBeamMod*)value;
      QwBeamMod* input = dynamic_cast<QwBeamMod*>(value);

      for(size_t i=0;i<input->fModChannel.size();i++)
	*(this->fModChannel[i]) -= input->fModChannel[i];
//       for(size_t i=0;i<input->fWord.size();i++)
//         this->fWord[i]-=input->fWord[i];
      this->fFFB_ErrorFlag |= input->fFFB_ErrorFlag;
     
    }
  return *this;
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
	this->fModChannel[i]->Ratio(innumer->fModChannel[i],indenom->fModChannel[i]);
      for(size_t i=0;i<innumer->fWord.size();i++)
	this->fWord[i].fValue=innumer->fWord[i].fValue;

    }
}


void QwBeamMod::Scale(Double_t factor)
{
  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i]->Scale(factor);
}

void QwBeamMod::CalculateRunningAverage() { }

void QwBeamMod::AccumulateRunningSum(VQwSubsystem*, Int_t count, Int_t ErrorMask) { }

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

  // Beam modulation channels
  //for (size_t i = 0; i < fModChannel.size(); i++)
  //  fModChannel[i]->ConstructHistograms(folder,prefix);
}

void  QwBeamMod::FillHistograms()
{
  // No data loaded
  if (! HasDataLoaded()) return;

  // No histograms defined
  if (fHistograms.size() == 0) return;

  // Beam modulation channels
  //for (size_t i = 0; i < fModChannel.size(); i++)
  //  fModChannel[i]->FillHistograms();

  // Do we have a ramp channel?
  if (fRampChannelIndex < 0 || fRampChannelIndex > Int_t(fModChannel.size())) return;

  // Do we have a pattern word?
  if (fPatternWordIndex < 0 || fPatternWordIndex > Int_t(fWord.size())) return;

  // Determine the ramp/phase
  Double_t ramp = fModChannel[fRampChannelIndex]->GetValue();
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

  Double_t ramp_block_41 = fModChannel[fRampChannelIndex]->GetValue(4) + fModChannel[fRampChannelIndex]->GetValue(1); 
  Double_t ramp_block_32 = fModChannel[fRampChannelIndex]->GetValue(3) + fModChannel[fRampChannelIndex]->GetValue(2);
  Double_t ramp_block    = ramp_block_41 - ramp_block_32;  

}


void QwBeamMod::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  TString basename;
  
  for(size_t i = 0; i < fModChannel.size(); i++){
    fModChannel[i]->ConstructBranchAndVector(tree, prefix, values);
  }
  //   for (size_t i=0;i<fWord.size();i++)
  //     fWord[i].ConstructBranchAndVector(tree, prefix, values);
  fTreeArrayIndex  = values.size();
  for (size_t i=0; i<fWord.size(); i++) {
    // 	  basename = fWord[i].fWordName;
    basename = prefix(0, (prefix.First("|") >= 0)? prefix.First("|"): prefix.Length());
    basename += fWord[i].fWordName;
    values.push_back(0.0);
    tree->Branch(basename, &(values.back()), basename+"/D");
  }
}

void QwBeamMod::FillTreeVector(std::vector<Double_t> &values) const
{
  //std::cout << "inside FillTreeVector"<< std::endl; 
  //std::cout << "fTreeArrayIndex: " << fTreeArrayIndex << std::endl;
  size_t index = fTreeArrayIndex;
  
  for (size_t i = 0; i < fModChannel.size(); i++){
    fModChannel[i]->FillTreeVector(values);
    // fModChannel[i]->PrintValue();
  }
  for (size_t i = 0; i < fWord.size(); i++){
    values[index++] = fWord[i].fValue;
    //std::cout << fWord[i].fValue<< std::endl;
  }
  //for (size_t i=0; i<values.size(); i++){
  //  std::cout << values[i] << " ";
  //  if ((i+1)%8==0) std::cout << std::endl;
  // }

}


//*****************************************************************
void  QwBeamMod::Print()
{
  QwMessage << "Name of the subsystem =" << fSystemName << QwLog::endl;

  QwMessage << "there are " << fModChannel.size() << " mods" << QwLog::endl;

  QwMessage << " Printing Running AVG and other channel info for fModChannel" << QwLog::endl;
  for(size_t i=0;i<fModChannel.size();i++)
    fModChannel[i]->PrintValue();
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


#ifdef __USE_DATABASE__
//*****************************************************************
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
