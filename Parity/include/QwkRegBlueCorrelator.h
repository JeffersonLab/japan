#ifndef JBCORRELATOR_H
#define JBCORRELATOR_H
/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Finds correlation and alphas for  QW event 
 *********************************************************************/

#include <assert.h>
#include <TString.h> 
class TH1;

#include "LinReg_Bevington_Pebay.h"

class QwkRegBlueCorrelator  {
 private:

  TString mCore;
  int nP,nY;
  
  // histograms
  enum {mxHA=4}; TH1 * hA[mxHA];

  // monitoring histos for iv & dv
  TH1 ** h1iv, **h2iv, ** h1dv, **h2dv;
  void initHistos(std::vector < TString > ivName, std::vector < TString > dvName);
  LinRegBevPeb linReg;
  
  bool fDisableHistos;

 public:
  QwkRegBlueCorrelator(const char *core); 
  ~QwkRegBlueCorrelator();
  void print();
  void init(std::vector < TString > ivName, std::vector < TString > dvName);
  void finish();
  void addEvent(double *Pvec, double *Yvec);
  void exportAlphas(TString outPath, std::vector < TString > ivName, std::vector < TString > dvName);
  void exportAlias(TString outPath, TString macroName, std::vector < TString > ivName, std::vector < TString > dvName);

  void SetDisableHistogramFlag(const bool &flag) {
    fDisableHistos = flag;
  }

};

#endif
