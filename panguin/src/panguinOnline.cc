///////////////////////////////////////////////////////////////////
//  Macro to help with online analysis
//    B. Moffit  Oct. 2003
///////////////////////////////////////////////////////////////////

#include "panguinOnline.hh"
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <TMath.h>
#include <TBranch.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TGImageMap.h>
#include <TGFileDialog.h>
#include <TKey.h>
#include <TSystem.h>
#include <TLatex.h>
#include <TText.h>
#include "TPaveText.h"
#include <TApplication.h>
#include "TEnv.h"
#include "TRegexp.h"
#include "TGraph.h"

#define OLDTIMERUPDATE

using namespace std;

///////////////////////////////////////////////////////////////////
//  Class: OnlineGUI
//
//    Creates a GUI to display the commands used in OnlineConfig
//
//

OnlineGUI::OnlineGUI(OnlineConfig& config, Bool_t printonly=0, int ver=0):
  runNumber(0),
  timer(0), 
  timerNow(0),
  fFileAlive(kFALSE),
  fVerbosity(ver)
{
  // Constructor.  Get the config pointer, and make the GUI.

  fConfig = &config;
  int bin2Dx(0), bin2Dy(0);
  fConfig->Get2DnumberBins(bin2Dx,bin2Dy);    
  if(bin2Dx>0 && bin2Dy>0){
    gEnv->SetValue("Hist.Binning.2D.x",bin2Dx);
    gEnv->SetValue("Hist.Binning.2D.y",bin2Dy);
    if(fVerbosity>1){
      cout<<"Set 2D default bins to x, y: "<<bin2Dx<<", "<<bin2Dy<<endl;
    }
  }

  if(printonly) {
    fPrintOnly=kTRUE;
    PrintPages();
  } else {
    fPrintOnly=kFALSE;
    CreateGUI(gClient->GetRoot(),800,600);
  }
}

