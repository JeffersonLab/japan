/**********************************************************\
 * File: QwLumi.C                                          *
 *                                                         *
 * Author:                                                 *
 * Time-stamp:                                             *
\**********************************************************/

#include "QwLumi.h"

// System headers
#include <stdexcept>
#include <iostream>

// Qweak headers
#include "QwSubsystemArray.h"
#ifdef __USE_DATABASE__
//#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif
#include "QwPromptSummary.h"
#include "QwParameterFile.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwLumi);

void QwLumi::DefineOptions(QwOptions &options)
{
  options.AddOptions()
    ("QwLumi.normalize",
     po::value<bool>()->default_bool_value(true),
     "Normalize the detectors by beam current");
}


/********************************************************/
void QwLumi::ProcessOptions(QwOptions &options)
{
  bNormalization = options.GetValue<bool>("QwLumi.normalize");
  if (! bNormalization){
    QwWarning << "QwLumi::ProcessOptions:  "
	      << "Detector yields WILL NOT be normalized."
	      << QwLog::endl;
  }
}

/********************************************************/
Int_t QwLumi::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  TString varname, varvalue;
  TString modtype, dettype, namech, nameofcombinedchan, keyword, keyword2;
  Int_t modnum = 0;
  Int_t channum = 0;
  Int_t combinedchans = 0;
  std::vector<TString> combinedchannelnames;
  std::vector<Double_t> weight;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t offset = -1;
  Int_t currentsubbankindex=-1;
  Int_t fSample_size=0;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  mapstr.EnableGreediness();
  mapstr.SetCommentChars("!");

  UInt_t value;

  while (mapstr.ReadNextLine()) {
      RegisterRocBankMarker(mapstr);
      if (mapstr.PopValue("sample_size",value)) {
	fSample_size=value;
      }
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;

	  Bool_t lineok=kTRUE;
	  keyword = "";
	  keyword2 = "";
	  //  Break this line into tokens to process it.
	  modtype   = mapstr.GetTypedNextToken<TString>();	// module type
	  if (modtype == "VQWK" || modtype == "SCALER")
	    {
	      modnum    = mapstr.GetTypedNextToken<Int_t>();	//slot number
	      channum   = mapstr.GetTypedNextToken<Int_t>();	//channel number
	      dettype   = mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
	      dettype.ToLower();
	      namech    = mapstr.GetTypedNextToken<TString>();  //name of the detector
	      namech.ToLower();
	      keyword = mapstr.GetTypedNextToken<TString>();
	      keyword2 = mapstr.GetTypedNextToken<TString>();
	      keyword.ToLower();
	      keyword2.ToLower();
	    }
	  else if (modtype == "VPMT")
	    {
	      channum       = mapstr.GetTypedNextToken<Int_t>();	//channel number
	      combinedchans = mapstr.GetTypedNextToken<Int_t>();	//number of combined channels
	      dettype   = mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
	      dettype.ToLower();
	      namech    = mapstr.GetTypedNextToken<TString>();  //name of the detector
	      namech.ToLower();
	      //TString nameofchannel;
	      combinedchannelnames.clear();
	      for (int i=0; i<combinedchans; i++)
		{
		  nameofcombinedchan = mapstr.GetTypedNextToken<TString>();
		  nameofcombinedchan.ToLower();
		  combinedchannelnames.push_back(nameofcombinedchan);
		}
	      weight.clear();
	      for (int i=0; i<combinedchans; i++)
		{
		  weight.push_back( mapstr.GetTypedNextToken<Double_t>());
		}
	      keyword   = mapstr.GetTypedNextToken<TString>();
	      keyword2   = mapstr.GetTypedNextToken<TString>();
	      keyword.ToLower();
      	      keyword2.ToLower();
      
      
      
      
      
      
      
	    }  
	  if(currentsubbankindex!=GetSubbankIndex(fCurrentROC_ID,fCurrentBank_ID))
	    {
	      currentsubbankindex=GetSubbankIndex(fCurrentROC_ID,fCurrentBank_ID);
	    }



	  QwLumiDetectorID localLumiDetectorID;
	  localLumiDetectorID.fdetectorname=namech;
	  localLumiDetectorID.fmoduletype=modtype;
	  localLumiDetectorID.fSubbankIndex=currentsubbankindex;
	  localLumiDetectorID.fdetectortype=dettype;


	  if(modtype=="VQWK")
	    {
	      offset = QwVQWK_Channel::GetBufferOffset(modnum, channum);
	      localLumiDetectorID.fWordInSubbank = offset;
	    }
	  else if(modtype=="SCALER")
	    {
	      offset = QwSIS3801D24_Channel::GetBufferOffset(modnum, channum);
	      localLumiDetectorID.fWordInSubbank = offset;
	    }          
	  else if (modtype=="VPMT")
	    {
	      localLumiDetectorID.fCombinedChannelNames = combinedchannelnames;
	      localLumiDetectorID.fWeight = weight;
	      //std::cout<<"Add in a combined channel"<<std::endl;
	    }

	  if(offset<0)
	    {
	      QwError << "QwLumi::LoadChannelMap:  Unknown module type: "
		      << modtype <<", the detector "<<namech<<" will not be decoded "
		      << QwLog::endl;
	      lineok=kFALSE;
	      continue;
	    }




	  localLumiDetectorID.fTypeID=GetDetectorTypeID(dettype);
	  if(localLumiDetectorID.fTypeID==-1)
	    {
	      QwError << "QwLumi::LoadChannelMap:  Unknown detector type: "
		      << dettype <<", the detector "<<namech<<" will not be decoded "
		      << QwLog::endl;
	      lineok=kFALSE;
	      continue;
	    }

	  localLumiDetectorID.fIndex=
	    GetDetectorIndex(localLumiDetectorID.fTypeID,
			     localLumiDetectorID.fdetectorname);

	  if(localLumiDetectorID.fIndex==-1)
	    {
	      if(localLumiDetectorID.fTypeID==kQwIntegrationPMT)
		{
		  QwIntegrationPMT<QwVQWK_Channel> localIntegrationPMT(GetSubsystemName(),localLumiDetectorID.fdetectorname);

		  //  Force all Lumi PMTs to be not_blindable.
		  //  This isn't really needed, since the subsystem
		  //  doesn't call QwIntegrationPMT::Blind, but let's
		  //  do it anyway.
		  localIntegrationPMT.SetBlindability(kFALSE);
		  if (keyword=="not_normalizable"
		      || keyword2=="not_normalizable")
		    localIntegrationPMT.SetNormalizability(kFALSE);
		  else
		    localIntegrationPMT.SetNormalizability(kTRUE);

		  fIntegrationPMT.push_back(localIntegrationPMT);
		  fIntegrationPMT[fIntegrationPMT.size()-1].SetDefaultSampleSize(fSample_size);
		  localLumiDetectorID.fIndex=fIntegrationPMT.size()-1;
		}
	      else if (localLumiDetectorID.fTypeID==kQwCombinedPMT)
		{
		  QwCombinedPMT<QwVQWK_Channel> localcombinedPMT(GetSubsystemName(),localLumiDetectorID.fdetectorname);
		  //  Force all Lumi PMTs to be not_blindable.
		  //  This isn't really needed, since the subsystem
		  //  doesn't call QwCombinedPMT::Blind, but let's
		  //  do it anyway.
		  localcombinedPMT.SetBlindability(kFALSE);
		  if (keyword=="not_normalizable" 
		      || keyword2=="not_normalizable")
		    localcombinedPMT.SetNormalizability(kFALSE);
		  else
		    localcombinedPMT.SetNormalizability(kTRUE);

		  fCombinedPMT.push_back(localcombinedPMT);
		  fCombinedPMT[fCombinedPMT.size()-1].SetDefaultSampleSize(fSample_size);
		  localLumiDetectorID.fIndex=fCombinedPMT.size()-1;
		}








	      if(localLumiDetectorID.fTypeID==kQwScalerPMT)
		{
		  QwSIS3801D24_Channel localcounter(localLumiDetectorID.fdetectorname);
		  ///  TODO:  Instead of just forcing the external clock to not be used, we should
		  ///         figure out how to use the clock from teh beamline subsystem.
		  localcounter.SetNeedsExternalClock(kFALSE);
		  fScalerPMT.push_back(localcounter);
		  localLumiDetectorID.fIndex=fScalerPMT.size()-1;
		}

	    }


	  if(ldebug)
	    {
	      localLumiDetectorID.Print();
	      std::cout<<"line ok=";
	      if(lineok) std::cout<<"TRUE"<<std::endl;
	      else
		std::cout<<"FALSE"<<std::endl;
	    }

	  if(lineok)
	    fLumiDetectorID.push_back(localLumiDetectorID);
  } // End of "while (mapstr.ReadNextLine())"

  //std::cout<<"linking combined channels"<<std::endl;

  for (size_t i=0; i<fLumiDetectorID.size(); i++)
    {
      if (fLumiDetectorID[i].fTypeID==kQwCombinedPMT)
        {
          Int_t ind = fLumiDetectorID[i].fIndex;

          //check to see if all required channels are available
          if (ldebug)
            {
              std::cout<<"fLumiDetectorID[i].fCombinedChannelNames.size()="
		       <<fLumiDetectorID[i].fCombinedChannelNames.size()<<std::endl<<"name list: ";
              for (size_t n=0; n<fLumiDetectorID[i].fCombinedChannelNames.size(); n++)
                std::cout<<"  "<<fLumiDetectorID[i].fCombinedChannelNames[n];
              std::cout<<std::endl;
            }

          Int_t chanmatched=0;
          for (size_t j=0; j<fLumiDetectorID[i].fCombinedChannelNames.size(); j++)
            {

              for (size_t k=0; k<fLumiDetectorID.size(); k++)
                {
                  if (fLumiDetectorID[i].fCombinedChannelNames[j]==fLumiDetectorID[k].fdetectorname)
                    {
                      if (ldebug)
                        std::cout<<"found a to-be-combined channel candidate"<<std::endl;
                      chanmatched ++;
                      break;
                    }
                }
            }

          if ((Int_t) fLumiDetectorID[i].fCombinedChannelNames.size()==chanmatched)
            {
              for (size_t l=0; l<fLumiDetectorID[i].fCombinedChannelNames.size(); l++)
                {
                  Int_t ind_pmt = GetDetectorIndex(GetDetectorTypeID("integrationpmt"),
                                                   fLumiDetectorID[i].fCombinedChannelNames[l]);

                  fCombinedPMT[ind].Add(&fIntegrationPMT[ind_pmt],fLumiDetectorID[i].fWeight[l]);

		  if (ldebug) std::cout<<"added combined pmt "<<fLumiDetectorID[i].fWeight[l] <<" to array"<<std::endl;
                }
              fCombinedPMT[ind].LinkChannel(fLumiDetectorID[i].fdetectorname);
              if (ldebug)
                std::cout<<"linked a combined channel"<<std::endl;
            }
          else
            {
              std::cerr<<"cannot combine void channels for "<<fLumiDetectorID[i].fdetectorname<<std::endl;
              fLumiDetectorID[i].fIndex = -1;
              continue;
            }
        }
    }


  if(ldebug)
    {
      std::cout<<"QwLumi::Done with Load map channel \n";
      for(size_t i=0;i<fLumiDetectorID.size();i++)
	fLumiDetectorID[i].Print();
    }
  ldebug=kFALSE;
  mapstr.Close(); // Close the file (ifstream)

  return 0;
}


