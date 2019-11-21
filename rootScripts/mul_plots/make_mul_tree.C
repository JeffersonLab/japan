#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"

using namespace std;

void make_mul_tree(const char * name, const char* runs[], const int nruns=1){
    TChain * treg = new TChain("reg");
    TChain * tdit = new TChain("dit");
    for(int i=0; i<nruns; i++) {
      const char * run = runs[i];	
      char * reg_file = Form("/chafs2/work1/apar/postpan-outputs/prexPrompt_%s_regress_postpan.root", run);
      treg->AddFile(reg_file);
      cout << "add reg file: " << reg_file << endl; 
      char * dit_file = Form("/chafs2/work1/apar/DitherCorrection/prexPrompt_dither_%s.root", run);
      tdit->AddFile(dit_file);
      cout << "add dit file: " << dit_file << endl; 
    }

    // int run = runs[0];	// how to make sure each run contains the mul tree ?
    typedef struct {double hw_sum, block0, block1, block2, block3, num_samples, Device_Error_Code;} DEVICE;
    struct {double ppm, ppb;} units = {1e-6, 1e-9};
    double ErrorFlag, ok_cut;
    // DEVICE bpm4aX, bpm4aY, bpm4eX, bpm4eY, bpm11X, bpm12X;
    // DEVICE sam1, sam2, sam3, sam4, sam5, sam6, sam7, sam8;
    // DEVICE atr1, atr2, atl1, atl2;
    // DEVICE bcm_an_us, bcm_an_ds, bcm_an_ds3, bcm_dg_us, bcm_dg_ds;
    // DEVICE usl, usr, dsl, dsr;
    DEVICE dit_usl, dit_usr, dit_dsl, dit_dsr;
    // DEVICE us_avg, us_dd, ds_avg, ds_dd;
    // DEVICE left_avg, left_dd, right_avg, right_dd;
    // DEVICE lr_avg, lr_dd, ud_avg, ud_dd;
    // double diff_bpm4aX, diff_bpm4aY, diff_bpm4eX, diff_bpm4eY, diff_bpm11X;
    // double asym_sam1, asym_sam2, asym_sam3, asym_sam4, asym_sam5, asym_sam6, asym_sam7, asym_sam8;
    // double asym_atr1, asym_atr2, asym_atl1, asym_atl2;
    // double asym_bcm_an_us, asym_bcm_an_ds, asym_bcm_an_ds3, asym_bcm_dg_us, asym_bcm_dg_ds;
    double raw_asym_usl, raw_asym_usr, raw_asym_dsl, raw_asym_dsr;
    double raw_asym_us_avg, raw_asym_us_dd, raw_asym_ds_avg, raw_asym_ds_dd;
    double reg_asym_usl, reg_asym_usr, reg_asym_dsl, reg_asym_dsr;
    double reg_asym_us_avg, reg_asym_us_dd, reg_asym_ds_avg, reg_asym_ds_dd;
    double dit_asym_usl, dit_asym_usr, dit_asym_dsl, dit_asym_dsr;
    double dit_asym_us_avg, dit_asym_us_dd, dit_asym_ds_avg, dit_asym_ds_dd;
    // double asym_left_avg, asym_left_dd, asym_right_avg, asym_right_dd;
    // double asym_lr_avg, asym_lr_dd, asym_ud_avg, asym_ud_dd;
    // tdit->SetBranchAddress("units", &units);
    tdit->SetBranchAddress("ErrorFlag", &ErrorFlag);
    treg->SetBranchAddress("ok_cut", &ok_cut);
    // treg->SetBranchAddress("diff_bpm4aX", &bpm4aX);
    // treg->SetBranchAddress("diff_bpm4aY", &bpm4aY);
    // treg->SetBranchAddress("diff_bpm4eX", &bpm4eX);
    // treg->SetBranchAddress("diff_bpm4eY", &bpm4eY);
    // if (treg->GetBranch("diff_bpm11X"))
    //   treg->SetBranchAddress("diff_bpm11X", &bpm11X);
    // else
    //   treg->SetBranchAddress("diff_bpm12X", &bpm11X);  // convert 12X to 11X for uniform interface
    // treg->SetBranchAddress("asym_sam1", &sam1);
    // treg->SetBranchAddress("asym_sam2", &sam2);
    // treg->SetBranchAddress("asym_sam3", &sam3);
    // treg->SetBranchAddress("asym_sam4", &sam4);
    // treg->SetBranchAddress("asym_sam5", &sam5);
    // treg->SetBranchAddress("asym_sam6", &sam6);
    // treg->SetBranchAddress("asym_sam7", &sam7);
    // treg->SetBranchAddress("asym_sam8", &sam8);
    // treg->SetBranchAddress("asym_atr1", &atr1);
    // treg->SetBranchAddress("asym_atr2", &atr2);
    // treg->SetBranchAddress("asym_atl1", &atl1);
    // treg->SetBranchAddress("asym_atl2", &atl2);
    // treg->SetBranchAddress("asym_bcm_an_us", &bcm_an_us);
    // treg->SetBranchAddress("asym_bcm_an_ds", &bcm_an_ds);
    // treg->SetBranchAddress("asym_bcm_an_ds3", &bcm_an_ds3);
    // treg->SetBranchAddress("asym_bcm_dg_us", &bcm_dg_us);
    // treg->SetBranchAddress("asym_bcm_dg_ds", &bcm_dg_ds);
    treg->SetBranchAddress("asym_usl", &raw_asym_usl);
    treg->SetBranchAddress("asym_usr", &raw_asym_usr);
    treg->SetBranchAddress("asym_dsl", &raw_asym_dsl);
    treg->SetBranchAddress("asym_dsr", &raw_asym_dsr);
    treg->SetBranchAddress("asym_us_avg", &raw_asym_us_avg);
    treg->SetBranchAddress("asym_us_dd", &raw_asym_us_dd);
    treg->SetBranchAddress("asym_ds_avg", &raw_asym_ds_avg);
    treg->SetBranchAddress("asym_ds_dd", &raw_asym_ds_dd);
    treg->SetBranchAddress("reg_asym_usl", &reg_asym_usl);
    treg->SetBranchAddress("reg_asym_usr", &reg_asym_usr);
    treg->SetBranchAddress("reg_asym_dsl", &reg_asym_dsl);
    treg->SetBranchAddress("reg_asym_dsr", &reg_asym_dsr);
    treg->SetBranchAddress("reg_asym_us_avg", &reg_asym_us_avg);
    treg->SetBranchAddress("reg_asym_us_dd", &reg_asym_us_dd);
    treg->SetBranchAddress("reg_asym_ds_avg", &reg_asym_ds_avg);
    treg->SetBranchAddress("reg_asym_ds_dd", &reg_asym_ds_dd);
    // treg->SetBranchAddress("asym_left_avg", &left_avg);
    // treg->SetBranchAddress("asym_left_dd", &left_dd);
    // treg->SetBranchAddress("asym_right_avg", &right_avg);
    // treg->SetBranchAddress("asym_right_dd", &right_dd);

    tdit->SetBranchAddress("dit_asym_usl", &dit_usl);
    tdit->SetBranchAddress("dit_asym_usr", &dit_usr);
    tdit->SetBranchAddress("dit_asym_dsl", &dit_dsl);
    tdit->SetBranchAddress("dit_asym_dsr", &dit_dsr);

    TTree * tout_reg = new TTree("reg", "reg tree");
    TTree * tout_dit = new TTree("dit", "dit tree");
    if (!tout_reg || !tout_dit) {
      cout << "Error: unable to create new tree" << endl;
      exit(2);
    }

    tout_reg->Branch("units", &units, "ppm/D:ppb/D");
    tout_reg->Branch("ok_cut", &ok_cut, "ok_cut/D");
    // tout_reg->Branch("diff_bpm4aX", &diff_bpm4aX, "hw_sum/D");
    // tout_reg->Branch("diff_bpm4aY", &diff_bpm4aY, "hw_sum/D");
    // tout_reg->Branch("diff_bpm4eX", &diff_bpm4eX, "hw_sum/D");
    // tout_reg->Branch("diff_bpm4eY", &diff_bpm4eY, "hw_sum/D");
    // tout_reg->Branch("diff_bpm11X", &diff_bpm11X, "hw_sum/D");
    // tout_reg->Branch("asym_sam1", &asym_sam1, "hw_sum/D");
    // tout_reg->Branch("asym_sam2", &asym_sam2, "hw_sum/D");
    // tout_reg->Branch("asym_sam3", &asym_sam3, "hw_sum/D");
    // tout_reg->Branch("asym_sam4", &asym_sam4, "hw_sum/D");
    // tout_reg->Branch("asym_sam5", &asym_sam5, "hw_sum/D");
    // tout_reg->Branch("asym_sam6", &asym_sam6, "hw_sum/D");
    // tout_reg->Branch("asym_sam7", &asym_sam7, "hw_sum/D");
    // tout_reg->Branch("asym_sam8", &asym_sam8, "hw_sum/D");
    // tout_reg->Branch("asym_atr1", &asym_atr1, "hw_sum/D");
    // tout_reg->Branch("asym_atr2", &asym_atr2, "hw_sum/D");
    // tout_reg->Branch("asym_atl1", &asym_atl1, "hw_sum/D");
    // tout_reg->Branch("asym_atl2", &asym_atl2, "hw_sum/D");
    // tout_reg->Branch("asym_bcm_an_us", &asym_bcm_an_us, "hw_sum/D");
    // tout_reg->Branch("asym_bcm_an_ds", &asym_bcm_an_ds, "hw_sum/D");
    // tout_reg->Branch("asym_bcm_an_ds3", &asym_bcm_an_ds3, "hw_sum/D");
    // tout_reg->Branch("asym_bcm_dg_us", &asym_bcm_dg_us, "hw_sum/D");
    // tout_reg->Branch("asym_bcm_dg_ds", &asym_bcm_dg_ds, "hw_sum/D");
    tout_reg->Branch("raw_asym_usl", &raw_asym_usl, "raw_asym_usl/D");
    tout_reg->Branch("raw_asym_usr", &raw_asym_usr, "raw_asym_usr/D");
    tout_reg->Branch("raw_asym_dsl", &raw_asym_dsl, "raw_asym_dsl/D");
    tout_reg->Branch("raw_asym_dsr", &raw_asym_dsr, "raw_asym_dsr/D");
    tout_reg->Branch("raw_asym_us_avg", &raw_asym_us_avg, "raw_asym_us_avg/D");
    tout_reg->Branch("raw_asym_us_dd", &raw_asym_us_dd, "raw_asym_us_dd/D");
    tout_reg->Branch("raw_asym_ds_avg", &raw_asym_ds_avg, "raw_asym_ds_avg/D");
    tout_reg->Branch("raw_asym_ds_dd", &raw_asym_ds_dd, "raw_asym_ds_dd/D");
    tout_reg->Branch("reg_asym_usl", &reg_asym_usl, "reg_asym_usl/D");
    tout_reg->Branch("reg_asym_usr", &reg_asym_usr, "reg_asym_usr/D");
    tout_reg->Branch("reg_asym_dsl", &reg_asym_dsl, "reg_asym_dsl/D");
    tout_reg->Branch("reg_asym_dsr", &reg_asym_dsr, "reg_asym_dsr/D");
    tout_reg->Branch("reg_asym_us_avg", &reg_asym_us_avg, "reg_asym_us_avg/D");
    tout_reg->Branch("reg_asym_us_dd", &reg_asym_us_dd, "reg_asym_us_dd/D");
    tout_reg->Branch("reg_asym_ds_avg", &reg_asym_ds_avg, "reg_asym_ds_avg/D");
    tout_reg->Branch("reg_asym_ds_dd", &reg_asym_ds_dd, "reg_asym_ds_dd/D");
    tout_dit->Branch("ErrorFlag", &ErrorFlag, "ErrorFlag/D");
    tout_dit->Branch("units", &units, "ppm/D:ppb/D");
    tout_dit->Branch("dit_asym_usl", &dit_asym_usl, "dit_asym_usl/D");
    tout_dit->Branch("dit_asym_usr", &dit_asym_usr, "dit_asym_usr/D");
    tout_dit->Branch("dit_asym_dsl", &dit_asym_dsl, "dit_asym_dsl/D");
    tout_dit->Branch("dit_asym_dsr", &dit_asym_dsr, "dit_asym_dsr/D");
    tout_dit->Branch("dit_asym_us_avg", &dit_asym_us_avg, "dit_asym_us_avg/D");
    tout_dit->Branch("dit_asym_us_dd", &dit_asym_us_dd, "dit_asym_us_dd/D");
    tout_dit->Branch("dit_asym_ds_avg", &dit_asym_ds_avg, "dit_asym_ds_avg/D");
    tout_dit->Branch("dit_asym_ds_dd", &dit_asym_ds_dd, "dit_asym_ds_dd/D");

    const int nreg = treg->GetEntries();
    const int ndit = tdit->GetEntries();
    if (nreg != ndit) {
      cout << "Warning: unequal entries in reg and dit trees" << endl;
    }
    for(int i=0; i<nreg; i++) {
      if (i % 100000 == 0) {
        cout << "Processing " << i+1 << " reg event" << endl;
      }
      treg->GetEntry(i);
      // diff_bpm4aX = bpm4aX.hw_sum;
      // diff_bpm4aY = bpm4aY.hw_sum;
      // diff_bpm4eX = bpm4eX.hw_sum;
      // diff_bpm4eY = bpm4eY.hw_sum;
      // diff_bpm11X = bpm11X.hw_sum;
      // asym_sam1 = sam1.hw_sum;
      // asym_sam2 = sam2.hw_sum;
      // asym_sam3 = sam3.hw_sum;
      // asym_sam4 = sam4.hw_sum;
      // asym_sam5 = sam5.hw_sum;
      // asym_sam6 = sam6.hw_sum;
      // asym_sam7 = sam7.hw_sum;
      // asym_sam8 = sam8.hw_sum;
      // asym_atr1 = atr1.hw_sum;
      // asym_atr2 = atr2.hw_sum;
      // asym_atl1 = atl1.hw_sum;
      // asym_atl2 = atl2.hw_sum;
      // asym_bcm_an_us = bcm_an_us.hw_sum;
      // asym_bcm_an_ds = bcm_an_ds.hw_sum;
      // asym_bcm_an_ds3 = bcm_an_ds3.hw_sum;
      // asym_bcm_dg_us = bcm_dg_us.hw_sum;
      // asym_bcm_dg_ds = bcm_dg_ds.hw_sum;
      tout_reg->Fill();
    }
    cout << "Process " << nreg << " reg events in total" << endl;

    if (ndit != 0) {
      for(int i=0; i<ndit; i++) {
        if (i % 100000 == 0) {
          cout << "Processing " << i+1 << " dit event" << endl;
        }
        tdit->GetEntry(i);
        dit_asym_usl = dit_usl.hw_sum;
        dit_asym_usr = dit_usr.hw_sum;
        dit_asym_dsl = dit_dsl.hw_sum;
        dit_asym_dsr = dit_dsr.hw_sum;
        dit_asym_us_avg = (dit_asym_usl + dit_asym_usr) / 2;
        dit_asym_us_dd = (dit_asym_usl - dit_asym_usr) / 2;
        dit_asym_ds_avg = (dit_asym_dsl + dit_asym_dsr) / 2;
        dit_asym_ds_dd = (dit_asym_dsl - dit_asym_dsr) / 2;
        tout_dit->Fill();
      }
      cout << "Process " << ndit << " dit events in total" << endl;
    } else {
      cout << "Warning: No dithering data" << endl;
    }

    TFile * fout = new TFile(Form("%s_mul_tree.root", name), "recreate");
    if (!fout->IsOpen()) {
      printf("Error, unable to create new root file: %s_mul_tree.root, please check it.\n", name);
      exit(2);
    }

    fout->cd();
    tout_reg->Write();
    tout_dit->Write();
    fout->Close();
}

int main(int argc, char *argv[]) {
    char opt;
    char const * name = "default";
    while( (opt=getopt(argc,argv,"n:"))!=-1){
      switch(opt){
        case ':':
          cout << argv[optind-1] << "requires a value. " << endl;
          exit(1);
        case '?':
          // cout << "Unknown arguments " << optopt << endl;
          exit(1);
        case 'n':
          name = optarg;
          break;
      }   
    }

    if (argc == optind) {
      cout << "Error, at least one run number needed" << endl;
      exit(1);
    }

    const char * runs[argc-optind];
    for(int i=optind; i<argc; i++)
      runs[i-optind] = argv[i];

    make_mul_tree(name, runs, argc-optind);

    return(0);
}
