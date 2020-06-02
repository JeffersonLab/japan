
#include "QwPromptSummary.h"

#include "QwColor.h"
#include "QwLog.h"

#include "TROOT.h"
#include "TMath.h"

/**
 *  \file   QwPromptSummary.cc
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Friday, December 16 10:55:31 EST 2011
 */

//
//
//
//  PromptSummaryElement
//
//
//
//

PromptSummaryElement::PromptSummaryElement()
{
  fElementName         = "";
  
  fYield               = 0.0;
  fYieldError          = 0.0;
  fYieldWidth          = 0.0;
  fYieldUnit           = "";

  fAsymDiff            = 0.0;
  fAsymDiffError       = 0.0;
  fAsymDiffWidth       = 0.0;
  fAsymDiffUnit        = "";

};


PromptSummaryElement::PromptSummaryElement(TString name)
{
  fElementName         = name;
  
  fYield               = 0.0;
  fYieldError          = 0.0;
  fYieldWidth          = 0.0;
  fYieldUnit           = "";

  fAsymDiff            = 0.0;
  fAsymDiffError       = 0.0;
  fAsymDiffWidth       = 0.0;
  fAsymDiffUnit        = "";

};

PromptSummaryElement::~PromptSummaryElement()
{
};


void
PromptSummaryElement::FillData(
			       Double_t yield, 
			       Double_t yield_err, 
			       Double_t yield_width,
			       TString  yield_unit,
			       Double_t asym_diff, 
			       Double_t asym_diff_err, 
			       Double_t asym_diff_width, 
			       TString  asym_diff_unit
			       )
{
  fYield                = yield;
  fYieldError           = yield_err;
  fYieldWidth           = yield_width;
  fYieldUnit            = yield_unit;

  fAsymDiff            = asym_diff;
  fAsymDiffError       = asym_diff_err;
  fAsymDiffWidth       = asym_diff_width;
  fAsymDiffUnit        = asym_diff_unit;

  return;
};


TString
PromptSummaryElement::GetTextSummary()
{
  //  return Form("%14s | %12.2lf +/- %8.2lf | %+12.2lf +/- %8.2lf | %+12.2lf +/- %8.2lf\n", fElementName.Data(), fYield, fYieldError, fAsymDiff, fAsymDiffError, fAsymDiffWidth, fAsymDiffWidthError);
  TString test;
  return test;
};

TString
PromptSummaryElement::GetCSVSummary(TString type)
{

TString out = "";

Bool_t dd= fElementName.Contains("_dd");
Bool_t da= fElementName.Contains("_da");


if (type.Contains("yield")&& !(dd||da)){     
   		out = Form("%20s | Mean: %8.3f +/- %8.3f \t Width: %8.3f\n", fElementName.Data(), fYield, fYieldError, fYieldWidth); 
}
if (type.Contains("asy")&& !(dd||da)){
      out = Form("%20s | Mean: %8.3f +/- %8.3f \t Width: %8.3f\n", fElementName.Data(), fAsymDiff, fAsymDiffError, fAsymDiffWidth);
}
if (type.Contains("double")&& (dd||da)) {
     	out = Form ("%20s | Mean: %8.3f +/- %8.3f \t Width: %8.3f\n", fElementName.Data(), fAsymDiff, fAsymDiffError, fAsymDiffWidth);
}     


return out;
 
};