/********************************************************/
Int_t QwLumi::LoadEventCuts(TString  filename)
{
  Int_t eventcut_flag = 1;

  // Open the file
  QwParameterFile mapstr(filename.Data());
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine()){
    //std::cout<<"********* In the loop  *************"<<std::endl;
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname == "EVENTCUTS"){
	//varname="";
	eventcut_flag = QwParameterFile::GetUInt(varvalue);
	//std::cout<<"EVENT CUT FLAG "<<eventcut_flag<<std::endl;
      }
    }
    else{
      TString device_type = mapstr.GetTypedNextToken<TString>();
      device_type.ToLower();
      TString device_name = mapstr.GetTypedNextToken<TString>();
      device_name.ToLower();

      Int_t det_index = GetDetectorIndex(GetDetectorTypeID(device_type),device_name);
      if (det_index == -1) {
	QwWarning << " Device not found " << device_name << " of type " << device_type << QwLog::endl;
	continue;
      }

      if (device_type == GetQwPMTInstrumentTypeName(kQwIntegrationPMT)){
        Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for IntegrationPMT value
        Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for IntegrationPMT value
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	varvalue.ToLower();
	QwMessage << "QwLumi Error Code passing to QwIntegrationPMT " << GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut) << QwLog::endl;

	//std::cout<<"*****************************"<<std::endl;
	//std::cout<<" Type "<<device_type<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<check_flag<<std::endl;

	fIntegrationPMT[det_index].SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut),LLX,ULX,stabilitycut);
	//std::cout<<"*****************************"<<std::endl;

      } else if (device_type == GetQwPMTInstrumentTypeName(kQwCombinedPMT)){
        Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for IntegrationPMT value
        Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for IntegrationPMT value
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	Double_t stabilitycut=mapstr.GetTypedNextToken<Double_t>();
	varvalue.ToLower();
	QwMessage << "QwLumi Error Code passing to QwCombinedPMT " << GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut) << QwLog::endl;

	//std::cout<<"*****************************"<<std::endl;
	//std::cout<<" Type "<<device_type<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<check_flag<<std::endl;

	fCombinedPMT[det_index].SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut),LLX,ULX,stabilitycut);
	//std::cout<<"*****************************"<<std::endl;

      }

    }

  }
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].SetEventCutMode(eventcut_flag);
  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].SetEventCutMode(eventcut_flag);

  fQwLumiErrorCount=0; //set the error counter to zero
  mapstr.Close(); // Close the file (ifstream)
  return 0;
}