void OnlineGUI::CreateGUI(const TGWindow *p, UInt_t w, UInt_t h) 
{
  
  // Open the RootFile.  Die if it doesn't exist.
  //  unless we're watching a file.
  fRootFile = new TFile(fConfig->GetRootFile(),"READ");
  if(!fRootFile->IsOpen()) {
    cout << "ERROR:  rootfile: " << fConfig->GetRootFile()
	 << " does not exist"
	 << endl;
    if(fConfig->IsMonitor()) {
      cout << "Will wait... hopefully.." << endl;
    } else {
      gApplication->Terminate();
    }
  } else {
    fFileAlive = kTRUE;
    runNumber = fConfig->GetRunNumber();
    // Open the Root Trees.  Give a warning if it's not there..
    GetFileObjects();
    GetRootTree();
    GetTreeVars();
    for(UInt_t i=0; i<fRootTree.size(); i++) {
      if(fRootTree[i]==0) {
	fRootTree.erase(fRootTree.begin() + i);
      }
    }

  }
  TString goldenfilename=fConfig->GetGoldenFile();
  if(!goldenfilename.IsNull()) {
    fGoldenFile = new TFile(goldenfilename,"READ");
    if(!fGoldenFile->IsOpen()) {
      cout << "ERROR: goldenrootfile: " << goldenfilename
	   << " does not exist.  Oh well, no comparison plots."
	   << endl;
      doGolden = kFALSE;
      fGoldenFile=NULL;
    } else {
      doGolden = kTRUE;
    }
  } else {
    doGolden=kFALSE;
    fGoldenFile=NULL;
  }


  // Create the main frame
  fMain = new TGMainFrame(p,w,h);
  fMain->Connect("CloseWindow()", "OnlineGUI", this, "MyCloseWindow()");
  ULong_t lightgreen, lightblue, red, mainguicolor;
  gClient->GetColorByName("lightgreen",lightgreen);
  gClient->GetColorByName("lightblue",lightblue);
  gClient->GetColorByName("red",red);

  Bool_t good_color=kFALSE; 
  TString usercolor = fConfig->GetGuiColor();
  if(!usercolor.IsNull()) {
    good_color = gClient->GetColorByName(usercolor,mainguicolor);
  }

  if(!good_color) {
    if(!usercolor.IsNull()) {
      cout << "Bad guicolor (" << usercolor << ").. using default." << endl;
    }
    if(fConfig->IsMonitor()) {
      // Default background color for Online Monitor
      mainguicolor = lightgreen;
    } else {
      // Default background color for Normal Online Display
      mainguicolor = lightblue;
    }
  }

  fMain->SetBackgroundColor(mainguicolor);

  // Top frame, to hold page buttons and canvas
  fTopframe = new TGHorizontalFrame(fMain,w,UInt_t(h*0.9));
  fTopframe->SetBackgroundColor(mainguicolor);
  fMain->AddFrame(fTopframe, new TGLayoutHints(kLHintsExpandX 
					       | kLHintsExpandY,10,10,10,1));

  // Create a verticle frame widget with radio buttons
  //  This will hold the page buttons
  vframe = new TGVerticalFrame(fTopframe,UInt_t(w*0.3),UInt_t(h*0.9));
  vframe->SetBackgroundColor(mainguicolor);
  TString buff;
  for(UInt_t i=0; i<fConfig->GetPageCount(); i++) {
    buff = fConfig->GetPageTitle(i);
    fRadioPage[i] = new TGRadioButton(vframe,buff,i);
    fRadioPage[i]->SetBackgroundColor(mainguicolor);
  }

  fRadioPage[0]->SetState(kButtonDown);
  current_page = 0;

  for (UInt_t i=0; i<fConfig->GetPageCount(); i++) {
    vframe->AddFrame(fRadioPage[i], new TGLayoutHints(kLHintsLeft |
						      kLHintsCenterY,5,5,3,4));
    fRadioPage[i]->Connect("Pressed()", "OnlineGUI", this, "DoRadio()");
  }

  // heartbeat below the picture, watchfile only
  if(fConfig->IsMonitor()){
    fRootFileLastUpdated = new TGTextButton(vframe, "File updated at: XX:XX:XX");
    // fRootFileLastUpdated->SetWidth(156);
    vframe->AddFrame(fRootFileLastUpdated, new TGLayoutHints(kLHintsBottom | kLHintsRight, 5, 5, 3, 4));

    fLastUpdated = new TGTextButton(vframe, "Plots updated at: XX:XX:XX");
    // fLastUpdated->SetWidth(156);
    vframe->AddFrame(fLastUpdated, new TGLayoutHints(kLHintsBottom | kLHintsRight, 5, 5, 3, 4));

    fNow = new TGTextButton(vframe, "Current time: XX:XX:XX");
    // fNow->SetWidth(156);
    vframe->AddFrame(fNow, new TGLayoutHints(kLHintsBottom | kLHintsRight, 5, 5, 3, 4));
  }

  if(!fConfig->IsMonitor()) {
    wile = 
      new TGPictureButton(vframe,gClient->GetPicture(guiDirectory+"/genius.xpm"));
    wile->Connect("Pressed()","OnlineGUI", this,"DoDraw()");
  } else {
    wile = 
      new TGPictureButton(vframe,gClient->GetPicture(guiDirectory+"/panguin.xpm"));
    wile->Connect("Pressed()","OnlineGUI", this,"DoDrawClear()");
  }
  wile->SetBackgroundColor(mainguicolor);

  vframe->AddFrame(wile,new TGLayoutHints(kLHintsBottom|kLHintsLeft,5,10,4,2));


  fTopframe->AddFrame(vframe,new TGLayoutHints(kLHintsLeft|
                                               kLHintsCenterY,2,2,2,2));
  
  // Create canvas widget
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas", fTopframe, UInt_t(w*0.7), UInt_t(h*0.9));
  fEcanvas->SetBackgroundColor(mainguicolor);
  fTopframe->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY
						  ,10,10,10,1));
  fCanvas = fEcanvas->GetCanvas();

  // Create the bottom frame.  Contains control buttons
  fBottomFrame = new TGHorizontalFrame(fMain,w,UInt_t(h*0.1));
  fBottomFrame->SetBackgroundColor(mainguicolor);
  fMain->AddFrame(fBottomFrame, new TGLayoutHints(kLHintsExpandX,10,10,10,10));
  
  // Create a horizontal frame widget with buttons
  hframe = new TGHorizontalFrame(fBottomFrame,1200,40);
  hframe->SetBackgroundColor(mainguicolor);
  //fBottomFrame->AddFrame(hframe,new TGLayoutHints(kLHintsExpandX,200,20,2,2));
  fBottomFrame->AddFrame(hframe,new TGLayoutHints(kLHintsExpandX,2,2,2,2));

  fPrev = new TGTextButton(hframe,"Prev");
  fPrev->SetBackgroundColor(mainguicolor);
  fPrev->Connect("Clicked()","OnlineGUI",this,"DrawPrev()");
  hframe->AddFrame(fPrev, new TGLayoutHints(kLHintsCenterX,5,5,1,1));

  fNext = new TGTextButton(hframe,"Next");
  fNext->SetBackgroundColor(mainguicolor);
  fNext->Connect("Clicked()","OnlineGUI",this,"DrawNext()");
  hframe->AddFrame(fNext, new TGLayoutHints(kLHintsCenterX,5,5,1,1));

  fExit = new TGTextButton(hframe,"Exit GUI");
  fExit->SetBackgroundColor(red);
  fExit->Connect("Clicked()","OnlineGUI",this,"CloseGUI()");

  hframe->AddFrame(fExit, new TGLayoutHints(kLHintsCenterX,5,5,1,1));
  
  TString Buff;
  if(runNumber==0) {
    Buff = "";
  } else {
    Buff = "Run #";
    Buff += runNumber;
  }
  TGString labelBuff(Buff);
  
  fRunNumber = new TGLabel(hframe,Buff);
  fRunNumber->SetBackgroundColor(mainguicolor);
  hframe->AddFrame(fRunNumber,new TGLayoutHints(kLHintsCenterX,5,5,1,1));

  fPrint = new TGTextButton(hframe,"Print To &File");
  fPrint->SetBackgroundColor(mainguicolor);
  fPrint->Connect("Clicked()","OnlineGUI",this,"PrintToFile()");
  hframe->AddFrame(fPrint, new TGLayoutHints(kLHintsCenterX,5,5,1,1));


  // Set a name to the main frame
  if(fConfig->IsMonitor()) {
    fMain->SetWindowName("Parity ANalysis GUI moNitor");
  } else {
    fMain->SetWindowName("Online Analysis GUI");
  }

  // Map all sub windows to main frame
  fMain->MapSubwindows();
  
  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());
  
  // Map main frame
  fMain->MapWindow();

  if(fVerbosity>=1)
    fMain->Print();

  if(fFileAlive) DoDraw();

  if(fConfig->IsMonitor()) {
    timerNow = new TTimer();
    timerNow->Connect(timerNow, "Timeout()", "OnlineGUI", this, "UpdateCurrentTime()");
    timerNow->Start(1000);  // update every second
  }

  if(fConfig->IsMonitor()) {
    timer = new TTimer();
    if(fFileAlive) {
      timer->Connect(timer,"Timeout()","OnlineGUI",this,"TimerUpdate()");
    } else {
      timer->Connect(timer,"Timeout()","OnlineGUI",this,"CheckRootFile()");
    }
    timer->Start(UPDATETIME);
  }

}

