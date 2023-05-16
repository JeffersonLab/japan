/**********************************************************\
* File: QwBeamDetectorID.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBeamDetectorID.h"

// Qweak headers
#include "QwVQWK_Channel.h"
#include "QwADC18_Channel.h"
#include "QwScaler_Channel.h"
#include "QwBPMStripline.h"
#include "QwQPD.h"
#include "QwLinearDiodeArray.h"
#include "QwBPMCavity.h"
#include "QwMollerADC_Channel.h"

//*****************************************************************
QwBeamDetectorID::QwBeamDetectorID(Int_t subbankid, Int_t offset,
				   TString name, TString dettype,
				   TString modtype):
  fSubbankIndex(subbankid),fWordInSubbank(offset),
  fmoduletype(modtype),fdetectorname(name),fdetectortype(dettype),
  fIndex(-1)
{
  fTypeID = GetQwBeamInstrumentType(dettype);
}

QwBeamDetectorID::QwBeamDetectorID(Int_t subbankid,
				   QwParameterFile &paramfile):
  fSubbankIndex(subbankid), fIndex(-1)
{
  Int_t modnum, channum;
  fmoduletype   = paramfile.GetTypedNextToken<TString>();
  modnum        = paramfile.GetTypedNextToken<Int_t>();    //slot number
  channum       = paramfile.GetTypedNextToken<Int_t>();    //channel number
  fdetectortype = paramfile.GetTypedNextToken<TString>();  //type-purpose of the detector
  fChannelName  = paramfile.GetTypedNextToken<TString>();  //name of the detector
  fmoduletype.ToUpper();
  fdetectortype.ToLower();
  fChannelName.ToLower();

  Int_t offset;
  if (fmoduletype == "VQWK") {
    fWordInSubbank = QwVQWK_Channel::GetBufferOffset(modnum, channum);
    if (paramfile.ReturnValue("vqwk_buffer_offset",offset)) {
      fWordInSubbank += offset;
    }
  } else if (fmoduletype == "ADC18") {
    fWordInSubbank = QwADC18_Channel::GetBufferOffset(modnum, channum);
    if (paramfile.ReturnValue("adc18_buffer_offset",offset)) {
      fWordInSubbank += offset;
    }
  } else if (fmoduletype == "SCALER") {
    fWordInSubbank = VQwScaler_Channel::GetBufferOffset(modnum, channum);
    if (paramfile.ReturnValue("scaler_buffer_offset",offset)) {
      fWordInSubbank += offset;
    }
  } else if (fmoduletype == "MOLLERADC") {
    fWordInSubbank = QwMollerADC_Channel::GetBufferOffset(modnum, channum);
    if (paramfile.ReturnValue("MollerADC_buffer_offset",offset)) {
      fWordInSubbank += offset;
    }
  } else {
    fWordInSubbank = -1;
  }
  fTypeID = GetQwBeamInstrumentType(fdetectortype);

  size_t namesize = fChannelName.Sizeof();
  switch (fTypeID){
  case kQwBPMStripline:
    fdetectorname   = fChannelName(0,namesize-3);
    fSubelementName = fChannelName(namesize-3,2);
    fSubelement = QwBPMStripline<QwVQWK_Channel>::GetSubElementIndex(fSubelementName);
    break;
  case kQwQPD:
    fdetectorname   = fChannelName(0,namesize-3);
    fSubelementName = fChannelName(namesize-3,2);
    fSubelement = QwQPD::GetSubElementIndex(fSubelementName);
    break;
  case kQwLinearArray:
    fdetectorname   = fChannelName(0,namesize-3);
    fSubelementName = fChannelName(namesize-3,2);
    fSubelement = QwLinearDiodeArray::GetSubElementIndex(fSubelementName);
    break;
  case kQwBPMCavity:
    QwBPMCavity::ParseChannelName(fChannelName,fdetectorname,
				  fSubelementName, fSubelement);
    break;
  default:
    fdetectorname   = fChannelName;
    fSubelementName = "";
    fSubelement = 0;
    break;
  }
}

QwBeamDetectorID::QwBeamDetectorID(const QwBeamDetectorID& input)
{
  fSubbankIndex   = input.fSubbankIndex;
  fWordInSubbank  = input.fWordInSubbank;
  fmoduletype     = input.fmoduletype;
  fdetectorname   = input.fdetectorname;
  fdetectortype   = input.fdetectortype;
  fTypeID         = input.fTypeID;
  fIndex          = input.fIndex;
  fSubelement     = input.fSubelement;
  fChannelName    = input.fChannelName;
  fSubelementName = input.fSubelementName;
}


Bool_t QwBeamDetectorID::ReportInitErrors() const
{
  Bool_t lineok;
  lineok = kTRUE;
  if(fWordInSubbank<0){
    QwError<< "QwBeamLine::LoadChannelMap:  Unknown module type, "
	   << fmoduletype <<", the detector "
	   << fChannelName <<" will not be decoded "
	   << QwLog::endl;
    lineok=kFALSE;
  }
  if(fTypeID==kQwUnknownDeviceType){
    QwError << "QwBeamLine::LoadChannelMap:  Unknown detector type, "
	    << fdetectortype <<", the detector "
	    << fChannelName <<" will not be decoded "
	    << QwLog::endl;
    lineok=kFALSE;
  }
  if(fSubelement==kInvalidSubelementIndex){
    QwError << "QwBeamLine::LoadChannelMap: "<< fSubelementName
	    << " was not recognized as a valid subelement for "
	    << fdetectortype << "device type (channel name=="
	    << fChannelName << ")" <<QwLog::endl;;
    lineok=kFALSE;
  }
  return lineok;
}

//*****************************************************************
void  QwBeamDetectorID::Print() const
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
  std::cout<<"---------------------------------------------------"<<std::endl;
  std::cout<<std::endl;


  return;
}