void 
PromptSummaryElement::Set(TString type, const Double_t a, const Double_t a_err, const Double_t a_width)   
{
  Double_t unit= 1;
//REMOVE THIS:  Bool_t qtarg=fElementName.EqualTo("bcm_an_ds3"); //Estimator for good events
  Bool_t bcm= fElementName.Contains("bcm");
  Bool_t bpm= fElementName.Contains("bpm");
  Bool_t sam= fElementName.Contains("sam");
  Bool_t md= fElementName.Contains("md");
  Bool_t dd=fElementName.Contains("_dd");
  Bool_t da=fElementName.Contains("_da");
  

  if (type.Contains("yield")){
    if (bcm) {
      this->SetYieldUnit("uA");
      unit=Qw::uA;
    }
    else if (bpm) {
      this->SetYieldUnit("mm");
      unit=Qw::mm;
    }
    else if (sam||md) {
      this->SetYieldUnit("mV/uA");
      unit=Qw::mV_uA;
    }
    else {
      this->SetYieldUnit("---");
    }
    this->SetYield(a/unit);
    this->SetYieldError(a_err/unit);
    this->SetYieldWidth(a_width/unit);

  }
  else if(type.Contains("asymmetry")) {
    if (bpm) {
      this->SetDifferenceUnit("um");
      unit=Qw::um;
      this->SetDifference(a/unit);
      this->SetDifferenceError(a_err/unit);
      this->SetDifferenceWidth(a_width/unit);
    } 
    else {
      this->SetAsymmetryUnit("ppm");
      unit=Qw::ppm;
      this->SetAsymmetry(a/unit);
      this->SetAsymmetryError(a_err/unit);
      this->SetAsymmetryWidth(a_width/unit);
    } 
  } 
  else if(type.Contains("difference")) {
  } 
  else {
  }
  return;
};

QwPromptSummary::QwPromptSummary()
{
  fRunNumber    = 0;
  fRunletNumber = 0;
 
 
  fNElements = 0;

  fLocalDebug = kTRUE;
  
  this->SetupElementList();

};


QwPromptSummary::QwPromptSummary(Int_t run_number, Int_t runlet_number)
{
  fRunNumber    = run_number;
  fRunletNumber = runlet_number;

 
  fNElements = 0;

  fLocalDebug = kFALSE;
  
  this->SetupElementList();

};


QwPromptSummary::~QwPromptSummary()
{

  for (auto i=fElementList.begin() ; i!=fElementList.end();i++){
  	delete *i;
  }
  fElementList.clear();

};



void 
QwPromptSummary::SetupElementList()
{

  
/* 
  this->AddElement(new PromptSummaryElement("bcm_an_us-bcm_an_ds"));
  this->AddElement(new PromptSummaryElement("bcm_an_us-bcm_an_ds3"));
  this->AddElement(new PromptSummaryElement("bcm_an_us-bcm_an_ds10"));
  this->AddElement(new PromptSummaryElement("bcm_an_us-bcm_dg_us"));
  this->AddElement(new PromptSummaryElement("bcm_an_us-bcm_dg_ds"));
  
  this->AddElement(new PromptSummaryElement("bcm_an_ds-bcm_an_ds3"));
  this->AddElement(new PromptSummaryElement("bcm_an_ds-bcm_an_ds10"));
  this->AddElement(new PromptSummaryElement("bcm_an_ds-bcm_dg_us"));
  this->AddElement(new PromptSummaryElement("bcm_an_ds-bcm_dg_ds"));
  
  this->AddElement(new PromptSummaryElement("bcm_an_ds3-bcm_an_ds10"));
  this->AddElement(new PromptSummaryElement("bcm_an_ds3-bcm_dg_us"));
  this->AddElement(new PromptSummaryElement("bcm_an_ds3-bcm_dg_ds"));

  
  this->AddElement(new PromptSummaryElement("bcm_an_ds10-bcm_dg_us"));
  this->AddElement(new PromptSummaryElement("bcm_an_ds10-bcm_dg_ds"));

  this->AddElement(new PromptSummaryElement("bcm_dg_us-bcm_dg_ds"));

*/   

};


void 
QwPromptSummary::AddElement(PromptSummaryElement *in)
{
  Int_t pos = 0;

  fElementList.push_back(in);
  if(fLocalDebug) {
    printf("AddElement at pos %d\n", pos);
  }

  fNElements++;
  return;
};


PromptSummaryElement *
QwPromptSummary::GetElementByName(TString name)
{

  
  TString get_name = "";

  for (auto i=fElementList.begin(); i!=fElementList.end(); i++  )
    {
      PromptSummaryElement* an_element = *i;
      get_name   = an_element->GetName();
      if( get_name.EqualTo(name) ) {
	if(fLocalDebug) {
	  std::cout << "System " << an_element->GetName()
		    << " QwPromptSummary::GetElementByName address at" << an_element << std::endl;
	}
	return an_element;
      }
    }

  return NULL;
};