void OnlineGUI::DoDraw() 
{
  // The main Drawing Routine.

  gStyle->SetOptStat(1110);
  gStyle->SetStatFontSize(0.1);
  if (fConfig->IsLogy(current_page)) {
    gStyle->SetOptLogy(1);
  } else {
    gStyle->SetOptLogy(0);
  }
  //   gStyle->SetTitleH(0.10);
  //   gStyle->SetTitleW(0.40);
  gStyle->SetTitleH(0.10);
  gStyle->SetTitleW(0.60);
  gStyle->SetStatH(0.70);
  gStyle->SetStatW(0.35);
  //   gStyle->SetLabelSize(0.10,"X");
  //   gStyle->SetLabelSize(0.10,"Y");
  gStyle->SetLabelSize(0.05,"X");
  gStyle->SetLabelSize(0.05,"Y");
  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetNdivisions(505,"X");
  gStyle->SetNdivisions(404,"Y");
  gROOT->ForceStyle();

  // Determine the dimensions of the canvas..
  UInt_t draw_count = fConfig->GetDrawCount(current_page);
  if(draw_count>=8) {
    gStyle->SetLabelSize(0.08,"X");
    gStyle->SetLabelSize(0.08,"Y");
  }
  //   Int_t dim = Int_t(round(sqrt(double(draw_count))));
  pair <UInt_t,UInt_t> dim = fConfig->GetPageDim(current_page);

  if(fVerbosity>=1)
    cout << "Dimensions: " << dim.first << "X" 
	 << dim.second << endl;

  // Create a nice clean canvas.
  fCanvas->Clear();
  fCanvas->Divide(dim.first,dim.second);

  vector <TString> drawcommand(5);
  // Draw the histograms.
  for(UInt_t i=0; i<draw_count; i++) {    
    drawcommand = fConfig->GetDrawCommand(current_page,i);
    fCanvas->cd(i+1);
    if (drawcommand[0] == "macro") {
      MacroDraw(drawcommand);
    } else if (drawcommand[0] == "loadmacro") {
      LoadDraw(drawcommand);
    } else if (drawcommand[0] == "loadlib") {
      LoadLib(drawcommand);
    } else if (IsHistogram(drawcommand[0])) {
      HistDraw(drawcommand);
    } else {
      TreeDraw(drawcommand);
    }
  }
      
  fCanvas->cd();
  fCanvas->Update();

  if(fConfig->IsMonitor()) {
    char buffer[9]; // HH:MM:SS
    time_t t = time(0);
    TString sLastUpdated("Plots updated at: ");
    strftime(buffer, 9, "%T", localtime(&t));
    sLastUpdated += buffer;
    fLastUpdated->SetText(sLastUpdated);

    struct stat result;
    stat(fConfig->GetRootFile().Data(), &result);
    time_t tf = result.st_mtime;
    strftime(buffer, 9, "%T", localtime(&tf));

    TString sRootFileLastUpdated("File updated at: ");
    sRootFileLastUpdated += buffer;
    fRootFileLastUpdated->SetText(sRootFileLastUpdated);
    ULong_t backgroundColor = fLastUpdated->GetBackground();
    if(fVerbosity>=4)
      cout<<"Updating plots (current, file, diff[s]):\t"<<t<<"\t"<<tf<<"\t"<<t - tf<<endl;
    if( t - tf > 60 ){
      ULong_t red;
      gClient->GetColorByName("red",red);
      fRootFileLastUpdated->SetBackgroundColor(red);
    }
    else{
      fRootFileLastUpdated->SetBackgroundColor(backgroundColor);
    }
  }

  if(!fPrintOnly) {
    CheckPageButtons();
  }

}

void OnlineGUI::DrawNext()
{
  // Handler for the "Next" button.
  fRadioPage[current_page]->SetState(kButtonUp);
  // The following line triggers DoRadio(), or at least.. used to
  fRadioPage[current_page+1]->SetState(kButtonDown);
  current_page++;
  DoDraw();
}

void OnlineGUI::DrawPrev()
{
  // Handler for the "Prev" button.
  fRadioPage[current_page]->SetState(kButtonUp);
  // The following line triggers DoRadio(), or at least.. used to
  fRadioPage[current_page-1]->SetState(kButtonDown);
  current_page--;
  DoDraw();  
}

void OnlineGUI::DoRadio()
{
  // Handle the radio buttons
  //  Find out which button has been pressed..
  //   turn off the previous button...
  //   then draw the appropriate page.
  // This routine also handles the Draw from the Prev/Next buttons
  //   - makes a call to DoDraw()

  UInt_t pagecount = fConfig->GetPageCount();
  TGButton *btn = (TGButton *) gTQSender;
  UInt_t id = btn->WidgetId();
  
  if (id <= pagecount) {  
    fRadioPage[current_page]->SetState(kButtonUp);
  }

  current_page = id;
  DoDraw();
}

