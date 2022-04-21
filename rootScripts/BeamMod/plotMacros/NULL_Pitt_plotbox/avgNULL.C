void avgNULL(TString cutAna = "ErrorFlag") {

  //TCut cut = "1==1";//"rcdb_flip_state <= 2 && rcdb_run_type == 1 && rcdb_run_flag == 1";
  TCut cut = "rcdb_flip_state <= 2 && rcdb_run_type == 1 && rcdb_run_flag == 1";

  gSystem->Exec(Form("hadd -f9 ../processed_respin2_data/results/CREX_All_%s_crex_pitt_Avg_Outputs_PartlyNULL_both_main_det_corrections.root  ../processed_respin2_data/results/CREX_All_%s_crex_pitt_Avg_Outputs_NULL_IN_main_det_corrections.root  ../processed_respin2_data/results/CREX_All_%s_crex_pitt_Avg_Outputs_NULL_OUT_main_det_corrections.root",cutAna.Data(),cutAna.Data(),cutAna.Data()));

  // Open input file
  TFile infile(Form("../processed_respin2_data/results/CREX_All_%s_crex_pitt_Avg_Outputs_PartlyNULL_both_main_det_corrections.root",cutAna.Data()),"read");
  // Open output file
  TFile newfile(Form("../processed_respin2_data/results/CREX_All_%s_crex_pitt_Avg_Outputs_NULL_both_main_det_corrections.root",cutAna.Data()), "recreate");
  TKey *key;
  TIter nextkey(infile.GetListOfKeys());//,kIterBackward);
  // Loop over input file's trees
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *intree = (TTree*)infile.Get(key->GetName());
      if (!newfile.FindObject(key->GetName())) {
        Printf("Trying tree %s",(((TString)key->GetName()).Data()));
        newfile.cd();
        //TTree *outtree = intree->CloneTree(-1,"fast");
        TTree *outtree = new TTree(intree->GetName(),intree->GetName());

        // Loop over tree's branches
        TObjArray *var_list=intree->GetListOfBranches();
        TObjArray *var_list2=(TObjArray*)var_list->Clone();
        TIter var_iter(var_list);

        Int_t nChans = 0; 
        std::map<TString,Int_t> nens;
        std::map<TString,Double_t> new_means;
        std::map<TString,Double_t> new_mean_errs;
        std::map<TString,Double_t> new_stddevs;
        std::map<TString,Double_t> new_nentriess;
        while (auto *var= var_iter.Next()) {
          TString vname = "";
          // if branch name ends in _mean_err then do calculation per timestep (crex_pitt)
          if (((TString)(var->GetName())).Contains("_mean_err_global_rescale")) {
            TString lname(((TString)(var->GetName()))(0,((TString)(var->GetName())).Length()-24));
            vname = lname;
            Int_t    nen          = 0;
            Double_t new_mean     = 0;
            Double_t new_mean_err = 0;
            Double_t new_stddev   = 0;
            Double_t new_nentries = 0;
            nens[vname] = nen;
            new_means[vname] = new_mean;
            new_mean_errs[vname] = new_mean_err;
            new_stddevs[vname] = new_stddev;
            new_nentriess[vname] = new_nentries;
            outtree->Branch(Form("%s_averaging_nentries",vname.Data()),&nens.at(vname));
            outtree->Branch(Form("%s_mean",vname.Data()),&new_means.at(vname));
            outtree->Branch(Form("%s_mean_err",vname.Data()),&new_mean_errs.at(vname));
            outtree->Branch(Form("%s_stddev",vname.Data()),&new_stddevs.at(vname));
            outtree->Branch(Form("%s_mean_nentries",vname.Data()),&new_nentriess.at(vname));
          }
        }

        std::map<Int_t,Int_t> crex_pitts;
        Int_t segment;
        Int_t segment_run;
        Int_t int_segment;
        intree->SetBranchAddress(Form("crex_pitt"),&int_segment);
        Int_t nentries = intree->Draw(Form(">>elist_1"), cut, "entrylist");
        TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_1"));
        intree->SetEntryList(elist);
        Printf("Tree has %d entries",nentries);
        for (Int_t nent = 0 ; nent<nentries ; nent++) {
          intree->GetEntry(intree->GetEntryNumber(nent));
          if (crex_pitts.count(int_segment) == 0) {
            crex_pitts[int_segment] = 1;
          }
          else {
            crex_pitts[int_segment] = crex_pitts.at(int_segment) + 1;
          }
        }
        intree->SetEntryList(0);
        delete elist;

        // Add rcdb info if available
        TObjArray *rcdb_var_list=intree->GetListOfBranches();
        TIter rcdb_var_iter(rcdb_var_list);
        std::vector<Double_t> rcdb_vals;
        std::vector<TString> rcdb_names;
        Int_t nRCDB = 0;

        TString rcdb_draws = "";
        while (auto *var= rcdb_var_iter.Next()){
          TString name(var->GetName());
          if (name.Contains("rcdb_")) {
            //if ((name == "rcdb_ihwp" && averaging == "rcdb_ihwp") || (name == "rcdb_slug" && averaging == "rcdb_slug") || (name == "rcdb_flip_state" && averaging == "rcdb_flip_state")) {
            // Safety check case
            //Printf("Skipping %s",name.Data());
            //}
            //else {
            rcdb_names.push_back(name);
            Double_t tmpval = 0.0;
            rcdb_vals.push_back(tmpval);
            nRCDB++;
            rcdb_draws = rcdb_draws + name + "+";
            //}
          }   
        }
        for (int m = 0 ; m < nRCDB ; m++) {
          // Add new branch to out_dit_tree
          intree->SetBranchAddress(rcdb_names[m],&rcdb_vals[m]);
          outtree->Branch(rcdb_names[m],&rcdb_vals[m]);
        }

        // Get the number of crex_pitts from the tree directly
        Int_t run = 0;
        Double_t run_number = 0;
        Double_t rcdb_ihwp = 0;
        Double_t crex_part_save = 0;
        Double_t crex_part_b_save = 0;
        Double_t crex_pitt = 0;
        Double_t crex_slow_control = 0;
        Double_t crex_slow_control_simple = 0;
        Int_t flag = 0;
        Double_t cyclenum = 0.0;
        Int_t totalEntries = intree->GetEntries();
        Int_t int_avg_time_scale = 0;
        intree->SetBranchAddress("crex_pitt",&int_avg_time_scale);
        rcdb_draws = rcdb_draws + "crex_pitt" + "+";
        if (intree->GetBranch("run")) {
          intree->SetBranchAddress("run",&run);
          outtree->Branch("run",&run);
          rcdb_draws = rcdb_draws + "run" + "+";
        }
        if (intree->GetBranch("cyclenum")) {
          intree->SetBranchAddress("cyclenum",&cyclenum);
          outtree->Branch("cyclenum",&cyclenum);
          rcdb_draws = rcdb_draws + "cyclenum" + "+";
        }
        if (intree->GetBranch("flag")) {
          intree->SetBranchAddress("flag",&flag);
          outtree->Branch("dit_flag",&flag);
          rcdb_draws = rcdb_draws + "flag" + "+";
        }
        if (intree->GetBranch("dit_flag")) {
          intree->SetBranchAddress("dit_flag",&flag);
          outtree->Branch("dit_flag",&flag);
          rcdb_draws = rcdb_draws + "dit_flag" + "+";
        }
        if (intree->GetBranch("crex_part")) {
          intree->SetBranchAddress("crex_part",&crex_part_save);
          outtree->Branch("crex_part",&crex_part_save);
          rcdb_draws = rcdb_draws + "crex_part" + "+";
        }
        if (intree->GetBranch("crex_part_b")) {
          intree->SetBranchAddress("crex_part_b",&crex_part_b_save);
          outtree->Branch("crex_part_b",&crex_part_b_save);
          rcdb_draws = rcdb_draws + "crex_part_b" + "+";
        }
        if (intree->GetBranch("crex_slow_control")) {
          intree->SetBranchAddress("crex_slow_control",&crex_slow_control);
          outtree->Branch("crex_slow_control",&crex_slow_control);
          rcdb_draws = rcdb_draws + "crex_slow_control" + "+";
        }
        if (intree->GetBranch("crex_slow_control_simple")) {
          intree->SetBranchAddress("crex_slow_control_simple",&crex_slow_control_simple);
          outtree->Branch("crex_slow_control_simple",&crex_slow_control_simple);
          rcdb_draws = rcdb_draws + "crex_slow_control_simple" + "+";
        }
        if (intree->GetBranch("run_number")) {
          intree->SetBranchAddress("run_number",&run_number);
          outtree->Branch("run_number",&run_number);
          rcdb_draws = rcdb_draws + "run_number" + "+";
        }
        rcdb_draws = rcdb_draws + "1.0";


        // Use TGraphErrors
        TGraphErrors * tge;

        // Use a regular TGraph
        //TGraph * tge;

        // FIXME Should this calculation be done once per each minirun entry in intree->outtree? Or store a giant map and print per entry later (seems inefficient). I am storing the number of entries that mattch the localAvg_time_scale == X cut, but there are possible entries with NO localAvg_time_scale value, or something like that, which would be edge cases... also I want to preserve the order of points from intree->outtree for friendability later, so I need to loop over friend tree for sure with some kind of lookup reference.
        // FIXME Idea: check if the localAvg_time_scale branch value has changed since the last event, if it has then calculate a new set of means and Fill, but doing a Fill && Continue if not
        Int_t localAvg_time_scale = -1000; // Default initialize to rediculous number
        Int_t iterLocalAvg_time_scale = -1000; // Default initialize to rediculous number
        //if (nentries_orig == 0){
          outtree->Branch("crex_pitt",&localAvg_time_scale);
        //}
        Int_t previousAvg_time_scale = -1000; // Default initialize to rediculous number
        Int_t lnen=0;
        Int_t outN=0;
        Int_t nSegments=0;
        for (std::map<Int_t,Int_t>::iterator iter = crex_pitts.begin() ; iter != crex_pitts.end(); ++iter){
          nSegments++;
          Printf("Averaging crex_pitt %d",iter->first);
          localAvg_time_scale = iter->first;
          previousAvg_time_scale  = iter->first;
          iterLocalAvg_time_scale = iter->first;


          // Extract vector of time step values, loop over time steps
          //    if first entry in time stepping then generate output tree Branch and variables to go in them (init to 0)
          //    loop over entries within each time step
          TIter var_iter2(var_list2);
          while (auto *var= var_iter2.Next()) {
            TString vname = "";
            // if branch name ends in _mean_err then do calculation per timestep (crex_pitt)
            if (((TString)(var->GetName())).Contains("_mean_err_global_rescale")) {
              TString lname(((TString)(var->GetName()))(0,((TString)(var->GetName())).Length()-24));
              vname = lname;
              nens[vname]           = 0;
              new_means[vname]      = 0;
              new_mean_errs[vname]  = 0;
              new_nentriess[vname]  = 0;
              new_stddevs[vname]    = 0;
              Printf("NULL Pitt averaging %s, crex_pitt==%d",vname.Data(),localAvg_time_scale);


              // Loop over tree's branches
              lnen = intree->Draw(Form("%s_mean:%s_mean_err:%s_stddev:%s_mean_nentries",vname.Data(),vname.Data(),vname.Data(),vname.Data()),cut+(TCut)((TString)Form("%s_mean_nentries!=0 && %s_mean_err != 0",vname.Data(),vname.Data()))+(TCut)((TString)Form("crex_pitt==%d",localAvg_time_scale)),"goff");
              nens[vname]           = lnen;
              for (Int_t i = 0 ; i < lnen; i++) {
                new_means[vname]     += intree->GetV1()[i]/lnen;
                new_mean_errs[vname]  = sqrt(pow(new_mean_errs.at(vname),2) + pow(intree->GetV2()[i],2));
                new_stddevs[vname]    = sqrt(pow(new_stddevs.at(vname),2) + pow(intree->GetV3()[i],2));
                new_nentriess[vname] += intree->GetV4()[i];
              }

              new_stddevs[vname]    = new_stddevs.at(vname)/sqrt(lnen);
              new_mean_errs[vname]  = new_mean_errs.at(vname)/lnen; // Feels wrong but it is right

              //   Print the new values to the tree once per time step
              // Go to next branch
            }
          }
          // Finish loop over branches
          intree->Draw(Form("%s",rcdb_draws.Data()),((TCut)(Form("crex_pitt>=%d-0.1 && crex_pitt<=%d+0.1",iterLocalAvg_time_scale,iterLocalAvg_time_scale)))+cut,"goff"); // Grab the entry list with the "global" cut (not sing rcdb_arm_flag cuts) to fill the RCDB branches properly
          outtree->Fill();
        }
        // Write
        outtree->Write();
      }
    }
    // Go to next tree
  }
  infile.Close();
  newfile.Close();
  // Close
}
