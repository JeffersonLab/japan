/*
author: Cameron Clarke
last update: February 2021

*/
// FIXME 
// Major bug: If I include more than one device, the next first device is garbage... something about struct having improper memory management/size allocation I guess

#include "getRCDB.hh"

void addTripCounter(int runNum = 5408, TString ana = "prompt"){ //(int start, int end)
  // FIXME replace with read-from-dit tree
  //TFile eig_input(Form("dataRootfiles/Full_mini_eigen_reg_allbpms_AT.root"));
  TFile* input;//(Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",runNum));
  TFile* output;//(Form("trip_%d.root",runNum),"recreate");
  if (ana.Contains("prompt")) {
    input = new TFile(Form("/chafs2/work1/apar/japanOutput/prexPrompt_pass2_%d.000.root",runNum));
    output = new TFile(Form("prompt_rootfiles/trip_%d.root",runNum),"recreate");
    Printf("Using prompt");
  }
  else if (ana.Contains("respin1")) {
    input = new TFile(Form("/lustre/expphy/volatile/halla/parity/crex-respin1/japanOutput/prexPrompt_pass2_%d.000.root",runNum));
    output = new TFile(Form("respin1_rootfiles/trip_%d.root",runNum),"recreate");
    Printf("Using respin1");
  }
  else if (ana.Contains("respin2")) {
    input = new TFile(Form("/lustre/expphy/volatile/halla/parity/crex-respin2/japanOutput/prexPrompt_pass2_%d.000.root",runNum));
    output = new TFile(Form("respin2_rootfiles/trip_%d.root",runNum),"recreate");
    Printf("Using respin2");
  }
  else {
    Printf("Error, no valid analysis name given");
  }
  if (!input || !output){
    Printf("Failed to get rootfiles");
  }

  TTree *mul_tree;
  TTree *mulc_lrb_alldet_burst_tree;
  TTree *mulc;
  //TTree *eig_reg_tree_5_tr;
  input->GetObject("mul", mul_tree);
  input->GetObject("mulc_lrb_alldet_burst", mulc_lrb_alldet_burst_tree);
  input->GetObject("mulc", mulc);
  mul_tree->AddFriend(mulc_lrb_alldet_burst_tree);
  mul_tree->AddFriend(mulc);
  TTree* out_mul_tree = new TTree("mul_trip","pruned and beam trip detected mul tree");

  Short_t BurstCounter;
  Short_t out_BurstCounter;
  Double_t CodaEventNumber, ErrorFlag, PreviousErrorFlag;
  Double_t out_CodaEventNumber, out_ErrorFlag, out_PreviousErrorFlag, out_NonTripErrorFlag, out_BeamTripActive;
  Int_t UntilNextTripStart, UntilNextTripEnd, SinceLastTripStart, SinceLastTripEnd;
  mul_tree->SetBranchAddress("BurstCounter",&BurstCounter);
  mul_tree->SetBranchAddress("CodaEventNumber",&CodaEventNumber);
  mul_tree->SetBranchAddress("ErrorFlag",&ErrorFlag);
  Double_t out_run_number;
  out_mul_tree->Branch("run_number",&out_run_number);
  out_mul_tree->Branch("BurstCounter",&out_BurstCounter);
  out_mul_tree->Branch("CodaEventNumber",&out_CodaEventNumber);
  out_mul_tree->Branch("ErrorFlag",&out_ErrorFlag);
  out_mul_tree->Branch("PreviousErrorFlag",&out_PreviousErrorFlag);
  out_mul_tree->Branch("NonTripErrorFlag",&out_NonTripErrorFlag);
  out_mul_tree->Branch("BeamTripActive",&out_BeamTripActive);
  out_mul_tree->Branch("SinceLastTripStart",&SinceLastTripStart);
  out_mul_tree->Branch("SinceLastTripEnd",&SinceLastTripEnd);
  out_mul_tree->Branch("UntilNextTripStart",&UntilNextTripStart);
  out_mul_tree->Branch("UntilNextTripEnd",&UntilNextTripEnd);
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
  japan_device bcm_an_us;  // FIXME For now do it all by pointer... does = by pointer in loop work? do I need to write a function to = each internal component?
  japan_device bcm_an_ds;
  // RAW Asyms
  japan_device_mulc asym_us_avg;
  japan_device_mulc asym_us_dd;
  japan_device asym_usl;
  japan_device asym_usr;
  // Corrected Asyms
  japan_device cor_asym_us_avg;
  japan_device cor_asym_us_dd;
  japan_device cor_asym_usl;
  japan_device cor_asym_usr;
  // SAMs
  japan_device asym_sam1;
  japan_device asym_sam2;
  japan_device asym_sam3;
  japan_device asym_sam4;
  japan_device asym_sam5;
  japan_device asym_sam6;
  japan_device asym_sam7;
  japan_device asym_sam8;
  japan_device yield_sam1;
  japan_device yield_sam2;
  japan_device yield_sam3;
  japan_device yield_sam4;
  japan_device yield_sam5;
  japan_device yield_sam6;
  japan_device yield_sam7;
  japan_device yield_sam8;
  // BPMs
  japan_device bpm4aX;
  japan_device bpm4aY;
  japan_device bpm4eX;
  japan_device bpm4eY;
  japan_device bpm1X ;
  japan_device bpm1Y ;
  japan_device bpm11X;
  japan_device bpm11Y;
  japan_device bpm12X;
  japan_device bpm12Y;
  japan_device bpm16X;
  japan_device bpm16Y;
  //japan_device * bcm_an_ds;  // FIXME For now do it all by pointer... does = by pointer in loop work? do I need to write a function to = each internal component?
  //japan_device out_bcm_an_us;// = nullptr;
  //japan_device * out_bcm_an_ds = nullptr;
  mul_tree->SetBranchAddress("asym_bcm_an_us",&bcm_an_us);
  mul_tree->SetBranchAddress("asym_bcm_an_ds",&bcm_an_ds);
  //mul_tree->SetBranchAddress("asym_bcm_an_ds",&bcm_an_ds);
  //out_mul_tree->Branch("asym_bcm_an_us",&bcm_an_us,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  //out_mul_tree->Branch("asym_bcm_an_ds",&bcm_an_ds,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");

  //out_mul_tree->Branch("asym_bcm_an_us",&out_bcm_an_us,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  //out_mul_tree->Branch("asym_bcm_an_ds",&out_bcm_an_ds,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  mul_tree->SetBranchAddress("mulc.asym_us_avg",&asym_us_avg);
  mul_tree->SetBranchAddress("mulc.asym_us_dd",&asym_us_dd);
  mul_tree->SetBranchAddress("asym_usl",&asym_usl);
  mul_tree->SetBranchAddress("asym_usr",&asym_usr);
  mul_tree->SetBranchAddress("mulc_lrb_alldet_burst.cor_asym_us_avg",&cor_asym_us_avg);
  mul_tree->SetBranchAddress("mulc_lrb_alldet_burst.cor_asym_us_dd",&cor_asym_us_dd);
  mul_tree->SetBranchAddress("mulc_lrb_alldet_burst.cor_usl",&cor_asym_usl);
  mul_tree->SetBranchAddress("mulc_lrb_alldet_burst.cor_usr",&cor_asym_usr);
  // SAMs
  mul_tree->SetBranchAddress("asym_sam1",&asym_sam1);
  mul_tree->SetBranchAddress("asym_sam2",&asym_sam2);
  mul_tree->SetBranchAddress("asym_sam3",&asym_sam3);
  mul_tree->SetBranchAddress("asym_sam4",&asym_sam4);
  mul_tree->SetBranchAddress("asym_sam5",&asym_sam5);
  mul_tree->SetBranchAddress("asym_sam6",&asym_sam6);
  mul_tree->SetBranchAddress("asym_sam7",&asym_sam7);
  mul_tree->SetBranchAddress("asym_sam8",&asym_sam8);
  mul_tree->SetBranchAddress("yield_sam1",&yield_sam1);
  mul_tree->SetBranchAddress("yield_sam2",&yield_sam2);
  mul_tree->SetBranchAddress("yield_sam3",&yield_sam3);
  mul_tree->SetBranchAddress("yield_sam4",&yield_sam4);
  mul_tree->SetBranchAddress("yield_sam5",&yield_sam5);
  mul_tree->SetBranchAddress("yield_sam6",&yield_sam6);
  mul_tree->SetBranchAddress("yield_sam7",&yield_sam7);
  mul_tree->SetBranchAddress("yield_sam8",&yield_sam8);
  // BPMs
  mul_tree->SetBranchAddress("diff_bpm4aX",&bpm4aX);
  mul_tree->SetBranchAddress("diff_bpm4aY",&bpm4aY);
  mul_tree->SetBranchAddress("diff_bpm4eX",&bpm4eX);
  mul_tree->SetBranchAddress("diff_bpm4eY",&bpm4eY);
  mul_tree->SetBranchAddress("diff_bpm1X", &bpm1X );
  mul_tree->SetBranchAddress("diff_bpm1Y", &bpm1Y );
  mul_tree->SetBranchAddress("diff_bpm11X",&bpm11X);
  mul_tree->SetBranchAddress("diff_bpm11Y",&bpm11Y);
  mul_tree->SetBranchAddress("diff_bpm12X",&bpm12X);
  mul_tree->SetBranchAddress("diff_bpm12Y",&bpm12Y);
  mul_tree->SetBranchAddress("diff_bpm16X",&bpm16X);
  mul_tree->SetBranchAddress("diff_bpm16Y",&bpm16Y);

  out_mul_tree->Branch("asym_bcm_an_us",&bcm_an_us,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_bcm_an_ds",&bcm_an_ds,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_us_avg",&asym_us_avg,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D:hw_sum_raw/D:block0_raw/D:block1_raw/D:block2_raw/D:block3_raw/D:sequence_number/D");
  out_mul_tree->Branch("asym_us_dd",&asym_us_dd,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D:hw_sum_raw/D:block0_raw/D:block1_raw/D:block2_raw/D:block3_raw/D:sequence_number/D");
  out_mul_tree->Branch("asym_usl",&asym_usl,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_usr",&asym_usr,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("cor_asym_us_avg",&cor_asym_us_avg,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("cor_asym_us_dd",&cor_asym_us_dd,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("cor_asym_usl",&cor_asym_usl,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("cor_asym_usr",&cor_asym_usr,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_sam1",&asym_sam1,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_sam2",&asym_sam2,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_sam3",&asym_sam3,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_sam4",&asym_sam4,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_sam5",&asym_sam5,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_sam6",&asym_sam6,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_sam7",&asym_sam7,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("asym_sam8",&asym_sam8,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("yield_sam1",&yield_sam1,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("yield_sam2",&yield_sam2,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("yield_sam3",&yield_sam3,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("yield_sam4",&yield_sam4,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("yield_sam5",&yield_sam5,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("yield_sam6",&yield_sam6,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("yield_sam7",&yield_sam7,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("yield_sam8",&yield_sam8,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm4aX",&bpm4aX,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm4aY",&bpm4aY,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm4eX",&bpm4eX,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm4eY",&bpm4eY,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm1X", &bpm1X ,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm1Y", &bpm1Y ,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm11X",&bpm11X,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm11Y",&bpm11Y,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm12X",&bpm12X,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm12Y",&bpm12Y,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm16X",&bpm16X,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");
  out_mul_tree->Branch("diff_bpm16Y",&bpm16Y,"hw_sum/D:block0/D:block1/D:block2/D:block3/D:num_samples/D:Device_Error_Code/D");

  Long64_t nEntries = mul_tree->GetEntries();

  Int_t LS = 0;
  Int_t LE = 0;
  Int_t NS = 0;
  Int_t NE = 0;
  
  std::vector<Int_t> SLTSV = {};
  std::vector<Int_t> SLTEV = {};
  SLTSV.reserve((Int_t)1e6);
  SLTEV.reserve((Int_t)1e6);
  for(int ievt=0;ievt<nEntries;ievt++){
    if (ievt%10000 == 0) {
      Printf("%f percent reading last",100.0*ievt/nEntries);
    }
    mul_tree->GetEntry(ievt);
    // Logic about time since last trip - reset start at start, end at end
  //if ErrorFlag == a beam trip && Ignore user cuts
    if (((Int_t)ErrorFlag) == 0x8000000) {
  //  time since last trip end ++
      LE++;
  //  if PreviousErrorFlag != a beam trip && Ignore user cuts
      if (((Int_t)PreviousErrorFlag) != 0x8000000) {
  //    time since last trip start = 0
        LS = 0;
      }
      else {
  //    time since last trip start ++
        LS++;
      }
    }
    else {
  //  time since last trip start ++
      LS++;
  //  if PreviousErrorFlag == a beam trip
      if ((((Int_t)ErrorFlag)==0) && (((Int_t)PreviousErrorFlag) & 0x8000000) != 0) {
  //    time since last trip end = 0
        LE = 0;
      }
      else {
  //    time since last trip end ++
        LE++;
      }
    }
  //push back time since last trip start and end
  //Printf("LS = %d, LE = %d",LS,LE);
    SLTSV.push_back(LS);
    SLTEV.push_back(LE);
    
  //Update previous error flag
    PreviousErrorFlag = ErrorFlag;
  }
  PreviousErrorFlag = 0;
  std::vector<Int_t> UNTSV = {};
  std::vector<Int_t> UNTEV = {}; 
  UNTSV.reserve((Int_t)1e6);
  UNTEV.reserve((Int_t)1e6);
  for(int ievt=(nEntries-1);ievt>=0;ievt--){
    if (ievt%10000 == 0) {
      Printf("%f percent done reading next",100.0*(nEntries-ievt)/nEntries);
    }
    mul_tree->GetEntry(ievt);
  //Logic about time until next trip - reset start at start, end at end
  //if ErrorFlag == a beam trip
    if (((Int_t)ErrorFlag) == 0x8000000) {
  //  time until next trip start ++
      NS++;
  //  if PreviousErrorFlag != a beam trip
      if ((((Int_t)ErrorFlag)==0) && (((Int_t)PreviousErrorFlag) & 0x8000000) != 0) {
  //    time until next trip end = 0
        NE=0;
      }
      else {
  //    time until next trip end ++
        NE++;
      }
    }
    else {
  //  time until next trip end ++
      NE++;
  //  if PreviousErrorFlag == a beam trip
      if ((((Int_t)PreviousErrorFlag) & 0x8000000) != 0) {
  //    time until next trip start = 0
        NS=0;
      }
      else {
  //    time until next trip start ++
        NS++;
      }
    }
  //push back time until next trip start and end
  //Printf("NS = %d, NE = %d",NS,NE);
    UNTSV.push_back(NS);
    UNTEV.push_back(NE);
    
  //Update previous error flag
    PreviousErrorFlag = ErrorFlag;
  }
  std::reverse(UNTSV.begin(),UNTSV.end());
  std::reverse(UNTEV.begin(),UNTEV.end());

  std::vector<Double_t> singleEntriesValues;
  std::vector<Double_t> singleEntries;

  //getRCDB(out_mul_tree, (Int_t)runNum, singleEntries, singleEntriesValues);
  getRCDB(out_mul_tree, (Int_t)runNum, singleEntries, singleEntriesValues);

  Double_t tmp_PreviousErrorFlag = ErrorFlag;
  out_PreviousErrorFlag = tmp_PreviousErrorFlag;
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
    out_PreviousErrorFlag = tmp_PreviousErrorFlag;
    tmp_PreviousErrorFlag = ErrorFlag;
    out_NonTripErrorFlag = (Double_t)(((Int_t)ErrorFlag)&0xff7ffffff); // bits other than beam trip cut;
    out_BeamTripActive = (Double_t)((((Int_t)ErrorFlag)&0x8000000) != 0);
    SinceLastTripStart = SLTSV.at(ievt);
    SinceLastTripEnd   = SLTEV.at(ievt);
    UntilNextTripStart = UNTSV.at(ievt);
    UntilNextTripEnd   = UNTEV.at(ievt);
    for (Int_t k = 0 ; k < singleEntries.size() ; k++){
      //Printf("2) Entry %d = %f",k,singleEntries.at(k));
    }
    out_mul_tree->Fill();
    //out_mul_tree->Scan("rcdb_sign:rcdb_ihwp:rcdb_slug:rcdb_horizontal_wien:rcdb_vertical_wien:rcdb_target_type:rcdb_target_90encoder:rcdb_flip_state");
  }

  //output.cd();
  out_mul_tree->Write("mul_trip",TObject::kOverwrite);
  output->Close();
  //delete output;
  //delete mul_tree;
  //delete out_mul_tree;
}
