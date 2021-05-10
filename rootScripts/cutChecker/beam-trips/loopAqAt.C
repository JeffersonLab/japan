void loopAqAt() {
  //TFile *_file0 = TFile::Open("respin1_rootfiles/trip_6344.root");
  //TFile *_file1 = TFile::Open("prompt_rootfiles/trip_6344.root");
  //TFile *_file0 = TFile::Open("respin1_rootfiles/test.root");
  //TFile *_file1 = TFile::Open("prompt_rootfiles/test.root");
  //TString outplotname = "plots/Aq_direct_compare_respin1_prompt.pdf";
  //TFile *_file0 = TFile::Open("respin1_rootfiles/trip_PB_new.root");
  //TFile *_file1 = TFile::Open("prompt_rootfiles/trip_PB_new.root");
  //TString outplotname = "plots/AT_Pb_new_respin1_prompt.pdf";
  TFile *_file0 = TFile::Open("respin1_rootfiles/test_AT.root");
  TFile *_file1 = TFile::Open("prompt_rootfiles/test_AT.root");
  TString outplotname = "plots/AT_all_respin1_prompt.pdf";
  //TFile *_file0 = TFile::Open("respin1_rootfiles/trip_6377.root");
  //TFile *_file1 = TFile::Open("prompt_rootfiles/trip_6377.root");
  //TString outplotname = "plots/AT_6377_respin1_prompt.pdf";
  TTree *mul_trip_respin1;
  TTree *mul_trip_prompt;
  _file0->GetObject("mul_trip", mul_trip_respin1);
  _file1->GetObject("mul_trip", mul_trip_prompt);


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
  japan_device r1_bcm_an_us;
  japan_device pr_bcm_an_us;
  //japan_device_mulc r1_asym_us_avg;
  //japan_device_mulc pr_asym_us_avg;
  //japan_device_mulc r1_asym_us_dd;
  //japan_device_mulc pr_asym_us_dd;
  japan_device r1_asym_usl;
  japan_device pr_asym_usl;
  japan_device r1_asym_usr;
  japan_device pr_asym_usr;
  japan_device r1_cor_asym_us_avg;
  japan_device pr_cor_asym_us_avg;
  japan_device r1_cor_asym_us_dd;
  japan_device pr_cor_asym_us_dd;
  japan_device r1_cor_asym_usl;
  japan_device pr_cor_asym_usl;
  japan_device r1_cor_asym_usr;
  japan_device pr_cor_asym_usr;


  Double_t r1_run_number = 0.0;
  Double_t r1_ErrorFlag = 0.0;
  Double_t r1_CEN = 0.0;
  Double_t r1_wien = 0.0;
  Double_t r1_ihwp = 0.0;

  Double_t pr_ErrorFlag = 0.0;
  Double_t pr_run_number = 0.0;
  Double_t pr_CEN = 0.0;
  Double_t pr_wien = 0.0;
  Double_t pr_ihwp = 0.0;

  mul_trip_respin1->BuildIndex("run_number","CodaEventNumber");
  mul_trip_prompt ->BuildIndex("run_number","CodaEventNumber");

  mul_trip_respin1->SetBranchAddress("run_number",&r1_run_number);
  mul_trip_respin1->SetBranchAddress("CodaEventNumber",&r1_CEN);
  mul_trip_respin1->SetBranchAddress("ErrorFlag",&r1_ErrorFlag);
  mul_trip_respin1->SetBranchAddress("rcdb_flip_state",&r1_wien);
  mul_trip_respin1->SetBranchAddress("rcdb_ihwp",&r1_ihwp);
  mul_trip_respin1->SetBranchAddress("asym_bcm_an_us",&r1_bcm_an_us);
  //mul_trip_respin1->SetBranchAddress("asym_us_avg",&r1_asym_us_avg);
  //mul_trip_respin1->SetBranchAddress("asym_us_dd",&r1_asym_us_dd);
  mul_trip_respin1->SetBranchAddress("asym_usl",&r1_asym_usl);
  mul_trip_respin1->SetBranchAddress("asym_usr",&r1_asym_usr);
  mul_trip_respin1->SetBranchAddress("cor_asym_us_avg",&r1_cor_asym_us_avg);
  mul_trip_respin1->SetBranchAddress("cor_asym_us_dd",&r1_cor_asym_us_dd);
  mul_trip_respin1->SetBranchAddress("cor_asym_usl",&r1_cor_asym_usl);
  mul_trip_respin1->SetBranchAddress("cor_asym_usr",&r1_cor_asym_usr);

  mul_trip_prompt ->SetBranchAddress("run_number",&pr_run_number);
  mul_trip_prompt ->SetBranchAddress("CodaEventNumber",&pr_CEN);
  mul_trip_prompt ->SetBranchAddress("ErrorFlag",&pr_ErrorFlag);
  mul_trip_prompt ->SetBranchAddress("rcdb_flip_state",&pr_wien);
  mul_trip_prompt ->SetBranchAddress("rcdb_ihwp",&pr_ihwp);
  mul_trip_prompt ->SetBranchAddress("asym_bcm_an_us",&pr_bcm_an_us);
  //mul_trip_prompt ->SetBranchAddress("asym_us_avg",&pr_asym_us_avg);
  //mul_trip_prompt ->SetBranchAddress("asym_us_dd",&pr_asym_us_dd);
  mul_trip_prompt ->SetBranchAddress("asym_usl",&pr_asym_usl);
  mul_trip_prompt ->SetBranchAddress("asym_usr",&pr_asym_usr);
  mul_trip_prompt ->SetBranchAddress("cor_asym_us_avg",&pr_cor_asym_us_avg);
  mul_trip_prompt ->SetBranchAddress("cor_asym_us_dd",&pr_cor_asym_us_dd);
  mul_trip_prompt ->SetBranchAddress("cor_asym_usl",&pr_cor_asym_usl);
  mul_trip_prompt ->SetBranchAddress("cor_asym_usr",&pr_cor_asym_usr);

  Int_t respin1_entries = mul_trip_respin1->GetEntries("run_number != 6784 && run_number != 6684");
  Int_t prompt_entries  = mul_trip_prompt->GetEntries("run_number != 6784 && run_number != 6684");

  TH1F * hr1 = new TH1F("respin1 missing from prompt","respin1 missing from prompt",100,6330,6429);
  TH1F * hr1_EF = new TH1F("respin1 ErrorFlag - prompt ErrorFlag","respin1 ErrorFlag - prompt ErrorFlag counts, Intersect ROOT files (+ = more respin1 data)",100,6330,6429);
  TH1F * hr1_Aq_r1EF = new TH1F("Sign corrected, Respin 1 Aq, respin 1 ErrorFlag","respin1 Aq, respin 1 ErrorFlag",200,-15.0e6,15.0e6);
  TH1F * hr1_Aq_prEF = new TH1F("Sign corrected, Respin 1 Aq, prompt ErrorFlag","respin1 Aq, prompt ErrorFlag",200,-15.0e6,15.0e6);
  TH1F * h_DD_prEF = new TH1F("Sign corrected, Respin 1 - prompt, prompt ErrorFlag","respin1 - prompt Aq DD, prompt ErrorFlag",200,-.5e6,.5e6);
  TH1F * h_DD_r1EF = new TH1F("Sign corrected, Respin 1 - prompt, respin1 ErrorFlag","respin1 - prompt Aq DD, respin 1 ErrorFlag",200,-.5e6,.5e6);
  TH1F * hr1_Aq_r1EF_w3_in  = new TH1F("respin1 Aq, respin 1 ErrorFlag, in right" ,"Sign corrected, Respin 1 Aq, wien right, IHWP IN, respin 1 ErrorFlag",200,-15.0e6,15.0e6);
  TH1F * hr1_Aq_prEF_w3_in  = new TH1F("respin1 Aq, prompt ErrorFlag, in right"   ,"Sign corrected, Respin 1 Aq, wien right, IHWP IN, prompt ErrorFlag",   200,-15.0e6,15.0e6);
  TH1F * hr1_Aq_r1EF_w3_out = new TH1F("respin1 Aq, respin 1 ErrorFlag, out right","Sign corrected, Respin 1 Aq, wien right, IHWP OUT, respin 1 ErrorFlag",200,-15.0e6,15.0e6);
  TH1F * hr1_Aq_prEF_w3_out = new TH1F("respin1 Aq, prompt ErrorFlag, out right"  ,"Sign corrected, Respin 1 Aq, wien right, IHWP OUT, prompt ErrorFlag",  200,-15.0e6,15.0e6);
  TH1F * hr1_rAT_r1EF_w3_in  = new TH1F("respin1 raw AT, respin 1 ErrorFlag, in left"  ,"Sign corrected, Respin 1 raw AT, wien left, IHWP IN, respin 1 ErrorFlag",  200,-15.0e6,15.0e6);
  TH1F * hr1_rAT_prEF_w3_in  = new TH1F("respin1 raw AT, prompt ErrorFlag, in left"    ,"Sign corrected, Respin 1 raw AT, wien left, IHWP IN, prompt ErrorFlag",    200,-15.0e6,15.0e6);
  TH1F * hr1_rAT_r1EF_w3_out = new TH1F("respin1 raw AT, respin 1 ErrorFlag, out left" ,"Sign corrected, Respin 1 raw AT, wien left, IHWP OUT, respin 1 ErrorFlag", 200,-15.0e6,15.0e6);
  TH1F * hr1_rAT_prEF_w3_out = new TH1F("respin1 raw AT, prompt ErrorFlag, out left"   ,"Sign corrected, Respin 1 raw AT, wien left, IHWP OUT, prompt ErrorFlag",   200,-15.0e6,15.0e6);
  TH1F * hr1_AT_r1EF_w3_in  = new TH1F("respin1 AT, respin 1 ErrorFlag, in left"  ,"Sign corrected, Respin 1 AT, wien left, IHWP IN, respin 1 ErrorFlag",  200,-15.0e6,15.0e6);
  TH1F * hr1_AT_prEF_w3_in  = new TH1F("respin1 AT, prompt ErrorFlag, in left"    ,"Sign corrected, Respin 1 AT, wien left, IHWP IN, prompt ErrorFlag",    200,-15.0e6,15.0e6);
  TH1F * hr1_AT_r1EF_w3_out = new TH1F("respin1 AT, respin 1 ErrorFlag, out left" ,"Sign corrected, Respin 1 AT, wien left, IHWP OUT, respin 1 ErrorFlag", 200,-15.0e6,15.0e6);
  TH1F * hr1_AT_prEF_w3_out = new TH1F("respin1 AT, prompt ErrorFlag, out left"   ,"Sign corrected, Respin 1 AT, wien left, IHWP OUT, prompt ErrorFlag",   200,-15.0e6,15.0e6);
  TH1F * hpr = new TH1F("prompt missing from respin1","prompt missing from respin1",3200,5400,8600);
  TH1F * hpr_Aq_r1EF = new TH1F("Sign corrected, Prompt Aq, respin 1 ErrorFlag","prompt Aq, respin 1 ErrorFlag",200,-15.0e6,15.0e6);
  TH1F * hpr_Aq_prEF = new TH1F("Sign corrected, Prompt Aq, prompt ErrorFlag","prompt Aq, prompt ErrorFlag",200,-15.0e6,15.0e6);
  TH1F * hpr_Aq_r1EF_w3_in  = new TH1F("prompt Aq, respin 1 ErrorFlag, in right" ,"Sign corrected, Prompt Aq, wien right, IHWP IN, respin 1 ErrorFlag" ,200,-15.0e6,15.0e6);
  TH1F * hpr_Aq_prEF_w3_in  = new TH1F("prompt Aq, prompt ErrorFlag, in right"   ,"Sign corrected, Prompt Aq, wien right, IHWP IN, prompt ErrorFlag"   ,200,-15.0e6,15.0e6);
  TH1F * hpr_Aq_r1EF_w3_out = new TH1F("prompt Aq, respin 1 ErrorFlag, out right","Sign corrected, Prompt Aq, wien right, IHWP OUT, respin 1 ErrorFlag",200,-15.0e6,15.0e6);
  TH1F * hpr_Aq_prEF_w3_out = new TH1F("prompt Aq, prompt ErrorFlag, out right"  ,"Sign corrected, Prompt Aq, wien right, IHWP OUT, prompt ErrorFlag"  ,200,-15.0e6,15.0e6);
  TH1F * hpr_rAT_r1EF_w3_in  = new TH1F("prompt raw AT, respin 1 ErrorFlag, in left"  ,"Sign corrected, Prompt raw AT, wien left, IHWP IN, respin 1 ErrorFlag"  ,200,-15.0e6,15.0e6);
  TH1F * hpr_rAT_prEF_w3_in  = new TH1F("prompt raw AT, prompt ErrorFlag, in left"    ,"Sign corrected, Prompt raw AT, wien left, IHWP IN, prompt ErrorFlag"    ,200,-15.0e6,15.0e6);
  TH1F * hpr_rAT_r1EF_w3_out = new TH1F("prompt raw AT, respin 1 ErrorFlag, out left" ,"Sign corrected, Prompt raw AT, wien left, IHWP OUT, respin 1 ErrorFlag" ,200,-15.0e6,15.0e6);
  TH1F * hpr_rAT_prEF_w3_out = new TH1F("prompt raw AT, prompt ErrorFlag, out left"   ,"Sign corrected, Prompt raw AT, wien left, IHWP OUT, prompt ErrorFlag"   ,200,-15.0e6,15.0e6);
  TH1F * hpr_AT_r1EF_w3_in  = new TH1F("prompt AT, respin 1 ErrorFlag, in left"  ,"Sign corrected, Prompt AT, wien left, IHWP IN, respin 1 ErrorFlag"  ,200,-15.0e6,15.0e6);
  TH1F * hpr_AT_prEF_w3_in  = new TH1F("prompt AT, prompt ErrorFlag, in left"    ,"Sign corrected, Prompt AT, wien left, IHWP IN, prompt ErrorFlag"    ,200,-15.0e6,15.0e6);
  TH1F * hpr_AT_r1EF_w3_out = new TH1F("prompt AT, respin 1 ErrorFlag, out left" ,"Sign corrected, Prompt AT, wien left, IHWP OUT, respin 1 ErrorFlag" ,200,-15.0e6,15.0e6);
  TH1F * hpr_AT_prEF_w3_out = new TH1F("prompt AT, prompt ErrorFlag, out left"   ,"Sign corrected, Prompt AT, wien left, IHWP OUT, prompt ErrorFlag"   ,200,-15.0e6,15.0e6);

  Printf("Respin1 has %d entries, prompt has %d",respin1_entries,prompt_entries);

  //Int_t nruns = mul_trip_respin1->Draw("run_number","CodaEventNumber>=4000 && CodaEventNumber<=4003","goff");
  Int_t nruns = 0;
  Int_t ndiff_plus  = 0;
  Int_t ndiff_minus = 0;


  Double_t previousRun = 0;
  Int_t counter_plus  = 0;
  Int_t counter_minus = 0;
  std::vector<Double_t> runs_plus;
  std::vector<Double_t> runs_minus;
  std::vector<Double_t> counts_plus;
  std::vector<Double_t> counts_minus;

  for (Int_t i = 0 ; i < respin1_entries ; i++){
    mul_trip_respin1->GetEntry(i);
    Int_t succeed = mul_trip_prompt->GetEntryWithIndex(r1_run_number,r1_CEN);
    if (r1_run_number == 6784.0 || pr_run_number == 6784.0) {continue;}
    if (r1_run_number == 6684.0 || pr_run_number == 6684.0) {continue;}
    if ((r1_run_number != pr_run_number) || (r1_CEN != pr_CEN)) {
   //   Printf("Respin1 event %f, %f, Prompt fails with stale %f, %f",r1_run_number,r1_CEN, pr_run_number, pr_CEN);
      hr1->Fill(r1_run_number,1);
      continue;
    }
    if (r1_ErrorFlag == 0) {
      if (pr_ErrorFlag!=0) {
        hr1_EF->Fill(r1_run_number,1.0);
        ndiff_plus += 1;
        if (previousRun == 0) { 
          previousRun = r1_run_number;
          counter_plus++;
        }
        else {
          counter_plus++;
        }
      }

      h_DD_r1EF->Fill((r1_bcm_an_us.hw_sum-pr_bcm_an_us.hw_sum)/1.0e-9);

      hr1_Aq_r1EF->Fill(r1_bcm_an_us.hw_sum/1.0e-9);
      hpr_Aq_r1EF->Fill(pr_bcm_an_us.hw_sum/1.0e-9);

      if (r1_wien == 3.0 && r1_ihwp == 1.0) {
        hr1_Aq_r1EF_w3_in->Fill(r1_bcm_an_us.hw_sum/1.0e-9);
        hpr_Aq_r1EF_w3_in->Fill(pr_bcm_an_us.hw_sum/1.0e-9);
      }
      if (r1_wien == 3.0 && r1_ihwp == 2.0) {
        hr1_Aq_r1EF_w3_out->Fill(r1_bcm_an_us.hw_sum/1.0e-9);
        hpr_Aq_r1EF_w3_out->Fill(pr_bcm_an_us.hw_sum/1.0e-9);
      }
      if (r1_wien == 3.0 && r1_ihwp == 1.0) {
        hr1_rAT_r1EF_w3_in->Fill((r1_asym_usl.hw_sum-r1_asym_usr.hw_sum)/1.0e-9);
        hpr_rAT_r1EF_w3_in->Fill((pr_asym_usl.hw_sum-pr_asym_usr.hw_sum)/1.0e-9);
        hr1_AT_r1EF_w3_in->Fill(r1_cor_asym_us_dd.hw_sum/1.0e-9);
        hpr_AT_r1EF_w3_in->Fill(pr_cor_asym_us_dd.hw_sum/1.0e-9);
      }
      if (r1_wien == 3.0 && r1_ihwp == 2.0) {
        hr1_rAT_r1EF_w3_out->Fill((r1_asym_usl.hw_sum-r1_asym_usr.hw_sum)/1.0e-9);
        hpr_rAT_r1EF_w3_out->Fill((pr_asym_usl.hw_sum-pr_asym_usr.hw_sum)/1.0e-9);
        hr1_AT_r1EF_w3_out->Fill(r1_cor_asym_us_dd.hw_sum/1.0e-9);
        hpr_AT_r1EF_w3_out->Fill(pr_cor_asym_us_dd.hw_sum/1.0e-9);
      }
    }
    if (previousRun == 0) {
      previousRun = r1_run_number;
    }
    if (r1_run_number!=previousRun) {
      Printf("Run %f had %d entries",r1_run_number,counter_plus);
      Double_t run_number_local = 0.0;
      run_number_local = previousRun;
      previousRun = r1_run_number;
      runs_plus.push_back(run_number_local);
      Int_t counter_plus_local = 0;
      counter_plus_local = counter_plus;
      counts_plus.push_back((Double_t)counter_plus_local);
      counter_plus = 0;
    }
  }
  Double_t run_number_local2 = 0.0;
  run_number_local2 = previousRun;
  previousRun = r1_run_number;
  runs_plus.push_back(run_number_local2);
  Int_t counter_plus_local = 0;
  counter_plus_local = counter_plus;
  counts_plus.push_back((Double_t)counter_plus_local);
  counter_plus = 0;

  previousRun = 0;

  for (Int_t i = 0 ; i < prompt_entries ; i++){
    mul_trip_prompt->GetEntry(i);
    Int_t succeed = mul_trip_respin1->GetEntryWithIndex(pr_run_number,pr_CEN);
    if (r1_run_number == 6684.0 || pr_run_number == 6684.0) {continue;}
    if (r1_run_number == 6784.0 || pr_run_number == 6784.0) {continue;}
    if ((r1_run_number != pr_run_number) || (r1_CEN != pr_CEN)) {
  //    Printf("Prompt event %f, %f, Respin1 fails with stale %f, %f",pr_run_number,pr_CEN, r1_run_number, r1_CEN);
      hpr->Fill(r1_run_number,1);
      continue;
    }
    if (pr_ErrorFlag == 0) {
      if (r1_ErrorFlag != 0) {
        hr1_EF->Fill(r1_run_number,-1.0);
        ndiff_minus -= 1;
        if (previousRun == 0) { 
          previousRun = r1_run_number;
          counter_minus--;
        }
        else {
          counter_minus--;
        }
      }
      h_DD_prEF->Fill((r1_bcm_an_us.hw_sum-pr_bcm_an_us.hw_sum)/1.0e-9);

      hr1_Aq_prEF->Fill(r1_bcm_an_us.hw_sum/1.0e-9);
      hpr_Aq_prEF->Fill(pr_bcm_an_us.hw_sum/1.0e-9);

      if (r1_wien == 3.0 && r1_ihwp == 1.0) {
        hr1_Aq_prEF_w3_in->Fill(r1_bcm_an_us.hw_sum/1.0e-9);
        hpr_Aq_prEF_w3_in->Fill(pr_bcm_an_us.hw_sum/1.0e-9);
      }
      if (r1_wien == 3.0 && r1_ihwp == 2.0) {
        hr1_Aq_prEF_w3_out->Fill(r1_bcm_an_us.hw_sum/1.0e-9);
        hpr_Aq_prEF_w3_out->Fill(pr_bcm_an_us.hw_sum/1.0e-9);
      }
      if (r1_wien == 3.0 && r1_ihwp == 1.0) {
        hr1_rAT_prEF_w3_in->Fill((r1_asym_usl.hw_sum-r1_asym_usr.hw_sum)/1.0e-9);
        hpr_rAT_prEF_w3_in->Fill((pr_asym_usl.hw_sum-pr_asym_usr.hw_sum)/1.0e-9);
        hr1_AT_prEF_w3_in->Fill(r1_cor_asym_us_dd.hw_sum/1.0e-9);
        hpr_AT_prEF_w3_in->Fill(pr_cor_asym_us_dd.hw_sum/1.0e-9);
      }
      if (r1_wien == 3.0 && r1_ihwp == 2.0) {
        hr1_rAT_prEF_w3_out->Fill((r1_asym_usl.hw_sum-r1_asym_usr.hw_sum)/1.0e-9);
        hpr_rAT_prEF_w3_out->Fill((pr_asym_usl.hw_sum-pr_asym_usr.hw_sum)/1.0e-9);
        hr1_AT_prEF_w3_out->Fill(r1_cor_asym_us_dd.hw_sum/1.0e-9);
        hpr_AT_prEF_w3_out->Fill(pr_cor_asym_us_dd.hw_sum/1.0e-9);
      }
    }
    if (previousRun == 0) {
      previousRun = r1_run_number;
    }
    if (r1_run_number!=previousRun) {
      Printf("Run %f had %d entries",r1_run_number,counter_minus);
      Double_t run_number_local = 0.0;
      run_number_local = previousRun;
      previousRun = r1_run_number;
      runs_minus.push_back(run_number_local);
      Int_t counter_minus_local = 0;
      counter_minus_local = counter_minus;
      counts_minus.push_back((Double_t)counter_minus_local);
      counter_minus = 0;
      nruns++;
    }
  }
  run_number_local2 = previousRun;
  previousRun = r1_run_number;
  runs_minus.push_back(run_number_local2);
  Int_t counter_minus_local = 0;
  counter_minus_local = counter_minus;
  counts_minus.push_back((Double_t)counter_minus_local);
  counter_minus = 0;
  nruns++;
  Printf("Number of added multiplets to respin1 cut = %d",ndiff_plus);
  Printf("Number of lost multiplets to respin1 cut = %d",ndiff_minus);
  Printf("Total change to respin1 cut = %d",ndiff_plus+ndiff_minus);
  Printf("Number of new multiplets to respin1 cut, per run = %f, %d runs",1.0*(ndiff_plus+ndiff_minus)/nruns,nruns);
  //Printf("Number of new multiplets to respin1 cut, per run = %f",1.0*(ndiff_plus-ndiff_minus)/nruns);

  gStyle->SetOptStat(112211);
  TCanvas* c1 = new TCanvas();
  c1->Divide(2,2);
  c1->cd(1);
  hr1->Draw();
  c1->cd(2);
  hpr->Draw();
  TVirtualPad* p1 = c1->cd(3);

  TGraph tgraph_r1_plus = TGraph(runs_plus.size(),&runs_plus[0],&counts_plus[0]);
  TGraph tgraph_r1_minus = TGraph(runs_minus.size(),&runs_minus[0],&counts_minus[0]);
  std::vector<Double_t> runs_sum;
  std::vector<Double_t> counts_sum;
  Printf("%zu plus, %zu minus",runs_plus.size(),runs_minus.size());
  for (Int_t i = 0 ; i < runs_plus.size() ; i++) {
    runs_sum.push_back(1.0*runs_minus.at(i));
    if (runs_minus.at(i) != runs_plus.at(i)){
      Printf("%f != %f",runs_minus.at(i),runs_plus.at(i));
    }
    counts_sum.push_back(1.0*counts_plus.at(i)+1.0*counts_minus.at(i));
  }
  TGraph tgraph_r1_sum = TGraph(runs_sum.size(),&runs_sum[0],&counts_sum[0]);
  TMultiGraph mg;
  tgraph_r1_plus.SetNameTitle("tgraph plus","TGraph of difference in ErrorFlag from Intersect rootfiles (respin1 - prompt) new in respin 1");
  tgraph_r1_minus.SetNameTitle("tgraph minus","TGraph of difference in ErrorFlag from Intersect rootfiles (respin1 - prompt) missing in respin 1");
  tgraph_r1_sum.SetNameTitle("tgraph sum","TGraph of summed difference in ErrorFlag from Intersect rootfiles (respin1 - prompt) net gain in respin 1");
  mg.SetNameTitle("tgraph diff","TGraph of Respin 1 ErrorFlag change. Respin gain (left, blue), lost (middle, red), net (right)");
  TF1* fp =new TF1("fp","pol0");
  TF1* fm =new TF1("fm","pol0");
  TF1* fa =new TF1("fa","pol0");
  fp->SetLineColorAlpha(kBlue,0.0);
  fm->SetLineColorAlpha(kRed,0.0);
  fa->SetLineColorAlpha(kBlack,0.9);
  fp->SetLineWidth(1);
  fm->SetLineWidth(1);
  fa->SetLineWidth(1);
  tgraph_r1_plus.SetMarkerStyle(22);
  tgraph_r1_minus.SetMarkerStyle(23);
  tgraph_r1_sum.SetMarkerStyle(20);
  tgraph_r1_plus.SetMarkerSize(0.2);
  tgraph_r1_minus.SetMarkerSize(0.2);
  tgraph_r1_sum.SetMarkerSize(0.0);
  tgraph_r1_plus.SetMarkerColor(kBlue);
  tgraph_r1_minus.SetMarkerColor(kRed);
  tgraph_r1_sum.SetMarkerColor(kBlack);

  gStyle->SetOptFit(1);

  tgraph_r1_plus.Fit(fp,"Q");
  //tgraph_r1_plus.Draw("AP");
  tgraph_r1_plus.Draw("AP");
  p1->Update();
  p1->Modified();
  //TPaveStats * ps = (TPaveStats *)tgraph_r1_plus.GetListOfFunctions()->FindObject("stats");
  TPaveStats * ps = (TPaveStats *)p1->GetPrimitive("stats");
  ps->SetName("plus");
  ps->SetOptFit(1);
  p1->Update();
  p1->Modified();
  ps->SetX1NDC(0.05);
  ps->SetX2NDC(0.35);
  //TList * plol = ps->GetListOfLines();
  TLatex *pmyt = new TLatex(0,0,"Respin 1 add in");
  pmyt ->SetTextFont(42);
  pmyt ->SetTextSize(0.04);
  pmyt ->SetTextColor(kBlue);
  ps->GetListOfLines()->Add(pmyt);
  //tgraph_r1_plus.GetListOfFunctions()->Remove(ps);
  //gStyle->SetOptFit(0);
  //ps->SetOptFit(1);
  //ps->Draw();
  p1->Update();
  p1->Modified();
  //p1->Clear();

  gStyle->SetOptFit(1);
  tgraph_r1_minus.Fit(fm,"Q");
  //tgraph_r1_minus.Draw("AP");
  tgraph_r1_minus.Draw("sames");
  p1->Update();
  p1->Modified();
  TPaveStats * ms = (TPaveStats *)p1->GetPrimitive("stats");
  ms->SetName("minus");
  ms->SetOptFit(1);
  p1->Update();
  p1->Modified();
  ms->SetX1NDC(0.35);
  ms->SetX2NDC(0.65);
  //TList * mlol = ms->GetListOfLines();
  TLatex *mmyt = new TLatex(0,0,"Respin 1 take out");
  mmyt ->SetTextFont(42);
  mmyt ->SetTextSize(0.04);
  mmyt ->SetTextColor(kRed);
  ms->GetListOfLines()->Add(mmyt);
  //tgraph_r1_minus.GetListOfFunctions()->Remove(ms);
  //gStyle->SetOptFit(0);
  //ms->SetOptFit(1);
  //ms->Draw();
  p1->Update();
  p1->Modified();
  //p1->Clear();

  gStyle->SetOptFit(1);
  tgraph_r1_sum.Fit(fa,"Q");
  //tgraph_r1_sum.Draw("AP");
  tgraph_r1_sum.Draw("sames");
  p1->Update();
  p1->Modified();
  TPaveStats * as = (TPaveStats *)p1->GetPrimitive("stats");
  as->SetName("sum");
  as->SetOptFit(1);
  p1->Update();
  p1->Modified();
  as->SetX1NDC(0.65);
  as->SetX2NDC(0.95);
  //TList * alol = as->GetListOfLines();
  TLatex *amyt = new TLatex(0,0,"Respin 1 net gain");
  amyt ->SetTextFont(42);
  amyt ->SetTextSize(0.04);
  amyt ->SetTextColor(kBlack);
  as->GetListOfLines()->Add(amyt);
  //tgraph_r1_sum.GetListOfFunctions()->Remove(as);
  gStyle->SetOptFit(0);
  //as->SetOptFit(1);
  //as->Draw();
  p1->Update();
  p1->Modified();
  //p1->Clear();

  mg.Add(&tgraph_r1_plus);
  mg.Add(&tgraph_r1_minus);
  mg.Add(&tgraph_r1_sum);
  mg.Draw("ap");
  //gStyle->SetOptFit(0);
  //gStyle->SetOptStat(0);
  //p1->Update();
  //p1->Modified();
  //gStyle->SetOptFit(1);


  //hr1_EF->Draw();
  //hr1_EF->Fit("pol0");
  //tgraph_r1->Draw("ap");
  //tgraph_r1->Fit("pol0");
  c1->cd(4);
  //gStyle->SetOptStat(0);
  TProfile* tprof_r1 = new TProfile("hprof","Profile of difference in ErrorFlag from Intersect rootfiles (respin1 - prompt)",190,5407.5,9300.5);

  for (Int_t i = 0 ; i<hr1_EF->GetNbinsX() ; i++){
    if (hr1_EF->GetBinContent(i)!=0){
      tprof_r1->Fill(hr1_EF->GetXaxis()->GetBinCenter(i),hr1_EF->GetBinContent(i));
    }
  }
  tprof_r1->Draw();
  c1->SaveAs(Form("%s(",outplotname.Data()));
  //delete ps;
  //delete ms;
  //delete as;



  gStyle->SetOptStat(112211);

  TCanvas* c2 = new TCanvas();
  c2->Divide(2,3);
  c2->cd(1);
  gPad->SetLogy();
  hr1_Aq_r1EF->Draw();
  c2->cd(2);
  gPad->SetLogy();
  hr1_Aq_prEF->Draw();
  c2->cd(3);
  gPad->SetLogy();
  hpr_Aq_r1EF->Draw();
  c2->cd(4);
  gPad->SetLogy();
  hpr_Aq_prEF->Draw();
  c2->cd(5);
  gPad->SetLogy();
  h_DD_r1EF->Draw();
  c2->cd(6);
  gPad->SetLogy();
  h_DD_prEF->Draw();
  c2->SaveAs(Form("%s",outplotname.Data()));

  TCanvas* c3 = new TCanvas();
  c3->Divide(2,2);
  c3->cd(1);
  gPad->SetLogy();
  hr1_Aq_r1EF_w3_in->Draw();
  c3->cd(2);
  gPad->SetLogy();
  hr1_Aq_prEF_w3_in->Draw();
  c3->cd(3);
  gPad->SetLogy();
  hpr_Aq_r1EF_w3_in->Draw();
  c3->cd(4);
  gPad->SetLogy();
  hpr_Aq_prEF_w3_in->Draw();
  c3->SaveAs(Form("%s",outplotname.Data()));
  //c3->SaveAs(Form("test.pdf"));
  //delete c3;

  TCanvas* c4 = new TCanvas();
  c4->Divide(2,2);
  c4->cd(1);
  gPad->SetLogy();
  hr1_Aq_r1EF_w3_out->Draw();
  c4->cd(2);
  gPad->SetLogy();
  hr1_Aq_prEF_w3_out->Draw();
  c4->cd(3);
  gPad->SetLogy();
  hpr_Aq_r1EF_w3_out->Draw();
  c4->cd(4);
  gPad->SetLogy();
  hpr_Aq_prEF_w3_out->Draw();
  c4->SaveAs(Form("%s",outplotname.Data()));

  TCanvas* c5 = new TCanvas();
  c5->Divide(2,2);
  c5->cd(1);
  gPad->SetLogy();
  hr1_rAT_r1EF_w3_in->Draw();
  c5->cd(2);
  gPad->SetLogy();
  hr1_rAT_prEF_w3_in->Draw();
  c5->cd(3);
  gPad->SetLogy();
  hpr_rAT_r1EF_w3_in->Draw();
  c5->cd(4);
  gPad->SetLogy();
  hpr_rAT_prEF_w3_in->Draw();
  c5->SaveAs(Form("%s",outplotname.Data()));

  TCanvas* c6 = new TCanvas();
  c6->Divide(2,2);
  c6->cd(1);
  gPad->SetLogy();
  hr1_rAT_r1EF_w3_out->Draw();
  c6->cd(2);
  gPad->SetLogy();
  hr1_rAT_prEF_w3_out->Draw();
  c6->cd(3);
  gPad->SetLogy();
  hpr_rAT_r1EF_w3_out->Draw();
  c6->cd(4);
  gPad->SetLogy();
  hpr_rAT_prEF_w3_out->Draw();
  c6->SaveAs(Form("%s",outplotname.Data()));

  TCanvas* c7 = new TCanvas();
  c7->Divide(2,2);
  c7->cd(1);
  gPad->SetLogy();
  hr1_AT_r1EF_w3_in->Draw();
  c7->cd(2);
  gPad->SetLogy();
  hr1_AT_prEF_w3_in->Draw();
  c7->cd(3);
  gPad->SetLogy();
  hpr_AT_r1EF_w3_in->Draw();
  c7->cd(4);
  gPad->SetLogy();
  hpr_AT_prEF_w3_in->Draw();
  c7->SaveAs(Form("%s",outplotname.Data()));

  TCanvas* c8 = new TCanvas();
  c8->Divide(2,2);
  c8->cd(1);
  gPad->SetLogy();
  hr1_AT_r1EF_w3_out->Draw();
  c8->cd(2);
  gPad->SetLogy();
  hr1_AT_prEF_w3_out->Draw();
  c8->cd(3);
  gPad->SetLogy();
  hpr_AT_r1EF_w3_out->Draw();
  c8->cd(4);
  gPad->SetLogy();
  hpr_AT_prEF_w3_out->Draw();
  c8->SaveAs(Form("%s)",outplotname.Data()));

}
