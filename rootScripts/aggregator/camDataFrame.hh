#ifndef __CAMDF__
#define __CAMDF__
using namespace ROOT;

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
    void getData();
    void getSlowData();
    void storeData(TTree *);
    ROOT::RDF::RResultPtr<TH1D> histo;
    TString name;
    TString branchName;
    //TString branchCuts; Do a search and replace for hw_sum, if exists replace with Device_Error_Code ..... == 0, else no added cut
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

void Channel::getData(){
  if (histo){
    avg = histo->GetMean();
    avgErr = histo->GetMeanError();
    rms = histo->GetRMS();
    rmsErr = histo->GetRMSError();
    nEntries = histo->GetEntries();
  }
  else {
    if (debug>1) {
      Printf("Error, histogram for %s empty",name.Data());
    }
  }
};

void Channel::getSlowData(){
  if (histo){
    singleEntry = histo->GetBinCenter(histo->GetMaximumBin());
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
    outputTree->Branch(Form("%s_mean",name.Data()),&singleEntry);
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
  TString ditStub = gSystem->Getenv("DITHERING_STUB");
  TString ditSlopeFileName = ditSlopeFileNamebase + "/dit_alldet_slopes" + ditStub + "_slug" + nRuns + ".root";
  if( !gSystem->AccessPathName(ditSlopeFileName) ) {
    Printf("Getting dithering slopes from %s",ditSlopeFileName.Data());
    TChain *ditTree = new TChain("dit");
    ditTree->Add(ditSlopeFileName);
    //TLeaf *ditRunNum = ditTree->GetLeaf("cyclenum");
    TObjArray *slopesList = ditTree->GetListOfLeaves();
    TString outname = "";
    /*for (Int_t a = 0; a<ditTree->GetEntries(); a++){
      ditTree->GetEntry(a);
      if (debug>3) Printf("Entry number %d, run number %d",a,(Int_t)ditRunNum->GetValue(0));
      TIter slopesIter(slopesList);
      while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
        if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
        if ((TString)slopes->GetName() != "cyclenum" && (Int_t)ditRunNum->GetValue(0) == runNumber){
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
      if ((TString)slopes->GetName() == "cyclenum" || (TString)slopes->GetName() == "run" || (TString)slopes->GetName() == "flag" ||  (TString)slopes->GetName() == "scandata1" || (TString)slopes->GetName() == "scandata2"){
        Int_t nen = ditTree->Draw(Form("%s",slopes->GetName()),Form("flag==1 && run==%d",runNumber),"goff");
        Channel tmpChan;
        outname = "dit_"+(TString)slopes->GetName()+"_mean";
        tmpChan.type = "meanrms";
        tmpChan.name = "dit_"+(TString)slopes->GetName();
        if (nen == 0) { 
          tmpChan.avg = -1.0e6;
          tmpChan.avgErr = -1.0e6;
          tmpChan.rms = -1.0e6;
          tmpChan.rmsErr = -1.0e6;
          tmpChan.nEntries = 0;
        }
        else {
          TH1* tmpHist = (TH1*)gROOT->FindObject("htemp");
          if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),1e-3*(Double_t)tmpHist->GetMean());
          tmpChan.avg = (Double_t)tmpHist->GetMean();
          tmpChan.avgErr = (Double_t)tmpHist->GetMeanError();
          tmpChan.rms = (Double_t)tmpHist->GetMean();
          tmpChan.rmsErr = (Double_t)tmpHist->GetRMSError();
          tmpChan.nEntries = (Double_t)tmpHist->GetEntries();
        }
        channels.push_back(tmpChan);
      }
      else if (((TString)slopes->GetName()).Contains("coil")){
        Int_t nen = ditTree->Draw(Form("%s",slopes->GetName()),Form("flag==1 && run==%d",runNumber),"goff");
        Channel tmpChan;
        outname = "dit_"+(TString)slopes->GetName()+"_slope";
        tmpChan.type = "slow";
        tmpChan.name = "dit_"+(TString)slopes->GetName();
        if (nen == 0) {
          tmpChan.singleEntry = -1.0e6;
        }
        else {
          TH1* tmpHist = (TH1*)gROOT->FindObject("htemp");
          if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)tmpHist->GetMean());
          if (tmpHist->GetEntries() == 0) {
            tmpChan.singleEntry = (Double_t)tmpHist->GetMean();
          }
          else {
            tmpChan.singleEntry = -1.0e6;
          }
        }
        channels.push_back(tmpChan);
      }
      else if (((TString)slopes->GetName()).Contains("alpha") || ((TString)slopes->GetName()).Contains("beta") || ((TString)slopes->GetName()).Contains("delta") || ((TString)slopes->GetName()).Contains("coil")){
        // SKIP -> Let dithering plots do all this
        continue;
        /*
        ditTree->Draw(Form("%s",slopes->GetName()),Form("flag==1 && run==%d",runNumber),"goff");
        TH1* tmpHist = (TH1*)gROOT->FindObject("htemp");
        outname = "dit_"+(TString)slopes->GetName()+"_slope";
        if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)tmpHist->GetMean());
        Channel tmpChan;
        tmpChan.type = "meanrms";
        tmpChan.name = "dit_"+(TString)slopes->GetName();
        if (tmpHist->GetEntries() == 0) {
          tmpChan.avg = -1.0e6;
          tmpChan.avgErr = -1.0e6;
          tmpChan.rms = -1.0e6;
          tmpChan.rmsErr = -1.0e6;
          tmpChan.nEntries = -1.0e6;
          channels.push_back(tmpChan);
        }
        else {
          tmpChan.avg = (Double_t)tmpHist->GetMean();
          tmpChan.avgErr = (Double_t)tmpHist->GetRMSError();
          tmpChan.rms = (Double_t)tmpHist->GetMean();
          tmpChan.rmsErr = (Double_t)tmpHist->GetRMSError();
          tmpChan.nEntries = (Double_t)tmpHist->GetEntries();
          channels.push_back(tmpChan);
        }
        */
      }
      else if (((TString)slopes->GetName()).Contains("Ndata")) {
        continue;
      }
      else {
        // Do slug averaging
        Int_t nen = ditTree->Draw(Form("%s",slopes->GetName()),Form("flag==1"),"goff");
        Channel tmpChan;
        outname = "dit_"+(TString)slopes->GetName()+"_slope";
        tmpChan.type = "slopes";
        tmpChan.name = "dit_"+(TString)slopes->GetName();
        if (nen == 0) {
          tmpChan.slope = -1.0e6;
          tmpChan.slopeError = -1.0e6;
        }
        else {
          TH1* tmpHist = (TH1*)gROOT->FindObject("htemp");
          if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),1e-3*(Double_t)tmpHist->GetMean());
          tmpChan.slope = 1e-3*(Double_t)tmpHist->GetMean();
          tmpChan.slopeError = 1e-3*(Double_t)tmpHist->GetMeanError();
        }
        channels.push_back(tmpChan);
      }
    }
  }
  TString ditRunSlopeFileNamebase = gSystem->Getenv("DITHERING_ROOTFILES_SLOPES");
  TString BlessedDitRunSlopeFileName = ditRunSlopeFileNamebase + "/dit_alldet_slopes_slug63.root";
  TString ditRunSlopeFileName = ditRunSlopeFileNamebase + "/dit_alldet_slopes_slug" + nRuns + ".root";
  if( 1==0 && !gSystem->AccessPathName(ditRunSlopeFileName) ) { // FIXME Turn off for now
    Printf("Getting dithering slopes from %s",ditRunSlopeFileName.Data());
    TChain *ditTree = new TChain("dit");
    ditTree->Add(ditRunSlopeFileName);
    TChain *BlessedDitTree = new TChain("dit");
    BlessedDitTree->Add(BlessedDitRunSlopeFileName);
    TLeaf *ditRunNum = ditTree->GetLeaf("run");
    TObjArray *slopesList = ditTree->GetListOfLeaves();
    TObjArray *BlessedSlopesList = BlessedDitTree->GetListOfLeaves();
    TString outname = "";
    for (Int_t a = 0; a<ditTree->GetEntries(); a++){
      ditTree->GetEntry(a);
      if (debug>3) Printf("Entry number %d, run number %d",a,(Int_t)ditRunNum->GetValue(0));
      // FIXME This uses the names list from a known working case and then proceeds from there looping over the one in question's data
      TIter slopesIter(BlessedSlopesList);
      while (TLeaf *slopes=(TLeaf*)slopesIter.Next()){
        if (debug>4) Printf("Checking dither slope %s",((TString)slopes->GetName()).Data());
        if ((TString)slopes->GetName() != "run" && (TString)slopes->GetName() != "cyclenum" && (Int_t)ditRunNum->GetValue(0) == runNumber){
          outname = "dit_"+(TString)slopes->GetName()+"_slope";
          Int_t nen = ditTree->Draw(Form("%s",slopes->GetName()),Form("run==%d && flag==1",runNumber),"goff");
          Channel tmpChan;
          tmpChan.type = "slopes";
          tmpChan.name = "dit_run_"+(TString)slopes->GetName();
          if (nen == 0) {
            tmpChan.slope      = -1.0e6;
            tmpChan.slopeError = -1.0e6;
          }
          else {
            TH1* tmpHist = (TH1*)gROOT->FindObject("htemp");
            if (tmpHist != 0){
              tmpChan.slope      = 1e-3*(Double_t)tmpHist->GetMean();
              tmpChan.slopeError = 1e-3*(Double_t)tmpHist->GetMeanError();
            }
            else {
              tmpChan.slope      = -1.0e6;
              tmpChan.slopeError = -1.0e6;
            }
          }
          // FIXME This method assumes the run and cycle exist in the slopes file (not true if it failed at slope calculation)
          //if (debug>5) Printf("Adding to agg: %s = %f",outname.Data(),(Double_t)slopes->GetValue(0));
          //tmpChan.slopeError = 0.0001; // Dithering has no errors?
          //if (abs((Double_t)slopes->GetValue(0))<200.0) { 
          //  tmpChan.slope = 1e-3*(Double_t)slopes->GetValue(0);
          //  tmpChan.slopeError = -1.0e6;
          //}
          channels.push_back(tmpChan);
        }
      }
    } // Loop Over Entries - Ideal per/cycle way of doing things FIXME
  }
  if(minirunNumber<0){
    TString lrbFileNameBase = gSystem->Getenv("LRB_ROOTFILES");
    TString lrbFileName = lrbFileNameBase + "/blueR"+runNumber+".000new.slope.root";  
    if ( !gSystem->AccessPathName(lrbFileName)) {
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
  TChain * mul_tree      = new TChain("mul");
  TChain * slow_tree     = new TChain("slow");
  TChain * reg_tree      = new TChain("reg");
  TChain * dit_tree      = new TChain("dit");
  TChain * mini_tree     = new TChain("mini");
  TChain * mulc_tree     = new TChain("mulc");
  TChain * mulc_lrb_burst_tree = new TChain("mulc_lrb_burst");
  TChain * mulc_lrb_alldet_burst_tree = new TChain("mulc_lrb_alldet_burst");

  TString baseDir = gSystem->Getenv("QW_ROOTFILES");
  TString postpanBaseDir = gSystem->Getenv("POSTPAN_ROOTFILES");
  Printf("Checking for postpan files in %s",postpanBaseDir.Data());
  if ( gSystem->AccessPathName(Form("%s/%s_%s.%s.root",baseDir.Data(),basename.Data(), run.Data(),split.Data())) ) {
    Printf("%s/%s_%s.%s.root not found",baseDir.Data(),basename.Data(), run.Data(),split.Data());
  }
  mul_tree->Add(Form("%s/%s_%s.%s.root",baseDir.Data(),basename.Data(), run.Data(),split.Data()));
  slow_tree->Add(Form("%s/%s_%s.%s.root",baseDir.Data(),basename.Data(), run.Data(),split.Data()));
  Int_t reg_tree_valid = 0;
  if ( !gSystem->AccessPathName(Form("%s/prexPrompt_%s_%s_regress_postpan.root",postpanBaseDir.Data(), run.Data(),split.Data())) ) {
    Printf("Using %s/prexPrompt_%s_%s_regress_postpan.root",postpanBaseDir.Data(), run.Data(),split.Data());
    reg_tree->Add(Form("%s/prexPrompt_%s_%s_regress_postpan.root",postpanBaseDir.Data(), run.Data(),split.Data()));
    mini_tree->Add(Form("%s/prexPrompt_%s_%s_regress_postpan.root",postpanBaseDir.Data(), run.Data(),split.Data()));
    reg_tree_valid = 1;
  }
  else {
    Printf("No postpan file");
  }
  TString ditheringFileNameDF = gSystem->Getenv("DITHERING_ROOTFILES");
  TString ditheringFileStub = gSystem->Getenv("DITHERING_STUB");
  if (ditheringFileNameDF != ""){
    Printf("Looking for Dithering corrected files in %s",ditheringFileNameDF.Data());
    dit_tree->Add(Form("%s/prexPrompt_dither%s_%s_000.root", ditheringFileNameDF.Data(), ditheringFileStub.Data(), run.Data()));
  }
  TFile tmpFile(Form("%s/%s_%s.%s.root", baseDir.Data(),basename.Data(), run.Data(),split.Data()));
  Int_t mulc_valid = (tmpFile.GetListOfKeys())->Contains("mulc");
  mulc_tree->Add(Form("%s/%s_%s.%s.root",baseDir.Data(),basename.Data(), run.Data(),split.Data()));
  mulc_lrb_burst_tree->Add(Form("%s/%s_%s.%s.root", baseDir.Data(),basename.Data(), run.Data(),split.Data()));
  Int_t mulc_lrb_alldet_burst_valid = (tmpFile.GetListOfKeys())->Contains("mulc_lrb_alldet_burst");
  mulc_lrb_alldet_burst_tree->Add(Form("%s/%s_%s.%s.root", baseDir.Data(),basename.Data(), run.Data(),split.Data()));

  if (reg_tree_valid) {
    mul_tree->AddFriend(reg_tree);
  }
  ///mul_tree->AddFriend(mulc_tree);
  TString outputDir = getOutputDir_h();
  //if (outputDir.Contains("SAM") || outputDir.Contains("AT")) {
  //  mul_tree->AddFriend(mulc_tree);
  //}
  //else {
    if (ditheringFileNameDF != ""){
      Printf("Obtained Dithering corrected files in %s",ditheringFileNameDF.Data());
      if (dit_tree) {
        Printf("Using dit tree");
        mul_tree->AddFriend(dit_tree);
      }
   //   mul_tree->AddFriend(mulc_tree);
    }
  //  else {
  //    Printf("Using mulc_lrb_burst");
  //    mul_tree->AddFriend(mulc_lrb_burst_tree);
   //   mul_tree->AddFriend(mulc_tree);
   // }
  //}
  if (mulc_lrb_alldet_burst_valid) {
    Printf("Using mulc_lrb_alldet_burst");
    mul_tree->AddFriend(mulc_lrb_alldet_burst_tree);
  }
  if (mulc_valid) {
    mul_tree->AddFriend(mulc_tree);
  }

  //miniruns = mini_tree->Scan("minirun",""); // FIXME for later minirun looping addition

  RDataFrame d(*mul_tree);//,device_list);
  RDataFrame slow(*slow_tree);
  if (debug > 1) { cout << "Filtering through ErrorFlag==0 --"; tsw.Print(); cout << endl; }
  tsw.Start();
//////// FIXME default CREX cut: auto d_good=d.Filter("reg.ok_cut==1");
  //auto d_good=d.Filter("(ErrorFlag&0xda7e6bff)==0");
  Int_t test = 0;
  auto d_good=d.Filter([test](Double_t c){
      return ((((Int_t)c))==test); // ErrorFlag == 0 -> All Global cuts pass
      //return ((((Int_t)c)&0xda7e6bff)==test); // ErrorFlag&0xda7e6bff -> (BMOD is active || Global ErrorFlag cut) == 0
      //return ((((Int_t)c)&0xda7e6bff)==test && (((Int_t)c)&0x9000)==0x9000); // ErrorFlag&0xda7e6bff -> (BMOD is active && Global ErrorFlag cut) == 0 -> BMOD only
      //return ((((Int_t)c)&0x99726bff)==0 && (((Int_t)c)&0x20000000)==0x20000000); // Explicitly look at all burp cut failing data, but still cut out beam trip and Beam+Regular stability cuts, and user "bad" event cuts, and all device cuts -> Include BMOD
      }
      ,{"ErrorFlag"});
  //auto d_good = d.Filter"(ErrorFlag&0xda7e6bff)==0");

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

  // Getting device list
  string line;
  std::vector<string> tokens;
  string typedefault = "meanrms";
  ifstream infile(input);
  if (infile.is_open()){
    while(getline(infile,line)){
      string token;
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
      }
      else { 
        Printf("Error: invalid input file");
        return 1; 
      }
      if (tokens.size() > 2){
        type=tokens.at(2);
      }
      else {
        type=typedefault;
      }
      tokens.clear();
      Channel tmpChan;
      tmpChan.name = name;
      tmpChan.branchName = "agg_"+name;
      tmpChan.draw = device;
      tmpChan.type = type;
      try{
        if (minirun != "-1" && (tmpChan.type != "slopes" && tmpChan.type != "slow")){
          //if (((TString)tmpChan.branchName.Data()).Contains(".hw_sum")) {
          //  TString errCode(tmpChan.branchName(0,tmpChan.branchName.Length()-7));
          //  errCode = errCode + ".Device_Error_Code";
          //  if (debug > -1) Printf("Executing \"tmpChan.histo = d_good.Define("+tmpChan.branchName+","+tmpChan.draw+").Filter(Form(\"reg.minirun==%s&&%s==0))\".Histo1D("+tmpChan.branchName+")",minirun.Data(),errCode.Data());
            //tmpChan.histo = d_good.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Filter(Form("reg.minirun==%s&&%s==0",minirun.Data(),errCode.Data())).Histo1D(tmpChan.branchName.Data());
          //}
          //else {
          if (debug > 1) Printf("Executing \"tmpChan.histo = d_good.Define("+tmpChan.branchName+","+tmpChan.draw+").Filter(Form(\"reg.minirun==%s\".Histo1D("+tmpChan.branchName+")",minirun.Data());
          // FIXME Original Postpan dependent minirun numbering//  
          //tmpChan.histo = d_good.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Filter(Form("reg.minirun==%s",minirun.Data())).Histo1D(tmpChan.branchName.Data());
          // FIXME NEW JAPAN BASED 
          tmpChan.histo = d_good.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Filter(Form("BurstCounter==%s",minirun.Data())).Histo1D(tmpChan.branchName.Data());
          /*tmpChan.histo = d_good.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Filter([minirun.Data()](Double_t b) {
          *    return (((Int_t)b)==(Int_t)atoi(minirun.c_str()));
          *    }
          *    ,{"BurstCounter"}).Histo1D(tmpChan.branchName.Data());
          */
          //}
          channels.push_back(tmpChan);
          if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;}
          tsw.Start();
        }
        else if (tmpChan.type != "slopes" && tmpChan.type != "slow") {
          tmpChan.histo = d_good.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Histo1D(tmpChan.branchName.Data());
          channels.push_back(tmpChan);
          if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;}
          tsw.Start();
        }
        if (tmpChan.type == "slow"){
          tmpChan.histo = slow.Define(tmpChan.branchName.Data(),tmpChan.draw.Data()).Histo1D(tmpChan.branchName.Data());
          //tmpChan.histo = slow.Histo1D(tmpChan.name.Data()); // FIXME want try/except to fill a histo that returns -1e6 mean -1e6 rms
          channels.push_back(tmpChan);
          if (debug > 1) {cout << "Done Getting Histo1D for " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;}
          tsw.Start();
        }
      }
      catch (...){
        Printf("Channel %s not available",tmpChan.draw.Data());
        channels.push_back(tmpChan);
      }
      if (tmpChan.type == "slopes") {
        getSlopes(channels, run.Atoi(), minirun.Atoi(), split.Atoi(), nruns.Atof());
        if (debug > 1) {cout << "Done Getting " << tmpChan.draw.Data() << " --"; tsw.Print(); cout << endl;}
        tsw.Start();
      }
    }
  }

  cout << "Done with getting All Histos --"; tsw.Print();
  tsw.Start();

  for (auto &tmpChan:channels){
    if (tmpChan.type=="meanrms"){
      tmpChan.getData();
      if (debug > 1) {cout<< tmpChan.name << " Mean = :" << tmpChan.avg<< " --"; tsw.Print(); cout <<std::endl;}
      tsw.Start();
    }
    if (tmpChan.type=="slow"){
      tmpChan.getSlowData();
      if (debug > 1) {cout<< tmpChan.name << " Single Entry = :" << tmpChan.singleEntry<< " --"; tsw.Print(); cout <<std::endl;}
      tsw.Start();
    }
    if (tmpChan.type=="slopes"){
      if (debug > 1) {cout<< tmpChan.name << " Slope = :" << tmpChan.slope<< " --"; tsw.Print(); cout <<std::endl;}
      tsw.Start();
    }
  }
  cout << "Done with getting data --"; tsw.Print(); cout << endl;
  tsw.Start();

  Double_t tmpRunN = run.Atof();
  Double_t tmpNRuns = nruns.Atof();
  Double_t tmpSplitN = split.Atof();
  Double_t tmpMinirunN = minirun.Atof();

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

  //for (auto tmpChan:channels) 
  for (Int_t loop = 0 ; loop<channels.size() ; loop++) {
    channels.at(loop).storeData(outputTree);
  }

  outputTree->Fill();
  outputTree->Write("agg",TObject::kOverwrite);
  aggregatorFile->Close();

  cout << "Done with ALL, run " << run << " and minirun " << minirun << " --"; tswAll.Print(); cout << endl;
  tswAll.Start();
  return d;
}
#endif // __CAMDF__
