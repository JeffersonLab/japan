///////////////////////////////////////////////////////////////////
//  Macro to help with online analysis
//    B. Moffit  Oct. 2003
#ifndef panguinOnline_h
#define panguinOnline_h 1

#include <TTree.h>
#include <TFile.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include "TGLabel.h"
#include "TGString.h"
#include <RQ_OBJECT.h>
#include <TQObject.h>
#include <vector>
#include <TString.h>
#include <TCut.h>
#include <TTimer.h>
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "panguinOnlineConfig.hh"

#define UPDATETIME 10000

class OnlineGUI {
  // Class that takes care of the GUI
  RQ_OBJECT("OnlineGUI")
private:
  TGMainFrame                      *fMain;
  TGHorizontalFrame                *fTopframe;
  TGVerticalFrame                  *vframe;
  TGRadioButton                    *fRadioPage[50];
  TGPictureButton                  *wile;
  TGTextButton                     *fNow; // current time
  TGTextButton                     *fLastUpdated; // plots last updated
  TGTextButton                     *fRootFileLastUpdated; // Root file last updated
  TRootEmbeddedCanvas              *fEcanvas;
  TGHorizontalFrame                *fBottomFrame;
  TGHorizontalFrame                *hframe;
  TGTextButton                     *fNext;
  TGTextButton                     *fPrev;
  TGTextButton                     *fExit;
  TGLabel                          *fRunNumber;
  TGTextButton                     *fPrint;
  TCanvas                          *fCanvas; // Present Embedded canvas
  OnlineConfig                     *fConfig;
  UInt_t                            current_page;
  TFile*                            fRootFile;
  TFile*                            fGoldenFile;
  Bool_t                            doGolden;
  std::vector <TTree*>                   fRootTree;
  std::vector <Int_t>                    fTreeEntries;
  std::vector < std::pair <TString,TString> > fileObjects;
  std::vector < std::vector <TString> >       treeVars;
  UInt_t                            runNumber;
  TTimer                           *timer;
  TTimer                           *timerNow; // used to update time
  Bool_t                            fUpdate;
  Bool_t                            fFileAlive;
  Bool_t                            fPrintOnly;
  TH1D                             *mytemp1d;
  TH2D                             *mytemp2d;
  TH3D                             *mytemp3d;
  TH1D                             *mytemp1d_golden;
  TH2D                             *mytemp2d_golden;
  TH3D                             *mytemp3d_golden;

  int fVerbosity;

public:
  OnlineGUI(OnlineConfig&, Bool_t,int);
  void CreateGUI(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~OnlineGUI();
  void DoDraw();
  void DrawPrev();
  void DrawNext();
  void DoRadio();
  void CheckPageButtons();
  // Specific Draw Methods
  Bool_t IsHistogram(TString);
  void GetFileObjects();
  void GetTreeVars();
  void GetRootTree();
  UInt_t GetTreeIndex(TString);
  UInt_t GetTreeIndexFromName(TString);
  void TreeDraw(std::vector <TString>); 
  void HistDraw(std::vector <TString>);
  void MacroDraw(std::vector <TString>);
  void LoadDraw(std::vector <TString>);
  void LoadLib(std::vector <TString>);
  void DoDrawClear();
  void TimerUpdate();
  void UpdateCurrentTime();  // update current time
  void BadDraw(TString);
  void CheckRootFile();
  Int_t OpenRootFile();
  void PrintToFile();
  void PrintPages();
  void MyCloseWindow();
  void CloseGUI();
  void SetVerbosity(int ver){fVerbosity=ver;}
  ClassDef(OnlineGUI,0);
};
#endif //panguinOnline_h
