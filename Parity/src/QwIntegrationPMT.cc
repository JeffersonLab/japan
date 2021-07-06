/**********************************************************\
* File: QwIntegrationPMT.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwIntegrationPMT.h"

// System headers
#include <stdexcept>


// Qweak headers
#ifdef __USE_DATABASE__
#include "QwDBInterface.h"
#endif

/********************************************************/
void QwIntegrationPMT::SetPedestal(Double_t pedestal)
{
	fPedestal=pedestal;
	fTriumf_ADC.SetPedestal(fPedestal);
	return;
}

void QwIntegrationPMT::SetCalibrationFactor(Double_t calib)
{
	fCalibration=calib;
	fTriumf_ADC.SetCalibrationFactor(fCalibration);
	return;
}
/********************************************************/
void  QwIntegrationPMT::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(name,datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
  return;
}
/********************************************************/
void  QwIntegrationPMT::InitializeChannel(TString subsystem, TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(subsystem,"QwIntegrationPMT", name, datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
  return;
}
/********************************************************/
void  QwIntegrationPMT::InitializeChannel(TString subsystem, TString module, TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(subsystem,module, name, datatosave);
  SetElementName(name);
  SetBlindability(kTRUE);
  SetNormalizability(kTRUE);
  return;
}
/********************************************************/
void QwIntegrationPMT::ClearEventData()
{
  fTriumf_ADC.ClearEventData();
  return;
}
/********************************************************/
void QwIntegrationPMT::PrintErrorCounters(){
  fTriumf_ADC.PrintErrorCounters();
}
/********************************************************/
void QwIntegrationPMT::UseExternalRandomVariable()
{
  fTriumf_ADC.UseExternalRandomVariable();
  return;
}
/********************************************************/
void QwIntegrationPMT::SetExternalRandomVariable(double random_variable)
{
  fTriumf_ADC.SetExternalRandomVariable(random_variable);
  return;
}
/********************************************************/
void QwIntegrationPMT::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.SetRandomEventDriftParameters(amplitude, phase, frequency);
  return;
}
/********************************************************/
void QwIntegrationPMT::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.AddRandomEventDriftParameters(amplitude, phase, frequency);
  return;
}
/********************************************************/
void QwIntegrationPMT::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fTriumf_ADC.SetRandomEventParameters(mean, sigma);
  return;
}
/********************************************************/
void QwIntegrationPMT::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fTriumf_ADC.SetRandomEventAsymmetry(asymmetry);
  return;
}
/********************************************************/
void QwIntegrationPMT::RandomizeEventData(int helicity, double time)
{
  fTriumf_ADC.RandomizeEventData(helicity, time);
  return;
}

/********************************************************/
void QwIntegrationPMT::RandomizeMollerEvent(int helicity, const QwBeamCharge& charge, const QwBeamPosition& xpos, const QwBeamPosition& ypos, const QwBeamAngle& xprime, const QwBeamAngle& yprime, const QwBeamEnergy& energy)
{
  QwMollerADC_Channel temp(this->fTriumf_ADC);
  fTriumf_ADC.ClearEventData();

  temp.AssignScaledValue(xpos, fCoeff_x);
  fTriumf_ADC += temp;

  temp.AssignScaledValue(ypos, fCoeff_y);
  fTriumf_ADC += temp;

  temp.AssignScaledValue(xprime, fCoeff_xp);
  fTriumf_ADC += temp;

  temp.AssignScaledValue(yprime, fCoeff_yp);
  fTriumf_ADC += temp;

  temp.AssignScaledValue(energy, fCoeff_e);
  fTriumf_ADC += temp;

//fTriumf_ADC.AddChannelOffset(fBaseRate * (1+helicity*fAsym));
  fTriumf_ADC.AddChannelOffset(1.0+helicity*fAsym);

  fTriumf_ADC *= charge;
  fTriumf_ADC.Scale(fNormRate*fVoltPerHz);  //  After this Scale function, fTriumf_ADC should be the detector signal in volts.
  fTriumf_ADC.ForceMapfileSampleSize();
  //  Double_t voltage_width = sqrt(fTriumf_ADC.GetValue()*window_length/fVoltPerHz)/(window_length/fVoltPerHz);
  Double_t voltage_width = sqrt( fTriumf_ADC.GetValue() / (fTriumf_ADC.GetNumberOfSamples()*QwMollerADC_Channel::kTimePerSample/Qw::sec/fVoltPerHz) );
  //std::cout << "Voltage Width: " << voltage_width << std::endl;
  fTriumf_ADC.SmearByResolution(voltage_width);
  fTriumf_ADC.SetRawEventData();

  return;

}


