#ifndef __CAMDF__
#define __CAMDF__

#include <boost/any.hpp>
using namespace ROOT;
using boost::any_cast;


// Best in root >= 6.16

class Channel{
  public: 
    /* Minirun update:
    std::vector <TH1D> histos;
    TString name;
    TString draw;
    std::vector <Int_t> miniruns;
    std::vector <Double_t> means;
    std::vector <Double_t> meanErrs;
    std::vector <Double_t> rmss;
    std::vector <Double_t> rmsErrs;
    std::vector <Double_t> nentries;*/
    void drawPlot (std::map<TString,TPad*>);
    void getData();
    void getSlowData();
    void storeData(TTree *);
    ROOT::RDF::RResultPtr<TGraph> graph2D;
    ROOT::RDF::RResultPtr<TH1D> histo1D;
    ROOT::RDF::RResultPtr<TH2D> histo2D;
    ROOT::RDF::RResultPtr<TH3D> histo3D;
    std::map<TString,TString> details;
    TString name;
    TString branchName;
    TString type = "meanrms";
    TString draw;
    //Int_t minirun;
    Double_t singleEntry = -1.0e6; // For single entry getting
    Double_t slope = -1.0e6; // For Slope getting
    Double_t slopeError = -1.0e6;
    Double_t avg = -1.0e6; // For normal meanrms getting
    Double_t avgErr = -1.0e6;
    Double_t rms = -1.0e6;
    Double_t rmsErr = -1.0e6;
    Double_t nEntries = 0;
};
/* I would like to define the channels in a loop over input.txt and apply all filter/defines to dataframe, but I don't know the dataframe.filter's data type... just do it in line
void Channel::fill_channels(TString inputFile = "input.txt"){
  return;
}; */