TString
QwPromptSummary::PrintTextSummaryHeader()
{
  TString out = "";
  //  TString filename = "";

  //  filename = Form("summary_%d_%d.txt", fRunNumber, fRunletNumber);
  
  out = "======= BEGIN ======= \n";
  out += Form("        RUN = %5d RUNLET %2d \n\n\n\n\n\n", fRunNumber, fRunletNumber);
  return out;
};



TString
QwPromptSummary::PrintTextSummaryTailer()
{
  TString out = "";
  
  out =  "==================================================\n";
  return out;
};


TString
QwPromptSummary::PrintCSVHeader(Int_t nEvents, TString start_time, TString end_time)
{
  TString out = "";
   
  Double_t goodEvents = ((this->GetElementByName("bcm_an_us"))->GetNumGoodEvents())*fPatternSize;

  out += Form("Run: %d \n",fRunNumber);
  out += "Start Time: "+start_time+"\nEnd Time: "+end_time+"\n";
  out += Form("Number of events processed: %i\n",nEvents);
  out += Form("Number of events in good multiplicity patterns: %3.0f\n", goodEvents);
  out += Form("Percentage of good events: %3.1f \%\n", goodEvents/nEvents*100);
  out += "=========================================================================\n";
  out += "Yield Units: bcm(uA), cavq(uA), bpm(mm), sam(mV/uA)\n";
  out += "Asymmetry/Difference Units: bcm(ppm), cavq(ppm), bpm(um), sam(ppm)\n";

  out += "=========================================================================\n";
  
  

  return out;
};



void
QwPromptSummary::FillDataInElement(TString  name, 
				   Double_t yield, 
				   Double_t yield_err, 
				   Double_t yield_width, 
				   TString  yield_unit, 
				   Double_t asym_diff, 
				   Double_t asym_diff_err, 
				   Double_t asym_diff_width,
				   TString  asym_diff_unit
				   )
{
 
  PromptSummaryElement* an_element = NULL;
  an_element = this->GetElementByName(name);
  if(an_element) {
    an_element->FillData(yield, yield_err, yield_width, yield_unit, asym_diff, asym_diff_err, asym_diff_width, asym_diff_unit);
  }
  else {
    if(fLocalDebug) {
      std::cout 
	<< "QwPromptSummary::FillDataInElement : No Element with the name " 
	<< name 
	<<  std::endl;
    }
  }
  return;
};


void
QwPromptSummary::FillYieldToElement(TString name, 
				    Double_t yield, 
				    Double_t yield_error, 
				    Double_t yield_width,
				    TString yield_unit
				    )
{
  PromptSummaryElement* an_element = NULL;
  an_element = this->GetElementByName(name);
  if(an_element) {
    an_element->SetYield(yield);
    an_element->SetYieldError(yield_error);
    an_element->SetYieldWidth(yield_width);
  }
  else {
    if(fLocalDebug) {
      std::cout 
	<< "QwPromptSummary::FillYieldToElement : No Element with the name " 
	<< name 
	<<  std::endl;
    }
  }
  return;
};

void 
QwPromptSummary::FillAsymDiffToElement(TString name, 
				       Double_t asym_diff, 
				       Double_t asym_diff_err, 
				       Double_t asym_diff_width,
				       TString asym_diff_unit
				       )
{
  PromptSummaryElement* an_element = NULL;
  an_element = this->GetElementByName(name);
  if(an_element) {
    an_element->SetAsymmetry(asym_diff);
    an_element->SetAsymmetryError(asym_diff_err);
    an_element->SetAsymmetryWidth(asym_diff_width);
  }
  else {
    if(fLocalDebug) {
      std::cout 
	<< "QwPromptSummary::FillYieldToElement : No Element with the name " 
	<< name 
	<<  std::endl;
    }
  }

  return;
};

