//us_avg      err         usl         err         usr         err
//1.064e-05 * 1.165e-05 * 2.586e-05 * 1.647e-05 * -4.56e-06 * 1.634e-05
// RMSs
//0.0007723 * 8.241e-06 * 0.0010919 * 1.165e-05 * 0.0010827 * 1.155e-05

void fix_mini_friended_agg_8082(TString oldFileName = "processed_respin2_data/mini_friended_agg.root", Double_t runnumber = 8082){
  // agg_ErrorFlag_part_avgd_friendable->Scan("run_number:minirun_n:eigen_dit_asym_us_avg_mean:eigen_dit_asym_us_avg_mean_error:eigen_dit_asym_usl_mean:eigen_dit_asym_usl_mean_error:eigen_dit_asym_usr_mean:eigen_dit_asym_usr_mean_error:
  // Get old file, old tree and set top branch address
  cout << "Updating respin2 data for plain dit asyms " << std::endl;
  Bool_t newFile = gSystem->AccessPathName(oldFileName);
  // Opposite return convention from the usual
  if (!newFile){

    TFile* oldfile = TFile::Open(oldFileName);
    TTree *oldtree;
    oldfile->GetObject("agg_plain_friendable", oldtree);
    Int_t nentries = oldtree->GetEntries();

    TFile newfile(Form("localTmp_%d.root",(Int_t)runnumber), "recreate");
    auto newtree = oldtree->CloneTree(0);

    Double_t dit_asym_us_avg_mean;
    Double_t dit_asym_us_avg_mean_error;
    Double_t dit_asym_usl_mean;
    Double_t dit_asym_usl_mean_error;
    Double_t dit_asym_usr_mean ;
    Double_t dit_asym_usr_mean_error;
    Double_t dit_asym_us_avg_rms;
    Double_t dit_asym_us_avg_rms_error;
    Double_t dit_asym_usl_rms;
    Double_t dit_asym_usl_rms_error;
    Double_t dit_asym_usr_rms ;
    Double_t dit_asym_usr_rms_error;
    oldtree->SetBranchAddress("dit_asym_us_avg_mean",&dit_asym_us_avg_mean);
    oldtree->SetBranchAddress("dit_asym_us_avg_mean_error",&dit_asym_us_avg_mean_error);
    oldtree->SetBranchAddress("dit_asym_usl_mean",&dit_asym_usl_mean       );
    oldtree->SetBranchAddress("dit_asym_usl_mean_error",&dit_asym_usl_mean_error   );
    oldtree->SetBranchAddress("dit_asym_usr_mean",&dit_asym_usr_mean       );
    oldtree->SetBranchAddress("dit_asym_usr_mean_error",&dit_asym_usr_mean_error   );
    oldtree->SetBranchAddress("dit_asym_us_avg_rms",&dit_asym_us_avg_rms);
    oldtree->SetBranchAddress("dit_asym_us_avg_rms_error",&dit_asym_us_avg_rms_error);
    oldtree->SetBranchAddress("dit_asym_usl_rms",&dit_asym_usl_rms       );
    oldtree->SetBranchAddress("dit_asym_usl_rms_error",&dit_asym_usl_rms_error   );
    oldtree->SetBranchAddress("dit_asym_usr_rms",&dit_asym_usr_rms       );
    oldtree->SetBranchAddress("dit_asym_usr_rms_error",&dit_asym_usr_rms_error   );

    TLeaf* runNumL = oldtree->GetLeaf("run_number");
    Double_t tmprunNum = 0.0;
    for (auto i : ROOT::TSeqI(nentries)) {
      runNumL->GetBranch()->GetEntry(i);
      tmprunNum = runNumL->GetValue();
      oldtree->GetEntry(i);
      if (tmprunNum==runnumber) {
        // Overwrite run 8082 dit data with eigen dit values .... evil hack
        dit_asym_us_avg_mean       =1.064e-05;
        dit_asym_us_avg_mean_error =1.165e-05;
        dit_asym_usl_mean          =2.586e-05;
        dit_asym_usl_mean_error    =1.647e-05;
        dit_asym_usr_mean          =-4.56e-06;
        dit_asym_usr_mean_error    =1.634e-05;
        dit_asym_us_avg_rms        =0.0007723;
        dit_asym_us_avg_rms_error  =8.241e-06;
        dit_asym_usl_rms           =0.0010919;
        dit_asym_usl_rms_error     =1.165e-05;
        dit_asym_usr_rms           =0.0010827;
        dit_asym_usr_rms_error     =1.155e-05;
      }
      newtree->Fill();
    }
    newtree->Write("agg_plain_friendable",TObject::kOverwrite);
    
    TKey *key;
    TIter nextkey(oldfile->GetListOfKeys(),kIterBackward);
    while ((key = (TKey*)nextkey())) {
      const char *classname = key->GetClassName();
      TClass *cl = gROOT->GetClass(classname); 
      if (!cl) continue;
      if (cl->InheritsFrom(TTree::Class())) {
        TTree *T = (TTree*)oldfile->Get(key->GetName());
        if ((TString)key->GetName() == "agg_plain_friendable") {
          // Skip the tree we've been working with
          continue;
        }
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
    newfile.Close();
    oldfile->Close();
    delete oldfile;
    //gSystem->Exec(Form("mv localTmp_%d.root %s",(Int_t)runnumber,oldFileName.Data()));
  }
}
