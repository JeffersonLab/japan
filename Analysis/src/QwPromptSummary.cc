
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

Bool_t isDouble= fElementName.Contains("_dd") || fElementName.Contains("_da");

        if (type.Contains("yield")&& !isDouble){     
   		out = Form("%14s | Mean: %.2e +/- %.2e \t Width: %.2e\n", fElementName.Data(), fYield, fYieldError, fYieldWidth); 
   	}
        if (type.Contains("asy")&& !isDouble){
 	        out = Form("%14s | Mean: %.2e +/- %.2e \t Width: %0.2e  \n", fElementName.Data(), fAsymDiff, fAsymDiffError, fAsymDiffWidth);
        }
        if (type.Contains("double")&& isDouble) {
	       	out = Form ("%14s | Mean: %.2e +/- %.2e \t Width: %0.2e  \n", fElementName.Data(), fAsymDiff, fAsymDiffError, fAsymDiffWidth);
	}


return out;
 
};


void 
PromptSummaryElement::Set(TString type, const Double_t a, const Double_t a_err, const Double_t a_width)    // Fix Me - This function could use some cleaning up. Use maps.
{
  Double_t asymmetry_ppm = 1e-6;

  if(type.Contains("yield")) {
    if (fElementName.Contains("bcm")) {
      this->SetYieldUnit("uA");
    }
    else if (fElementName.Contains("bpm")) {
      this->SetYieldUnit("mm");
    }
    else if (fElementName.Contains("MD")) {
      this->SetYieldUnit("V/uA");
    }
    else if (fElementName.Contains("lumi")) {
      this->SetYieldUnit("V/uA");
    }
    else {
      this->SetYieldUnit("---");
    }
    this->SetYield(a);
    this->SetYieldError(a_err);
    this->SetYieldWidth(a_width);
  } 
  else if(type.Contains("asymmetry")) {
    
    if (fElementName.Contains("bpm")) {
      this->SetDifferenceUnit("nm");
      this->SetDifference(a);
      this->SetDifferenceError(a_err);
      this->SetDifferenceWidth(a_width);
    } 
    else {
      this->SetAsymmetryUnit("ppm");
      this->SetAsymmetry(a/asymmetry_ppm);
      this->SetAsymmetryError(a_err/asymmetry_ppm);
      this->SetAsymmetryWidth(a_width/asymmetry_ppm);
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
  
  out =  "======================END OF SUMMARY============================\n";
  return out;
};


TString
QwPromptSummary::PrintCSVHeader() 
{
  TString out = "";
   
  
  out += Form("Distribution parameters for run %d \n",fRunNumber);
  out += "================================================================\n";

  out += "Yield Units: bcm*(uA), cav*q(uA), bpm*(mm), sam*(V/uA)\n";
  out += "Asymmetry/Difference Units: bpm*(nm), bcm*(ppm), cav*q(ppm) \n";

  out += "================================================================\n";
  
  

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
QwPromptSummary::PrintCSV()
{
  printf("-----------------------\n");
  TString filename = getenv_safe_TString("QW_ROOTFILES"); 
  filename+=Form("/summary_%d.txt", fRunNumber);
  TString header= this->PrintCSVHeader();
  std::ofstream output;
  output.open(filename.Data());
  output<< header.Data();
  
  TString secheader= "================================================================\n";
  secheader+="\t\t\t Yields \t\t\t\n";
  secheader+="================================================================\n" ;
  output << secheader.Data() ;
 
  for (auto i=fElementList.begin(); i!=fElementList.end(); i++  )
    {
      output << (*i)->GetCSVSummary("yield") ;
    }
  

  secheader= "================================================================\n";
  secheader+="\t\t\t Asymmetries/Differences \t\t\t\n";
  secheader+="================================================================\n";
  output << secheader.Data();
 
  for ( auto j=fElementList.begin(); j!=fElementList.end(); j++ )
    {
      output << (*j)->GetCSVSummary("asymmetry");
    }



  secheader= "================================================================\n";
  secheader+="\t\t\t Combined Differences/Averages \t\t\t\n";
  secheader+="================================================================\n";
  output << secheader.Data();

  for ( auto j=fElementList.begin(); j!=fElementList.end(); j++ )
    {
      output << (*j)->GetCSVSummary("double");
    }

  output<< "=======================END OF SUMMARY===========================\n";

  output.close();
  
  return;
};

void
QwPromptSummary::PrintTextSummary()
{  
  return;
};

