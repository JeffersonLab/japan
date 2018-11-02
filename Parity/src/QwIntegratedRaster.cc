/**********************************************************\
 * File: QwIntegratedRaster.C                              *
 *                                                         *
 * Author:                                                 *
 * Time-stamp:                                             *
\**********************************************************/

#include "QwIntegratedRaster.h"

// System headers
#include <stdexcept>
#include <iostream>

// Qweak headers
#include "QwSubsystemArray.h"
#define MYSQLPP_SSQLS_NO_STATICS
#ifdef __USE_DATABASE__
#include "QwParitySSQLS.h"
#include "QwParityDB.h"
#endif // __USE_DATABASE__
#include "QwVQWK_Channel.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwIntegratedRaster);

void QwIntegratedRaster::DefineOptions(QwOptions &options)
{
}


//*****************************************************************
void QwIntegratedRaster::ProcessOptions(QwOptions &options)
{
}

//*****************************************************************
Int_t QwIntegratedRaster::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  TString varname, varvalue;
  TString modtype, dettype, namech, nameofcombinedchan, keyword;
  Int_t modnum = 0;
  Int_t channum = 0;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t offset = -1;
  Int_t currentsubbankindex=-1;
  Int_t fSample_size=0;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine())
    {
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;

      if (mapstr.HasVariablePair("=",varname,varvalue))
	{
	  //  This is a declaration line.  Decode it.
	  varname.ToLower();
	  UInt_t value = QwParameterFile::GetUInt(varvalue);

	  if (varname=="roc")
	    {
	      currentrocread=value;
	      RegisterROCNumber(value,0);
	    }
	  else if (varname=="bank")
	    {
	      currentbankread=value;
	      RegisterSubbank(value);
	    }
	  else if (varname=="sample_size")
	    {
	      fSample_size=value;
	    }
	}
      else
	{
	  Bool_t lineok=kTRUE;
	  //  Break this line into tokens to process it.
	  modtype   = mapstr.GetTypedNextToken<TString>();	// module type
	  if (modtype == "VQWK")
	    {
	      modnum    = mapstr.GetTypedNextToken<Int_t>();	//slot number
	      channum   = mapstr.GetTypedNextToken<Int_t>();	//channel number
	      dettype   = mapstr.GetTypedNextToken<TString>();	//type-purpose of the detector
	      dettype.ToLower();
	      namech    = mapstr.GetTypedNextToken<TString>();  //name of the detector
	      namech.ToLower();
	      keyword = mapstr.GetTypedNextToken<TString>();
	      keyword.ToLower();
	    }

	  if(currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread))
	    {
	      currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
	    }



	  QwIntegratedRasterDetectorID detector_id;
	  detector_id.fdetectorname=namech;
	  detector_id.fmoduletype=modtype;
	  detector_id.fSubbankIndex=currentsubbankindex;
	  detector_id.fdetectortype=dettype;


	  if(modtype=="VQWK")
	    {
	      offset = QwVQWK_Channel::GetBufferOffset(modnum, channum);
	      detector_id.fWordInSubbank = offset;
	    }

	  if(offset<0)
	    {
	      QwError << "QwIntegratedRaster::LoadChannelMap:  Unknown module type: "
		      << modtype <<", the detector "<<namech<<" will not be decoded "
		      << QwLog::endl;
	      lineok=kFALSE;
	      continue;
	    }


	  detector_id.fdetectortype.ToLower();
	  if(detector_id.fdetectortype!="raster") {
	    //  Silently ignore the line if this isn't a raster element.
	    QwVerbose << "QwIntegratedRaster::LoadChannelMap:  Unknown detector type: "
		      << dettype <<", the detector "<<namech<<" will not be decoded "
		      << QwLog::endl;
	    lineok=kFALSE;
	    continue;
	  }

	  detector_id.fIndex=
	    GetDetectorIndex(detector_id.fdetectorname);

	  if(detector_id.fIndex==-1)
	    {
	      QwIntegratedRasterChannel<QwVQWK_Channel> localIntegratedRasterChannel(GetSubsystemName(),detector_id.fdetectorname);
	      fIntegratedRasterChannel.push_back(localIntegratedRasterChannel);
	      fIntegratedRasterChannel[fIntegratedRasterChannel.size()-1].SetDefaultSampleSize(fSample_size);
	      detector_id.fIndex=fIntegratedRasterChannel.size()-1;
	    }

	  if(ldebug)
	    {
	      detector_id.Print();
	      std::cout<<"line ok=";
	      if(lineok) std::cout<<"TRUE"<<std::endl;
	      else
		std::cout<<"FALSE"<<std::endl;
	    }

	  if(lineok)
	    fDetectorIDs.push_back(detector_id);
	}
    }

  if(ldebug) {
    std::cout<<"QwIntegratedRaster::Done with Load map channel \n";
    for(size_t i=0;i<fDetectorIDs.size();i++)
      fDetectorIDs[i].Print();
  }
  ldebug=kFALSE;
  mapstr.Close(); // Close the file (ifstream)
  return 0;
}