void OnlineGUI::CheckPageButtons() 
{
  // Checks the current page to see if it's the first or last page.
  //  If so... turn off the appropriate button.
  //  If not.. turn on both buttons.

  if(current_page==0) {
    fPrev->SetState(kButtonDisabled);
    if(fConfig->GetPageCount()!=1)
      fNext->SetState(kButtonUp);
  } else if(current_page==fConfig->GetPageCount()-1) {
    fNext->SetState(kButtonDisabled);
    if(fConfig->GetPageCount()!=1)
      fPrev->SetState(kButtonUp);
  } else {
    fPrev->SetState(kButtonUp);
    fNext->SetState(kButtonUp);
  }
}

Bool_t OnlineGUI::IsHistogram(TString objectname) 
{
  // Utility to determine if the objectname provided is a histogram

  for(UInt_t i=0; i<fileObjects.size(); i++) {
    if (fileObjects[i].first.Contains(objectname)) {
      if(fVerbosity>=2)
	cout << fileObjects[i].first << "      "
	     << fileObjects[i].second << endl;

      if(fileObjects[i].second.Contains("TH"))
	return kTRUE;
    }
  }

  return kFALSE;

}

void OnlineGUI::GetFileObjects() 
{
  // Utility to find all of the objects within a File (TTree, TH1F, etc).
  //  The pair stored in the vector is <ObjName, ObjType>
  //  If there's no good keys.. do nothing.
  if(fVerbosity>=1)
    cout << "Keys = " << fRootFile->ReadKeys() << endl;

  if(fRootFile->ReadKeys()==0) {
    fUpdate = kFALSE;
    //     delete fRootFile;
    //     fRootFile = 0;
    //     CheckRootFile();
    return;
  }
  fileObjects.clear();
  TIter next(fRootFile->GetListOfKeys());
  TKey *key = new TKey();

  // Do the search
  while((key=(TKey*)next())!=0) {
    if(fVerbosity>=1)
      cout << "Key = " << key << endl;    

    TString objname = key->GetName();
    TString objtype = key->GetClassName();

    if(fVerbosity>=1)
      cout << objname << " " << objtype << endl;

    fileObjects.push_back(make_pair(objname,objtype));
  }
  fUpdate = kTRUE;
  delete key;
}

void OnlineGUI::GetTreeVars() 
{
  // Utility to find all of the variables (leaf's/branches) within a
  // Specified TTree and put them within the treeVars vector.
  treeVars.clear();
  TObjArray *branchList;
  vector <TString> currentTree;

  for(UInt_t i=0; i<fRootTree.size(); i++) {
    currentTree.clear();
    branchList = fRootTree[i]->GetListOfBranches();
    TIter next(branchList);
    TBranch *brc;

    while((brc=(TBranch*)next())!=0) {
      TString found = brc->GetName();
      // Not sure if the line below is so smart...
      currentTree.push_back(found);
    }
    treeVars.push_back(currentTree);
  }

  if(fVerbosity>=5){
    for(UInt_t iTree=0; iTree<treeVars.size(); iTree++) {
      cout << "In Tree " << iTree << ": " << endl;
      for(UInt_t i=0; i<treeVars[iTree].size(); i++) {
	cout << treeVars[iTree][i] << endl;
      }
    }
  }
}


void OnlineGUI::GetRootTree() {
  // Utility to search a ROOT File for ROOT Trees
  // Fills the fRootTree vector
  fRootTree.clear();

  list <TString> found;
  for(UInt_t i=0; i<fileObjects.size(); i++) {
    
    if(fVerbosity>=2)
      cout << "Object = " << fileObjects[i].second <<
	"     Name = " << fileObjects[i].first << endl;

    if(fileObjects[i].second.Contains("TTree"))
      found.push_back(fileObjects[i].first);
  }

  // Remove duplicates, then insert into fRootTree
  found.unique();
  UInt_t nTrees = found.size();

  for(UInt_t i=0; i<nTrees; i++) {
    fRootTree.push_back((TTree*)fRootFile->Get(found.front()));
    found.pop_front();
  }  
  // Initialize the fTreeEntries vector
  fTreeEntries.clear();
  for(UInt_t i=0;i<fRootTree.size();i++) {
    fTreeEntries.push_back(0);
  }
  
}

