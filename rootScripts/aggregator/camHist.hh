using namespace std;
TH1 * rebinTH1_h(TH1 * h1, TString mode = "manual", Int_t lookAbove = 0, Int_t lookAround = 0, Int_t nBinsX = 0, Int_t sampleFactor = 0, Double_t lowBinValX = 0.0, Double_t highBinValX = 0.0){

  //gROOT->SetBatch(kTRUE);

  if (sampleFactor == 0){
    sampleFactor = 1;
  }
  if (nBinsX == 0){
    nBinsX = (Int_t)(h1->GetNbinsX()/sampleFactor);
  }
  if (lowBinValX == 0.0 && highBinValX == 0.0){
    lowBinValX  = h1->GetBinCenter(h1->FindFirstBinAbove(lookAbove,TH1::kXaxis) - lookAround);
    highBinValX = h1->GetBinCenter(h1->FindLastBinAbove(lookAbove,TH1::kXaxis)  + lookAround);
    //Printf(Form("Low: %f",lowBinValX));//  = h1->GetBinContent(h1->FindFirstBinAbove(lookAbove,1) - lookAround);
    //Printf(Form("high: %f",highBinValX));//highBinValX = h1->GetBinContent(h1->FindLastBinAbove(lookAbove,1)  + lookAround);
  }

  TString title    = h1->GetTitle();
  TString xTitle   = h1->GetXaxis()->GetTitle();
  TString yTitle   = h1->GetYaxis()->GetTitle();
  TString name     = h1->GetName();
  Int_t old_nBinsX = h1->GetNbinsX();

  TH1 *h2 = new TH1F((const char*)(name+"_rebin"),(const char*)("Rebinned: "+title),nBinsX,lowBinValX,highBinValX);
  //Printf(Form("name = %s_rebin, title = Rebinned: %s, nBinsX = %d, lowBinValX = %f, highBinValX = %f",(const char*)name,(const char*)title,nBinsX,lowBinValX,highBinValX));
  h2->GetXaxis()->SetTitle(xTitle);h2->GetXaxis()->CenterTitle();
  h2->GetYaxis()->SetTitle(yTitle);h2->GetYaxis()->CenterTitle();

  // Generate empty Hist
  if (mode == "clean"){
    //Printf("clean\n");
    h2->GetXaxis()->SetCanExtend(false);
  }
  else if (mode == "manual"){
    //Printf("manual\n");
    h2->GetXaxis()->SetCanExtend(true);
  }
  // Loop rebin, down or upsample
  else if (mode == "loop"){
    //Printf("loop\n");
    for (Int_t bin = 0; bin < old_nBinsX; bin++) {
      h2->Fill(h1->GetBinCenter(bin));
    }
  }
  // Auto rebin, down sample by factor sampleFactor
  else if (mode == "auto"){
    //Printf("auto\n");
    h2 = h1->Rebin(sampleFactor,name+"_rebin");
  }

  return h2;
}

TString getCuts_h(TString cut = "defaultCut", Int_t overWriteCut = 0, TString branchToCheck = "asym_vqwk_04_0ch0"){
  TString defaultCut = "(ErrorFlag==0 && "+branchToCheck+".Device_Error_Code==0)"; // Make the default JAPAN cut on the user's given branch (assumes its a device ... excplicitly use noCut for non-device branches
  if (cut == "defaultCut" || cut == "default" || cut == "def" || cut == "defaultCuts" || cut == "prex" || cut == "PREX"){
    //Printf("Cut = %s",(const char*)defaultCut);
    return defaultCut; // Just return the default JAPAN cut
  }
  if (cut == "noCut" || cut == "1"){
    return "1"; // Just return true
  }
  if (overWriteCut == 1){
    return cut; // Just return the user's cut
  }
  cut = "(" + cut + ")&&" + defaultCut; // Add the user cut to default JAPAN cut
  return cut;
}

TH1 * getHistogram_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber           = getRunNumber_h(runNumber);
  nRuns               = getNruns_h(nRuns);
  TString channel     = tree + "_" + branch + "_" + leaf;
  cut                 = getCuts_h(cut,overWriteCut,branch);
  // Make an instance of the relevant data source 
  TLeaf   *Leaf       = getLeaf_h(tree,branch,leaf,runNumber,nRuns);
  if (!Leaf){
    return 0;
  }
  TString  leafName   = branch+"."+(TString)Leaf->GetName();
  TBranch *Branch     = Leaf->GetBranch();
  TTree   *Tree       = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  gROOT->SetBatch(kTRUE);
  //Printf("Leaf name: %s",(const char*)leafName);
  Tree->Draw(Form("%s",(const char*)leafName),cut,"");
  //Printf("Tree->Draw(\"%s,%s,\"\")",(const char*)leafName,(const char*) cut);
  TH1 *h1 = (TH1*)gROOT->FindObject("htemp");
  //Printf("Histogram mean = %f",h1->GetMean());
  TH1 *h2 = new TH1F();
  
  if (mode == "defaultHist" || mode == "default" || mode == "normal"){
    //Printf("Run %d histogram of branch %s returned",runNumber,(const char*)leafName);
    return h1;
  }
  else if (mode == "clean" || mode == "manual"){
    h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
    TString h2_name = h2->GetName();
    Tree->Draw(Form("%s>>%s",(const char*)leafName,(const char*)h2_name),cut,""); // Manual
  }
  else if (mode == "auto" || mode == "loop"){
    h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
  }

  //Printf("Run %d histogram of branch %s returned",runNumber,(const char*)leafName);
  return h2; 
}