void 
QwPromptSummary::FillDoubleDifference(TString type, TString name1, TString name2)
{
  PromptSummaryElement* an_element = NULL;


  PromptSummaryElement* one_element = NULL;
  PromptSummaryElement* two_element = NULL;

  one_element = this->GetElementByName(name1);
  two_element = this->GetElementByName(name2);

  if(one_element && two_element ) {

    an_element = this->GetElementByName(name1+"-"+name2);

    if(an_element) {
   
      
      Double_t diff       = 0.0;
      Double_t error_diff = 0.0;
      Double_t width_diff = 0.0;
      
      Double_t a = 0.0;
      Double_t b = 0.0;
      Double_t a_err = 0.0;
      Double_t b_err = 0.0;
      Double_t a_wit = 0.0;
      Double_t b_wit = 0.0;
     

      if(type.Contains("yield")) {
	a     = one_element -> GetYield();
	b     = two_element -> GetYield();
	a_err = one_element -> GetYieldError();
	b_err = two_element -> GetYieldError();
	a_wit = one_element -> GetYieldWidth();
	b_wit = two_element -> GetYieldWidth();
	
	diff       = a - b;
	error_diff = TMath::Sqrt(a_err*a_err + b_err*b_err);
	width_diff = a_wit - b_wit;

	an_element -> SetYield(diff);
	an_element -> SetYieldError(error_diff);
	an_element -> SetYieldWidth(width_diff);
	
      } else {
	a     = one_element -> GetAsymmetry();
	b     = two_element -> GetAsymmetry();
	a_err = one_element -> GetAsymmetryError();
	b_err = two_element -> GetAsymmetryError();
	a_wit = one_element -> GetAsymmetryWidth();
	b_wit = two_element -> GetAsymmetryWidth();
	
	diff       = a - b;
	error_diff = TMath::Sqrt(a_err*a_err + b_err*b_err);
	width_diff = a_wit - b_wit;

	an_element -> SetAsymmetry(diff);
	an_element -> SetAsymmetryError(error_diff);
	an_element -> SetAsymmetryWidth(width_diff);
        
	
      }
    
    }
    else {
      if(fLocalDebug) {
	std::cout 
	  << "QwPromptSummary::FillYieldToElement : No Element with the name " 
	  << name1 + "-" + name2 
	  <<  std::endl;
      }
  }

  }
  else {
    if(fLocalDebug) {
      std::cout 
	<< "QwPromptSummary::FillDoubleDifference: No Elements with the name " 
	<< name1 << " and " << name2  
	<<  std::endl;
    }
  }
}

void
QwPromptSummary::PrintCSV(Int_t nEvents, TString start_time, TString end_time)
{
  printf("-----------------------\n");
  TString filename = gQwOptions.GetValue<std::string>("rootfiles");
  filename+=Form("/summary_%d.txt", fRunNumber);
  TString header= this->PrintCSVHeader(nEvents, start_time, end_time);
  std::ofstream output;
  output.open(filename.Data());
  output<< header.Data();
  
  TString secheader= "=========================================================================\n";
  secheader+=Form("%40s \n","Yields");
  secheader+="=========================================================================\n" ;
  output << secheader.Data() ;
 
  for (auto i=fElementList.begin(); i!=fElementList.end(); i++  )
    {
      output << (*i)->GetCSVSummary("yield") ;
    }
  

  secheader= "=========================================================================\n";
  secheader+=Form("%50s\n","Asymmetries/Differences");
  secheader+="=========================================================================\n";
  output << secheader.Data();
 
  for ( auto j=fElementList.begin(); j!=fElementList.end(); j++ )
    {
      output << (*j)->GetCSVSummary("asymmetry");
    }



  secheader= "=========================================================================\n";
  secheader+=Form("%55s\n", "Combined Differences/Averages");
  secheader+="=========================================================================\n";
  output << secheader.Data();

  for ( auto j=fElementList.begin(); j!=fElementList.end(); j++ )
    {
      output << (*j)->GetCSVSummary("double");
    }

  output<< "=========================================================================\n";
  output<< Form("%45s\n"," End of Summary");
  output<< "=========================================================================\n";
  output.close();
  
  return;
};

void
QwPromptSummary::PrintTextSummary()
{  
  return;
};