void Channel::drawPlot(std::map<TString,TPad*> pads){
  pads[details["Canvas Name"]]->cd(details["Canvas X"].Atoi()+(details["Canvas Nx"].Atoi()*(details["Canvas Y"].Atoi()-1)));
  // FIXME insert auto->histo* pointer to be assigned later based on dimension
  if (details.count("Draw Option") == 0) {
    details["Draw Option"] = "";
  }
  Printf("Draw Option = %s",details["Draw Option"].Data());
  if (details.count("SetOptFit") == 0) {
    details["SetOptFit"] = "1";
  }
  if (details.count("SetOptStat") == 0) {
    details["SetOptStat"] = "0";
  }
  Printf("SetOptStat = %s",details["SetOptStat"].Data());
  if (details["SetOptStat"] == "0") {
    if (details["Dimension"] == "1") {
      histo1D->SetStats(0);
    }
    if (details["Dimension"] == "2") {
      if (details.count("Graph") == 0 || details["Graph"] == "False") {
        histo2D->SetStats(0);
      }
    }
    if (details["Dimension"] == "3") {
      histo3D->SetStats(0);
    }
  }
  gStyle->SetStatW(0.3);
  gStyle->SetStatH(0.25);
  if (details.count("Marker Color") > 0) { // To get a red plot of similar data with a different cut you need to do a draw same on a fresh channel with the same Canvas coordinates
    if (details["Dimension"] == "1") {
      histo1D->SetMarkerColor(details["Marker Color"].Atoi()); // Integer
    }
    if (details["Dimension"] == "2") {
      if (details.count("Graph") > 0 && details["Graph"] == "True") {
        graph2D->SetMarkerColor(details["Marker Color"].Atoi()); // Integer
      }
      else {
        histo2D->SetMarkerColor(details["Marker Color"].Atoi()); // Integer
      }
    }
    if (details["Dimension"] == "3") {
      histo3D->SetMarkerColor(details["Marker Color"].Atoi()); // Integer
    }
  }
  if (details.count("Marker Style") > 0) {
    if (details["Dimension"] == "1") {
      histo1D->SetMarkerStyle(details["Marker Style"].Atoi()); // Integer
    }
    if (details["Dimension"] == "2") {
      if (details.count("Graph") > 0 && details["Graph"] == "True") {
        graph2D->SetMarkerStyle(details["Marker Style"].Atoi()); // Integer
      }
      else {
        histo2D->SetMarkerStyle(details["Marker Style"].Atoi()); // Integer
      }
    }
    if (details["Dimension"] == "3") {
      histo3D->SetMarkerStyle(details["Marker Style"].Atoi()); // Integer
    }
  }
  if (details.count("Marker Size") > 0) {
    if (details["Dimension"] == "1") {
      histo1D->SetMarkerSize(details["Marker Size"].Atoi()); // Integer
    }
    if (details["Dimension"] == "2") {
      if (details.count("Graph") > 0 && details["Graph"] == "True") {
        graph2D->SetMarkerSize(details["Marker Size"].Atoi());
      }
      else {
        histo2D->SetMarkerSize(details["Marker Size"].Atoi());
      }
    }
    if (details["Dimension"] == "3") {
      histo3D->SetMarkerSize(details["Marker Size"].Atoi());
    }
  }
  // Initial Draw
  gStyle->SetOptFit(details["SetOptFit"].Atoi());
  gStyle->SetOptStat(details["SetOptStat"].Atoi());
  if (details["Dimension"] == "1") histo1D->Draw(details["Draw Option"]); // "SAME" would be nice to work automatically here
  if (details["Dimension"] == "2") {
    if (details.count("Graph") > 0 && details["Graph"] == "True") {
      graph2D->Draw(details["Draw Option"]);
    }
    else {
      histo2D->Draw(details["Draw Option"]);
    }
  }
  if (details["Dimension"] == "3") histo3D->Draw(details["Draw Option"]);
  pads[details["Canvas Name"]]->Update();
  // Fitting
  
  if (details.count("Fit Function") > 0) {
    if (details.count("SetOptFit") == 0) {
      details["SetOptStat"] = "0";
      details["SetOptFit"] = "1";
    }
  }
  else {
    if (details.count("SetOptFit") == 0) {
      details["SetOptFit"] = "0";
    }
  }

  if (details.count("Fit Function") > 0) {
    if (details.count("Fit X Low Limit") > 0 && details.count("Fit X High Limit") > 0){
      if (details["Dimension"] == "1") histo1D->Fit(details["Fit Function"],details["Fit Options"],"",details["Fit X Low Limit"].Atof(),details["Fit X High Limit"].Atof());
      if (details["Dimension"] == "2") histo2D->Fit(details["Fit Function"],details["Fit Options"],"",details["Fit X Low Limit"].Atof(),details["Fit X High Limit"].Atof());
      if (details["Dimension"] == "3") histo3D->Fit(details["Fit Function"],details["Fit Options"],"",details["Fit X Low Limit"].Atof(),details["Fit X High Limit"].Atof());
    }
    if (details.count("Fit X Sigma Limit") > 0) {
      if (details["Dimension"] == "1") details["Fit X Low Limit"] = histo1D->GetMean()-details["Fit X Sigma Limit"].Atoi()*(histo1D->GetRMS());
      if (details["Dimension"] == "2") details["Fit X Low Limit"] = histo2D->GetMean()-details["Fit X Sigma Limit"].Atoi()*(histo2D->GetRMS());
      if (details["Dimension"] == "3") details["Fit X Low Limit"] = histo3D->GetMean()-details["Fit X Sigma Limit"].Atoi()*(histo3D->GetRMS());
      if (details["Dimension"] == "1") details["Fit X High Limit"] = histo1D->GetMean()+details["Fit X Sigma Limit"].Atoi()*(histo1D->GetRMS());
      if (details["Dimension"] == "2") details["Fit X High Limit"] = histo2D->GetMean()+details["Fit X Sigma Limit"].Atoi()*(histo2D->GetRMS());
      if (details["Dimension"] == "3") details["Fit X High Limit"] = histo3D->GetMean()+details["Fit X Sigma Limit"].Atoi()*(histo3D->GetRMS());
      if (details["Dimension"] == "1") {
        histo1D->Fit(details["Fit Function"],details["Fit Options"],"",details["Fit X Low Limit"].Atof(),details["Fit X High Limit"].Atof());
        pads[details["Canvas Name"]]->Update();
      }
      if (details["Dimension"] == "2") {
        histo2D->Fit(details["Fit Function"],details["Fit Options"],"",details["Fit X Low Limit"].Atof(),details["Fit X High Limit"].Atof());
        pads[details["Canvas Name"]]->Update();
      }
      if (details["Dimension"] == "3") {
        histo3D->Fit(details["Fit Function"],details["Fit Options"],"",details["Fit X Low Limit"].Atof(),details["Fit X High Limit"].Atof());
        pads[details["Canvas Name"]]->Update();
      }
    }
    else {
      if (details["Dimension"] == "1") {
        histo1D->Fit(details["Fit Function"],details["Fit Options"]);
        pads[details["Canvas Name"]]->Update();
      }
      if (details["Dimension"] == "2") {
        histo2D->Fit(details["Fit Function"],details["Fit Options"]);
        pads[details["Canvas Name"]]->Update();
      }
      if (details["Dimension"] == "3") {
        histo3D->Fit(details["Fit Function"],details["Fit Options"]);
        pads[details["Canvas Name"]]->Update();
      }
    }
    if (details["Dimension"] == "1") { 
      histo1D->Draw(details["Draw Option"]);
      if (details.count("Fit Function") > 0) {
        histo1D->GetFunction(details["Fit Function"])->Draw("same");
      }
    }
    if (details["Dimension"] == "2") { 
      histo2D->Draw(details["Draw Option"]);
      if (details.count("Fit Function") > 0) {
        histo2D->GetFunction(details["Fit Function"])->Draw("same");
        pads[details["Canvas Name"]]->Update();
        TPaveStats* st = (TPaveStats*)pads[details["Canvas Name"]]->GetPrimitive("stats");
        if (st) {
          st->SetName("MyStats");
          st->SetOptFit(1);
          st->SetOptStat(0);
          histo2D->SetStats(0);
          pads[details["Canvas Name"]]->Modified();
          pads[details["Canvas Name"]]->Update();
          st->Draw();
          ///st->Delete();
        }
        else {
          Printf("Error: Fit object failed");
        }
      }
    }
    if (details["Dimension"] == "3") { 
      histo3D->Draw(details["Draw Option"]);
      if (details.count("Fit Function") > 0) {
        histo3D->GetFunction(details["Fit Function"])->Draw("same");
      }
    }
  }
  else {
    // Stat Box
    TPaveStats *psus;// = (TPaveStats*)pads[details["Canvas Name"]]->GetPrimitive("stats");
    if (details["Dimension"] == "1") { 
      psus = (TPaveStats*)histo1D->FindObject("stats");
    }
    if (details["Dimension"] == "2") { 
      if (details.count("Graph") > 0 && details["Graph"] == "True") {
        psus = (TPaveStats*)pads[details["Canvas Name"]]->GetPrimitive("stats");
      }
      else {
        psus = (TPaveStats*)histo2D->FindObject("stats");
      }
    }
    if (details["Dimension"] == "3") { 
      psus = (TPaveStats*)histo3D->FindObject("stats");
    }
    if(psus!=NULL){
      psus->SetName("MyStats");
      psus->SetOptFit(details["SetOptFit"].Atoi());
      psus->SetOptStat(details["SetOptStat"].Atoi());
      /*psus->SetX1NDC(0.0); // 2D stat box on left (assumes positive correlation statbox)
        psus->SetY1NDC(0.95);
        psus->SetX2NDC(0.35);
        psus->SetY2NDC(0.7);
        */
      if (details["Dimension"] == "1") histo1D->SetStats(0);
      if (details["Dimension"] == "2") histo2D->SetStats(0);
      if (details["Dimension"] == "3") histo3D->SetStats(0);
      pads[details["Canvas Name"]]->Modified();
      psus->Draw();
    }
    else {
      Printf("No stats object");
    }
  }

  if (details.count("Top Label") > 0){
    TText * t1 = new TText(0.2,0.95,details["Top Label"]);
    t1->SetNDC();
    t1->SetTextSize(0.05);
    t1->Draw("same");
  }
  if (details.count("Side Label") > 0){
    TText * t1 = new TText(0.05,0.4,details["Side Label"]);
    t1->SetNDC();
    t1->SetTextSize(0.05);
    t1->Draw("same");
  }
  if (details.count("Full Label") > 0){
    TText * t1 = new TText(0.3,0.4,details["Full Label"]);
    t1->SetNDC();
    t1->SetTextSize(0.05);
    t1->Draw();
  }
};

void Channel::getData(){
  if (histo1D){
    avg = histo1D->GetMean();
    avgErr = histo1D->GetMeanError();
    rms = histo1D->GetRMS();
    rmsErr = histo1D->GetRMSError();
    nEntries = histo1D->GetEntries();
  }
  else {
    if (debug>1) {
      Printf("Error, histogram for %s empty",name.Data());
    }
  }
};

void Channel::getSlowData(){
  if (histo1D){
    singleEntry = histo1D->GetBinCenter(histo1D->GetMaximumBin());
  }
  else {
    if (debug>1) {
      Printf("Error, histogram for %s empty",name.Data());
    }
  }
};