/********************************************************/
Int_t QwLumi::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;
  TString varname;
  Double_t varped;
  Double_t varcal;
  TString localname;

  Int_t lineread=0;

  if(ldebug)std::cout<<"QwLumi::LoadInputParameters("<< pedestalfile<<")\n";

  QwParameterFile mapstr(pedestalfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while (mapstr.ReadNextLine())
    {
      lineread+=1;
      if(ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;
      else
	{
	  varname = mapstr.GetTypedNextToken<TString>();	//name of the channel
	  varname.ToLower();
	  varname.Remove(TString::kBoth,' ');
	  varped= mapstr.GetTypedNextToken<Double_t>(); // value of the pedestal
	  varcal= mapstr.GetTypedNextToken<Double_t>(); // value of the calibration factor
	  if(ldebug) std::cout<<"inputs for channel "<<varname
			      <<": ped="<<varped<<": cal="<<varcal<<"\n";
	  Bool_t notfound=kTRUE;

	  if(notfound)
	    for(size_t i=0;i<fIntegrationPMT.size();i++)
	      if(fIntegrationPMT[i].GetElementName()==varname)
		{
		  fIntegrationPMT[i].SetPedestal(varped);
		  fIntegrationPMT[i].SetCalibrationFactor(varcal);
		  i=fIntegrationPMT.size()+1;
		  notfound=kFALSE;
		  i=fIntegrationPMT.size()+1;
		}
	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  mapstr.Close(); // Close the file (ifstream)
  return 0;
}

/********************************************************/
void QwLumi::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].SetRandomEventParameters(mean, sigma);
}

/********************************************************/
void QwLumi::SetRandomEventAsymmetry(Double_t asymmetry)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].SetRandomEventAsymmetry(asymmetry);
}