/********************************************************/
void QwIntegrationPMT::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fTriumf_ADC.SetHardwareSum(hwsum, sequencenumber);
  return;
}

Double_t QwIntegrationPMT::GetValue()
{
  return fTriumf_ADC.GetValue();
}

Double_t QwIntegrationPMT::GetValue(Int_t blocknum)
{
  return fTriumf_ADC.GetValue(blocknum);
}

/********************************************************/
void QwIntegrationPMT::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fTriumf_ADC.SetEventData(block, sequencenumber);
  return;
}
/********************************************************/
void QwIntegrationPMT::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fTriumf_ADC.EncodeEventData(buffer);
}
/********************************************************/
void  QwIntegrationPMT::ProcessEvent()
{
  ApplyHWChecks();//first apply HW checks and update HW  error flags.
  fTriumf_ADC.ProcessEvent();

  return;
}
/********************************************************/
Bool_t QwIntegrationPMT::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=fTriumf_ADC.ApplyHWChecks();//will check for consistancy between HWSUM and SWSUM also check for sample size
  eventokay=(deviceerror & 0x0);//if no HW error return true


  return eventokay;
}
/********************************************************/

Int_t QwIntegrationPMT::SetSingleEventCuts(Double_t LL=0, Double_t UL=0){//std::vector<Double_t> & dEventCuts){//two limts and sample size
  fTriumf_ADC.SetSingleEventCuts(LL,UL);
  return 1;
}

/********************************************************/
void QwIntegrationPMT::SetSingleEventCuts(UInt_t errorflag, Double_t LL=0, Double_t UL=0, Double_t stability=0, Double_t burplevel=0){
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kPMTErrorFlag;
  QwMessage<<"QwIntegrationPMT Error Code passing to QwMollerADC_Ch "<<errorflag<<QwLog::endl;
  fTriumf_ADC.SetSingleEventCuts(errorflag,LL,UL,stability,burplevel);

}

/********************************************************/

void QwIntegrationPMT::SetDefaultSampleSize(Int_t sample_size){
 fTriumf_ADC.SetDefaultSampleSize((size_t)sample_size);
}

/********************************************************/
void QwIntegrationPMT::SetSaturationLimit(Double_t saturation_volt){
  fTriumf_ADC.SetMollerADCSaturationLimt(saturation_volt);
}
//*/

/********************************************************/
Bool_t QwIntegrationPMT::ApplySingleEventCuts(){


//std::cout<<" QwBCM::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;

  if (fTriumf_ADC.ApplySingleEventCuts()){
    status=kTRUE;
    //std::cout<<" BCM Sample size "<<fTriumf_ADC.GetNumberOfSamples()<<std::endl;
  }
  else{
    status&=kFALSE;//kTRUE;//kFALSE;
  }

  return status;

}

/********************************************************/

void QwIntegrationPMT::PrintErrorCounters() const{// report number of events failed due to HW and event cut faliure
  fTriumf_ADC.PrintErrorCounters();
}

/*********************************************************/

Bool_t QwIntegrationPMT::CheckForBurpFail(const VQwDataElement *ev_error){
  Bool_t burpstatus = kFALSE;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      //std::cout<<" Here in QwIntegrationPMT::CheckForBurpFail \n";
      if (this->GetElementName()!="") {
        const QwIntegrationPMT* value_pmt = dynamic_cast<const QwIntegrationPMT* >(ev_error);
        burpstatus |= fTriumf_ADC.CheckForBurpFail(&(value_pmt->fTriumf_ADC)); 
      }
    } else {
      TString loc="Standard exception from QwIntegrationPMT::CheckForBurpFail :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }
  return burpstatus;
};

/********************************************************/
void QwIntegrationPMT::UpdateErrorFlag(const QwIntegrationPMT* ev_error){
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwIntegrationPMT::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
	fTriumf_ADC.UpdateErrorFlag(ev_error->fTriumf_ADC);
      }
    } else {
      TString loc="Standard exception from QwIntegrationPMT::UpdateErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }  
};

/********************************************************/


Int_t QwIntegrationPMT::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fTriumf_ADC.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
}  Double_t fULimit, fLLimit;

/********************************************************/
QwIntegrationPMT& QwIntegrationPMT::operator= (const QwIntegrationPMT &value)
{
//   std::cout<<" Here in QwIntegrationPMT::operator= \n";
  if (GetElementName()!="")
    {
      this->fTriumf_ADC=value.fTriumf_ADC;
      this->fPedestal=value.fPedestal;
      this->fCalibration=value.fCalibration;
    }
//   std::cout<<" to be copied \n";
//   value.Print();
//   std::cout<<" copied \n";
//   this->Print();

  return *this;
}

