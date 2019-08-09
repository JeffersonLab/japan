#include "../camIO.hh"
#include "getNMiniruns_postpan.C"
void dataFrame_test(TString runnum = "3424"){
  TChain* chain1 = new TChain("reg");
  TChain* mini = new TChain("mini");
  TChain* chain2 = new TChain("mul");
  // FIXME use Open command structure for friended chain
  chain1->Add("/chafs2/work1/apar/postpan-outputs/prexPrompt_"+runnum+"_000_regress_postpan.root"); 
  chain2->Add("/chafs2/work1/apar/japanOutput/prexPrompt_pass1_"+runnum+".000.root");
  //chain3->Add(""); // FIXME add dithering
  chain1->AddFriend(chain2);
  //TChain* chain1 = getTree_h("reg",3424,0,0,5); // getTree_h doesn't successfully transfer friend status to DataFrame's standards...
  ROOT::RDataFrame d1(*chain1);
  // FIXME find nMiniruns and find cuts of all in advance, loop over number for histogram definitions
  Int_t nMiniruns = mini->Scan("minirun","");
  Int_t minirun = -1;
  if (nMiniruns>=0) { // Establish loop over miniruns for defining cuts here
    minirun = 0;
  }
  std::vector<TString> cuts;
  RVec cutData; // FIXME try to make a RVec typed vector of cutDatas to be accessed later
  auto errflagPass = d1.Filter("ErrorFlag==0");
  for (Int_t cuti = 0; cuti < nMiniruns; cuti++){
    cuts.push_back(Form("(ErrorFlag==0 && minirun==%d)",cuti)); 
    cutData.push_back(errflagPass.Filter(Form("(ErrorFlag==0 && minirun==%d)",cuti)));
  }
  auto cutData = errflagPass.Filter(cut.Data());
  //auto cutData = d1.Filter("ErrorFlag==0&&minirun==0");
  // FIXME prepare all names, draw commands, and variables to find Nth items, and the sub-method to call to achieve this in a loop over config file, generating fresh branches that get filled once per minirun.
  TString name = "asym_bcm_dg_ds_agg";
  TString draw = "mul.asym_bcm_dg_ds.hw_sum";
  // FIXME use more than just histograms, also do some get Nth entry stuff (for slow tree or summary trees or mini tree for simple data getting) 
  // FIXME need to implement the slope getting in here too
  // FIXME probably sub methods that accept the cutData as input and return the number or hist
  auto hist1 = cutData.Define(name.Data(),draw.Data()).Histo1D(name.Data());
  //auto hist1 = cutData.Define("defined1","mul.asym_bcm_dg_ds.hw_sum").Histo1D("defined1");
  hist1->Draw();
  Printf("Entries = %f, mean = %f",hist1->GetEntries(), hist1->GetMean());
}