//*****************************************************************
Int_t QwIntegratedRaster::LoadEventCuts(TString  filename)
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
      if (varname == "EVENTCUTS") {
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

      if (device_type != "raster") {
	QwVerbose << "QwIntegratedRaster::LoadEventCuts:  Skipping detector type: "
		  << device_type << ", detector name "
		  << device_name << QwLog::endl;
	continue;
      }

      Int_t det_index = GetDetectorIndex(device_name);
      if (det_index == -1) {
	QwWarning << " Device not found " << device_name << " of type " << device_type << QwLog::endl;
	continue;
      }

      Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for IntegratedRasterChannel value
      Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for IntegratedRasterChannel value
      varvalue = mapstr.GetTypedNextToken<TString>();//global/local
      varvalue.ToLower();
      Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
      QwMessage << "QwIntegratedRaster Error Code passing to QwIntegratedRasterChannel " << GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut) << QwLog::endl;

      //std::cout<<"*****************************"<<std::endl;
      //std::cout<<" Type "<<device_type<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<check_flag<<std::endl;

      fIntegratedRasterChannel[det_index].SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut),LLX,ULX,stabilitycut);
      //std::cout<<"*****************************"<<std::endl;
    }

  }
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].SetEventCutMode(eventcut_flag);

  fQwIntegratedRasterErrorCount = 0; //set the error counter to zero

  return 0;
}

//*****************************************************************
Int_t QwIntegratedRaster::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;
  TString varname;
  Double_t varped;
  Double_t varcal;
  TString localname;

  Int_t lineread=0;

  if(ldebug)std::cout<<"QwIntegratedRaster::LoadInputParameters("<< pedestalfile<<")\n";

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
	    for(size_t i=0;i<fIntegratedRasterChannel.size();i++)
	      if(fIntegratedRasterChannel[i].GetElementName()==varname)
		{
		  fIntegratedRasterChannel[i].SetPedestal(varped);
		  fIntegratedRasterChannel[i].SetCalibrationFactor(varcal);
		  i=fIntegratedRasterChannel.size()+1;
		  notfound=kFALSE;
		  i=fIntegratedRasterChannel.size()+1;
		}
	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  mapstr.Close(); // Close the file (ifstream)

  return 0;
}

//*****************************************************************
void QwIntegratedRaster::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].SetRandomEventParameters(mean, sigma);
}

//*****************************************************************
void QwIntegratedRaster::SetRandomEventAsymmetry(Double_t asymmetry)
{
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].SetRandomEventAsymmetry(asymmetry);
}

//*****************************************************************
void QwIntegratedRaster::RandomizeEventData(int helicity, double time)
{
  // Randomize all QwIntegratedRasterChannel buffers
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].RandomizeEventData(helicity, time);
}

