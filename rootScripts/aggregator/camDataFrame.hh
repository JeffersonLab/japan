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
  if (type == "rcdb"){
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
    std::vector<TString> rcdbNames;
    void printInfo() { std::cout << "Processing run  " << run  << ". " << std::endl;} 
    void drawAll();
    void getSlopes(std::vector<Channel>&, Int_t, Int_t, Int_t, Double_t);
    void getRCDB(std::vector<Channel>&, Int_t, Int_t, Int_t, Double_t);
    Channel GetChannelByName(TString name);    
};

void Source::getRCDB(std::vector<Channel> &channels, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  this->rcdbNames = {
    "slug",
    "run_type",       // = 'Production'
    "run_flag",       // = 'Good'
    "ihwp",           // = 'IN' or 'OUT'
    "flip_state",     // = 'FLIP-RIGHT' or 'FLIP-LEFT'
    "horizontal_wien",// = -29.6402
    "vertical_wien",  // = 88.0008
    "rhwp",           // = 1500.0
    "arm_flag",       // = 0, or 1, or 2
    "beam_current",   // = 109.044
    "total_charge",   // = 584807.0
    "target_type",    // = '48Ca', or '40Ca'
    "target_45encoder",//= 2.0
    "target_90encoder",//= 13163100.0
    "dpp",            // = 0.000142342
    "dpp_stdev",      // = 1.4e-6
    "energy_avg",     // = 2.156234
    "energy_sig",     // = 1.4e-6
  };
  Int_t success = 0;
  // Experimenting Function to Get slug number based on run number 
  // Author : Tao Ye

  TSQLResult* res;
  TSQLRow *row;
  cout << "Getting Info from RCDB" << endl;
  //cout << " -- Connecting to RCDB -- " << endl;
  TSQLServer *rcdb = TSQLServer::Connect("mysql://hallcdb.jlab.org:3306/a-rcdb","rcdb","");
  //cout << " -- ServerInfo: " << rcdb->ServerInfo() << endl;
  cout << " -- Host : " << rcdb->GetHost() << endl;
  //cout << " -- Query DataBase " << endl;
  TString select_q ="SELECT run_number,name,int_value ";  //int_value
  TString from_q =  "FROM `a-rcdb`.conditions,`a-rcdb`.condition_types ";
  TString where_q = Form("WHERE conditions.condition_type_id=condition_types.id and name='slug' and run_number='%d'",
			 runNumber);
  res = rcdb->Query(select_q + from_q + where_q);
  if(res==NULL){
    cout << " -- ERROR: Failed to Query " << endl;
    delete row;
    delete res;
    cout << " -- Closing Connection to RCDB " << endl;
    rcdb->Close();
    delete rcdb;
    success = -1;
  }

  int nFields =res->GetFieldCount();
  row = res->Next();
  if(row==NULL){
    cout << " -- ERROR: Failed to load slug number " << endl;
    delete row;
    delete res;
    cout << " -- Closing Connection to RCDB " << endl;
    rcdb->Close();
    delete rcdb;
    success = -1;
  }
  //cout << " ----------------------------- " << endl;
  //for(int j=0; j< nFields; j++)
    //cout << "\t" << row->GetField(j) ;
  //cout << endl;
  const char* tmp_char = row->GetField(2);
  int slug_id = TString(tmp_char).Atoi();
  int wien = 0;
  int ihwp = 0;
  int sign = 0;
  if ( ((Int_t)slug_id) != ((Int_t)nRuns) ){
    std::cout << " -- ERROR: Slug number from RCDB disagrees with slug number in aggregator command line arguments" << std::endl;
  }
  // For each rcdb entry get the entry from RCDB, convert to double or integer, and fill a tmpChan into the vector channels into a singleEntry field.
  for (Int_t i = 0 ; i<this->rcdbNames.size() ; i++) {
    Channel tmpChan;
    tmpChan.type = "rcdb";
    tmpChan.name = "rcdb_"+(TString)this->rcdbNames.at(i);
    where_q = Form("WHERE conditions.condition_type_id=condition_types.id and name='%s' and run_number='%d'",
        this->rcdbNames.at(i).Data(),runNumber);
    if (this->rcdbNames.at(i) == "slug" || this->rcdbNames.at(i) == "arm_flag") {
      select_q ="SELECT run_number,name,int_value ";
    }
    else if (this->rcdbNames.at(i) == "run_type" || this->rcdbNames.at(i) == "run_flag" || this->rcdbNames.at(i) == "ihwp" || this->rcdbNames.at(i) == "flip_state" || this->rcdbNames.at(i) == "target_type") {
      select_q ="SELECT run_number,name,text_value ";
    }
    else{
      select_q ="SELECT run_number,name,float_value ";
    }
    res = rcdb->Query(select_q + from_q + where_q);
    nFields =res->GetFieldCount();
    row = res->Next();
    if(row==NULL || nFields <= 1 || success==-1){
      std::cout << " -- RCDB failed to load " << this->rcdbNames.at(i) << std::endl;
      tmpChan.singleEntry = -1.0e6;
    }
    else {
      // Good connection, good stuff
      const char* tmp_char2 = row->GetField(2);
      if (debug > 1) std::cout<<"For " << row->GetField(1) << " we have " << row->GetField(2)<<std::endl;
      if ( this->rcdbNames.at(i) == "run_type" ){
        if ((TString)tmp_char2 == "Production"){
          tmpChan.singleEntry = 1.0;
        }
        else if ((TString)tmp_char2 == "Pedestal"){
          tmpChan.singleEntry = 2.0;
        }
        else if ((TString)tmp_char2 == "Calibration"){
          tmpChan.singleEntry = 3.0;
        }
        else if ((TString)tmp_char2 == "ParityScan"){
          tmpChan.singleEntry = 4.0;
        }
        else if ((TString)tmp_char2 == "Test"){
          tmpChan.singleEntry = 5.0;
        }
        else {
          tmpChan.singleEntry = -1.0;
        }
      }
      else if ( this->rcdbNames.at(i) == "run_flag" ){
        if ((TString)tmp_char2 == "Good") {
          tmpChan.singleEntry = 1.0;
        }
        else if ((TString)tmp_char2 == "NeedCut" || (TString)tmp_char2 == "NeedCuts") {
          tmpChan.singleEntry = 2.0;
        }
        else if ((TString)tmp_char2 == "Suspicious") {
          tmpChan.singleEntry = 3.0;
        }
        else if ((TString)tmp_char2 == "Bad") {
          tmpChan.singleEntry = 4.0;
        }
        else {
          tmpChan.singleEntry = -1.0;
        }
      }
      else if ( this->rcdbNames.at(i) == "ihwp" ){
        if ((TString)tmp_char2 == "IN"){
          tmpChan.singleEntry = 1.0;
          ihwp = 1;
        }
        else if ((TString)tmp_char2 == "OUT"){
          tmpChan.singleEntry = 2.0;
          ihwp = -1;
        }
        else {
          tmpChan.singleEntry = -1.0;
          ihwp = 0;
        }
      }
      else if ( this->rcdbNames.at(i) == "flip_state" ){
        if ((TString)tmp_char2 == "Vertical(UP)"){
          // Transverse Asymmetry
          tmpChan.singleEntry = 3.0;
          wien = 1;
        }
        else if ((TString)tmp_char2 == "FLIP-LEFT"){
          tmpChan.singleEntry = 2.0;
          wien = -1;
        }
        else if ((TString)tmp_char2 == "FLIP-RIGHT"){
          tmpChan.singleEntry = 1.0;
          wien = 1;
        }
        else {
          tmpChan.singleEntry = -1.0;
          wien = 0;
        }
      }
      else if ( this->rcdbNames.at(i) == "target_type" ){
        if ((TString)tmp_char2 == "48Ca"){
          // Transverse Asymmetry
          tmpChan.singleEntry = 1.0;
        }
        else if ((TString)tmp_char2 == "40Ca"){
          tmpChan.singleEntry = 2.0;
        }
        else if (((TString)tmp_char2).Contains("Pb")){
          tmpChan.singleEntry = 4.0;
        }
        else if (((TString)tmp_char2).Contains("Carbon 1")){
          tmpChan.singleEntry = 3.0;
        }
        else if ((TString)tmp_char2 == "Home"){
          tmpChan.singleEntry = 5.0;
        }
        else {
          tmpChan.singleEntry = -1.0;
        }
      }
      else {
        tmpChan.singleEntry = TString(tmp_char2).Atof();
      }
    }
    channels.push_back(tmpChan);
  }
  Channel signChan;
  signChan.name = "rcdb_sign";
  signChan.type = "rcdb";
  signChan.singleEntry = ((Double_t)(wien*ihwp));
  channels.push_back(signChan);
  delete row;
  //cout << " ----------------------------- " << endl;
  //cout << " -- Slug Number found  " << slug_id << endl;
  //cout << " -- Closing Connection " << endl;
  rcdb->Close();
  delete res;
  delete rcdb;
}

