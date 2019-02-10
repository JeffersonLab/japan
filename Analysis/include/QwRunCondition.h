#ifndef __QwRunCondition__
#define __QwRunCondition__
/**
 *  \file   QwRunCondition.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Thursday, September  9 21:42:30 EDT 2010
 */


#include <unistd.h>
#include <iostream>
#include <fstream>

#include "TROOT.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"
#include "TTimeStamp.h"

class QwRunCondition
{

 public:
  QwRunCondition(){fRunConditionList=NULL;};
  QwRunCondition(Int_t argc, Char_t* argv[], TString name);
  virtual ~QwRunCondition();

  TList* Get();
  TString GetName();
 
  void SetROCFlagsFileName(const TString in) {fROCFlagFileName = in;};



 private:
  
  void SetArgs(Int_t argc, Char_t* argv[]);
  void SetName(const TString name);
  void Add(TString in);
  
  TString GetROCFlags();
  
  TList *fRunConditionList;
 
  TString fROCFlagFileName;
  static const Int_t fCharLength;
  
  // Do not let compiler generate these automatically
  QwRunCondition(const QwRunCondition& rhs); // copy constructor
  QwRunCondition& operator = (const QwRunCondition& rhs); // assignment operator
  QwRunCondition* operator&(); // address-of operator
  const QwRunCondition* operator&() const; // address-of operator

};


// How to access fRunConditionList
// TList *list = (TList*) file->FindObjectAny(fRunConditoinList name);
// for example,
// 
// TFile *file = TFile::Open("Qweak_5150.root")
// TList *list = (TList*) file->FindObjectAny("5150_condition");
// if(list)  {
//    std::cout << list->Print() << std::endl;
//    std::cout << list->GetEntries() << std::endl;
//    std::cout << list->GetSize() << std::endl;
//    std::cout << list->GetName() << std::endl;
//    for(Int_t i=0; i < list->GetSize(); i++) {
//         std::cout << list->At(i)->GetName() << std::endl;
//    }
//  }

#endif