TH1 * getWeightedHistogram_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString weight = "1", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber           = getRunNumber_h(runNumber);
  nRuns               = getNruns_h(nRuns);
  TString channel     = tree + "_" + branch + "_" + leaf;
  cut                 = getCuts_h(cut,overWriteCut,branch);
  // Make an instance of the relevant data source 
  TLeaf   *Leaf       = getLeaf_h(tree,branch,leaf,runNumber,nRuns);
  if (!Leaf){
    return 0;
  }
  TString  leafName   = branch+"."+(TString)Leaf->GetName();
  TBranch *Branch     = Leaf->GetBranch();
  TTree   *Tree       = Branch->GetTree();
  Int_t    numEntries = Tree->GetEntries();

  gROOT->SetBatch(kTRUE);
  //Printf("Leaf name: %s",(const char*)leafName);
  Tree->Draw(Form("%s",(const char*)leafName),weight+"*("+cut+")","");
  //Printf("Tree->Draw(\"%s>>h1,%s,\"\")",(const char*)leafName,(const char*) cut);
  TH1 *h1 = (TH1*)gROOT->FindObject("htemp");
  //Printf("Histogram mean = %f",h1->GetMean());
  TH1 *h2 = new TH1F();
  
  if (mode == "defaultHist" || mode == "default" || mode == "normal"){
    //Printf("Run %d histogram of branch %s returned",runNumber,(const char*)leafName);
    return h1;
  }
  else if (mode == "clean" || mode == "manual"){
    h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
    TString h2_name = h2->GetName();
    Tree->Draw(Form("%s>>%s",(const char*)leafName,(const char*)h2_name),weight+"*("+cut+")",""); // Manual
  }
  else if (mode == "auto" || mode == "loop"){
    h2 = rebinTH1_h(h1,mode,2,1,1000); // example use case of rebinTH1_h method
  }

  //Printf("Run %d histogram of branch %s returned",runNumber,(const char*)leafName);
  return h2; 
}

void writeInt_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  TString weight   = branch+"."+leaf;
  TString integral = "integral_" + branch + "_" + leaf;
  Double_t data_integral = 0.0;
	//data_integral = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->Integral();
	data_integral = getWeightedHistogram_h(tree,branch,leaf,weight,cut,0,mode,runNumber,nRuns)->Integral();

  //Printf("Run %d integral %s: %f",runNumber,(const char*)integral,data_integral);
  writeFile_h(integral,data_integral,runNumber,nRuns);
}

void writeMeanRms_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  TString mean = "mean_" + branch + "_" + leaf;
  TString mean_error = "mean_" + branch + "_" + leaf + "_error";
  TString rms = "rms_" + branch + "_" + leaf;
  TString rms_error = "rms_" + branch + "_" + leaf + "_error";
  Double_t data_mean = -1e99;
  Double_t data_mean_error = -1e99;
  Double_t data_rms = -1e99;
  Double_t data_rms_error = -1e99;
  data_mean = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->GetMean(1);
  data_mean_error = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->GetMeanError(1);
  data_rms = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->GetRMS(1);
  data_rms_error = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->GetRMSError(1);

  //Printf("Run %d mean %s: %f+-%f",runNumber,(const char*)mean,data_mean,data_mean_error);
  //Printf("Run %d rms %s: %f+-%f",runNumber,(const char*)rms,data_rms,data_rms_error);
  writeFile_h(mean,data_mean,runNumber,nRuns);
  writeFile_h(mean_error,data_mean_error,runNumber,nRuns);
  writeFile_h(rms,data_rms,runNumber,nRuns);
  writeFile_h(rms_error,data_rms_error,runNumber,nRuns);
}

void writeMean_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  TString mean = "mean_" + branch + "_" + leaf;
  TString mean_error = "mean_" + branch + "_" + leaf + "_error";
  Double_t data_mean = -1e99;
  Double_t data_mean_error = -1e99;
  data_mean = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->GetMean(1);
  data_mean_error = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->GetMeanError(1);

  //Printf("Run %d mean %s: %f+-%f",runNumber,(const char*)mean,data_mean,data_mean_error);
  writeFile_h(mean,data_mean,runNumber,nRuns);
  writeFile_h(mean_error,data_mean_error,runNumber,nRuns);
}

void writeRMS_leafHist_h(TString tree = "mul", TString branch = "asym_vqwk_04_0ch0", TString leaf = "hw_sum", TString cut = "defaultCut", Int_t overWriteCut = 0, TString mode = "defaultHist", Int_t runNumber = 0, Int_t nRuns = -1){
  runNumber = getRunNumber_h(runNumber);
  nRuns     = getNruns_h(nRuns);
  TString rms = "rms_" + branch + "_" + leaf;
  TString rms_error = "rms_" + branch + "_" + leaf + "_error";
  Double_t data_rms = -1e99;
  Double_t data_rms_error = -1e99;
  data_rms = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->GetRMS(1);
  data_rms_error = getHistogram_h(tree,branch,leaf,cut,0,mode,runNumber,nRuns)->GetRMSError(1);

  //Printf("Run %d rms %s: %f+-%f",runNumber,(const char*)rms,data_rms,data_rms_error);
  writeFile_h(rms,data_rms,runNumber,nRuns);
  writeFile_h(rms_error,data_rms_error,runNumber,nRuns);
}
