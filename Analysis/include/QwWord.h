/*
 * QwWord.h
 *
 *  Created on: Dec 14, 2010
 *      Author: wdconinc
 */

#ifndef QWWORD_H_
#define QWWORD_H_

// ROOT headers
#include "Rtypes.h"

// Qweak headers
#include "QwLog.h"

///
/// \ingroup QwAnalysis
class QwWord
{
  public:
    QwWord()
    : fSubbankIndex(-1),fWordInSubbank(-1),fModuleType(""),
      fWordName(""),fWordType(""),fValue(-1){};

    Int_t fSubbankIndex;
    Int_t fWordInSubbank;
    TString fModuleType;
    TString fWordName;
    TString fWordType;
    Int_t fValue;
    
    Double_t GetValue() const       {return this->GetValue(0);};
    Double_t GetValue(size_t element) const      { return fValue; };
    TString GetModType() const {return fModuleType;};
  



  
    void PrintID() const {
      QwOut << fWordName << " :: " << fSubbankIndex << " ; "
            << fModuleType << " ; " << fWordType << QwLog::endl;
    }

    void Print() const {
      QwOut << fWordName<<" :: " << fValue << QwLog::endl;
    }

    void ClearEventData() {
      fValue = -1;
    }

    TString GetWordName() const { return fWordName; };
   

};



#endif /* QWWORD_H_ */