void Channel::storeData(TTree * outputTree){
  if (type == "meanrms"){
    outputTree->Branch(Form("%s_mean",name.Data()),&avg);
    outputTree->Branch(Form("%s_mean_error",name.Data()),&avgErr);
    outputTree->Branch(Form("%s_rms",name.Data()),&rms);
    outputTree->Branch(Form("%s_rms_error",name.Data()),&rmsErr);
    outputTree->Branch(Form("%s_nentries",name.Data()),&nEntries);
  }
  if (type == "slow"){
    outputTree->Branch(Form("%s",name.Data()),&singleEntry);
  }
  if (type == "slopes"){
    outputTree->Branch(Form("%s_slope",name.Data()),&slope);
    outputTree->Branch(Form("%s_slope_error",name.Data()),&slopeError);
  }
};

class Source {
  public:
    TString run,nruns,split,minirun,input,basename;
    Source(TString run_n, TString n_runs, TString n_minirun, TString n_split, TString in, TString base_name): run(run_n), nruns(n_runs), minirun(n_minirun), split(n_split), input(in), basename(base_name) {}
    RDataFrame readSource();
    void printInfo() { std::cout << "Processing run  " << run  << ". " << std::endl;} 
    void drawAll();
    void getSlopes(std::vector<Channel>&, Int_t, Int_t, Int_t, Double_t);
    Channel GetChannelByName(TString name);    
};

void Source::getSlopes(std::vector<Channel> &channels, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  splitNumber = getSplitNumber_h(splitNumber);
  minirunNumber = getMinirunNumber_h(minirunNumber);
  nRuns     = getNruns_h(nRuns);
  Double_t data_slope = 0;
  Double_t data_slope_error = 0;
  Printf("Getting slopes");

  TString ditSlopeFileNamebase = gSystem->Getenv("DITHERING_ROOTFILES_SLOPES");
  TString ditSlopeFileName = ditSlopeFileNamebase + "/dit_slopes_slug" + nRuns + ".root";
  if( !gSystem->AccessPathName(ditSlopeFileName) ) {
    Printf("Getting dithering slopes from %s",ditSlopeFileName.Data());
    TChain *ditTree = new TChain("dit");
    ditTree->Add(ditSlopeFileName);
    TLeaf *ditRunNum = ditTree->GetLeaf("cycnum");
    TObjArray *slopesList = ditTree->GetListOfLeaves();
    TString outname = "";
    /*for (Int_t a = 0; a<ditTree->GetEntries(); a++){
      ditTree->GetEntry(a);
      if (debug>3) Printf("Entry number %d, run number %d",a,(Int_t)ditRunNum->GetValue(0));
      TIter slopesIter(slopesList);
      while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
        if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
        if ((TString)slopes->GetName() != "cycnum" && (Int_t)ditRunNum->GetValue(0) == runNumber){
          outname = "dit_"+(TString)slopes->GetName()+"_slope";
          if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)slopes->GetValue(0));
          Channel tmpChan;
          tmpChan.type = "slopes";
          tmpChan.name = "dit_"+(TString)slopes->GetName();
          tmpChan.slope = (Double_t)slopes->GetValue(0);
          tmpChan.slopeError = 0.0; // Dithering has no errors?
          channels.push_back(tmpChan);
        }
      }
    }*/ // Loop Over Entries - Ideal per/cycle way of doing things FIXME
    TIter slopesIter(slopesList);
    while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
      if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
      if ((TString)slopes->GetName() != "cycnum"){
        ditTree->Draw(Form("%s",slopes->GetName()),"1==1","goff");
        TH1* tmpHist = (TH1*)gROOT->FindObject("htemp");
        outname = "dit_"+(TString)slopes->GetName()+"_slope";
        if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),1e-3*(Double_t)tmpHist->GetMean());
        Channel tmpChan;
        tmpChan.type = "slopes";
        tmpChan.name = "dit_"+(TString)slopes->GetName();
        tmpChan.slope = 1e-3*(Double_t)tmpHist->GetMean();
        tmpChan.slopeError = 1e-3*(Double_t)tmpHist->GetMeanError(); // Dithering has no errors?
        channels.push_back(tmpChan);
      }
    }
  }
  TString ditRunSlopeFileNamebase = gSystem->Getenv("DITHERING_ROOTFILES_SLOPES_RUN");
  TString ditRunSlopeFileName = ditRunSlopeFileNamebase + "/dit_slopes_slug" + nRuns + ".root";
  if( !gSystem->AccessPathName(ditRunSlopeFileName) ) {
    Printf("Getting dithering slopes from %s",ditRunSlopeFileName.Data());
    TChain *ditTree = new TChain("dit");
    ditTree->Add(ditRunSlopeFileName);
    TLeaf *ditRunNum = ditTree->GetLeaf("run");
    TObjArray *slopesList = ditTree->GetListOfLeaves();
    TString outname = "";
    for (Int_t a = 0; a<ditTree->GetEntries(); a++){
      ditTree->GetEntry(a);
      if (debug>3) Printf("Entry number %d, run number %d",a,(Int_t)ditRunNum->GetValue(0));
      TIter slopesIter(slopesList);
      while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
        if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
        if ((TString)slopes->GetName() != "run" && (Int_t)ditRunNum->GetValue(0) == runNumber){
          outname = "dit_"+(TString)slopes->GetName()+"_slope";
          if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)slopes->GetValue(0));
          Channel tmpChan;
          tmpChan.type = "slopes";
          tmpChan.name = "dit_run_"+(TString)slopes->GetName();
          tmpChan.slopeError = 0.0001; // Dithering has no errors?
          if (abs((Double_t)slopes->GetValue(0))<100.0) { 
            tmpChan.slope = 1e-3*(Double_t)slopes->GetValue(0);
            tmpChan.slopeError = -1.0e6;
          }
          channels.push_back(tmpChan);
        }
      }
    } // Loop Over Entries - Ideal per/cycle way of doing things FIXME
  }
  if(minirunNumber<0){
    TString lrbFileNameBase = gSystem->Getenv("LRB_ROOTFILES");
    TString lrbFileName = lrbFileNameBase + "/blueR"+runNumber+".000new.slope.root";  
    TFile f(lrbFileName);

    std::map<TString, Int_t> IVname;
    std::map<TString, Int_t> DVname;

    TH1D* m=(TH1D*) f.Get("IVname");
    for (auto i=0; i<m->GetEntries();i++){
      TString ivname=m->GetXaxis()->GetBinLabel(i+1);
      IVname[ivname]=i;
    }

    TH1D* n=(TH1D*) f.Get("DVname");
    for (auto i=0; i<n->GetEntries(); i++){
      TString dvname=n->GetXaxis()->GetBinLabel(i+1);
      DVname[dvname]=i;
    }


    TMatrixT<double> slopes=*(TMatrixT<double>*) f.Get("slopes");
    TMatrixT<double> sigSlopes=*(TMatrixT<double>*) f.Get("sigSlopes");
    for (auto& i: DVname){ 
      for (auto& j: IVname){
        Channel tmpChan;
        tmpChan.type = "slopes";
        tmpChan.name = "cor_"+i.first+"_"+j.first;
        tmpChan.slope = slopes(j.second,i.second);
        tmpChan.slopeError = sigSlopes(j.second,i.second);
        channels.push_back(tmpChan);
      }
    }
  }
  if (minirunNumber>=0){
    Printf("Getting postpan slopes");
    TString lpostpanFileNameBase = gSystem->Getenv("POSTPAN_ROOTFILES");
    TString lpostpanFileName = lpostpanFileNameBase + "/prexPrompt_" + runNumber + "_000_regress_postpan.root";
    TFile f(lpostpanFileName);

    TTreeReader theReader("mini", &f);
    TTreeReaderArray<double> slope(theReader,"coeff");
    TTreeReaderArray<double> slope_err(theReader,"err_coeff");

    std::vector<TString> ivname= *(std::vector<TString>*) f.Get("IVNames");
    std::vector<TString> dvname= *(std::vector<TString>*) f.Get("DVNames");
    std::vector<TString> comb;

    for (auto &i:dvname){
      for (auto &j:ivname){
        comb.push_back(i+"_"+j);
      }
    }

    Int_t mini=0;
    while(theReader.Next()){
      Int_t count=0;
      for (auto i=0; i<slope.GetSize(); i++){
        if (mini!=minirunNumber) continue;
        Channel tmpChan;
        tmpChan.type = "slopes";
        tmpChan.name = "reg_"+comb.at(count);
        tmpChan.slope = slope[i];
        tmpChan.slopeError = slope_err[i];
        channels.push_back(tmpChan);
        count++;
      }
      mini++; 
    }
  }	
};