/********************************************************/
void QwLumi::RandomizeEventData(int helicity, double time)
{
  // Randomize all QwIntegrationPMT buffers
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].RandomizeEventData(helicity, time);
}

/********************************************************/
void QwLumi::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fLumiDetectorID.size(); i++) {
    // This is a QwIntegrationPMT
    if (fLumiDetectorID.at(i).fTypeID == kQwIntegrationPMT)
      fIntegrationPMT[fLumiDetectorID.at(i).fIndex].EncodeEventData(elements);
  }

  // If there is element data, generate the subbank header
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;
  if (elements.size() > 0) {

    // Form CODA subbank header
    subbankheader.clear();
    subbankheader.push_back(elements.size() + 1);	// subbank size
    subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
    // subbank tag | subbank type | event number

    // Form CODA bank/roc header
    rocheader.clear();
    rocheader.push_back(subbankheader.size() + elements.size() + 1);	// bank/roc size
    rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
    // bank tag == ROC | bank type | event number

    // Add bank header, subbank header and element data to output buffer
    buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
    buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
    buffer.insert(buffer.end(), elements.begin(), elements.end());
  }
}

/********************************************************/
Int_t QwLumi::ProcessEvBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;
  Bool_t firsttime=kTRUE;
  Bool_t issingleevent=kTRUE;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    if (lkDEBUG)
      std::cout << "QwLumi::ProcessEvBuffer:  "
		<< "Begin processing ROC" << roc_id
		<< " and subbank "<<bank_id
		<< " number of words="<<num_words<<std::endl;

    for(size_t i=0;i<fLumiDetectorID.size();i++)
      {
	if(fLumiDetectorID[i].fSubbankIndex==index)
	  {

	    if(fLumiDetectorID[i].fTypeID == kQwIntegrationPMT)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found IntegrationPMT data for "<<fLumiDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"words left to read in this buffer:"<<num_words-fLumiDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fIntegrationPMT[fLumiDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fLumiDetectorID[i].fWordInSubbank]),
				  num_words-fLumiDetectorID[i].fWordInSubbank);
	      }

	    if(fLumiDetectorID[i].fTypeID == kQwScalerPMT)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found ScalerPMT data for "<<fLumiDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fLumiDetectorID[i].fWordInSubbank<<std::endl;
		  }

		// This was added to check if the buffer contains more than one event.  If it does then throw those events away.  A better way to do this would be to find how many events were in the buffer then change the offset to be able to read them all.
                if (firsttime)
                  {
                    firsttime=kFALSE;
                    if (buffer[0]/32!=1)
                      {
                        issingleevent=kFALSE;
			if(lkDEBUG) {
			  std::cout<<"QwLumi::ProcessEvBuffer: More than one event was found in the buffer.  Setting these events to zero."<<std::endl;
			}
                      }
                  }
                if (issingleevent==kFALSE) continue;





		fScalerPMT[fLumiDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fLumiDetectorID[i].fWordInSubbank]),
				  num_words-fLumiDetectorID[i].fWordInSubbank);
	      }

	  }
      }
    SetDataLoaded(kTRUE);
  }

  return 0;
}