UInt_t OnlineGUI::GetTreeIndex(TString var) {
  // Utility to find out which Tree (in fRootTree) has the specified
  // variable "var".  If the variable is a collection of Tree
  // variables (e.g. bcm1:lumi1), will only check the first
  // (e.g. bcm1).  
  // Returns the correct index.  if not found returns an index 1
  // larger than fRootTree.size()

  //  This is for 2d draws... look for the first only
  if(var.Contains(":")) {
    TString first_var = fConfig->SplitString(var,":")[0];
    var = first_var;
  }
  if(var.Contains("-")) {
    TString first_var = fConfig->SplitString(var,"-")[0];
    var = first_var;
  }
  if(var.Contains("/")) {
    TString first_var = fConfig->SplitString(var,"/")[0];
    var = first_var;
  }
  if(var.Contains("*")) {
    TString first_var = fConfig->SplitString(var,"*")[0];
    var = first_var;
  }
  if(var.Contains("+")) {
    TString first_var = fConfig->SplitString(var,"+")[0];
    var = first_var;
  }
  if(var.Contains("(")) {
    TString first_var = fConfig->SplitString(var,"(")[0];
    var = first_var;
  }
  //  This is for variables with multiple dimensions.
  if(var.Contains("[")) {
    TString first_var = fConfig->SplitString(var,"[")[0];
    var = first_var;
  }

  if(fVerbosity>=3)
    cout<<__PRETTY_FUNCTION__<<"\t"<<__LINE__<<endl
	<<"\t looking for variable: "<<var<<endl;
  for(UInt_t iTree=0; iTree<treeVars.size(); iTree++) {
    for(UInt_t ivar=0; ivar<treeVars[iTree].size(); ivar++) {
      if(fVerbosity>=4)
	cout<<"Checking tree "<<iTree<<" name:"<<fRootTree[iTree]->GetName()
	    <<" \t var "<<ivar<<" >> "<<treeVars[iTree][ivar]<<endl;
      if(var == treeVars[iTree][ivar]) return iTree;
    }
  }

  return fRootTree.size()+1;
}

UInt_t OnlineGUI::GetTreeIndexFromName(TString name) {
  // Called by TreeDraw().  Tries to find the Tree index provided the
  //  name.  If it doesn't match up, return a number that's one larger
  //  than the number of found trees.
  for(UInt_t iTree=0; iTree<fRootTree.size(); iTree++) {
    TString treename = fRootTree[iTree]->GetName();
    if(name == treename) {
      return iTree;
    }
  }

  return fRootTree.size()+1;
}

void OnlineGUI::MacroDraw(vector <TString> command) {
  // Called by DoDraw(), this will make a call to the defined macro, and
  //  plot it in it's own pad.  One plot per macro, please.

  if(command[1].IsNull()) {
    cout << "macro command doesn't contain a macro to execute" << endl;
    return;
  }

  if(doGolden) fRootFile->cd();
  gROOT->Macro(command[1]);
  

}

void OnlineGUI::LoadDraw(vector <TString> command) {
  // Called by DoDraw(), this will load a shared object library 
  // and then make a call to the defined macro, and
  // plot it in it's own pad.  One plot per macro, please.

  if(command[1].IsNull()) {
    cout << "load command doesn't contain a command to execute" << endl;
    return;
  }

  if(doGolden) fRootFile->cd();
  gSystem->Load(command[1]);
  gROOT->Macro(command[2]);
  

}

void OnlineGUI::LoadLib(vector <TString> command) {
  // Called by DoDraw(), this will load a shared object library

  if(command[1].IsNull()) {
    cout << "load command doesn't contain a shared object library path" << endl;
    return;
  }

  if(doGolden) fRootFile->cd();
  gSystem->Load(command[1]);


}

void OnlineGUI::DoDrawClear() {
  // Utility to grab the number of entries in each tree.  This info is
  // then used, if watching a file, to "clear" the TreeDraw
  // histograms, and begin looking at new data.
  for(UInt_t i=0; i<fTreeEntries.size(); i++) {
    fTreeEntries[i] = (Int_t) fRootTree[i]->GetEntries();
  }
  

}

void OnlineGUI::TimerUpdate() {
  // Called periodically by the timer, if "watchfile" is indicated
  // in the config.  Reloads the ROOT file, and updates the current page.
  if(fVerbosity>=1)
    cout<<__PRETTY_FUNCTION__<<"\t"<<__LINE__<<endl;

#ifdef OLDTIMERUPDATE
  if(fVerbosity>=2)
    cout<<"\t rtFile: "<<fRootFile<<"\t"<<fConfig->GetRootFile()<<endl;
  if(fRootFile){
    fRootFile->Close();
    fRootFile->Delete();
    delete fRootFile;
    fRootFile=0;
  }
  fRootFile = new TFile(fConfig->GetRootFile(),"READ");
  if(fRootFile->IsZombie()) {
    cout << "New run not yet available.  Waiting..." << endl;
    fRootFile->Close();
    delete fRootFile;
    fRootFile = 0;
    timer->Reset();
    timer->Disconnect();
    timer->Connect(timer,"Timeout()","OnlineGUI",this,"CheckRootFile()");
    return;
  }

  // Update the runnumber
  runNumber = fConfig -> GetRunNumber();
  if(runNumber != 0) {
    TString rnBuff = "Run #";
    rnBuff += runNumber;
    fRunNumber->SetText(rnBuff.Data());
    hframe->Layout();
  }

  // Open the Root Trees.  Give a warning if it's not there..
  GetFileObjects();
  if (fUpdate) { // Only do this stuff if their are valid keys
    GetRootTree();
    GetTreeVars();
    for(UInt_t i=0; i<fRootTree.size(); i++) {
      if(fRootTree[i]==0) {
	fRootTree.erase(fRootTree.begin() + i);
      }
    }
    DoDraw();
  }
  timer->Reset();

#else

  if(fRootFile->IsZombie() || (fRootFile->GetSize() == -1)
     || (fRootFile->ReadKeys()==0)) {
    cout << "New run not yet available.  Waiting..." << endl;
    fRootFile->Close();
    delete fRootFile;
    fRootFile = 0;
    timer->Reset();
    timer->Disconnect();
    timer->Connect(timer,"Timeout()","OnlineGUI",this,"CheckRootFile()");
    return;
  }
  for(UInt_t i=0; i<fRootTree.size(); i++) {
    fRootTree[i]->Refresh();
  }
  DoDraw();
  timer->Reset();

#endif

}