//*****************************************************************
void QwIntegratedRaster::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fDetectorIDs.size(); i++) {
    // This is a QwIntegratedRasterChannel
    fIntegratedRasterChannel[fDetectorIDs.at(i).fIndex].EncodeEventData(elements);
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

//*****************************************************************
Int_t QwIntegratedRaster::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    if (lkDEBUG)
      std::cout << "QwIntegratedRaster::ProcessEvBuffer:  "
		<< "Begin processing ROC" << roc_id
		<< " and subbank "<<bank_id
		<< " number of words="<<num_words<<std::endl;

    for(size_t i=0;i<fDetectorIDs.size();i++) {
      if(fDetectorIDs[i].fSubbankIndex==index) {
	if (lkDEBUG) {
	  std::cout<<"found IntegratedRasterChannel data for "<<fDetectorIDs[i].fdetectorname<<std::endl;
	  std::cout<<"words left to read in this buffer:"<<num_words-fDetectorIDs[i].fWordInSubbank<<std::endl;
	}

	fIntegratedRasterChannel[fDetectorIDs[i].fIndex].
	  ProcessEvBuffer(&(buffer[fDetectorIDs[i].fWordInSubbank]),
			  num_words-fDetectorIDs[i].fWordInSubbank);
      }
	
    }
    SetDataLoaded(kTRUE);
  }
  return 0;
}

//*****************************************************************
Bool_t QwIntegratedRaster::ApplySingleEventCuts(){

  Bool_t status=kTRUE;
  for(size_t i=0;i<fIntegratedRasterChannel.size();i++){
    status &= fIntegratedRasterChannel[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwIntegratedRaster::SingleEventCuts()->IntegratedRasterChannel[ "<<i<<" , "<<fIntegratedRasterChannel[i].GetElementName()<<" ] ******\n"; 
  }

  if (!status) 
    fQwIntegratedRasterErrorCount++;//failed  event counter for QwIntegratedRaster

  return status;
}


//*****************************************************************
void  QwIntegratedRaster::IncrementErrorCounters()
{
  for (size_t i = 0; i < fIntegratedRasterChannel.size();  i++)
    fIntegratedRasterChannel[i].IncrementErrorCounters();
}

//*****************************************************************
void QwIntegratedRaster::PrintErrorCounters() const
{
  //inherited from the VQwSubsystemParity; this will display the error summary
  QwMessage<<"*********QwIntegratedRaster Error Summary****************"<<QwLog::endl;
  QwVQWK_Channel::PrintErrorCounterHead();
  for(size_t i=0;i<fIntegratedRasterChannel.size();i++){
    //std::cout<<"  IntegratedRasterChannel ["<<i<<"] "<<std::endl;
    fIntegratedRasterChannel[i].PrintErrorCounters();
  }
  QwVQWK_Channel::PrintErrorCounterTail();
}


//*****************************************************************
UInt_t QwIntegratedRaster::GetEventcutErrorFlag(){//return the error flag
  UInt_t ErrorFlag;
  ErrorFlag=0;
  for(size_t i=0;i<fIntegratedRasterChannel.size();i++){
    ErrorFlag |= fIntegratedRasterChannel[i].GetEventcutErrorFlag();
  }
  return ErrorFlag;
}

//*****************************************************************
void  QwIntegratedRaster::ProcessEvent()
{
  if (! HasDataLoaded()) return;

  for (size_t i = 0; i < fIntegratedRasterChannel.size();  i++)
    fIntegratedRasterChannel[i].ProcessEvent();
}


//*****************************************************************
Int_t QwIntegratedRaster::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
}

//*****************************************************************
Bool_t QwIntegratedRaster::IsGoodEvent()
{
  Bool_t test=kTRUE;

  return test;
}


//*****************************************************************
void QwIntegratedRaster::ClearEventData()
{
  SetDataLoaded(kFALSE);
  for(size_t i=0;i<fIntegratedRasterChannel.size();i++)
    fIntegratedRasterChannel[i].ClearEventData();
}


