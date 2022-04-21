void remove_tree(TString removeName = "") {
  if (removeName != "") {

    std::vector<TString> fileNames = {
      "CREX_All_IncludeBMOD_rcdb_flip_state_Avg_Outputs_main_det_corrections_wien3.root",
      "CREX_All_IncludeBMOD_rcdb_flip_state_Avg_Outputs_main_det_corrections.root",
      "CREX_All_IncludeBMOD_rcdb_sign_Avg_Outputs_main_det_corrections.root",
      "CREX_All_IncludeBMOD_crex_slow_control_Avg_Outputs_main_det_corrections.root",
      "CREX_All_IncludeBMOD_crex_slow_control_simple_Avg_Outputs_main_det_corrections.root",
      "CREX_All_IncludeBMOD_crex_pitt_Avg_Outputs_NULL_IN_main_det_corrections.root",
      "CREX_All_IncludeBMOD_crex_pitt_Avg_Outputs_NULL_OUT_main_det_corrections.root",
      "CREX_All_IncludeBMOD_crex_pitt_Avg_Outputs_main_det_corrections.root",
      "CREX_All_IncludeBMOD_rcdb_slug_Avg_Outputs_main_det_corrections.root",
      "CREX_All_IncludeBMOD_rcdb_ihwp_Avg_Outputs_main_det_corrections.root",
      "CREX_All_IncludeBMOD_crex_part_Avg_Outputs_main_det_corrections.root",
      "CREX_All_IncludeBMOD_rcdb_flip_state_Avg_Outputs_main_det_corrections_wien1.root",
      "CREX_All_IncludeBMOD_rcdb_flip_state_Avg_Outputs_main_det_corrections_wien2.root",
    };
    for (Int_t i = 0 ; i < fileNames.size() ; i++) {
      TFile infile("processed_respin2_data/removed_results/"+fileNames.at(i));
      TKey *key;
      TIter nextkey(infile.GetListOfKeys());
      //TIter nextkey(infile.GetListOfKeys(),kIterBackward);
      TFile newfile("processed_respin2_data/removed_results/"+fileNames.at(i), "recreate");
      newfile.cd();
      while ((key = (TKey*)nextkey())) {
        const char *classname = key->GetClassName();
        TClass *cl = gROOT->GetClass(classname); 
        if (!cl) continue;
        if (cl->InheritsFrom(TTree::Class())) {
          TTree *T = (TTree*)infile.Get(key->GetName());
          // Avoid writing the data of a TTree more than once.
          // Note this assume that older cycles are (as expected) older
          // snapshots of the TTree meta data.
          // Also don't copy trees == removeName
          if (!newfile.FindObject(key->GetName()) && (key->GetName() != removeName)) {
            newfile.cd();
            TTree *newT = T->CloneTree(-1,"fast");
            newT->Write();
          }
        }
      }
      infile.Close();
    }
  }
}