void OnlineGUI::UpdateCurrentTime() {
  char buffer[9];
  time_t t = time(0);
  strftime(buffer, 9, "%T", localtime(&t));
  TString sNow("Current time: ");
  sNow += buffer;
  fNow->SetText(sNow); 
  timerNow->Reset();
}

void OnlineGUI::BadDraw(TString errMessage) {
  // Routine to display (in Pad) why a particular draw method has
  // failed.
  TPaveText *pt = new TPaveText(0.1,0.1,0.9,0.9,"brNDC");
  pt->SetBorderSize(3);
  pt->SetFillColor(10);
  pt->SetTextAlign(22);
  pt->SetTextFont(72);
  pt->SetTextColor(2);
  pt->AddText(errMessage.Data());
  pt->Draw();
  //   cout << errMessage << endl;

}


void OnlineGUI::CheckRootFile() {
  // Check the path to the rootfile (should follow symbolic links)
  // ... If found:
  //   Reopen new root file, 
  //   Reconnect the timer to TimerUpdate()

  if(gSystem->AccessPathName(fConfig->GetRootFile())==0) {
    cout << "Found the new run" << endl;
#ifndef OLDTIMERUPDATE
    if(OpenRootFile()==0) {
#endif
      timer->Reset();
      timer->Disconnect();
      timer->Connect(timer,"Timeout()","OnlineGUI",this,"TimerUpdate()");
#ifndef OLDTIMERUPDATE
    }
#endif
  } else {
    TString rnBuff = "Waiting for run";
    fRunNumber->SetText(rnBuff.Data());
    hframe->Layout();
  }

}

Int_t OnlineGUI::OpenRootFile() {


  fRootFile = new TFile(fConfig->GetRootFile(),"READ");
  if(fRootFile->IsZombie() || (fRootFile->GetSize() == -1)
     || (fRootFile->ReadKeys()==0)) {
    cout << "New run not yet available.  Waiting..." << endl;
    fRootFile->Close();
    delete fRootFile;
    fRootFile = 0;
    timer->Reset();
    timer->Disconnect();
    timer->Connect(timer,"Timeout()","OnlineGUI",this,"CheckRootFile()");
    return -1;
  }

  // Update the runnumber
  runNumber = fConfig->GetRunNumber();
  if(runNumber != 0) {
    TString rnBuff = "Run #";
    rnBuff += runNumber;
    fRunNumber->SetText(rnBuff.Data());
    hframe->Layout();
  }

  // Open the Root Trees.  Give a warning if it's not there..
  GetFileObjects();
  if (fUpdate) { // Only do this stuff if their are valid keys
    GetRootTree();
    GetTreeVars();
    for(UInt_t i=0; i<fRootTree.size(); i++) {
      if(fRootTree[i]==0) {
	fRootTree.erase(fRootTree.begin() + i);
      }
    }
    DoDraw();
  } else {
    return -1;
  }
  return 0;

}

void OnlineGUI::HistDraw(vector <TString> command) {
  // Called by DoDraw(), this will plot a histogram.

  Bool_t showGolden=kFALSE;
  if(doGolden) showGolden=kTRUE;

  if(command.size()==2)
    if(command[1]=="noshowgolden") {
      showGolden = kFALSE;
    }
  cout<<"showGolden= "<<showGolden<<endl;

  // Determine dimensionality of histogram
  for(UInt_t i=0; i<fileObjects.size(); i++) {
    if (fileObjects[i].first.Contains(command[0])) {
      if(fileObjects[i].second.Contains("TH1")) {
	if(showGolden) fRootFile->cd();
	mytemp1d = (TH1D*)gDirectory->Get(command[0]);
	if(mytemp1d->GetEntries()==0) {
	  BadDraw("Empty Histogram");
	} else {
	  if(showGolden) {
	    fGoldenFile->cd();
	    mytemp1d_golden = (TH1D*)gDirectory->Get(command[0]);
	    mytemp1d_golden->SetLineColor(30);
	    mytemp1d_golden->SetFillColor(30);
	    Int_t fillstyle=3027;
	    if(fPrintOnly) fillstyle=3010;
	    mytemp1d_golden->SetFillStyle(fillstyle);
	    mytemp1d_golden->Draw();
	    cout<<"one golden histo drawn"<<endl;
	    mytemp1d->Draw("sames");
	  } else {
	    mytemp1d->Draw();
	  }
	}
	break;
      }
      if(fileObjects[i].second.Contains("TH2")) {
	if(showGolden) fRootFile->cd();
	mytemp2d = (TH2D*)gDirectory->Get(command[0]);
	if(mytemp2d->GetEntries()==0) {
	  BadDraw("Empty Histogram");
	} else {
	  // These are commented out for some reason (specific to DVCS?)
	  // 	  if(showGolden) {
	  // 	    fGoldenFile->cd();
	  // 	    mytemp2d_golden = (TH2D*)gDirectory->Get(command[0]);
	  // 	    mytemp2d_golden->SetMarkerColor(2);
	  // 	    mytemp2d_golden->Draw();
	  //mytemp2d->Draw("sames");
	  // 	  } else {
	  mytemp2d->Draw();
	  // 	  }
	}
	break;
      }
      if(fileObjects[i].second.Contains("TH3")) {
	if(showGolden) fRootFile->cd();
	mytemp3d = (TH3D*)gDirectory->Get(command[0]);
	if(mytemp3d->GetEntries()==0) {
	  BadDraw("Empty Histogram");
	} else {
	  mytemp3d->Draw();
	  if(showGolden) {
	    fGoldenFile->cd();
	    mytemp3d_golden = (TH3D*)gDirectory->Get(command[0]);
	    mytemp3d_golden->SetMarkerColor(2);
	    mytemp3d_golden->Draw();
	    mytemp3d->Draw("sames");
	  } else {
	    mytemp3d->Draw();
	  }
	}
	break;
      }
    }
  }

}