/********************************************************/
Bool_t QwLumi::ApplySingleEventCuts(){

  Bool_t status=kTRUE;
  for(size_t i=0;i<fIntegrationPMT.size();i++){
    status &= fIntegrationPMT[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwLumi::SingleEventCuts()->IntegrationPMT[ "<<i<<" , "<<fIntegrationPMT[i].GetElementName()<<" ] ******\n"; 
  }
  for(size_t i=0;i<fCombinedPMT.size();i++){
    status &= fCombinedPMT[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwLumi::SingleEventCuts()->CombinedPMT[ "<<i<<" , "<<fCombinedPMT[i].GetElementName()<<" ] ******\n"; 
  }


  if (!status) 
    fQwLumiErrorCount++;//failed  event counter for QwLumi

  return status;
}


void QwLumi::IncrementErrorCounters()
{
  for(size_t i=0;i<fIntegrationPMT.size();i++){
    fIntegrationPMT[i].IncrementErrorCounters();
  }
  for(size_t i=0;i<fCombinedPMT.size();i++){
    fCombinedPMT[i].IncrementErrorCounters();
  }
}

/********************************************************/
void QwLumi::PrintErrorCounters() const
{
  //inherited from the VQwSubsystemParity; this will display the error summary
  QwMessage<<"*********QwLumi Error Summary****************"<<QwLog::endl;
  QwVQWK_Channel::PrintErrorCounterHead();
  for(size_t i=0;i<fIntegrationPMT.size();i++){
    //std::cout<<"  IntegrationPMT ["<<i<<"] "<<std::endl;
    fIntegrationPMT[i].PrintErrorCounters();
  }
  for(size_t i=0;i<fCombinedPMT.size();i++){
    //std::cout<<"  CombinedPMT ["<<i<<"] "<<std::endl;
    fCombinedPMT[i].PrintErrorCounters();
  }
  QwVQWK_Channel::PrintErrorCounterTail();
}


/********************************************************/
UInt_t QwLumi::GetEventcutErrorFlag(){//return the error flag
  UInt_t ErrorFlag;
  ErrorFlag=0;
  for(size_t i=0;i<fIntegrationPMT.size();i++){
    ErrorFlag |= fIntegrationPMT[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fCombinedPMT.size();i++){
    ErrorFlag |= fCombinedPMT[i].GetEventcutErrorFlag();
  }
  return ErrorFlag;
}

void QwLumi::UpdateErrorFlag(const VQwSubsystem *ev_error){
  VQwSubsystem* tmp = const_cast<VQwSubsystem*>(ev_error);
  if(Compare(tmp)){
    const QwLumi * input = dynamic_cast<const QwLumi *> (ev_error);

    for (size_t i=0;i<input->fIntegrationPMT.size();i++)
      (this->fIntegrationPMT[i]).UpdateErrorFlag(&(input->fIntegrationPMT[i]));
    
    for (size_t i=0;i<input->fCombinedPMT.size();i++)
      (this->fCombinedPMT[i]).UpdateErrorFlag(&(input->fCombinedPMT[i]));
  }  
};


/********************************************************/
void  QwLumi::ProcessEvent()
{
  if (! HasDataLoaded()) return;

  for (size_t i = 0; i < fIntegrationPMT.size();  i++)
    fIntegrationPMT[i].ProcessEvent();
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].ProcessEvent();
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].ProcessEvent();
}


/**
 * Exchange data between subsystems
 */
void  QwLumi::ExchangeProcessedData()
{
  if (! HasDataLoaded()) return;

  bIsExchangedDataValid = kTRUE;
  if (bNormalization){
    if(RequestExternalValue("q_targ", &fTargetCharge)){
      if (bDEBUG){
	QwWarning << "QwLumi::ExchangeProcessedData Found "<<fTargetCharge.GetElementName()<< QwLog::endl;
	//QwWarning <<"****QwMainCerenkovDetector****"<< QwLog::endl;
	(dynamic_cast<QwVQWK_Channel*>(&fTargetCharge))->PrintInfo();
      }
    }
  }
}




void  QwLumi::ProcessEvent_2()
{
  if (! HasDataLoaded()) return;

  if (bIsExchangedDataValid)
    {
      //data is valid, process it
      if (bDEBUG)
        {
          Double_t  pedestal = fTargetCharge.GetPedestal();
          Double_t  calfactor = fTargetCharge.GetCalibrationFactor();
          Double_t  volts = fTargetCharge.GetAverageVolts();
          std::cout<<"QwLumi::ProcessEvent_2(): processing with exchanged data"<<std::endl;
          std::cout<<"pedestal, calfactor, average volts = "<<pedestal<<", "<<calfactor<<", "<<volts<<std::endl;
        }

      // assume fTargetCharge.fHardwareSum is a calibrated value,
      // detector signals will be normalized to it
      if (bNormalization) this->DoNormalization();
    }
  else
    {
      QwWarning <<"QwLumi::ProcessEvent_2(): could not get all external values."<<QwLog::endl;
    }
}

Bool_t QwLumi::PublishInternalValues() const
{
  // Publish variables
  Bool_t status = kTRUE;

  //  Don't publish anything explicitly, use the PublishByRequest
  //  mechanism instead.
  
  return status;
}

Bool_t QwLumi::PublishByRequest(TString device_name)
{
  Bool_t status = kFALSE;
  for(size_t i=0;i<fLumiDetectorID.size();i++) {
    if(device_name.CompareTo(fLumiDetectorID[i].fdetectorname)!=0) continue;
    
    if (fLumiDetectorID[i].fTypeID == kQwCombinedPMT){
      status = PublishInternalValue(device_name, "published-by-request",
				    fCombinedPMT[fLumiDetectorID[i].fIndex].GetChannel(device_name));
    } else if (fLumiDetectorID[i].fTypeID == kQwIntegrationPMT) {
      status = PublishInternalValue(device_name, "published-by-request",
				    fIntegrationPMT[fLumiDetectorID[i].fIndex].GetChannel(device_name));
    } else {
      QwError << "Unknown channel name:  " << device_name << QwLog::endl;
    }
    break;
  }
  if (!status)  
    QwError << "QwLumi::PublishByRequest:  Failed to publish channel name:  " << device_name << QwLog::endl;
  return status;
}

/********************************************************/
Int_t QwLumi::ProcessConfigurationBuffer(const ROCID_t roc_id, const BankID_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
}

/********************************************************/
Bool_t QwLumi::IsGoodEvent()
{
  Bool_t test=kTRUE;

  return test;
}


/********************************************************/
void QwLumi::ClearEventData()
{
  SetDataLoaded(kFALSE);
  for(size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ClearEventData();
  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].ClearEventData();
  for(size_t i=0;i<fScalerPMT.size();i++)
    fScalerPMT[i].ClearEventData();
}

EQwPMTInstrumentType QwLumi::GetDetectorTypeID(TString name)
{
  return GetQwPMTInstrumentType(name);
}

/********************************************************/
Int_t QwLumi::GetDetectorIndex(EQwPMTInstrumentType type_id, TString name)
{
  Bool_t ldebug=kFALSE;
  if(ldebug)
    {
      std::cout<<"QwLumi::GetDetectorIndex\n";
      std::cout<<"type_id=="<<type_id<<" name="<<name<<"\n";
      std::cout<<fLumiDetectorID.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for(size_t i=0;i<fLumiDetectorID.size();i++)
    {
      if(fLumiDetectorID[i].fTypeID==type_id)
	if(fLumiDetectorID[i].fdetectorname==name)
	  result=fLumiDetectorID[i].fIndex;
      if(ldebug)
	{
	  std::cout<<"testing against ("<<fLumiDetectorID[i].fTypeID
		   <<","<<fLumiDetectorID[i].fdetectorname<<")=>"<<result<<"\n";
	}
    }

  return result;
}

/********************************************************/
const QwIntegrationPMT<QwVQWK_Channel>* QwLumi::GetChannel(const TString name) const
{
  return GetIntegrationPMT(name);
}


/********************************************************/
const QwIntegrationPMT<QwVQWK_Channel>* QwLumi::GetIntegrationPMT(const TString name) const
{
  if (! fIntegrationPMT.empty()) {
    for (std::vector<QwIntegrationPMT<QwVQWK_Channel> >::const_iterator IntegrationPMT = fIntegrationPMT.begin(); IntegrationPMT != fIntegrationPMT.end(); ++IntegrationPMT) {
      if (IntegrationPMT->GetElementName() == name) {
        return &(*IntegrationPMT);
      }
    }
  }
  return 0;
}


const QwCombinedPMT<QwVQWK_Channel>* QwLumi::GetCombinedPMT(const TString name) const
{
  TString tmpname = name;
  tmpname.ToLower();
  if (! fCombinedPMT.empty())
    {
      for (size_t i=0;i<fCombinedPMT.size();i++)
        {
          if (fCombinedPMT.at(i).GetElementName() == tmpname)
            {
              //std::cout<<"Get CombinedPMT "<<tmpname<<std::endl;
              return &(fCombinedPMT.at(i));
            }
        }
    }
  QwMessage << "QwLumi::GetCombinedPMT: cannot find channel " << tmpname << QwLog::endl;
  return NULL;
}




/********************************************************/
VQwSubsystem&  QwLumi::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwLumi::operator= \n";
  if (Compare(value))
    {
      VQwSubsystem::operator=(value);
      QwLumi* input = dynamic_cast<QwLumi*> (value);

      for (size_t i = 0; i < input->fIntegrationPMT.size(); i++)
	this->fIntegrationPMT[i] = input->fIntegrationPMT[i];
      for (size_t i = 0; i < input->fCombinedPMT.size(); i++)
        this->fCombinedPMT[i] = input->fCombinedPMT[i];
      for (size_t i = 0; i < input->fScalerPMT.size(); i++)
	this->fScalerPMT[i] = input->fScalerPMT[i];

    }
  return *this;
}

/********************************************************/
VQwSubsystem&  QwLumi::operator+=  (VQwSubsystem *value)
{
  if (Compare(value))
    {
      QwLumi* input= dynamic_cast<QwLumi*>(value);

      for (size_t i = 0; i < input->fIntegrationPMT.size(); i++)
	this->fIntegrationPMT[i] += input->fIntegrationPMT[i];
      for (size_t i = 0; i < input->fCombinedPMT.size(); i++)
        this->fCombinedPMT[i] += input->fCombinedPMT[i];
      for (size_t i = 0; i < input->fScalerPMT.size(); i++)
	this->fScalerPMT[i] += input->fScalerPMT[i];
    }
  return *this;
}

/********************************************************/
VQwSubsystem&  QwLumi::operator-=  (VQwSubsystem *value)
{

  if (Compare(value))
    {
      QwLumi* input= dynamic_cast<QwLumi*>(value);

      for (size_t i = 0; i < input->fIntegrationPMT.size(); i++)
	this->fIntegrationPMT[i] -= input->fIntegrationPMT[i];
      for (size_t i = 0; i < input->fCombinedPMT.size(); i++)
        this->fCombinedPMT[i] -= input->fCombinedPMT[i];
      for (size_t i = 0; i < input->fScalerPMT.size(); i++)
	this->fScalerPMT[i] -= input->fScalerPMT[i];
    }
  return *this;
}

/********************************************************/
void  QwLumi::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
}