QwIntegrationPMT& QwIntegrationPMT::operator+= (const QwIntegrationPMT &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC+=value.fTriumf_ADC;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}

QwIntegrationPMT& QwIntegrationPMT::operator-= (const QwIntegrationPMT &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC-=value.fTriumf_ADC;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
}

void QwIntegrationPMT::Ratio(QwIntegrationPMT &numer, QwIntegrationPMT &denom)
{
  //  std::cout<<"QwIntegrationPMT::Ratio element name ="<<GetElementName()<<" \n";
  if (GetElementName()!="")
    {
      //  std::cout<<"here in \n";
      this->fTriumf_ADC.Ratio(numer.fTriumf_ADC,denom.fTriumf_ADC);
      this->fPedestal=0;
      this->fCalibration=0;
    }
  return;
}

void QwIntegrationPMT::Scale(Double_t factor)
{
  fTriumf_ADC.Scale(factor);
  return;
}

void QwIntegrationPMT::Normalize(VQwDataElement* denom)
{
  if (fIsNormalizable) {
    QwMollerADC_Channel* denom_ptr = dynamic_cast<QwMollerADC_Channel*>(denom);
    QwMollerADC_Channel vqwk_denom(*denom_ptr);
    fTriumf_ADC.DivideBy(vqwk_denom);
  }
}

void QwIntegrationPMT::PrintValue() const
{
  fTriumf_ADC.PrintValue();
}

void QwIntegrationPMT::PrintInfo() const
{
  //std::cout<<"QwMollerADC_Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<IntegrationPMT_Running_AVG<<std::endl;
  std::cout<<"QwMollerADC_Channel Info " <<std::endl;
  fTriumf_ADC.PrintInfo();
  std::cout<< "Blindability is "    << (fIsBlindable?"TRUE":"FALSE") 
	   <<std::endl;
  std::cout<< "Normalizability is " << (fIsNormalizable?"TRUE":"FALSE")
	   <<std::endl;
  std::cout << "fNormRate=" << fNormRate << "fVoltPerHz=" << fVoltPerHz 
            << " Asym=" << fAsym << " C_x=" << fCoeff_x << " C_y=" << fCoeff_y 
            << " C_xp=" << fCoeff_xp << " C_yp=" << fCoeff_yp 
            << " C_e=" << fCoeff_e << std::endl;
  return;
}

/********************************************************/
void  QwIntegrationPMT::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fTriumf_ADC.ConstructHistograms(folder, prefix);
    }
  return;
}

void  QwIntegrationPMT::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fTriumf_ADC.FillHistograms();
    }


  return;
}

void  QwIntegrationPMT::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.ConstructBranchAndVector(tree, prefix,values);
    }
  return;
}

void  QwIntegrationPMT::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.ConstructBranch(tree, prefix);
    }
  return;
}

void  QwIntegrationPMT::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
   TString devicename;
   devicename=GetElementName();
   devicename.ToLower();
   if (GetElementName()==""){
     //  This channel is not used, so skip filling the histograms.
   } else {
     if (modulelist.HasValue(devicename)){
       fTriumf_ADC.ConstructBranch(tree, prefix);
       QwMessage <<"QwIntegrationPMT Tree leave added to "<<devicename<<QwLog::endl;
       }

   }
  return;
}


void  QwIntegrationPMT::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {
    fTriumf_ADC.FillTreeVector(values);
  }
}

void QwIntegrationPMT::CalculateRunningAverage()
{
  fTriumf_ADC.CalculateRunningAverage();
}

void QwIntegrationPMT::AccumulateRunningSum(const QwIntegrationPMT& value, Int_t count, Int_t ErrorMask)
{
  fTriumf_ADC.AccumulateRunningSum(value.fTriumf_ADC, count, ErrorMask);
}

void QwIntegrationPMT::DeaccumulateRunningSum(QwIntegrationPMT& value, Int_t ErrorMask)
{
  fTriumf_ADC.DeaccumulateRunningSum(value.fTriumf_ADC, ErrorMask);
}


void QwIntegrationPMT::Blind(const QwBlinder *blinder)
{
  if (fIsBlindable)  fTriumf_ADC.Blind(blinder);
}

void QwIntegrationPMT::Blind(const QwBlinder *blinder, const QwIntegrationPMT& yield)
{
  if (fIsBlindable)  fTriumf_ADC.Blind(blinder, yield.fTriumf_ADC);
}

#ifdef __USE_DATABASE__
std::vector<QwDBInterface> QwIntegrationPMT::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  fTriumf_ADC.AddEntriesToList(row_list);
  return row_list;

}

std::vector<QwErrDBInterface> QwIntegrationPMT::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  fTriumf_ADC.AddErrEntriesToList(row_list);
  return row_list;
};
#endif