//*****************************************************************
Int_t QwIntegratedRaster::GetDetectorIndex(TString name)
{
  Bool_t ldebug=kFALSE;
  if(ldebug)
    {
      std::cout<<"QwIntegratedRaster::GetDetectorIndex\n";
      std::cout<<" name="<<name<<"\n";
      std::cout<<fDetectorIDs.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for(size_t i=0;i<fDetectorIDs.size();i++)
    {
      if(fDetectorIDs[i].fdetectorname==name)
	result=fDetectorIDs[i].fIndex;
      if(ldebug)
	{
	  std::cout<<"testing against ("
		   <<fDetectorIDs[i].fdetectorname
		   <<")=>"<<result<<"\n";
	}
    }

  return result;
}


//*****************************************************************
const VQwDataElement* QwIntegratedRaster::GetChannel(const TString name) const
{
  if (! fIntegratedRasterChannel.empty()) {
    for (std::vector<QwIntegratedRasterChannel<QwVQWK_Channel> >::const_iterator
        IntegratedRasterChannel  = fIntegratedRasterChannel.begin();
        IntegratedRasterChannel != fIntegratedRasterChannel.end(); ++IntegratedRasterChannel) {
      if (IntegratedRasterChannel->GetElementName() == name) {
        return &(*IntegratedRasterChannel);
      }
    }
  }
  return 0;
}


//*****************************************************************
VQwSubsystem&  QwIntegratedRaster::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwIntegratedRaster::operator= \n";
  if (Compare(value))
    {
      VQwSubsystem::operator=(value);
      QwIntegratedRaster* input = dynamic_cast<QwIntegratedRaster*> (value);

      for (size_t i = 0; i < input->fIntegratedRasterChannel.size(); i++)
	this->fIntegratedRasterChannel[i] = input->fIntegratedRasterChannel[i];
    }
  return *this;
}

//*****************************************************************
VQwSubsystem&  QwIntegratedRaster::operator+=  (VQwSubsystem *value)
{
  if (Compare(value))
    {
      QwIntegratedRaster* input= dynamic_cast<QwIntegratedRaster*>(value);

      for (size_t i = 0; i < input->fIntegratedRasterChannel.size(); i++)
	this->fIntegratedRasterChannel[i] += input->fIntegratedRasterChannel[i];
    }
  return *this;
}

//*****************************************************************
VQwSubsystem&  QwIntegratedRaster::operator-=  (VQwSubsystem *value)
{

  if (Compare(value))
    {
      QwIntegratedRaster* input= dynamic_cast<QwIntegratedRaster*>(value);

      for (size_t i = 0; i < input->fIntegratedRasterChannel.size(); i++)
	this->fIntegratedRasterChannel[i] -= input->fIntegratedRasterChannel[i];
    }
  return *this;
}

//*****************************************************************
void  QwIntegratedRaster::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
}

//*****************************************************************
void  QwIntegratedRaster::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
}

//*****************************************************************
void QwIntegratedRaster::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if (Compare(numer) && Compare(denom))
    {
      QwIntegratedRaster* innumer = dynamic_cast<QwIntegratedRaster*>(numer);
      QwIntegratedRaster* indenom = dynamic_cast<QwIntegratedRaster*>(denom);

      for (size_t i = 0; i < innumer->fIntegratedRasterChannel.size(); i++)
	this->fIntegratedRasterChannel[i].Ratio(innumer->fIntegratedRasterChannel[i],indenom->fIntegratedRasterChannel[i]);
    }
}

//*****************************************************************
void QwIntegratedRaster::Scale(Double_t factor)
{
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].Scale(factor);
}



//*****************************************************************
Bool_t QwIntegratedRaster::Compare(VQwSubsystem *value)
{
  //  std::cout<<" Here in QwIntegratedRaster::Compare \n";

  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
      //      std::cout<<" types are not ok \n";
      //      std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    }
  else
    {
      QwIntegratedRaster* input= dynamic_cast<QwIntegratedRaster*>(value);
      if(input->fIntegratedRasterChannel.size() !=fIntegratedRasterChannel.size()  )
	{
	  res=kFALSE;
	}
    }
  return res;
}


//*****************************************************************
void  QwIntegratedRaster::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].ConstructHistograms(folder,prefix);
}


//*****************************************************************
void  QwIntegratedRaster::FillHistograms()
{
  if (! HasDataLoaded()) return;

  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].FillHistograms();
}


//*****************************************************************
void QwIntegratedRaster::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].ConstructBranchAndVector(tree, prefix, values);
}

//*****************************************************************
void QwIntegratedRaster::ConstructBranch(TTree *tree, TString & prefix)
{
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].ConstructBranch(tree, prefix);
}

//*****************************************************************
void QwIntegratedRaster::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp;
  QwParameterFile* nextmodule;

  tmp="QwIntegratedRasterChannel";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();
    //This section contains sub modules and or channels to be included in the tree

    for(size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
      fIntegratedRasterChannel[i].ConstructBranch(tree, prefix,*nextmodule);
  }
}

//*****************************************************************
void  QwIntegratedRaster::FillTreeVector(std::vector<Double_t> &values) const
{
  if (! HasDataLoaded()) return;

  for(size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].FillTreeVector(values);
}