/********************************************************/
void  QwLumi::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
}

/********************************************************/
void QwLumi::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if (Compare(numer) && Compare(denom))
    {
      QwLumi* innumer = dynamic_cast<QwLumi*>(numer);
      QwLumi* indenom = dynamic_cast<QwLumi*>(denom);

      for (size_t i = 0; i < innumer->fIntegrationPMT.size(); i++)
	this->fIntegrationPMT[i].Ratio(innumer->fIntegrationPMT[i],indenom->fIntegrationPMT[i]);
      for (size_t i = 0; i < innumer->fCombinedPMT.size(); i++)
        this->fCombinedPMT[i].Ratio(innumer->fCombinedPMT[i],indenom->fCombinedPMT[i]);
      for (size_t i = 0; i < innumer->fScalerPMT.size(); i++)
	this->fScalerPMT[i].Ratio(innumer->fScalerPMT[i],indenom->fScalerPMT[i]);
    }
}

/********************************************************/
void QwLumi::Scale(Double_t factor)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].Scale(factor);
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].Scale(factor);
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].Scale(factor);
}

/********************************************************/
void QwLumi::Normalize(VQwDataElement* denom)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].Normalize(denom);
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].Normalize(denom);
  //for (size_t i = 0; i < fScalerPMT.size(); i++)
    //fScalerPMT[i].Normalize(denom);
}


