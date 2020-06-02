/********************************************************************
 *                                                                  *
 * Author:  Cameron Clarke                                          *
 * Date:    3-28-2019                                               *
 * Title:   Continuous Aggregate Monitor Graphical User INterfacea  *
 * Purpose: A set of methods useful for reading a root tree, doing  *
 *          some analysis, and printing those results into an       *
 *          updatable ROOT file.                                    *
 *          This is a fully functioning macro whose methods stand   *
 *          alone in PANGUIN and in ROOT in general as well, and it *
 *          represents several possible ways of accessing and       *
 *          manipulating ROOT output.                               *
 *                                                                  *
 *******************************************************************/
#ifndef __CAMGUIN__
#define __CAMGUIN__
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TChain.h>
#include <TLeaf.h>
#include <TH1.h>
#include <TFile.h>
#include <vector>
#include <TString.h>
#include <algorithm>
#include <iostream>
#include <TMatrix.h>
#include <TMatrixD.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TTreeReader.h>
#include <cmath>
using namespace std;

Int_t debug = -1;
Int_t alarmStatus = -1;
Int_t aggregatorStatus = -1;
Int_t postpanStatus = -1;

struct alarm_data {
  std::string type[5] = {"NULL","NULL","NULL","NULL","NULL"};
  char delim = ',';
  Int_t indexStart   = 0;
  Int_t indexEnd     = 0;
  Int_t column       = 0;
  Int_t columnIndex  = 0;
  std::string name   = "NULL";
  std::string value  = "NULL";
  Int_t color        = 0x0;
  Int_t alarm_status = 0;

  Int_t addEntry     = 0;
  Int_t editEntry    = 0;
  Int_t deleteEntry  = 0;
  Int_t changeIndex  = 0;
};

alarm_data fAlarmData;

#endif // __CAMGUIN__
