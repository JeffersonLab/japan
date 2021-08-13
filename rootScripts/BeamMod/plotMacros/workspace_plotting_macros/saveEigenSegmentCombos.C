// saveEigenSegmentCombos.C
// Cameron Clarke
// March 5 2021
//
// This program finds segmentwise averages (using goff TH1 auto-draws)
// of Eigen Vector BPM combo compositions (and slopes too) and prints to tree
//

// Define a new method which takes as argument the variable name against which to perform averaging, TTree of slopes to average, the 3 deep vector to assign as new Branches, and the DV, Monitor, and IV name vectors. Execute this for all trees of slopes that need averaging.
// For each DV, loop over Monitors, for each Monitor loop over component BPMs.
// For each thing create a branch to hold it - these need to be 3 deep vectors, pre-filled with 0.0's (each 0.0 needs to be a double, each layer needs to be a std::vector, etc. be careful)
// Loop over every event to fill a map with segment numbers (and number of miniruns within them)
// Loop over the map of segment numbers to generate averages per segment for the 3 deep vector created before (consider writing this code so it can be augmented in the future to do run-wise or slug-wise averaging as well)
//   After finishing the 3 deep vector, for each segment value Fill the tree with those numbers. Be sure to pass the htemp->GetMean() into a locally defined Double_t and then assign the vector's value = this new Double_t before doing Fill (otherwise the pointer to GetMean() will be obtained and you will fail)
void combo_segment_getter(TString averaging,TTree* intree, TTree* outtree, TString data_tree_name, TString cut, std::vector<TString> DVs, std::vector<TString> Monitors, std::vector<TString> IVs) {

  std::vector<std::vector<Double_t>> comboVec2_mean;
  std::vector<std::vector<Double_t>> comboVec2_mean_err;
  std::vector<std::vector<Double_t>> comboVec2_rms;
  std::vector<std::vector<Double_t>> slopeVec2_mean;
  std::vector<std::vector<Double_t>> slopeVec2_mean_err;
  std::vector<std::vector<Double_t>> slopeVec2_rms;
  std::vector<Double_t> slopeVec_mean;
  std::vector<Double_t> slopeVec_mean_err;
  std::vector<Double_t> slopeVec_rms;
  std::vector<Double_t> comboVec_mean;
  std::vector<Double_t> comboVec2_norm2;
  std::vector<Double_t> comboVec_mean_err;
  std::vector<Double_t> comboVec_rms;
  // FIXME Technically nDet and nBPM are 0 unless you've executed "calculateSensitivities" at least once...
  for(int idet=0;idet<DVs.size();idet++){
    slopeVec_mean.clear();
    slopeVec_mean_err.clear();
    slopeVec_rms.clear();
    for(int imon=0;imon<Monitors.size();imon++){
      Double_t slopeVec1_mean = 0.0;
      Double_t slopeVec1_mean_err = 0.0;
      Double_t slopeVec1_rms = 0.0;
      slopeVec_mean.push_back(slopeVec1_mean);
      slopeVec_mean_err.push_back(slopeVec1_mean_err);
      slopeVec_rms.push_back(slopeVec1_rms);
    }
    slopeVec2_mean.push_back(slopeVec_mean);
    slopeVec2_mean_err.push_back(slopeVec_mean_err);
    slopeVec2_rms.push_back(slopeVec_rms);
  }
  for(int imon=0;imon<Monitors.size();imon++){
    comboVec_mean.clear();
    comboVec_mean_err.clear();
    comboVec_rms.clear();
    for (Int_t ibpm = 0; ibpm<IVs.size() ; ibpm++){
      Double_t comboVec1_mean = 0.0;
      Double_t comboVec1_mean_err = 0.0;
      Double_t comboVec1_rms = 0.0;
      comboVec_mean.push_back(comboVec1_mean);
      comboVec_mean_err.push_back(comboVec1_mean_err);
      comboVec_rms.push_back(comboVec1_rms);
    }
    comboVec2_mean.push_back(comboVec_mean);
    comboVec2_mean_err.push_back(comboVec_mean_err);
    comboVec2_rms.push_back(comboVec_rms);

    Double_t comboVec1_norm2 = 0.0;
    comboVec2_norm2.push_back(comboVec1_norm2);
  }
  // Set branch address after the vectors are filled to avoid C++ dynamic memory allocation shenanigans
  for(int idet=0;idet<DVs.size();idet++){
    for(int imon=0;imon<Monitors.size();imon++){
      outtree->Branch(Form("%s_%s_mean",DVs.at(idet).Data(),Monitors.at(imon).Data()),&slopeVec2_mean[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_err",DVs.at(idet).Data(),Monitors.at(imon).Data()),&slopeVec2_mean_err[idet][imon]);
      outtree->Branch(Form("%s_%s_rms",DVs.at(idet).Data(),Monitors.at(imon).Data()),&slopeVec2_rms[idet][imon]);
    }
  }
  for(int imon=0;imon<Monitors.size();imon++){
    for (Int_t ibpm = 0; ibpm<IVs.size() ; ibpm++){
      outtree->Branch(Form("%s_%s_mean",Monitors.at(imon).Data(),IVs.at(ibpm).Data()),&comboVec2_mean[imon][ibpm]);
      outtree->Branch(Form("%s_%s_mean_err",Monitors.at(imon).Data(),IVs.at(ibpm).Data()),&comboVec2_mean_err[imon][ibpm]);
      outtree->Branch(Form("%s_%s_rms",Monitors.at(imon).Data(),IVs.at(ibpm).Data()),&comboVec2_rms[imon][ibpm]);
    }
    outtree->Branch(Form("%s_norm",Monitors.at(imon).Data()),&comboVec2_norm2[imon]);
  }

  // Get the number of segments from the tree directly
  Int_t totalEntries = intree->GetEntries();
  Int_t int_avg_time_scale = 0;
  Double_t double_avg_time_scale = 0;
  if (averaging == "rcdb_slug" || averaging == "rcdb_flip_state" || averaging == "crex_part" || averaging == "crex_part_b") {
    intree->SetBranchAddress(averaging,&double_avg_time_scale);
  }
  if (averaging == "dit_segment" || averaging == "run") {
    intree->SetBranchAddress(averaging,&int_avg_time_scale);
  }
  /*std::map<Int_t,Int_t> avg_time_scales;
  for (Int_t ient = 0 ; ient<totalEntries ; ient++) {
    intree->GetEntry(ient);
    if (avg_time_scales.count(avg_time_scale) == 0) {
      avg_time_scales[avg_time_scale] = 1;
    }
    else {
      avg_time_scales[avg_time_scale] = avg_time_scales.at(avg_time_scale) + 1;
    }
  }
  */

  // FIXME Should this calculation be done once per each minirun entry in intree->outtree? Or store a giant map and print per entry later (seems inefficient). I am storing the number of entries that mattch the localAvg_time_scale == X cut, but there are possible entries with NO localAvg_time_scale value, or something like that, which would be edge cases... also I want to preserve the order of points from intree->outtree for friendability later, so I need to loop over friend tree for sure with some kind of lookup reference.
  // FIXME Idea: check if the localAvg_time_scale branch value has changed since the last event, if it has then calculate a new set of means and Fill, but doing a Fill && Continue if not
  Int_t localAvg_time_scale = -1000; // Default initialize to rediculous number
  Int_t previousAvg_time_scale = -1000; // Default initialize to rediculous number
  Int_t nen=0;
  //for (std::map<Int_t,Int_t>::iterator iter = avg_time_scales.begin() ; iter != avg_time_scales.end(); ++iter){}
  for (Int_t ient = 0 ; ient<totalEntries ; ient++) {
    intree->GetEntry(ient);
    if (averaging == "rcdb_slug" || averaging == "rcdb_flip_state" || averaging == "crex_part" || averaging == "crex_part_b") {
      localAvg_time_scale = (Int_t)double_avg_time_scale;
    }
    if (averaging == "dit_segment" || averaging == "run") {
      localAvg_time_scale = int_avg_time_scale;
    }
    //Printf("A) Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient);
    if (localAvg_time_scale == previousAvg_time_scale) {
      //Printf("Same localAvg_time_scale, filling another instance of the previously calculated means, etc. for slopes and BPM eigen vector monitor combos");
      //Printf("B) Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient);
      outtree->Fill();
      continue;
    }
    else {
      previousAvg_time_scale = localAvg_time_scale;
      // else grab all of the slopes and combos for this localAvg_time_scale value
      // Slopes
      for (Int_t idet = 0 ; idet < DVs.size() ; idet++) {
        //Printf("test 1 slopes");
        for (Int_t imon = 0 ; imon < Monitors.size() ; imon++) {
          if (cut == "") {
      //Printf("Test e) localAvg_time_scale = %d",localAvg_time_scale);
            nen = intree->Draw(Form("(%s.%s_%s)",data_tree_name.Data(),DVs.at(idet).Data(),Monitors.at(imon).Data()),Form("%s>=%d-0.1 && %s<=%d+0.1",averaging.Data(),localAvg_time_scale,averaging.Data(),localAvg_time_scale),"goff");
      //Printf("Test f) localAvg_time_scale = %d",localAvg_time_scale);
            if (imon == 0 && idet == 0){
              //Printf("C) Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient);
              //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d",nen,data_tree_name.Data(),DVs.at(idet).Data(),Monitors.at(imon).Data(),averaging.Data(),localAvg_time_scale);
            }
          }
          else {
            nen = intree->Draw(Form("(%s.%s_%s)",data_tree_name.Data(),DVs.at(idet).Data(),Monitors.at(imon).Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),localAvg_time_scale,averaging.Data(),localAvg_time_scale,cut.Data()),"goff");
            //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d && %s",nen,data_tree_name.Data(),DVs.at(idet).Data(),Monitors.at(imon).Data(),averaging.Data(),localAvg_time_scale,cut.Data());
          }
          // Get mean
          if (nen != 0) {
            TH1* ah1;
            ah1 = (TH1*)(gROOT->FindObject("htemp"));
            if (nen != 0 && ah1 != 0) {
              //Printf("test 3 slopes means");
              Double_t avg_tmp1 = 0.0;
              Double_t err_tmp1 = 0.0;
              Double_t rms_tmp1 = 0.0;
              avg_tmp1 = (Double_t)(ah1->GetMean(1));
              err_tmp1 = (Double_t)(ah1->GetMeanError(1));
              rms_tmp1 = (Double_t)(ah1->GetRMS(1));
              if (imon == 0 && idet == 0){
                //Printf("New mean %s %s = %f, entries = %d",DVs.at(idet).Data(),Monitors.at(imon).Data(),avg_tmp1,nen);
              }
              slopeVec2_mean[idet][imon]     = avg_tmp1;
              slopeVec2_mean_err[idet][imon] = err_tmp1;
              slopeVec2_rms[idet][imon]      = rms_tmp1;
            }
          }
          //else {
            //Printf("Using prior segment's value for now...");
          //}
        }
      }
      // Combos
      for (Int_t imon = 0 ; imon < Monitors.size() ; imon++) {
        //Printf("test 1 combos");
        comboVec2_norm2[imon] = 0.0;
        for (Int_t ibpm = 0 ; ibpm < IVs.size() ; ibpm++) {
          //Printf("test 2 combos");
          if (cut == "") {
            nen = intree->Draw(Form("(%s.%s_%s)",data_tree_name.Data(),Monitors.at(imon).Data(),IVs.at(ibpm).Data()),Form("%s==%d",averaging.Data(),localAvg_time_scale),"goff");
            //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d",nen,data_tree_name.Data(),Monitors.at(imon).Data(),IVs.at(ibpm).Data(),averaging.Data(),localAvg_time_scale);
          }
          else {
            nen = intree->Draw(Form("(%s.%s_%s)",data_tree_name.Data(),Monitors.at(imon).Data(),IVs.at(ibpm).Data()),Form("%s==%d && %s",averaging.Data(),localAvg_time_scale,cut.Data()),"goff"); // FIXME "goff"???
            //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d && %s",nen,data_tree_name.Data(),Monitors.at(imon).Data(),IVs.at(ibpm).Data(),averaging.Data(),localAvg_time_scale,cut.Data());
          }
          // Get mean
          if (nen != 0) {
            TH1* ah1;
            ah1 = (TH1*)(gROOT->FindObject("htemp"));
            if (nen != 0 && ah1 != 0) {
              //Printf("test 3 combos means");
              Double_t avg_tmp1 = 0.0;
              Double_t err_tmp1 = 0.0;
              Double_t rms_tmp1 = 0.0;
              avg_tmp1 = (Double_t)(ah1->GetMean(1));
              err_tmp1 = (Double_t)(ah1->GetMeanError(1));
              rms_tmp1 = (Double_t)(ah1->GetRMS(1));
              //Printf("New mean %s %s = %f, entries = %d",Monitors.at(imon).Data(),IVs.at(ibpm).Data(),avg_tmp1,nen);
              comboVec2_mean[imon][ibpm]     = avg_tmp1;
              comboVec2_norm2[imon]         += (avg_tmp1*avg_tmp1);
              comboVec2_mean_err[imon][ibpm] = err_tmp1;
              comboVec2_rms[imon][ibpm]      = rms_tmp1;
            }
          }
          //else {
            //Printf("Using prior segment's value for now... for %d in %d",ient,localAvg_time_scale);
          //}
        }
      }
      // Renormalize the averaged eigenvector definition.
      for (Int_t imon = 0 ; imon < Monitors.size() ; imon++) {
        for (Int_t ibpm = 0 ; ibpm < IVs.size() ; ibpm++) {
          comboVec2_mean[imon][ibpm]     = comboVec2_mean[imon][ibpm]/sqrt(comboVec2_norm2[imon]);
          comboVec2_mean_err[imon][ibpm] = comboVec2_mean_err[imon][ibpm]/sqrt(comboVec2_norm2[imon]);
          comboVec2_rms[imon][ibpm]      = comboVec2_rms[imon][ibpm]/sqrt(comboVec2_norm2[imon]);
        }
        comboVec2_norm2[imon] = sqrt(comboVec2_norm2[imon]);
      }
      // FIXME FIXME HACK -> Entry 0 is a special case where the dit_flag == 0 and it fills the tree with nothing, so just workaround by not filling here and then filling 2x on the next good entry. Serious HACK for CREX
      // FIXME this hack needs to be not just for entry 0, but for all dit_segment == 0
      if (cut == "dit_flag==1") {
        if (ient == 0) {
          continue;
        }
        if (ient == 1) {
          outtree->Fill(); 
        } // Note that this hack is only needed for this initial value, as in subsequent failed cases it will just adopt the previous successful segment's old values (and this never happens again).
      }
      //Printf("test fill");
      outtree->Fill();
    }
  }
}

void saveEigenSegmentCombos(TString ana = "avgs", TString suffix = "", TString mod_cut = "1==1"){ //TString input = "NULL",TString runNum = "", TString slugNum = "0"){}
  std::map<int,Double_t> runs;
  // Grab all of the relevant combos
  TChain* mini = new TChain("mini");
  //TChain* mini_eigen_reg_allbpms_tr        = new TChain(Form("mini_eigen_reg_allbpms_tr"));     // Truncated one is not sorted... just deal with it I guess (could sort it later if urgently needed)
  TChain* mini_eigen_reg_allbpms_sorted    = new TChain(Form("mini_eigen_reg_allbpms_sorted")); // Unsorted versions have no _sorted in the name
  TChain* mini_eigen_reg_5bpms_sorted      = new TChain(Form("mini_eigen_reg_5bpms_sorted"));
  // Replace 13746 etc. with a smart-replaced command line derived ["","A",..."D"] or [13746, 15746, 13726] set
  TString infilename = Form("../respin2_data/rcdb_burstwise_eigenvectors_sorted%s.root",suffix.Data());
  mini->AddFile(infilename);
  //mini_eigen_reg_allbpms_tr->AddFile(infilename);
  mini_eigen_reg_allbpms_sorted->AddFile(infilename);
  mini_eigen_reg_5bpms_sorted->AddFile(infilename);
  // Default outputs, unsorted:
  //mini->AddFile("/lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles/respin2_all_eigenvector_basic.root");
  //mini->AddFriend(mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr");
  mini->AddFriend(mini_eigen_reg_allbpms_sorted,"mini_eigen_reg_allbpms_sorted");
  mini->AddFriend(mini_eigen_reg_5bpms_sorted,"mini_eigen_reg_5bpms_sorted");
  mini->BuildIndex("run","mini");

  std::vector<TString> detectors = {
    "usl", "usr", "us_avg" , "us_dd"/*, ATs, SAMs*/
  };
  std::vector<TString> X_BPMs = {
    "bpm4aX",
    "bpm4eX",
    "bpm1X",
    "bpm16X",
  };
  std::vector<TString> Y_BPMs = {
    "bpm4aY",
    "bpm4eY",
    "bpm1Y",
    "bpm16Y",
  };
  std::vector<TString> E_BPMs = {
    "bpm11X",
    "bpm12X",
  };
  std::vector<TString> monitors = {"evMon0","evMon1","evMon2","evMon3","evMon4","evMon5","evMon6","evMon7","evMon8","evMon9","evMon10","evMon11"};
  std::vector<TString> monitors5 = {"evMon0","evMon1","evMon2","evMon3","evMon4"};
  std::vector<TString> devices = {
    "bpm4aX",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm1X",
    "bpm1Y",
    "bpm11X",
    "bpm11Y",
    "bpm12X",
    "bpm12Y",
    "bpm16X",
    "bpm16Y"
  };
  std::vector<TString> devices5 = {
    "bpm1X",
    "bpm4aY",
    "bpm4eX",
    "bpm4eY",
    "bpm12X"
  };

  // FIXME temporary output rootfile name
  TString outFileName = Form("Segment_Avg_EigenCombos%s.root",suffix.Data());
  // Save the new TTree - make sure necessary and useful branches are also copied, as needed.
  TFile newfile(outFileName, "recreate");
  newfile.cd();

  //TTree * out_tree_mini_eigen_reg_allbpms_tr     = new TTree("mini_eigen_reg_allbpms_tr_seg_avg",    "mini_eigen_reg_allbpms_tr_seg_avg");
  // Error: The tr branch never had it's combo branches changed by any sorting algorithm, so it's combos still live in the unsorted main branch
  //combo_segment_getter("dit_segment",mini,out_tree_mini_eigen_reg_allbpms_tr,"mini_eigen_reg_allbpms_tr",cut,detectors,monitors,devices);

  // Start getting the averages
  // FIXME Using no cut (rcdb_run_type or run_flag or dit_flag) assumes that the dataset being fed in is a GOOD PRODUCTION and suitable for application to BMOD segments as well. For proof of concept comparisons the cut may not be that important, as we just want to compare eigen reg to eigen dit, etc. but for final numbers this will really matter.
  TString cut = "dit_flag==1";
  TString averaging_timescale = "dit_segment"; // Can also be run, rcdb_slug, rcdb_flip_state, or nothing (simply don't execute this script for nothing);
  if (ana == "avgs") {
    // Part averages
    /*
    TTree * out_tree_mini_eigen_reg_allbpms_sorted_part = new TTree("mini_eigen_reg_allbpms_sorted_part_avg","mini_eigen_reg_allbpms_sorted_part_avg");
    TTree * out_tree_mini_eigen_reg_5bpms_sorted_part   = new TTree("mini_eigen_reg_5bpms_sorted_part_avg",  "mini_eigen_reg_5bpms_sorted_part_avg");
    TTree * out_tree_mini_eigen_reg_allbpms_sorted_part_pruned = new TTree("mini_eigen_reg_allbpms_sorted_pruned_part_avg","mini_eigen_reg_allbpms_sorted_pruned_part_avg");
    TTree * out_tree_mini_eigen_reg_5bpms_sorted_part_pruned   = new TTree("mini_eigen_reg_5bpms_sorted_pruned_part_avg",  "mini_eigen_reg_5bpms_sorted_pruned_part_avg");

    // Segment averages
    TTree * out_tree_mini_eigen_reg_allbpms_sorted_seg_pruned = new TTree("mini_eigen_reg_allbpms_sorted_pruned_seg_avg","mini_eigen_reg_allbpms_sorted_pruned_seg_avg");
    TTree * out_tree_mini_eigen_reg_5bpms_sorted_seg_pruned   = new TTree("mini_eigen_reg_5bpms_sorted_pruned_seg_avg",  "mini_eigen_reg_5bpms_sorted_pruned_seg_avg");
    */
    TTree * out_tree_mini_eigen_reg_allbpms_sorted_seg = new TTree("mini_eigen_reg_allbpms_sorted_seg_avg","mini_eigen_reg_allbpms_sorted_seg_avg");
    TTree * out_tree_mini_eigen_reg_5bpms_sorted_seg   = new TTree("mini_eigen_reg_5bpms_sorted_seg_avg",  "mini_eigen_reg_5bpms_sorted_seg_avg");

    /*
    // Slug averages
    TTree * out_tree_mini_eigen_reg_allbpms_sorted_slug_pruned = new TTree("mini_eigen_reg_allbpms_sorted_pruned_slug_avg","mini_eigen_reg_allbpms_sorted_pruned_slug_avg");
    TTree * out_tree_mini_eigen_reg_5bpms_sorted_slug_pruned   = new TTree("mini_eigen_reg_5bpms_sorted_pruned_slug_avg",  "mini_eigen_reg_5bpms_sorted_pruned_slug_avg");
    TTree * out_tree_mini_eigen_reg_allbpms_sorted_slug = new TTree("mini_eigen_reg_allbpms_sorted_slug_avg","mini_eigen_reg_allbpms_sorted_slug_avg");
    TTree * out_tree_mini_eigen_reg_5bpms_sorted_slug   = new TTree("mini_eigen_reg_5bpms_sorted_slug_avg",  "mini_eigen_reg_5bpms_sorted_slug_avg");

    // Part averages
    averaging_timescale = "crex_part";
    cut = "dit_flag==1";
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_part_pruned,"mini_eigen_reg_allbpms_sorted",cut,detectors,monitors,devices);
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_part_pruned,"mini_eigen_reg_5bpms_sorted",cut,detectors,monitors5,devices5);
    out_tree_mini_eigen_reg_allbpms_sorted_part_pruned->Write();
    out_tree_mini_eigen_reg_5bpms_sorted_part_pruned->Write();
    delete out_tree_mini_eigen_reg_allbpms_sorted_part_pruned;
    delete out_tree_mini_eigen_reg_5bpms_sorted_part_pruned;
    */






    /*
    ////// Part averaging goes on here     
    averaging_timescale = "crex_part";
    //averaging_timescale = "crex_part_b";
    cut = "(1==1) && (" + mod_cut + ")";
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_part,"mini_eigen_reg_allbpms_sorted",cut,detectors,monitors,devices);
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_part,"mini_eigen_reg_5bpms_sorted",cut,detectors,monitors5,devices5);
    out_tree_mini_eigen_reg_allbpms_sorted_part->Write();
    out_tree_mini_eigen_reg_5bpms_sorted_part->Write();
    delete out_tree_mini_eigen_reg_allbpms_sorted_part;
    delete out_tree_mini_eigen_reg_5bpms_sorted_part;
    */


    /*
    //out_tree_mini_eigen_reg_allbpms_tr->Write();
    // Segment averages
    averaging_timescale = "dit_segment";
    cut = "dit_flag==1";
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_seg_pruned,"mini_eigen_reg_allbpms_sorted",cut,detectors,monitors,devices);
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_seg_pruned,"mini_eigen_reg_5bpms_sorted",cut,detectors,monitors5,devices5);
    out_tree_mini_eigen_reg_allbpms_sorted_seg_pruned->Write();
    out_tree_mini_eigen_reg_5bpms_sorted_seg_pruned->Write();
    delete out_tree_mini_eigen_reg_allbpms_sorted_seg_pruned;
    delete out_tree_mini_eigen_reg_5bpms_sorted_seg_pruned;
    */
    cut = "(1==1) && (" + mod_cut + ")";
    averaging_timescale = "dit_segment";
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_seg,"mini_eigen_reg_allbpms_sorted",cut,detectors,monitors,devices);
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_seg,"mini_eigen_reg_5bpms_sorted",cut,detectors,monitors5,devices5);

    out_tree_mini_eigen_reg_allbpms_sorted_seg->Write();
    out_tree_mini_eigen_reg_5bpms_sorted_seg->Write();
    delete out_tree_mini_eigen_reg_allbpms_sorted_seg;
    delete out_tree_mini_eigen_reg_5bpms_sorted_seg;
    /*

    // Slug averages
    cut = "dit_flag==1";
    averaging_timescale = "rcdb_slug";
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_slug_pruned,"mini_eigen_reg_allbpms_sorted",cut,detectors,monitors,devices);
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_slug_pruned,"mini_eigen_reg_5bpms_sorted",cut,detectors,monitors5,devices5);
    out_tree_mini_eigen_reg_allbpms_sorted_slug_pruned->Write();
    out_tree_mini_eigen_reg_5bpms_sorted_slug_pruned->Write();
    delete out_tree_mini_eigen_reg_allbpms_sorted_slug_pruned;
    delete out_tree_mini_eigen_reg_5bpms_sorted_slug_pruned;
    cut = "(1==1) && (" + mod_cut + ")";
    averaging_timescale = "rcdb_slug";
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_slug,"mini_eigen_reg_allbpms_sorted",cut,detectors,monitors,devices);
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_slug,"mini_eigen_reg_5bpms_sorted",cut,detectors,monitors5,devices5);

    out_tree_mini_eigen_reg_allbpms_sorted_slug->Write();
    out_tree_mini_eigen_reg_5bpms_sorted_slug->Write();
    delete out_tree_mini_eigen_reg_allbpms_sorted_slug;
    delete out_tree_mini_eigen_reg_5bpms_sorted_slug;
  //}
  //else if (ana == "runavg") {

    // Run averages
    TTree * out_tree_mini_eigen_reg_allbpms_sorted_run_pruned = new TTree("mini_eigen_reg_allbpms_sorted_pruned_run_avg","mini_eigen_reg_allbpms_sorted_pruned_run_avg");
    TTree * out_tree_mini_eigen_reg_5bpms_sorted_run_pruned   = new TTree("mini_eigen_reg_5bpms_sorted_pruned_run_avg",  "mini_eigen_reg_5bpms_sorted_pruned_run_avg");
    TTree * out_tree_mini_eigen_reg_allbpms_sorted_run = new TTree("mini_eigen_reg_allbpms_sorted_run_avg","mini_eigen_reg_allbpms_sorted_run_avg");
    TTree * out_tree_mini_eigen_reg_5bpms_sorted_run   = new TTree("mini_eigen_reg_5bpms_sorted_run_avg",  "mini_eigen_reg_5bpms_sorted_run_avg");

    // Run averages
    cut = "dit_flag==1";
    averaging_timescale = "run";
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_run_pruned,"mini_eigen_reg_allbpms_sorted",cut,detectors,monitors,devices);
    out_tree_mini_eigen_reg_allbpms_sorted_run_pruned->Write();
    delete out_tree_mini_eigen_reg_allbpms_sorted_run_pruned;
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_run_pruned,"mini_eigen_reg_5bpms_sorted",cut,detectors,monitors5,devices5);
    out_tree_mini_eigen_reg_5bpms_sorted_run_pruned->Write();
    delete out_tree_mini_eigen_reg_5bpms_sorted_run_pruned;
    cut = "";
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_allbpms_sorted_run,"mini_eigen_reg_allbpms_sorted",cut,detectors,monitors,devices);
    out_tree_mini_eigen_reg_allbpms_sorted_run->Write();
    delete out_tree_mini_eigen_reg_allbpms_sorted_run;
    combo_segment_getter(averaging_timescale,mini,out_tree_mini_eigen_reg_5bpms_sorted_run,"mini_eigen_reg_5bpms_sorted",cut,detectors,monitors5,devices5);
    out_tree_mini_eigen_reg_5bpms_sorted_run->Write();
    delete out_tree_mini_eigen_reg_5bpms_sorted_run;
    */
  }


  // Loop over the prior file's existing TTrees and print all of those as well, uneditted.
  TFile infile(infilename);
  TKey *key;
  TIter nextkey(infile.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)infile.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!newfile.FindObject(key->GetName())) {
        newfile.cd();
        TTree *newT = T->CloneTree(-1,"fast");
        newT->Write();
      }
    }
  }
  infile.Close();

  newfile.Close();
}
