#ifndef __QwPromptSummary__
#define __QwPromptSummary__
/**
 *  \file   QwPromptSummary.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Friday, December 16 10:29:10 EST 2011
 */

#include <iostream>


#include "TObject.h"
#include "TClonesArray.h"
#include "TList.h"
#include "TString.h"

#include "TROOT.h"

/**
 *  \class QwPromptSummary
 *  \ingroup QwAnalysis
 *
 *  \brief 
 *
 */

class PromptSummaryElement :  public TObject
{
 public:
  PromptSummaryElement();
  PromptSummaryElement(TString name);
  virtual ~PromptSummaryElement();
  //  friend std::ostream& operator<<(std::ostream& os, const PromptSummaryElement &ps_element);

  void FillData(Double_t yield, Double_t yield_err, Double_t yield_width, TString yield_unit, 
		Double_t asym_diff, Double_t asym_diff_err,  Double_t asym_diff_width, TString asym_diff_unit);

  void    SetName (const TString in)  {fElementName=in;};
  TString GetName ()                  {return fElementName;};


  // Yield      : fHardwareBlockSumM2 
  // YieldError : fHardwareBlockSumError = sqrt(fHardwareBlockSumM2) / fGoodEventCount;

  void SetYield                (const Double_t in) { fYield=in; };
  void SetYieldError           (const Double_t in) { fYieldError=in; };
  void SetYieldWidth           (const Double_t in) { fYieldWidth=in; };
  void SetYieldUnit            (const TString  in) { fYieldUnit=in; };

  // Asymmetry : 
  void SetAsymmetry           (const Double_t in) { fAsymDiff=in; };
  void SetAsymmetryError      (const Double_t in) { fAsymDiffError=in; };
  void SetAsymmetryWidth      (const Double_t in) { fAsymDiffWidth=in; };
  void SetAsymmetryUnit       (const TString  in) { fAsymDiffUnit=in; };


  // Difference : 
  void SetDifference           (const Double_t in) { fAsymDiff=in; };
  void SetDifferenceError      (const Double_t in) { fAsymDiffError=in; };
  void SetDifferenceWidth      (const Double_t in) { fAsymDiffWidth=in; };
  void SetDifferenceUnit       (const TString  in) { fAsymDiffUnit=in; };


  // Yield 
  const Double_t GetYield         () { return fYield; };
  const Double_t GetYieldError    () { return fYieldError; };
  const Double_t GetYieldWidth    () { return fYieldWidth; };
  const TString  GetYieldUnit     () { return  fYieldUnit; };

  // Asymmetry : 
  const Double_t GetAsymmetry     () { return fAsymDiff; };
  const Double_t GetAsymmetryError() { return fAsymDiffError; };
  const Double_t GetAsymmetryWidth() { return fAsymDiffWidth; };
  const TString  GetAsymmetryUnit () { return fAsymDiffUnit; };


  // Difference : 
  const Double_t GetDifference     () { return fAsymDiff; };
  const Double_t GetDifferenceError() { return fAsymDiffError; };
  const Double_t GetDifferenceWidth() { return fAsymDiffWidth; };
  const TString  GetDifferenceUnit () { return fAsymDiffUnit; };


  void Set(TString type, const Double_t a, const Double_t a_err, const Double_t a_width);

  //  void SetAsymmetryWidthError (const Double_t in) { fAsymmetryWidthError=in; };
  // void SetAsymmetryWidthUnit  (const TString  in) { fAsymmetryWidthUnit=in; };


  TString GetCSVSummary();
  TString GetTextSummary();
  //

  // This is not sigma, but error defined in QwVQWK_Channel::CalculateRunningAverage() in QwVQWK_Channel.cc as follows
  // fHardwareBlockSumError = sqrt(fHardwareBlockSumM2) / fGoodEventCount;
  //
 private:

  TString fElementName;
  
  Double_t fYield;
  Double_t fYieldError;
  Double_t fYieldWidth;
  TString  fYieldUnit;

  Double_t fAsymDiff;
  Double_t fAsymDiffError;
  Double_t fAsymDiffWidth;
  TString  fAsymDiffUnit;

  /* Double_t fAsymmetryWidth; */
  /* Double_t fAsymmetryWidthError; */
  /* TString  fAsymmetryWidthUnit; */

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(PromptSummaryElement,0);
  #endif

};


class QwPromptSummary  :  public TObject
{

 public:
  QwPromptSummary();
  QwPromptSummary(Int_t run_number, Int_t runlet_number);
  virtual ~QwPromptSummary();
  //  friend std::ostream& operator<<(std::ostream& os, const QwF1TDC &f1tdc);


  Int_t                    fNElements;
  TObjArray               *fElementList; 

  void SetRunNumber(const Int_t in) {fRunNumber = in;};
  const Int_t GetRunNumber() {return fRunNumber;};
  
  void SetRunletNumber(const Int_t in) {fRunletNumber = in;};
  const Int_t GetRunletNumber() {return fRunletNumber;};


  void AddElement(PromptSummaryElement *in);
  PromptSummaryElement* GetElementByName(TString name);
  
  void FillDataInElement(TString name, 
			 Double_t yield, Double_t yield_err, Double_t yield_width, TString yield_unit, 
			 Double_t asym_diff, Double_t asym_diff_err, Double_t asym_diff_width, TString asym_diff_unit);
  
  void FillYieldToElement(TString name, Double_t yield, Double_t yield_error, Double_t yield_width, TString yield_unit);
  void FillAsymDiffToElement(TString name, Double_t asym_diff, Double_t asym_diff_err, Double_t asym_diff_width, TString asym_diff_unit);
  //  void FillDifferenceToElement(Double_t asym_diff, Double_t asym_diff_err, TString asym_diff_unit);

  //  void Print(const Option_t* options = 0) const;

  void FillDoubleDifference(TString type, TString name1, TString name2);

  Int_t  GetSize()         const {return fNElements;};
  Int_t  Size()            const {return fNElements;};
  Int_t  HowManyElements() const {return fNElements;};


  void PrintCSV();
  void PrintTextSummary();

private:


 

  TString PrintTextSummaryHeader();
  TString PrintTextSummaryTailer();
  TString PrintCSVHeader();

  void    SetupElementList();
  
  Int_t   fRunNumber;
  Int_t   fRunletNumber;

  Bool_t  fLocalDebug;

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwPromptSummary,0);
  #endif

};


#endif