void OnlineGUI::TreeDraw(vector <TString> command) {
  // Called by DoDraw(), this will plot a Tree Variable

  TString var = command[0];

  //  Check to see if we're projecting to a specific histogram
  TString histoname = command[0](TRegexp(">>.+(?"));
  if (histoname.Length()>0){
    histoname.Remove(0,2);
    Int_t bracketindex = histoname.First("(");
    if (bracketindex>0) histoname.Remove(bracketindex);
    if(fVerbosity>=3)
      std::cout << histoname << " "<< command[0](TRegexp(">>.+(?")) <<std::endl;
  } else {
    histoname = "htemp";
  }
  
  // Combine the cuts (definecuts and specific cuts)
  TCut cut = "";
  TString tempCut;
  if(command.size()>1) {
    tempCut = command[1];
    vector <TString> cutIdents = fConfig->GetCutIdent();
    for(UInt_t i=0; i<cutIdents.size(); i++) {
      if(tempCut.Contains(cutIdents[i])) {
	TString cut_found = (TString)fConfig->GetDefinedCut(cutIdents[i]);
	tempCut.ReplaceAll(cutIdents[i],cut_found);
      }
    }
    cut = (TCut)tempCut;
  }

  // Determine which Tree the variable comes from, then draw it.
  UInt_t iTree;
  if(command[4].IsNull()) {
    iTree = GetTreeIndex(var);
    if(fVerbosity>=2)
      cout<<"got index from variable "<<iTree<<endl;
  } else {
    iTree = GetTreeIndexFromName(command[4]);
    if(fVerbosity>=2)
      cout<<"got index from command "<<iTree<<endl;
  }
  TString drawopt = command[2];

  if(fVerbosity>=3)
    cout<<"\tDraw option:"<<drawopt<<" and histo name "<<histoname<<endl;
  Int_t errcode=0;
  if (iTree <= fRootTree.size() ) {
    if(fVerbosity>=1){
      cout<<__PRETTY_FUNCTION__<<"\t"<<__LINE__<<endl;
      cout<<command[0]<<"\t"<<command[1]<<"\t"<<command[2]<<"\t"<<command[3]
	  <<"\t"<<command[4]<<endl;
      if(fVerbosity>=2)
	cout<<"\tProcessing from tree: "<<iTree<<"\t"<<fRootTree[iTree]->GetTitle()<<"\t"
	    <<fRootTree[iTree]->GetName()<<endl;
    }
    errcode = fRootTree[iTree]->Draw(var,cut,drawopt);
    if (command[5].EqualTo("grid")){
      gPad->SetGrid();
    }

    TObject *hobj = (TObject*)gROOT->FindObject(histoname);
    if(fVerbosity>=3)
      cout<<"Finished drawing with error code "<<errcode<<endl;

    if(errcode==-1) {
      BadDraw(var+" not found");
    } else if (errcode!=0) {
      if(!command[3].IsNull()) {
        //  Generate a "unique" histogram name based on the MD5 of the drawn variable, cut, drawopt,
        //  and plot title.
        //  Makes it less likely to cause a name collision if two plot titles are the same.
        //  If you draw the exact same plot twice, the histograms will have the same name, but
        //  since they are exactly the same, you likely won't notice (or it will complain at you).
        TString tmpstring(var);
        tmpstring += cut.GetTitle();
        tmpstring += drawopt;
        tmpstring += command[3];
        TString myMD5 = tmpstring.MD5();
	TH1* thathist = (TH1*)hobj;
	thathist->SetNameTitle(myMD5,command[3]);
      }
    } else {
      BadDraw("Empty Histogram");
    }
  } else {
    BadDraw(var+" not found");
    if (fConfig->IsMonitor()){
      // Maybe we missed it... look again.  I dont like the code
      // below... maybe I can come up with something better
      GetFileObjects();
      GetRootTree();
      GetTreeVars();
    }
  }
}

void OnlineGUI::PrintToFile()
{
  // Routine to print the current page to a File.
  //  A file dialog pops up to request the file name.
  fCanvas = fEcanvas->GetCanvas();
  gStyle->SetPaperSize(20,24);
  static TString dir("printouts");
  TGFileInfo fi;
  const char *myfiletypes[] = 
    { "All files","*",
      "Portable Document Format","*.pdf",
      "PostScript files","*.ps",
      "Encapsulated PostScript files","*.eps",
      "GIF files","*.gif",
      "JPG files","*.jpg",
      0,               0 };
  fi.fFileTypes = myfiletypes;
  fi.fIniDir    = StrDup(dir.Data());

  new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
  if(fi.fFilename!=NULL) fCanvas->Print(fi.fFilename);
}

