/*
author: Cameron Clarke
last update: September 2021
*/

#include "getRCDB.hh"

void pruneAllbpmLagr(int runNum = 5408, TString ana = "prompt"){ //(int start, int end)
  // FIXME replace with read-from-dit tree
  TFile* eig_input;
  TFile* input;
  TFile* output;
  eig_input = new TFile(Form("/lustre19/expphy/volatile/halla/parity/crex-respin2/bmodOutput/DitherCorrection/prexPrompt_drl_no_err_part_avg_allbpms_%d_000.root",runNum));
  input = new TFile(Form("/lustre/expphy/volatile/halla/parity/crex-respin2/japanOutput/prexPrompt_pass2_%d.000.root",runNum));
  output = new TFile(Form("respin2_lagr_rootfiles/prune_%d.root",runNum),"recreate");
  Printf("Using respin2");
  if (!input || !output || !eig_input){
    Printf("Failed to get rootfiles");
  }

  TTree *mul_tree;
  TTree *mulc_lrb_alldet_burst_tree;
  TTree *mulc;
  TTree *drl;
  //TTree *eig_reg_tree_5_tr;
  input->GetObject("mul", mul_tree);
  input->GetObject("mulc_lrb_alldet_burst", mulc_lrb_alldet_burst_tree);
  input->GetObject("mulc", mulc);
  eig_input->GetObject("drl",drl);
  mul_tree->AddFriend(mulc_lrb_alldet_burst_tree);
  mul_tree->AddFriend(mulc);
  mul_tree->AddFriend(drl);
  TTree* out_mul_tree = new TTree("mul_prune","pruned mul tree");

  Short_t BurstCounter;
  Short_t out_BurstCounter;
  Double_t CodaEventNumber, ErrorFlag;
  Double_t out_CodaEventNumber, out_ErrorFlag;
  mul_tree->SetBranchAddress("BurstCounter",&BurstCounter);
  mul_tree->SetBranchAddress("CodaEventNumber",&CodaEventNumber);
  mul_tree->SetBranchAddress("ErrorFlag",&ErrorFlag);
  Double_t out_run_number;
  out_mul_tree->Branch("run_number",&out_run_number);
  out_mul_tree->Branch("BurstCounter",&out_BurstCounter);
  out_mul_tree->Branch("CodaEventNumber",&out_CodaEventNumber);
  out_mul_tree->Branch("ErrorFlag",&out_ErrorFlag);
  typedef struct {
    Double_t hw_sum;
    Double_t block0;
    Double_t block1;
    Double_t block2;
    Double_t block3;
    Double_t num_samples;
    Double_t Device_Error_Code;
    Double_t hw_sum_raw;
    Double_t block0_raw;
    Double_t block1_raw;
    Double_t block2_raw;
    Double_t block3_raw;
    Double_t sequence_number;
  } japan_device_mulc ;
  typedef struct {
    Double_t hw_sum;
    Double_t block0;
    Double_t block1;
    Double_t block2;
    Double_t block3;
    Double_t num_samples;
    Double_t Device_Error_Code;
  } japan_device ;
  // BCMs 
  // RAW Asyms
  japan_device_mulc asym_us_avg;
  japan_device_mulc asym_us_dd;
  japan_device asym_usl;
  japan_device asym_usr;
  // Corrected Asyms
  japan_device eigen_lagr_asym_us_avg;
  japan_device eigen_lagr_asym_us_dd;
  japan_device eigen_lagr_asym_usl;
  japan_device eigen_lagr_asym_usr;
  japan_device eigen_reg_asym_us_avg;
  japan_device eigen_reg_asym_us_dd;
  japan_device eigen_reg_asym_usl;
  japan_device eigen_reg_asym_usr;

  mul_tree->SetBranchAddress("mulc.asym_us_avg",&asym_us_avg);
  mul_tree->SetBranchAddress("mulc.asym_us_dd",&asym_us_dd);
  mul_tree->SetBranchAddress("asym_usl",&asym_usl);
  mul_tree->SetBranchAddress("asym_usr",&asym_usr);
  mul_tree->SetBranchAddress("drl.eigen_lagr_asym_us_avg",&eigen_lagr_asym_us_avg);
  mul_tree->SetBranchAddress("drl.eigen_lagr_asym_us_dd",&eigen_lagr_asym_us_dd);
  mul_tree->SetBranchAddress("drl.eigen_lagr_asym_usl",&eigen_lagr_asym_usl);
  mul_tree->SetBranchAddress("drl.eigen_lagr_asym_usr",&eigen_lagr_asym_usr);
  mul_tree->SetBranchAddress("drl.eigen_reg_asym_us_avg",&eigen_reg_asym_us_avg);
  mul_tree->SetBranchAddress("drl.eigen_reg_asym_us_dd",&eigen_reg_asym_us_dd);
  mul_tree->SetBranchAddress("drl.eigen_reg_asym_usl",&eigen_reg_asym_usl);
  mul_tree->SetBranchAddress("drl.eigen_reg_asym_usr",&eigen_reg_asym_usr);

  out_mul_tree->Branch("asym_us_avg",&asym_us_avg,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D:hw_sum_raw/D:block0_raw/D:block1_raw/D:block2_raw/D:block3_raw/D:sequence_number/D");
  out_mul_tree->Branch("asym_us_dd",&asym_us_dd,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D:hw_sum_raw/D:block0_raw/D:block1_raw/D:block2_raw/D:block3_raw/D:sequence_number/D");
  out_mul_tree->Branch("asym_usl",&asym_usl,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_usr",&asym_usr,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("eigen_lagr_asym_us_avg",&eigen_lagr_asym_us_avg,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("eigen_lagr_asym_us_dd",&eigen_lagr_asym_us_dd,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("eigen_lagr_asym_usl",&eigen_lagr_asym_usl,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("eigen_lagr_asym_usr",&eigen_lagr_asym_usr,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("eigen_reg_asym_us_avg",&eigen_reg_asym_us_avg,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("eigen_reg_asym_us_dd",&eigen_reg_asym_us_dd,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("eigen_reg_asym_usl",&eigen_reg_asym_usl,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("eigen_reg_asym_usr",&eigen_reg_asym_usr,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  Long64_t nEntries = mul_tree->GetEntries();

  std::vector<Double_t> singleEntriesValues;
  std::vector<Double_t> singleEntries;
  //getRCDB(out_mul_tree, (Int_t)runNum, singleEntries, singleEntriesValues);
  getRCDB(out_mul_tree, (Int_t)runNum, singleEntries, singleEntriesValues);

  Printf("RCDB Info in tree after setting values = %zu, %zu",singleEntriesValues.size(),singleEntries.size());
  for(int ievt=0;ievt<nEntries;ievt++){
    if (((Int_t)ievt)%10000 == 0) {
      Printf("%f percent done printing",100.0*ievt/nEntries);
    }
    mul_tree->GetEntry(ievt);
    for (Int_t k = 0 ; k < singleEntries.size() ; k++) {
      singleEntries.at(k) = singleEntriesValues.at(k);
      //Printf("Entry %d = %f",k,singleEntriesValues.at(k));
    }
    out_run_number = (Double_t)runNum;
    out_BurstCounter = BurstCounter;
    out_CodaEventNumber = CodaEventNumber;
    out_ErrorFlag = ErrorFlag;
    out_mul_tree->Fill();
  }

  out_mul_tree->Write("mul_prune",TObject::kOverwrite);
  output->Close();
}