//*****************************************************************
void  QwIntegratedRaster::PrintValue() const
{
  QwMessage << "=== QwIntegratedRaster: " << GetSubsystemName() << " ===" << QwLog::endl;
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].PrintValue();
}

//*****************************************************************
void  QwIntegratedRaster::PrintInfo() const
{
  QwMessage << "Name of the subsystem = " << fSystemName << QwLog::endl;
  QwMessage << "There are " << fIntegratedRasterChannel.size() << " IntegratedRasterChannel" << QwLog::endl;

  QwMessage << " Printing Running AVG and other channel info" << QwLog::endl;
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].PrintInfo();
}

//*****************************************************************
void  QwIntegratedRaster::PrintDetectorID() const
{
  for (size_t i=0;i<fDetectorIDs.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fDetectorIDs[i].Print();
    }
}

//*****************************************************************
void  QwIntegratedRasterDetectorID::Print() const
{
  std::cout<<std::endl<<"Detector name= "<<fdetectorname<<std::endl;
  std::cout<<"SubbankkIndex= "<<fSubbankIndex<<std::endl;
  std::cout<<"word index in subbank= "<<fWordInSubbank<<std::endl;
  std::cout<<"module type= "<<fmoduletype<<std::endl;
  std::cout<<"detector type=  "<<fdetectortype<<std::endl;
  std::cout<<"Index of this detector in the vector of similar detector= "<<
    fIndex<<std::endl;
  std::cout<<"Subelement index= "<<
    fSubelement<<std::endl;
  std::cout<<"==========================================\n";
}

//*****************************************************************
void QwIntegratedRaster::CalculateRunningAverage()
{
  for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
    fIntegratedRasterChannel[i].CalculateRunningAverage();
}

//*****************************************************************
void QwIntegratedRaster::AccumulateRunningSum(VQwSubsystem* value1)
{
  if (Compare(value1)) {
    QwIntegratedRaster* value = dynamic_cast<QwIntegratedRaster*>(value1);

    for (size_t i = 0; i < fIntegratedRasterChannel.size(); i++)
      fIntegratedRasterChannel[i].AccumulateRunningSum(value->fIntegratedRasterChannel[i]);
  }
}


//*****************************************************************
#ifdef __USE_DATABASE__
void QwIntegratedRaster::FillDB(QwParityDB *db, TString datatype)
{

  Bool_t local_print_flag = false;
  if(local_print_flag){
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "                         QwIntegratedRaster::FillDB                          " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwDBInterface> interface;
  std::vector<QwParitySSQLS::beam> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  TString measurement_type;
  measurement_type = 
    QwDBInterface::DetermineMeasurementTypeID(datatype,"",kTRUE);

  UInt_t i,j;
  i = j = 0;
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "IntegratedRasterChannel" <<QwLog::endl;

  for(i=0; i< fIntegratedRasterChannel.size(); i++) {
    interface.clear();
    interface = fIntegratedRasterChannel[i].GetDBEntry();
    for(j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMonitorID( db );
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
    QwMessage << "QwIntegratedRaster::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }

  db->Disconnect();

  return;
}


void QwIntegratedRaster::FillErrDB(QwParityDB *db, TString datatype)
{

  Bool_t local_print_flag = false;
  if(local_print_flag){
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "               QwIntegratedRaster::FillErrDB                    " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwErrDBInterface> interface;
  std::vector<QwParitySSQLS::beam_errors> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  UInt_t i,j;
  i = j = 0;
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "IntegratedRasterChannel" <<QwLog::endl;

  for(i=0; i< fIntegratedRasterChannel.size(); i++) {
    interface.clear();
    interface = fIntegratedRasterChannel[i].GetErrDBEntry();
    for(j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID     ( analysis_id );
      interface.at(j).SetMonitorID      ( db );
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
    QwMessage << "QwIntegratedRaster::FillErrDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }

  db->Disconnect();

  return;
};
#endif //  __USE_DATABASE__

void QwIntegratedRaster::WritePromptSummary(QwPromptSummary *ps, TString type)
{
  QwMessage << "QwIntegratedRaster::WritePromptSummary()" << QwLog::endl;
  return;
};