RDataFrame Source::readSource(){
  EnableImplicitMT();

  /*--------------------------------------------*/

  TStopwatch tsw;
  TStopwatch tswAll;
  tsw.Start();
  tswAll.Start();
  if (debug > 1) { cout << "Beginning TChain Setup --"; tsw.Print(); cout << endl;}
  tsw.Start();
  TChain * evt_tree      = new TChain("evt");
  TChain * mul_tree      = new TChain("mul");
  TChain * slow_tree     = new TChain("slow");
  TChain * reg_tree      = new TChain("reg");
  TChain * dit_tree      = new TChain("dit");
  TChain * mini_tree     = new TChain("mini");
  TChain * mulc_tree     = new TChain("mulc");
  TChain * mulc_lrb_burst_tree = new TChain("mulc_lrb_burst");

  TString DataFolder = gSystem->Getenv("QW_ROOTFILES");
  evt_tree->Add(Form("%s/%s_%s.%s.root",DataFolder.Data(),basename.Data(),run.Data(),split.Data()));
  mul_tree->Add(Form("%s/%s_%s.%s.root",DataFolder.Data(),basename.Data(),run.Data(),split.Data()));
  slow_tree->Add(Form("%s/%s_%s.%s.root",DataFolder.Data(),basename.Data(),run.Data(),split.Data()));
  reg_tree->Add(Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_%s_regress_postpan.root", run.Data(),split.Data()));
  TString ditheringFileNameDF = gSystem->Getenv("DITHERING_ROOTFILES");
  if (ditheringFileNameDF != ""){
    Printf("Looking for Dithering corrected files in %s",ditheringFileNameDF.Data());
    dit_tree->Add(Form("%s/prexPrompt_dither_%s_000.root", ditheringFileNameDF.Data(), run.Data()));
  }
  mini_tree->Add(Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_%s_regress_postpan.root", run.Data(),split.Data()));
  mulc_tree->Add(Form("%s/%s_%s.%s.root", DataFolder.Data(),basename.Data(),run.Data(),split.Data()));
  mulc_lrb_burst_tree->Add(Form("%s/%s_%s.%s.root", DataFolder.Data(),basename.Data(),run.Data(),split.Data()));

  mul_tree->AddFriend(reg_tree);
  if (ditheringFileNameDF != ""){
    Printf("Obtained Dithering corrected files in %s",ditheringFileNameDF.Data());
    mul_tree->AddFriend(dit_tree);
  }
  mul_tree->AddFriend(mulc_lrb_burst_tree);
  mul_tree->AddFriend(mulc_tree);
  TChain * tree      = mul_tree;
  //        mul_tree->Draw("mul.asym_usl.hw_sum","ErrorFlag==0");
  //        TH1 * tmp1Xhist = (TH1*)gROOT->FindObject("htemp");
  //        Printf("Test 0.1, drawing Y:X");
  //        Printf("X mean = %f",tmp1Xhist->GetRMS());

  //miniruns = mini_tree->Scan("minirun",""); // FIXME for later minirun looping addition

  RDataFrame e(*evt_tree);
  RDataFrame d(*mul_tree);//,device_list);
  RDataFrame slow(*slow_tree);
  if (debug > 1) { cout << "Filtering through ErrorFlag==0 --"; tsw.Print(); cout << endl; }
  tsw.Start();
  auto e_good=e.Filter("ErrorFlag==0");
  auto d_good=d.Filter("ErrorFlag==0");
  //if (ditheringFileNameDF != ""){
  //  d_good=d.Filter("dit.ErrorFlag==0");
  //}
  if (debug > 1) { cout << "Filtered through ErrorFlag==0 --"; tsw.Print(); cout << endl;}
  tsw.Start();
  //std::vector<TInterface<TFilterBase> ROOT::Experimental::TDF::TInterface< Proxied >::Filter> minirunCuts;
  //std::vector<ROOT::RDF::RInterface<Filter> minirunCuts;
  //for (Int_t k = 0; k<miniruns; k++) {
  //  minirunCuts.push_back(d_good.Filter(Form("reg.minirun==%d",k)));
  //}

  if (debug > 1) { cout << "Done with Tree cuts --"; tsw.Print(); cout << endl;}
  tsw.Start();

  std::vector<Channel> channels;
  std::vector<Channel> summaries;
  std::map<TString,TPad*> pads;
  std::vector<TString> canvasNames;
  std::vector<TCanvas *> cs;
  //TCanvas * c1 = new TCanvas();
  Int_t winWidth = 600;
  //c1->SetWindowSize(1600,1200);
  //c1->SetCanvasSize(1600,1200);
  //c1->cd();

  // Getting device list
  string line;
  std::vector<string> tokens;
  std::map<TString,TString> details;
  string typedefault = "meanrms";
  ifstream infile(input);
  if (infile.is_open()){
    while(getline(infile,line)){
      string token;
      string detail;
      string name;
      string device;
      string type;
      std::istringstream tokenStream(line);
      while(getline(tokenStream,token,',')){
        tokens.push_back(token);
      }
      if (tokens.size() > 0){
        name = tokens.at(0);
      }
      if (tokens.size() > 1){
        device = tokens.at(1);
        if (name == "same"){
          name = device;
        }
        details["Dimension"] = Form("%d",((TString)device).CountChar(':')+1);
        details["Name"] = name;
        details["Draw"] = device;
        if (((TString)device).CountChar(':') == 0) { // Histo1D
          details["DrawX"] = device;
          if (details.count("Histogram Title") == 0){
            Printf("Histgram Title overwritten. 1D");
            details["Histogram Title"] = "Histo "+details["DrawX"];
          }
        }
        if (((TString)device).CountChar(':') == 1) { // Histo2D
          std::vector<string> dims;
          string dim;
          std::istringstream dimStream(device);
          while(getline(dimStream,dim,':')){
            dims.push_back(dim);
          }
          TString dim1 = dims.at(0);
          TString dim2 = dims.at(1);
          details["DrawY"] = dim1;
          details["DrawX"] = dim2;
          if (details.count("Histogram Title") == 0){
            details["Histogram Title"] = "Histo "+details["DrawY"]+" vs "+details["DrawX"];
          }
        }
        if (((TString)device).CountChar(':') == 2) { // Histo3D
          std::vector<string> dims;
          string dim;
          std::istringstream dimStream(device);
          while(getline(dimStream,dim,':')){
            dims.push_back(dim);
          }
          TString dim1 = dims.at(0);
          TString dim2 = dims.at(1);
          TString dim3 = dims.at(2);
          details["DrawZ"] = dim1;
          details["DrawY"] = dim2;
          details["DrawX"] = dim3;
          if (details.count("Histogram Title") == 0){
            details["Histogram Title"] = "Histo "+details["DrawZ"]+" vs "+details["DrawY"]+" vs "+details["DrawX"];
          }
        }
      }
      else { 
        Printf("Error: invalid input file");
        return 1; 
      }
      if (tokens.size() > 2){
        type=tokens.at(2);
        details["Type"] = type;
      }
      else {
        type=typedefault;
        details["Type"] = type;
      }
      if (tokens.size() > 3){
        std::istringstream detailsStream(tokens.at(3));
        while(getline(detailsStream,detail,';')){
          std::istringstream thingStream(detail);
          std::vector<string> things;
          string thing;
          while(getline(thingStream,thing,'=')){
            if (things.size() > 0 && things.at(0) == "Cut" && things.size() == 1){
              things.push_back(detail.substr(4)); // Treat Cut special since it likely has == signs in it
              continue;
            }
            else {
              things.push_back(thing);
            }
          }
          if (things.size()>1){
            TString thing1 = things.at(0);
            TString thing2 = things.at(1);
            Printf("Adding things to details[\"%s\"] = %s",thing1.Data(),thing2.Data());
            details[thing1]=thing2; // Details = standard map of '=' separated things/details
          }
        }
      }

      if (details.count("Canvas Name") == 0) details["Canvas Name"] = details["Name"] + "_" + details["Draw"];
      if (details.count("Canvas X") == 0) details["Canvas X"] = "1";
      if (details.count("Canvas Y") == 0) details["Canvas Y"] = "1";
      if (details.count("Canvas Nx") == 0) details["Canvas Nx"] = "1";
      if (details.count("Canvas Ny") == 0) details["Canvas Ny"] = "1";

      if (pads.count(details["Canvas Name"]) == 0) {
        // Should be TPads?
        TCanvas * cTemp = new TCanvas();
        cTemp->SetWindowSize((winWidth+1)*details["Canvas Nx"].Atoi(),winWidth*details["Canvas Ny"].Atoi());
        cTemp->SetCanvasSize((winWidth+1)*details["Canvas Nx"].Atoi(),winWidth*details["Canvas Ny"].Atoi());
        cTemp->cd();
        cs.push_back(cTemp);
        TPad * tempC = new TPad(details["Canvas Name"],details["Canvas Name"],0,0,1,1);//winWidth*details["Canvas Nx"].Atoi(),winWidth*details["Canvas Ny"].Atoi(),1,1);
        tempC->Divide(details["Canvas Nx"].Atoi(),details["Canvas Ny"].Atoi());
        pads[details["Canvas Name"]] = tempC;
        canvasNames.push_back(details["Canvas Name"]);
      }

      // Execute lazy channel building
      tokens.clear();
      Channel tmpChan;
      tmpChan.name = name;
      tmpChan.branchName = "agg_"+name;
      tmpChan.draw = device;
      tmpChan.type = type;
      tmpChan.details = details;
      details.clear();
      if (tmpChan.details.count("Tree")>0 && tmpChan.details["Tree"] == "evt") {
        d = e;
        d_good = e_good;
        tree = evt_tree;
      }
      if (tmpChan.type == "summary"){
        if (tmpChan.details.count("Dimension")>0 && (tmpChan.details["Dimension"].Atoi()==1)) {
          if (tmpChan.details.count("Cut") == 0){
            tmpChan.details["Cut"] = "ErrorFlag==0";
          }
          if (minirun != "-1"){
            if (tmpChan.details.count("Tree")==0 || tmpChan.details["Tree"] != "evt") {
              tmpChan.details["Cut"] += Form("&&reg.minirun==%s",minirun.Data());
            }
          }
          TString branchXname = Form("%s",tmpChan.details["DrawX"].Copy().ReplaceAll(".","_").ReplaceAll("-","_").ReplaceAll("1e3*","").ReplaceAll("1e6*","").Data());
          //TString branchXname = Form("x_%s",tmpChan.details["DrawX"].Copy().ReplaceAll(".","_").ReplaceAll("-","_").ReplaceAll("1e6*","").Data());

          tree->Draw(tmpChan.details["DrawX"]+">>htempX",tmpChan.details["Cut"],"goff");
          TH1 * tmp1Xhist = (TH1*)gROOT->FindObject("htempX");
          if (tmpChan.details.count("N Bins X") == 0){
            tmpChan.details["N Bins X"] = (TString)Form("%d",tmp1Xhist->GetNbinsX());
          }
          if (tmpChan.details.count("Low Bin Limit X") == 0){
            tmpChan.details["Low Bin Limit X"] = (TString)Form("%f",tmp1Xhist->GetBinCenter(0));
          }
          if (tmpChan.details.count("High Bin Limit X") == 0){
            tmpChan.details["High Bin Limit X"] = (TString)Form("%f",tmp1Xhist->GetBinCenter(tmp1Xhist->GetNbinsX()-1));
          }
          if (tmpChan.details.count("Cut") > 0){
            tmpChan.histo1D = d // FIXME should I use d_good here always? Is it safe to re-define the same node? Does Cut 1==1 work always?
              .Filter(Form("%s",tmpChan.details["Cut"].Data()))
              .Define(branchXname.Data(),tmpChan.details["DrawX"].Data())
              .Histo1D({branchXname, tmpChan.details["Histogram Title"], tmpChan.details["N Bins X"].Atoi(), tmpChan.details["Low Bin Limit X"].Atof(), tmpChan.details["High Bin Limit X"].Atof()}, branchXname.Data());
          }
          else {
            tmpChan.histo1D = d // If 1==1 works then these else's are unnecessary
              .Define(branchXname.Data(),tmpChan.details["DrawX"].Data())
              .Histo1D({branchXname, tmpChan.details["Histogram Title"], tmpChan.details["N Bins X"].Atoi(), tmpChan.details["Low Bin Limit X"].Atof(), tmpChan.details["High Bin Limit X"].Atof()}, branchXname.Data());
          }
          tmp1Xhist->Delete();
        }
        if (tmpChan.details.count("Dimension")>0 && (tmpChan.details["Dimension"].Atoi()==2)) {
          TString branchXname = Form("x_%s",tmpChan.details["DrawX"].Copy().ReplaceAll(".","_").ReplaceAll("-","_").ReplaceAll("1e3*","").ReplaceAll("1e6*","").Data());
          TString branchYname = Form("y_%s",tmpChan.details["DrawY"].Copy().ReplaceAll(".","_").ReplaceAll("-","_").ReplaceAll("1e3*","").ReplaceAll("1e6*","").Data());
          if (tmpChan.details.count("Cut") == 0){
            tmpChan.details["Cut"] = "ErrorFlag==0";
          }
          if (minirun != "-1"){
            if (tmpChan.details.count("Tree")==0 || tmpChan.details["Tree"] != "evt") {
              tmpChan.details["Cut"] += Form("&&reg.minirun==%s",minirun.Data());
            }
          }
          if (tmpChan.details.count("Cut") > 0){
            auto tmp_d_xy = d
              .Filter(Form("%s",tmpChan.details["Cut"].Data()));
            // FIXME, ROOT 6.18 and higher have a method called HasColumn which effectively does the same as the next 11 lines... would be ideal to use it instead, or methodize what I have here.
            RDFDetail::ColumnNames_t allCols = d.GetColumnNames();
            Bool_t hasX = false;
            Bool_t hasY = false;
            const auto ccolnamesEnd = allCols.end();
            hasX = (ccolnamesEnd != std::find(allCols.begin(), ccolnamesEnd, branchXname.Data()));
            hasY = (ccolnamesEnd != std::find(allCols.begin(), ccolnamesEnd, branchYname.Data()));
            if (!hasX) {
              tmp_d_xy = tmp_d_xy.Define(branchXname.Data(),tmpChan.details["DrawX"].Data());
            }
            if (!hasY) {
              tmp_d_xy = tmp_d_xy.Define(branchYname.Data(),tmpChan.details["DrawY"].Data());
            }
            //auto tmp_d_x
            //  .Define(branchXname.Data(),tmpChan.details["DrawX"].Data())
            //  .Define(branchYname.Data(),tmpChan.details["DrawY"].Data());
            if (tmpChan.details.count("Graph") > 0 && tmpChan.details["Graph"] == "True") {
              tmpChan.graph2D = tmp_d_xy
                .Graph(Form("%s",branchXname.Data()),Form("%s",branchYname.Data()));
              tmpChan.graph2D->Sort();
              tmpChan.graph2D->SetNameTitle(tmpChan.details["Histogram Title"],tmpChan.details["Histogram Title"]);
              tmpChan.graph2D->GetXaxis()->SetTitle(tmpChan.details["DrawX"]);
              tmpChan.graph2D->GetYaxis()->SetTitle(tmpChan.details["DrawY"]);
            }
            else {
              tree->Draw(tmpChan.details["DrawX"]+">>htempX",tmpChan.details["Cut"],"goff");
              TH1 * tmp2Xhist = (TH1*)gROOT->FindObject("htempX");
              tree->Draw(tmpChan.details["DrawY"]+">>htempY",tmpChan.details["Cut"],"goff");
              TH1 * tmp2Yhist = (TH1*)gROOT->FindObject("htempY");
              if (tmpChan.details.count("N Bins X") == 0){
                tmpChan.details["N Bins X"] = (TString)Form("%d",tmp2Xhist->GetNbinsX());
              }
              if (tmpChan.details.count("Low Bin Limit X") == 0){
                tmpChan.details["Low Bin Limit X"] = (TString)Form("%f",tmp2Xhist->GetBinCenter(0));
              }
              if (tmpChan.details.count("High Bin Limit X") == 0){
                tmpChan.details["High Bin Limit X"] = (TString)Form("%f",tmp2Xhist->GetBinCenter(tmp2Xhist->GetNbinsX()-1));
              }
              if (tmpChan.details.count("N Bins Y") == 0){
                tmpChan.details["N Bins Y"] = (TString)Form("%d",tmp2Yhist->GetNbinsX());
              }
              if (tmpChan.details.count("Low Bin Limit Y") == 0){
                tmpChan.details["Low Bin Limit Y"] = (TString)Form("%f",tmp2Yhist->GetBinCenter(0));
              }
              if (tmpChan.details.count("High Bin Limit Y") == 0){
                tmpChan.details["High Bin Limit Y"] = (TString)Form("%f",tmp2Yhist->GetBinCenter(tmp2Yhist->GetNbinsX()-1));
              }
              tmpChan.histo2D = tmp_d_xy
                .Histo2D({branchYname+"_"+branchXname, tmpChan.details["Histogram Title"], tmpChan.details["N Bins X"].Atoi(), tmpChan.details["Low Bin Limit X"].Atof(), tmpChan.details["High Bin Limit X"].Atof(), tmpChan.details["N Bins Y"].Atoi(), tmpChan.details["Low Bin Limit Y"].Atof(), tmpChan.details["High Bin Limit Y"].Atof()}, branchXname.Data(),branchYname.Data());
              //.Histo2D(ROOT::RDF::TH2DModel(), branchXname.Data(),branchYname.Data());
              tmp2Xhist->Delete();
              tmp2Yhist->Delete();
            }
          }
          else {
            auto tmp_d = d
              .Define(branchXname.Data(),tmpChan.details["DrawX"].Data())
              .Define(branchYname.Data(),tmpChan.details["DrawY"].Data());
            if (tmpChan.details.count("Graph") > 0 && tmpChan.details["Graph"] == "True") {
              tmpChan.graph2D = tmp_d
                .Graph(branchXname.Data(),branchYname.Data());
              tmpChan.graph2D->Sort();
              tmpChan.graph2D->SetNameTitle(tmpChan.details["Histogram Title"],tmpChan.details["Histogram Title"]);
              tmpChan.graph2D->GetXaxis()->SetTitle(tmpChan.details["DrawX"]);
              tmpChan.graph2D->GetYaxis()->SetTitle(tmpChan.details["DrawY"]);
            }
            else {
              tree->Draw(tmpChan.details["DrawX"]+">>htempX",tmpChan.details["Cut"],"goff");
              TH1 * tmp2Xhist = (TH1*)gROOT->FindObject("htempX");
              tree->Draw(tmpChan.details["DrawY"]+">>htempY",tmpChan.details["Cut"],"goff");
              TH1 * tmp2Yhist = (TH1*)gROOT->FindObject("htempY");
              if (tmpChan.details.count("N Bins X") == 0){
                tmpChan.details["N Bins X"] = (TString)Form("%d",tmp2Xhist->GetNbinsX());
              }
              if (tmpChan.details.count("Low Bin Limit X") == 0){
                tmpChan.details["Low Bin Limit X"] = (TString)Form("%f",tmp2Xhist->GetBinCenter(0));
              }
              if (tmpChan.details.count("High Bin Limit X") == 0){
                tmpChan.details["High Bin Limit X"] = (TString)Form("%f",tmp2Xhist->GetBinCenter(tmp2Xhist->GetNbinsX()-1));
              }
              if (tmpChan.details.count("N Bins Y") == 0){
                tmpChan.details["N Bins Y"] = (TString)Form("%d",tmp2Yhist->GetNbinsX());
              }
              if (tmpChan.details.count("Low Bin Limit Y") == 0){
                tmpChan.details["Low Bin Limit Y"] = (TString)Form("%f",tmp2Yhist->GetBinCenter(0));
              }
              if (tmpChan.details.count("High Bin Limit Y") == 0){
                tmpChan.details["High Bin Limit Y"] = (TString)Form("%f",tmp2Yhist->GetBinCenter(tmp2Yhist->GetNbinsX()-1));
              }
              tmpChan.histo2D = tmp_d
                .Histo2D({branchYname+"_"+branchXname, tmpChan.details["Histogram Title"], tmpChan.details["N Bins X"].Atoi(), tmpChan.details["Low Bin Limit X"].Atof(), tmpChan.details["High Bin Limit X"].Atof(), tmpChan.details["N Bins Y"].Atoi(), tmpChan.details["Low Bin Limit Y"].Atof(), tmpChan.details["High Bin Limit Y"].Atof()}, branchXname.Data(),branchYname.Data());
              //.Histo2D(ROOT::RDF::TH2DModel(), branchXname.Data(),branchYname.Data());
              tmp2Xhist->Delete();
              tmp2Yhist->Delete();
            }
          }
        }
        if (tmpChan.details.count("Dimension")>0 && (tmpChan.details["Dimension"].Atoi()==3)) {
          TString branchXname = Form("x_%s",tmpChan.details["DrawX"].Copy().ReplaceAll(".","_").ReplaceAll("-","_").ReplaceAll("1e3*","").ReplaceAll("1e6*","").Data());
          TString branchYname = Form("y_%s",tmpChan.details["DrawY"].Copy().ReplaceAll(".","_").ReplaceAll("-","_").ReplaceAll("1e3*","").ReplaceAll("1e6*","").Data());
          TString branchZname = Form("z_%s",tmpChan.details["DrawZ"].Copy().ReplaceAll(".","_").ReplaceAll("-","_").ReplaceAll("1e3*","").ReplaceAll("1e6*","").Data());
          if (minirun == "-1"){
            if (tmpChan.details.count("Cut") > 0){
              tmpChan.histo3D = d
                .Alias(branchXname.Data(),tmpChan.details["DrawX"].Data())
                .Define(branchYname.Data(),tmpChan.details["DrawY"].Data())
                .Define(branchZname.Data(),tmpChan.details["DrawZ"].Data())
                .Histo3D(ROOT::RDF::TH3DModel(), branchXname.Data(),branchYname.Data(),branchZname.Data());
            }
            else {
              tmpChan.histo3D = d
                .Alias(branchXname.Data(),tmpChan.details["DrawX"].Data())
                .Define(branchYname.Data(),tmpChan.details["DrawY"].Data())
                .Define(branchZname.Data(),tmpChan.details["DrawZ"].Data())
                .Histo3D(ROOT::RDF::TH3DModel(), branchXname.Data(),branchYname.Data(),branchZname.Data());
            }
          }
          else {
            if (tmpChan.details.count("Cut") > 0){
              tmpChan.histo3D = d
                .Filter(Form("%s",tmpChan.details["Cut"].Data()))
                .Alias(branchXname.Data(),tmpChan.details["DrawX"].Data())
                .Define(branchYname.Data(),tmpChan.details["DrawY"].Data())
                .Define(branchZname.Data(),tmpChan.details["DrawZ"].Data())
                .Filter(Form("reg.minirun==%s",minirun.Data()))
                .Histo3D(ROOT::RDF::TH3DModel(), branchXname.Data(),branchYname.Data(),branchZname.Data());
            }
            else {
              tmpChan.histo3D = d
                .Alias(branchXname.Data(),tmpChan.details["DrawX"].Data())
                .Define(branchYname.Data(),tmpChan.details["DrawY"].Data())
                .Define(branchZname.Data(),tmpChan.details["DrawZ"].Data())
                .Filter(Form("reg.minirun==%s",minirun.Data()))
                .Histo3D(ROOT::RDF::TH3DModel(), branchXname.Data(),branchYname.Data(),branchZname.Data()); // If I added a weight map entry then here is where it would get stuck in (default to 1 for ease?)
            }
          }
        }
        summaries.push_back(tmpChan);
      }
      else {
        try{
          if (minirun != "-1" && (tmpChan.type != "slopes" && tmpChan.type != "slow")){
            if (debug > 1) Printf("Executing \"tmpChan.histo = d_good.Define("+tmpChan.branchName+","+tmpChan.draw+").Filter(Form(\"reg.minirun==%s\".Histo1D("+tmpChan.branchName+")",minirun.Data());
            tmpChan.histo1D = d_good.Alias(tmpChan.branchName.Data(),tmpChan.draw.Data()).Filter(Form("reg.minirun==%s",minirun.Data())).Histo1D(tmpChan.branchName.Data());
            //tmpChan.histo1D = d_good.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Filter(Form("reg.minirun==%s",minirun.Data())).Histo1D({branchXname, tmpChan.details["Histogram Title"], tmpChan.details["N Bins X"].Atoi(), tmpChan.details["Low Bin Limit X"].Atof(), tmpChan.details["High Bin Limit X"].Atoi()}, branchXname.Data());
            channels.push_back(tmpChan);
            if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;
              tsw.Start();}
          }
          else if (tmpChan.type != "slopes" && tmpChan.type != "slow") {
            tmpChan.histo1D = d_good.Alias(tmpChan.branchName.Data(),tmpChan.draw.Data()).Histo1D(tmpChan.branchName.Data());
            channels.push_back(tmpChan);
            if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;
              tsw.Start();}
          }
          if (tmpChan.type == "slow"){
            tmpChan.histo1D = slow.Histo1D(tmpChan.name.Data()); // FIXME want try/except to fill a histo that returns -1e6 mean -1e6 rms
            channels.push_back(tmpChan);
            if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;
              tsw.Start();}
          }
        }
        catch (...){
          Printf("Channel %s not available",tmpChan.draw.Data());
          channels.push_back(tmpChan);
        }
        if (tmpChan.type == "slopes") {
          getSlopes(channels, run.Atoi(), minirun.Atoi(), split.Atoi(), nruns.Atof());
          if (debug > 1) {cout << "Done Getting " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;
            tsw.Start();}
        }
      }
    }
  }

  cout << "Done with getting All Histos --"; tsw.Print();
  tsw.Start();

  for (auto &tmpChan:channels){
    if (tmpChan.type=="meanrms"){
      tmpChan.getData();
      if (debug > 1) {cout<< tmpChan.name << " Mean = :" << tmpChan.avg<< " --"; tsw.Print(); cout <<std::endl;
        tsw.Start();}
    }
    if (tmpChan.type=="slow"){
      tmpChan.getSlowData();
      if (debug > 1) {cout<< tmpChan.name << " Single Entry = :" << tmpChan.singleEntry<< " --"; tsw.Print(); cout <<std::endl;
        tsw.Start();}
    }
    if (tmpChan.type=="slopes"){
      if (debug > 1) {cout<< tmpChan.name << " Slope = :" << tmpChan.slope<< " --"; tsw.Print(); cout <<std::endl;
        tsw.Start();}
    }
  }
  for (auto &tmpChan:summaries){
    if (tmpChan.details.count("Canvas Name") != 0) {
      if (pads.count(tmpChan.details["Canvas Name"])){
      //if (pads.count(tmpChan.details["Canvas Name"].Data()))
        Printf("Trying canvas named %s",tmpChan.details["Canvas Name"].Data());
        tmpChan.drawPlot(pads);
      }
      else {
        Printf("Error, no canvas of name %s available",tmpChan.details["Canvas Name"].Data());
      }
    }
    else {
      Printf("Error, no canvas available for channel: %s",tmpChan.name.Data());
    }
  }
  cout << "Done with getting data --"; tsw.Print(); cout << endl;
  tsw.Start();

  Double_t tmpRunN = run.Atof();
  Double_t tmpNRuns = nruns.Atof();
  Double_t tmpSplitN = split.Atof();
  Double_t tmpMinirunN = minirun.Atof();

  TString outputDir = getOutputDir_h();
  TString aggregatorFileName = Form("%s/aggregator.root",outputDir.Data()); // FIXME, this is very specific, and doesn't allow for aggregating over slugs, for instance
  // Store all trees
  if (tmpMinirunN <= -1) {
    aggregatorFileName = Form("%s/run_aggregator_%d.root",outputDir.Data(),(Int_t)tmpRunN);
  }
  else{
    aggregatorFileName = Form("%s/minirun_aggregator_%d_%d.root",outputDir.Data(),(Int_t)tmpRunN,(Int_t)tmpMinirunN);
  }
  TFile *aggregatorFile = new TFile(aggregatorFileName,"UPDATE");
  aggregatorFile->cd();
  TTree * outputTree = new TTree("agg","Aggregator Tree");
  // Intentionally not using a struct here to match prior aggregator output definition 
  // and to simplify life for other people, and to allow for non-mean kinds of variables to be used
  outputTree->Branch("run_number", &tmpRunN);
  outputTree->Branch("n_runs", &tmpNRuns);
  outputTree->Branch("split_n", &tmpSplitN);
  outputTree->Branch("minirun_n", &tmpMinirunN);

  if (debug > 1) {cout << "Done setting up output tree --"; tsw.Print(); cout << endl;}
  tsw.Start();

  for (Int_t loop = 0 ; loop<channels.size() ; loop++) {
    channels.at(loop).storeData(outputTree);
  }

  outputTree->Fill();
  outputTree->Write("agg",TObject::kOverwrite);
  aggregatorFile->Close();

  TString aggregatorPDFFileName = Form("%s/aggregator_%s",outputDir.Data(),basename.Data()); // FIXME, this is very specific, and doesn't allow for aggregating over slugs, for instance
  if (tmpMinirunN <= -1) {
    aggregatorPDFFileName = Form("%s/run_aggregator_%s_%d",outputDir.Data(),basename.Data(),(Int_t)tmpRunN);
  }
  else {
    aggregatorPDFFileName = Form("%s/minirun_aggregator_%s_%d_%d",outputDir.Data(),basename.Data(),(Int_t)tmpRunN,(Int_t)tmpMinirunN);
  }
  //for (Int_t loop = 0 ; loop<pads.size() ; loop++) 
  Int_t loop = 0;
  for (auto ite : canvasNames){
    if (loop == 0 && canvasNames.size()!=1) {
      pads[ite]->SaveAs(aggregatorPDFFileName+"_full.pdf"+"(");
      //pads[ite]->SaveAs(aggregatorPDFFileName+"_"+loop+".pdf");
      pads[ite]->SaveAs(aggregatorPDFFileName+"_"+loop+".png");
    }
    else if (loop == canvasNames.size()-1 && canvasNames.size()!=1) {
      pads[ite]->SaveAs(aggregatorPDFFileName+"_full.pdf"+")");
      //pads[ite]->SaveAs(aggregatorPDFFileName+"_"+loop+".pdf");
      pads[ite]->SaveAs(aggregatorPDFFileName+"_"+loop+".png");
    }
    else {
      pads[ite]->SaveAs(aggregatorPDFFileName+"_full.pdf");
      //pads[ite]->SaveAs(aggregatorPDFFileName+"_"+loop+".pdf");
      pads[ite]->SaveAs(aggregatorPDFFileName+"_"+loop+".png");
    }
    //gSystem->Exec(Form("convert -density 300 -trim %s_%d.pdf -quality 100 -flatten -sharpen 0x1.0 %s_%d.png",aggregatorPDFFileName.Data(),loop,aggregatorPDFFileName.Data(),loop));
    loop++;
  }
  //gSystem->Exec(Form("pdfunite %s_*.pdf %s.pdf",aggregatorPDFFileName.Data(),aggregatorPDFFileName.Data()));
//  gSystem->Exec(Form("convert %s_*.png %s.pdf",aggregatorPDFFileName.Data(),aggregatorPDFFileName.Data()));
  gSystem->Exec(Form("convert $(ls -rt %s_*.png) %s.pdf",aggregatorPDFFileName.Data(),aggregatorPDFFileName.Data()));
  gSystem->Exec(Form("rm -f $(ls -rt %s_*.png)",aggregatorPDFFileName.Data()));

  cout << "Done with ALL, run " << run << " and minirun " << minirun << " --"; tswAll.Print(); cout << endl;
  tswAll.Start();
  return d;
}
#endif // __CAMDF__