/********************************************************/
Bool_t QwLumi::Compare(VQwSubsystem *value)
{
  //  std::cout<<" Here in QwLumi::Compare \n";

  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
      //      std::cout<<" types are not ok \n";
      //      std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    }
  else
    {
      QwLumi* input= dynamic_cast<QwLumi*>(value);
      if(input->fIntegrationPMT.size()!=fIntegrationPMT.size() ||
	 input->fCombinedPMT.size()!=fCombinedPMT.size() )
	{
	  res=kFALSE;
	}
      if(input->fScalerPMT.size()!=fScalerPMT.size())
	{
	  res=kFALSE;
	}
    }
  return res;
}


/********************************************************/
void  QwLumi::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].ConstructHistograms(folder,prefix);
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].ConstructHistograms(folder,prefix);
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].ConstructHistograms(folder,prefix);
}


/********************************************************/
void  QwLumi::FillHistograms()
{
  if (! HasDataLoaded()) return;

  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].FillHistograms();
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].FillHistograms();
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].FillHistograms();
}


/********************************************************/
void QwLumi::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].ConstructBranchAndVector(tree, prefix, values);
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].ConstructBranchAndVector(tree, prefix, values);
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].ConstructBranchAndVector(tree, prefix, values);
}

/********************************************************/
void QwLumi::ConstructBranch(TTree *tree, TString & prefix)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].ConstructBranch(tree, prefix);
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].ConstructBranch(tree, prefix);
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].ConstructBranch(tree, prefix);
}

/********************************************************/
void QwLumi::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp;
  QwParameterFile* nextmodule;

  tmp="QwIntegrationPMT";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();
    //This section contains sub modules and or channels to be included in the tree

    for(size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].ConstructBranch(tree, prefix,*nextmodule);
    for(size_t i = 0; i < fIntegrationPMT.size(); i++)
      fScalerPMT[i].ConstructBranch(tree, prefix);

  }
  
  tmp="QwCombinedPMT";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for (size_t i=0;i<fCombinedPMT.size();i++)
      fCombinedPMT[i].ConstructBranch(tree, prefix, *nextmodule );
  }
}

/********************************************************/
void  QwLumi::FillTreeVector(std::vector<Double_t> &values) const
{
  if (! HasDataLoaded()) return;

  for(size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].FillTreeVector(values);
  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].FillTreeVector(values);
  for(size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].FillTreeVector(values);
}


/********************************************************/
void  QwLumi::PrintValue() const
{
  QwMessage << "=== QwLumi: " << GetSubsystemName() << " ===" << QwLog::endl;
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].PrintValue();
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].PrintValue();
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].PrintValue();
}

/********************************************************/
void  QwLumi::PrintInfo() const
{
  QwMessage << "Name of the subsystem = " << fSystemName << QwLog::endl;
  QwMessage << "There are " << fIntegrationPMT.size() << " IntegrationPMT" << QwLog::endl;
  QwMessage << "          " << fCombinedPMT.size() << " CombinedPMT" << QwLog::endl;
  QwMessage << "There are " << fScalerPMT.size() << " halo counters" << QwLog::endl;

  QwMessage << " Printing Running AVG and other channel info" << QwLog::endl;
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].PrintInfo();
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].PrintInfo();
  for (size_t i = 0; i < fScalerPMT.size(); i++)
    fScalerPMT[i].PrintInfo();
}

/********************************************************/
void  QwLumi::PrintDetectorID() const
{
  for (size_t i=0;i<fLumiDetectorID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fLumiDetectorID[i].Print();
    }
}