void Source::getSlopes(std::vector<Channel> &channels, Int_t runNumber = 0, Int_t minirunNumber = -2, Int_t splitNumber = -1, Double_t nRuns = -1){
  //runNumber = getRunNumber_h(runNumber);
  //splitNumber = getSplitNumber_h(splitNumber);
  //minirunNumber = getMinirunNumber_h(minirunNumber);
  //nRuns     = getNruns_h(nRuns);
  Double_t data_slope = 0;
  Double_t data_slope_error = 0;
  Printf("Getting slopes");

  TString ditSlopeFileNamebase = gSystem->Getenv("DITHERING_ROOTFILES_SLOPES");
  TString ditStub = gSystem->Getenv("DITHERING_STUB");
  TString ditSlopeFileName = ditSlopeFileNamebase + "/dit_alldet_slopes" + ditStub + "_slug" + nRuns + ".root";
  if( gSystem->AccessPathName(ditSlopeFileName) ) {
    Printf("Could not get dithering slopes from %s, trying experiment long version instead",ditSlopeFileName.Data());
    ditSlopeFileName = ditSlopeFileNamebase + "/dit_alldet_slopes" + ditStub + ".root";
  }
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
    Int_t localSegment = 1;
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
        // // FIXME This "slug averaging" assumes that the root file passed in is itself 1 slug at once. 
        // // In fact it could have been 1 slug with segments (correction is applied segmentwise), and for CREX respin1 I am now using 1 big giant rootfile with segments as the delimeter, rather than slug number
        // // Therefore let's adjust this algorithm to match the slope calculation used in segment-wise averaging
        /*
        Int_t nen = ditTree->Draw(Form("%s",slopes->GetName()),Form("flag==1"),"goff");
        */

        if (ditTree->GetBranch("segment")) {
          // Used to be that I would scan through the slug and find the nearest good run's segment number. Now, just get the run's segment directly
          //Int_t nEnts = ditTree->Draw(">>elistSeg",Form("run>=%d && flag==1",runNumber));//,run_cut); run>= cut allows for non-existant runs to be included
          // FIXME this fails to grab the nearest segment value ... can easily be done if using BuildIndex and GetEntryNumberWithIndex and GetEntry
          Int_t nEnts = ditTree->Draw(">>elistSeg",Form("run==%d",runNumber));//,run_cut); run>= cut allows for non-existant runs to be included
          TEventList *elistSeg = (TEventList*)gDirectory->Get("elistSeg");
          TLeaf* segmentL = ditTree->GetLeaf("segment");
          segmentL->GetBranch()->GetEntry(elistSeg->GetEntry(0));
          localSegment = segmentL->GetValue(); // Got the segment value of the 1st cycle in run runNumber
          if (localSegment == 0) {
            // I can do this kind of time looping test, or I could just add a cut on segment to the above Draw command... I kinda like this loop though
            for (Int_t n = 0 ; n < nEnts ; n++ ) {
              segmentL->GetBranch()->GetEntry(elistSeg->GetEntry(n));
              localSegment = segmentL->GetValue();
              if (localSegment != 0) {
                // Grab the first non-zero entry to get the probably accurate segment number
                // This allows us to get corrections for runs with no BMOD cycles in a given slug dithering file
                break;
              }
            }
          }
          if (localSegment==0) // Special case for broken rootfile... find the nearest segment number 
          {
            localSegment=1;
            // Just do the whole slug and use the largest segment number for runs >= the current run number
            Int_t nEnts = ditTree->Draw(">>elistSeg2","flag==1");//,run_cut); run>= cut allows for non-existant runs to be included
            TEventList *elistSeg2 = (TEventList*)gDirectory->Get("elistSeg2");
            TLeaf* segmentL = ditTree->GetLeaf("segment");
            TLeaf* runL = ditTree->GetLeaf("run");
            segmentL->GetBranch()->GetEntry(elistSeg2->GetEntry(0));
            runL->GetBranch()->GetEntry(elistSeg2->GetEntry(0));
            localSegment = segmentL->GetValue(); // Got the segment value of the 1st cycle in run runNumber
            for (Int_t j = 0; j<nEnts; j++) {
              segmentL->GetBranch()->GetEntry(elistSeg2->GetEntry(j));
              runL->GetBranch()->GetEntry(elistSeg2->GetEntry(j));
              if (runL->GetValue() <= runNumber)
              {
                localSegment = segmentL->GetValue();
              }
            }
            if (localSegment == 0) {
              localSegment = 1;
            }
          }
        }

        Int_t nen = ditTree->Draw(Form("%s",slopes->GetName()),Form("flag==1 && segment==%d",localSegment),"goff");
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
    Printf("Using segment %d of the slug for slug averaged slope calculation",localSegment);
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
  //debug=10;

  TStopwatch tsw;
  TStopwatch tswAll;
  tsw.Start();
  tswAll.Start();
  if (debug > 1) { cout << "Beginning TChain Setup --"; tsw.Print(); cout << endl;}
  tsw.Start();
  TChain * evt_tree      = new TChain("evt");
  TChain * evt_tree2      = new TChain("evt");
  TChain * mul_tree      = new TChain("mul");
  TChain * mul_tree2      = new TChain("mul");
  TChain * slow_tree     = new TChain("slow");
  TChain * reg_tree      = new TChain("reg");
  TChain * dit_tree      = new TChain("dit");
  TChain * mini_tree     = new TChain("mini");
  TChain * mulc_tree     = new TChain("mulc");
  TChain * mulc_lrb_burst_tree = new TChain("mulc_lrb_burst");
  TChain * mulc_lrb_evMon_burst_tree = new TChain("mulc_lrb_evMon_burst");
  TChain * mulc_lrb_all_burst_tree = new TChain("mulc_lrb_all_burst");
  TChain * mulc_lrb_alldet_burst_tree = new TChain("mulc_lrb_alldet_burst");
  TChain * mulc_lrb_alldet_evMon_burst_tree = new TChain("mulc_lrb_alldet_evMon_burst");
  TChain * mulc_dit_tree = new TChain("mulc_dit");
  TChain * mulc_dit_evMon_tree = new TChain("mulc_dit_evMon");
  TChain * mulc_dit_combo_tree = new TChain("mulc_dit_combo");
  TChain * mulc_dit_evMon_combo_tree = new TChain("mulc_dit_evMon_combo");

  TString baseDir = gSystem->Getenv("QW_ROOTFILES");
  TString postpanBaseDir = gSystem->Getenv("POSTPAN_ROOTFILES");
  Printf("Checking for postpan files in %s",postpanBaseDir.Data());
  TString base_file_name = Form("%s/%s_%s.%s.root",baseDir.Data(),basename.Data(), run.Data(),split.Data());
  if ( gSystem->AccessPathName(base_file_name) ) {
    Printf("%s not found!",base_file_name.Data());
    TString stemlist[5] = {"prexPrompt_pass2_",
      "prexPrompt_pass1_", 
      "prexALL_",
      "prexALLminusR_",
      "prexinj_"};
    for  (int i=0; i<5; i++){
      base_file_name = Form("%s/%s%s.%s.root",baseDir.Data(),
          stemlist[i].Data(),run.Data(),split.Data());
      if (!gSystem->AccessPathName(base_file_name)) {break;}
    }
    if (!gSystem->AccessPathName(base_file_name)) {
      std::cerr << "Opened file "<< base_file_name << std::endl;
    } else {
      std::cerr << "No file found for run " << run << " in path " 
          << baseDir << std::endl;
      return NULL;
    }
  }
  evt_tree->Add(base_file_name);
  evt_tree2->Add(base_file_name);
  mul_tree->Add(base_file_name);
  mul_tree2->Add(base_file_name);
  slow_tree->Add(base_file_name);
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
  Printf("Checking for Dithering corrected file prexPrompt_dither%s_%s_000.root in %s",ditheringFileStub.Data(), run.Data(),ditheringFileNameDF.Data());
  if (ditheringFileNameDF != "" && !gSystem->AccessPathName(Form("%s/prexPrompt_dither%s_%s_000.root", ditheringFileNameDF.Data(), ditheringFileStub.Data(), run.Data()))) {
    Printf("Getting Dithering corrected files in %s",ditheringFileNameDF.Data());
    dit_tree->Add(Form("%s/prexPrompt_dither%s_%s_000.root", ditheringFileNameDF.Data(), ditheringFileStub.Data(), run.Data()));
  }
  TFile tmpFile(base_file_name);
  Int_t mulc_valid = (tmpFile.GetListOfKeys())->Contains("mulc");
  mulc_tree->Add(base_file_name);
  Int_t mulc_lrb_burst_valid = (tmpFile.GetListOfKeys())->Contains("mulc_lrb_burst");
  mulc_lrb_burst_tree->Add(base_file_name);
  Int_t mulc_dit_valid = (tmpFile.GetListOfKeys())->Contains("mulc_dit");
  mulc_dit_tree->Add(base_file_name);
  Int_t mulc_dit_combo_valid = (tmpFile.GetListOfKeys())->Contains("mulc_dit_combo");
  mulc_dit_combo_tree->Add(base_file_name);
  Int_t mulc_lrb_all_burst_valid = (tmpFile.GetListOfKeys())->Contains("mulc_lrb_all_burst");
  mulc_lrb_all_burst_tree->Add(base_file_name);
  Int_t mulc_lrb_alldet_burst_valid = (tmpFile.GetListOfKeys())->Contains("mulc_lrb_alldet_burst");
  mulc_lrb_alldet_burst_tree->Add(base_file_name);
  Int_t mulc_lrb_evMon_burst_valid = (tmpFile.GetListOfKeys())->Contains("mulc_lrb_evMon_burst");
  mulc_lrb_evMon_burst_tree->Add(base_file_name);
  Int_t mulc_lrb_evMon_alldet_burst_valid = (tmpFile.GetListOfKeys())->Contains("mulc_lrb_alldet_evMon_burst");
  mulc_lrb_alldet_evMon_burst_tree->Add(base_file_name);
  Int_t mulc_dit_evMon_valid = (tmpFile.GetListOfKeys())->Contains("mulc_dit_evMon");
  mulc_dit_evMon_tree->Add(base_file_name);
  Int_t mulc_dit_evMon_combo_valid = (tmpFile.GetListOfKeys())->Contains("mulc_dit_evMon_combo");
  mulc_dit_evMon_combo_tree->Add(base_file_name);

  evt_tree->AddFriend(evt_tree2,"evt");
  mul_tree->AddFriend(mul_tree2,"mul");
  if (reg_tree_valid) {
    mul_tree->AddFriend(reg_tree);
  }
  ///mul_tree->AddFriend(mulc_tree);
  //// FIXME remove dependence on camIO.hh
  TString outputDir = gSystem->Getenv("CAM_OUTPUTDIR");
  if (outputDir == "" || outputDir == "NULL"){
    Printf("Error: Output dir (%s) invalid, must be a string\n",outputDir.Data());
    outputDir = "./";
  }
  //if (outputDir.Contains("SAM") || outputDir.Contains("AT")) {
  //  mul_tree->AddFriend(mulc_tree);
  //}
  //else {
    if (ditheringFileNameDF != "" && !gSystem->AccessPathName(Form("%s/prexPrompt_dither%s_%s_000.root", ditheringFileNameDF.Data(), ditheringFileStub.Data(), run.Data()))) {
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
  if (mulc_lrb_burst_valid) {
    Printf("Using mulc_lrb_burst");
    mul_tree->AddFriend(mulc_lrb_burst_tree);
  }
  if (mulc_dit_valid) {
    Printf("Using mulc_dit");
    mul_tree->AddFriend(mulc_dit_tree);
  }
  if (mulc_dit_combo_valid) {
    Printf("Using mulc_dit_combo");
    mul_tree->AddFriend(mulc_dit_combo_tree);
  }
  if (mulc_lrb_all_burst_valid) {
    Printf("Using mulc_lrb_all_burst");
    mul_tree->AddFriend(mulc_lrb_all_burst_tree);
  }
  if (mulc_lrb_alldet_burst_valid) {
    Printf("Using mulc_lrb_alldet_burst");
    mul_tree->AddFriend(mulc_lrb_alldet_burst_tree);
  }
  if (mulc_lrb_evMon_burst_valid) {
    Printf("Using mulc_lrb_evMon_burst");
    mul_tree->AddFriend(mulc_lrb_evMon_burst_tree);
  }
  if (mulc_lrb_evMon_alldet_burst_valid) {
    Printf("Using mulc_lrb_alldet_evMon_burst");
    mul_tree->AddFriend(mulc_lrb_alldet_evMon_burst_tree);
  }
  if (mulc_dit_evMon_valid) {
    Printf("Using mulc_dit_evMon");
    mul_tree->AddFriend(mulc_dit_evMon_tree);
  }
  if (mulc_dit_evMon_combo_valid) {
    Printf("Using mulc_dit_evMon_combo");
    mul_tree->AddFriend(mulc_dit_evMon_combo_tree);
  }
  if (mulc_valid) {
    Printf("Using mulc");
    mul_tree->AddFriend(mulc_tree);
  }

  //miniruns = mini_tree->Scan("minirun",""); // FIXME for later minirun looping addition
  //

  TString cutChoice = gSystem->Getenv("CAM_CUT");

  RDataFrame e(*evt_tree);//,device_list);
  RDataFrame d(*mul_tree);//,device_list);
  RDataFrame slow(*slow_tree);
  if (debug > 1) { cout << "Filtering through ErrorFlag==0 --"; tsw.Print(); cout << endl; }
  tsw.Start();
  Int_t test = 0;
  // c++ 11 lambda function - drdobbs.com/cpp/lambdas-in-c11/240168241
  //    [capture section] using = instead of test,cutChoice auto-grabs all values referenced. 
  //    "-> int" is optional type casting for compiler simplicity
  auto metCut = [test,cutChoice](Double_t c) -> bool {
    if (cutChoice=="" || cutChoice=="Default" || cutChoice=="ErrorFlag") {
      return ((((Int_t)c))==test);
    }
    if (cutChoice=="BMOD" || cutChoice=="IncludeBMOD") {
      return ((((Int_t)c)&0xda7e6bff)==test);
    }
    if (cutChoice=="OnlyBMOD" || cutChoice=="BMODonly") {
      return ((((Int_t)c)&0xda7e6bff)==test && (((Int_t)c)&0x9000)==0x9000);
    }
    if (cutChoice=="BurpOnly" || cutChoice=="BurpFailed") {
      return ((((Int_t)c)&0x99726bff)==test && (((Int_t)c)&0x20000000)==0x20000000);
    }
    if (cutChoice=="BeamOff" || cutChoice=="Pedestal" || cutChoice=="Pedestals") {
      //return (c < 3.0);
      //return (c < -185.0);
      return ((c/4096.0) < -185.0);
    }
    else return false;
  };
  auto d_good=d.Filter(metCut, {"ErrorFlag"});
  if (cutChoice == "BeamOff" || cutChoice == "Pedestal" || cutChoice == "Pedestals") {
    //d_good=e.Filter(metCut, {"bcm_an_us.hw_sum"});
    //d_good=e.Filter(metCut, {"bcm_dg_us.hw_sum_raw/bcm_dg_us.num_samples"});
    d_good=e.Filter(metCut, {"bcm_dg_us.hw_sum_raw"});
  }

//////// FIXME default CREX cut: auto d_good=d.Filter("reg.ok_cut==1");
  //auto d_good=d.Filter("(ErrorFlag)==0");
  //auto d_good=d.Filter("(ErrorFlag&0xda7e6bff)==0");
  //FIXME Original ErrorFlag case
  /*
  auto d_good=d.Filter([test](Double_t c){
      return ((((Int_t)c))==test); // ErrorFlag == 0 -> All Global cuts pass
      //return ((((Int_t)c)&0xda7e6bff)==test); // ErrorFlag&0xda7e6bff -> (BMOD is active || Global ErrorFlag cut) == 0
      //return ((((Int_t)c)&0xda7e6bff)==test && (((Int_t)c)&0x9000)==0x9000); // ErrorFlag&0xda7e6bff -> (BMOD is active && Global ErrorFlag cut) == 0 -> BMOD only
      //return ((((Int_t)c)&0x99726bff)==test && (((Int_t)c)&0x20000000)==0x20000000); // Explicitly look at all burp cut failing data, but still cut out beam trip and Beam+Regular stability cuts, and user "bad" event cuts, and all device cuts -> Include BMOD
      }
      ,{"ErrorFlag"});
  */

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
        if (minirun != "-1" && (tmpChan.type != "slopes" && tmpChan.type != "slow" && tmpChan.type != "rcdb")){
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
        else if (tmpChan.type != "slopes" && tmpChan.type != "slow" && tmpChan.type != "rcdb") {
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
      if (tmpChan.type == "rcdb") {
        getRCDB(channels, run.Atoi(), minirun.Atoi(), split.Atoi(), nruns.Atof());
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
    if (tmpChan.type=="rcdb"){
      if (debug > 1) {cout<< tmpChan.name << " RCDB Entry = :" << tmpChan.singleEntry << " --"; tsw.Print(); cout <<std::endl;}
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
  Printf("Printing agg outputs to %s",aggregatorFileName.Data());
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