void OnlineGUI::PrintPages() {
  // Routine to go through each defined page, and print the output to 
  // a postscript file. (good for making sample histograms).
  
  // Open the RootFile
  //  unless we're watching a file.
  fRootFile = new TFile(fConfig->GetRootFile(),"READ");
  if(!fRootFile->IsOpen()) {
    cout << "ERROR:  rootfile: " << fConfig->GetRootFile()
	 << " does not exist"
	 << endl;
    gApplication->Terminate();
  } else {
    fFileAlive = kTRUE;
    GetFileObjects();
    GetRootTree();
    GetTreeVars();
    for(UInt_t i=0; i<fRootTree.size(); i++) {
      if(fRootTree[i]==0) {
	fRootTree.erase(fRootTree.begin() + i);
      }
    }
    
  }
  TString goldenfilename=fConfig->GetGoldenFile();
  if(!goldenfilename.IsNull()) {
    fGoldenFile = new TFile(goldenfilename,"READ");
    if(!fGoldenFile->IsOpen()) {
      cout << "ERROR: goldenrootfile: " << goldenfilename
	   << " does not exist.  Oh well, no comparison plots."
	   << endl;
      doGolden = kFALSE;
      fGoldenFile=NULL;
    } else {
      doGolden = kTRUE;
    }
  } else {
    doGolden=kFALSE;
    fGoldenFile=NULL;
  }

  fCanvas = new TCanvas("fCanvas","trythis",1000,800);
  TLatex *lt = new TLatex();

  TString plotsdir = fConfig->GetPlotsDir();
  if(plotsdir.IsNull()) plotsdir=".";

  Bool_t pagePrint = kFALSE;
  TString printFormat = fConfig->GetPlotFormat();
  if(printFormat.IsNull()) printFormat="pdf";
  if(printFormat!="pdf") pagePrint = kTRUE;

  TString filename = "summaryPlots";
  runNumber = fConfig->GetRunNumber();
  if(runNumber!=0) {
    filename += "_";
    filename += runNumber;
  } else {
    printf(" Warning for pretty plots: runNumber = %i\n",runNumber);
  }

  filename.Prepend(plotsdir+"/");
  if(pagePrint) 
    filename += "_pageXXXX";
  TString fConfName = fConfig->GetConfFileName();
  TString fCfgNm = fConfName(fConfName.Last('/')+1,fConfName.Length());
  filename += "_" + fCfgNm(0,fCfgNm.Last('.'));
  filename += "."+printFormat;

  TString pagehead = "Summary Plots";
  if(runNumber!=0) {
    pagehead += "(Run #";
    pagehead += runNumber;
    pagehead += ")";
  }
  //  pagehead += ": ";

  gStyle->SetPalette(1);
  gStyle->SetTitleX(0.15);
  gStyle->SetTitleY(0.9);
  gStyle->SetPadBorderMode(0);
  gStyle->SetHistLineColor(1);
  gStyle->SetHistFillColor(1);
  if(!pagePrint) fCanvas->Print(filename+"[");
  TString origFilename = filename;
  for(UInt_t i=0; i<fConfig->GetPageCount(); i++) {
    current_page=i;
    DoDraw();
    TString pagename = pagehead;
    pagename += " ";   
    pagename += i;
    pagename += ": ";
    pagename += fConfig->GetPageTitle(current_page);
    lt->SetTextSize(0.025);
    lt->DrawLatex(0.05,0.98,pagename);
    if(pagePrint) {
      filename = origFilename;
      filename.ReplaceAll("XXXX",Form("%02d",current_page));
      cout << "Printing page " << current_page 
	   << " to file = " << filename << endl;
    }
    fCanvas->Print(filename);
  }
  if(!pagePrint) fCanvas->Print(filename+"]");
  
  gApplication->Terminate();
}

void OnlineGUI::MyCloseWindow()
{
  fMain->SendCloseMessage();
  cout << "OnlineGUI Closed." << endl;
  if(timer!=NULL) {
    timer->Stop();
    delete timer;
  }
  delete fPrint;
  delete fExit;
  delete fRunNumber;
  delete fPrev;
  delete fNext;
  delete wile;
  for(UInt_t i=0; i<fConfig->GetPageCount(); i++) 
    delete fRadioPage[i];
  delete hframe;
  delete fEcanvas;
  delete fBottomFrame;
  delete vframe;
  delete fTopframe;
  delete fMain;
  if(fGoldenFile!=NULL) delete fGoldenFile;
  if(fRootFile!=NULL) delete fRootFile;
  delete fConfig;

  gApplication->Terminate();
}

void OnlineGUI::CloseGUI() 
{
  // Routine to take care of the Exit GUI button
  fMain->SendCloseMessage();
}

OnlineGUI::~OnlineGUI()
{
  //  fMain->SendCloseMessage();
  if(timer!=NULL) {
    timer->Stop();
    delete timer;
  }
  delete fPrint;
  delete fExit;
  delete fRunNumber;
  delete fPrev;
  delete fNext;
  delete wile;
  for(UInt_t i=0; i<fConfig->GetPageCount(); i++) 
    delete fRadioPage[i];
  delete hframe;
  delete fEcanvas;
  delete vframe;
  delete fBottomFrame;
  delete fTopframe;
  delete fMain;
  if(fGoldenFile!=NULL) delete fGoldenFile;
  if(fRootFile!=NULL) delete fRootFile;
  delete fConfig;
}