/********************************************************/
void  QwLumiDetectorID::Print() const
{
  std::cout<<std::endl<<"Detector name= "<<fdetectorname<<std::endl;
  std::cout<<"SubbankkIndex= "<<fSubbankIndex<<std::endl;
  std::cout<<"word index in subbank= "<<fWordInSubbank<<std::endl;
  std::cout<<"module type= "<<fmoduletype<<std::endl;
  std::cout<<"detector type=  "<<fdetectortype<<" that is index="<<fTypeID<<std::endl;
  std::cout<<"Index of this detector in the vector of similar detector= "<<
    fIndex<<std::endl;
  std::cout<<"Subelement index= "<<
    fSubelement<<std::endl;
  std::cout<<"==========================================\n";
}

/********************************************************/
void QwLumi::CalculateRunningAverage()
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].CalculateRunningAverage();
    
  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].CalculateRunningAverage();
}

/********************************************************/
void QwLumi::AccumulateRunningSum(VQwSubsystem* value1)
{
  if (Compare(value1)) {
    QwLumi* value = dynamic_cast<QwLumi*>(value1);

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].AccumulateRunningSum(value->fIntegrationPMT[i]);
      
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
      fCombinedPMT[i].AccumulateRunningSum(value->fCombinedPMT[i]);
  }
}

void QwLumi::DeaccumulateRunningSum(VQwSubsystem* value1){
  if (Compare(value1)) {
    QwLumi* value = dynamic_cast<QwLumi *>(value1);

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].DeaccumulateRunningSum(value->fIntegrationPMT[i]);
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
      fCombinedPMT[i].DeaccumulateRunningSum(value->fCombinedPMT[i]);
  }  
};



void QwLumi::DoNormalization(Double_t factor)
{
  if (bIsExchangedDataValid)
    {
      if (bDEBUG)
        {
          Double_t  pedestal = fTargetCharge.GetPedestal();
          Double_t  calfactor = fTargetCharge.GetCalibrationFactor();
          Double_t  volts = fTargetCharge.GetAverageVolts();
          std::cout<<"QwLumi::ProcessEvent_2(): processing with exchanged data"<<std::endl;
          std::cout<<"pedestal, calfactor, average volts = "<<pedestal<<", "<<calfactor<<", "<<volts<<std::endl;
        }
      try
        {
	  this->Normalize(&fTargetCharge);
        }
      catch (std::exception& e)
        {
          std::cerr << e.what() << std::endl;
        }
    }
}






#ifdef __USE_DATABASE__
/********************************************************/
void QwLumi::FillDB(QwParityDB *db, TString datatype)
{

  Bool_t local_print_flag = false;
  if(local_print_flag){
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "                         QwLumi::FillDB                          " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwDBInterface> interface;
  std::vector<QwParitySSQLS::lumi_data> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  TString measurement_type;
  measurement_type = QwDBInterface::DetermineMeasurementTypeID(datatype);

  UInt_t i,j;
  i = j = 0;
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "IntegrationPMT" <<QwLog::endl;

  for(i=0; i< fIntegrationPMT.size(); i++) {
    interface.clear();
    interface = fIntegrationPMT[i].GetDBEntry();
    for(j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetLumiDetectorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Combined PMT" <<QwLog::endl;

  for(i=0; i< fCombinedPMT.size(); i++)
    {
      interface.clear();
      interface = fCombinedPMT[i].GetDBEntry();
      for(j=0; j<interface.size(); j++) {
	interface.at(j).SetAnalysisID( analysis_id );
	interface.at(j).SetLumiDetectorID( db );
	interface.at(j).SetMeasurementTypeID( measurement_type );
	interface.at(j).PrintStatus( local_print_flag );
	interface.at(j).AddThisEntryToList( entrylist );
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
    QwMessage << "QwLumi::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }

  db->Disconnect();

  return;
}




void QwLumi::FillErrDB(QwParityDB *db, TString datatype)
{

  Bool_t local_print_flag = false;
  if(local_print_flag){
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "                         QwLumi::FillErrDB                       " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwErrDBInterface> interface;
  std::vector<QwParitySSQLS::lumi_errors> entrylist;
  
  UInt_t analysis_id = db->GetAnalysisID();
  
  UInt_t i,j;
  i = j = 0;
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "IntegrationPMT" <<QwLog::endl;

  for(i=0; i< fIntegrationPMT.size(); i++) {
    interface.clear();
    interface = fIntegrationPMT[i].GetErrDBEntry();
    for(j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID     ( analysis_id );
      interface.at(j).SetLumiDetectorID ( db );
      interface.at(j).PrintStatus       ( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Combined PMT" <<QwLog::endl;

  for(i=0; i< fCombinedPMT.size(); i++)
    {
      interface.clear();
      interface = fCombinedPMT[i].GetErrDBEntry();
      for(j=0; j<interface.size(); j++) {
	interface.at(j).SetAnalysisID     ( analysis_id );
	interface.at(j).SetLumiDetectorID ( db );
	interface.at(j).PrintStatus       ( local_print_flag );
	interface.at(j).AddThisEntryToList( entrylist );
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
    QwMessage << "QwLumi::FillErrDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }

  db->Disconnect();

  return;
};
#endif


void QwLumi::WritePromptSummary(QwPromptSummary *ps, TString type)
{

  Bool_t local_print_flag = true;
  if(local_print_flag){
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "                         QwLumi::WritePromptSummary()            " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  return;
}
