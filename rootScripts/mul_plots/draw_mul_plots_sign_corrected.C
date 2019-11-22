#include <iostream>
#include <fstream>
#include "TStyle.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TCut.h"
#include "TString.h"
#include "TRegexp.h"

using namespace std;

void draw_mul_plots_sign_corrected(const char * name, const int slugs[], const int nslugs) {	// draw both raw and reg pair plot in normal and log scale
    ifstream fin_signs("/adaqfs/home/apar/weibin/analysis/slug_sign.list", std::ifstream::in);
    if (!fin_signs.is_open()) {
      cout << "Error, unable to open slug sign file: /adaqfs/home/apar/weibin/analysis/slug_sign.list, please check it" << endl;
      exit(1);
    }
    int slug_number;
    string ihwp, flip;
    int signs[95];
    for(int i=0; i<95; i++) {
      signs[i] = 0; // initialization
    }
    while(fin_signs >> slug_number >> ihwp >> flip) {
      if (slug_number > 94) {
        cout << "Error, invalid slug number " << slug_number << endl;
        exit(2);
      }
      if (ihwp == "OUT") {
        if (flip == "RIGHT") 
          signs[slug_number] = 1;
        else if (flip == "LEFT")
          signs[slug_number] = -1;
        else {
          cout << "Error: Invalid wien flip value: " << flip.c_str() << endl;
          exit(2);
        }
      } else if (ihwp == "IN") {
        if (flip == "RIGHT") 
          signs[slug_number] = -1;
        else if (flip == "LEFT")
          signs[slug_number] = 1;
        else {
          cout << "Error: Invalid wien flip value: " << flip.c_str() << endl;
          exit(2);
        }
      } else {
        cout << "Error: Invalid IHWP value: " << ihwp.c_str() << endl;
        exit(2);
      }
    }
    fin_signs.close();


    const char * prefix[] = {"raw", "reg", "dit"};
    const char * vars[] = {"us_avg", "us_dd", "ds_avg", "ds_dd"};
    map<const char*, TH1F*[4]> hist;
    for(int i=0; i<3; i++) {
      for (int j=0; j<4; j++) {
        char * var = Form("%s_%s", prefix[i], vars[j]);
        char * title = Form("%s %s;ppm", prefix[i], vars[j]);
        hist[prefix[i]][j] = new TH1F(var, title, 100, -3000, 3000);
      }
    }

    double ErrorFlag, ok_cut;
    double raw_asym_us_avg, raw_asym_us_dd, raw_asym_ds_avg, raw_asym_ds_dd;
    double reg_asym_us_avg, reg_asym_us_dd, reg_asym_ds_avg, reg_asym_ds_dd;
    double dit_asym_us_avg, dit_asym_us_dd, dit_asym_ds_avg, dit_asym_ds_dd;

    for(int i=0; i<nslugs; i++) {
      const int slug = slugs[i];
      const int sign = signs[slug];
      if (sign == 0) {
        cout << "Error, no corresponding sign for slug " << slug << " in slug sign file: /adaqfs/home/apar/weibin/analysis/slug_sign.list, please check it" << endl;
        exit(3);
      }
      cout << "Info: processing slug " << slug << endl;
      const char * file_name = Form("results/slug%d_mul_tree.root", slug);
      TFile fin(file_name, "read");
      if (!fin.IsOpen()) {
        cout << "Error, unable to open root file: " << file_name << ". Please check it" << endl;
        exit(1);
      }

      TTree *treg = (TTree*) fin.Get("reg");
      if (!treg) {
        cout << "Error, unable to receive reg tree from root file: " << file_name << ". Please check it" << endl;
        exit(2);
      }
      TTree *tdit = (TTree*) fin.Get("dit");
      if (!tdit) {
        cout << "Error, unable to receive dit tree from root file: " << file_name << ". Please check it" << endl;
        exit(2);
      }

      treg->SetBranchAddress("ok_cut", &ok_cut);
      treg->SetBranchAddress("raw_asym_us_avg", &raw_asym_us_avg);
      treg->SetBranchAddress("raw_asym_us_dd", &raw_asym_us_dd);
      treg->SetBranchAddress("raw_asym_ds_avg", &raw_asym_ds_avg);
      treg->SetBranchAddress("raw_asym_ds_dd", &raw_asym_ds_dd);
      treg->SetBranchAddress("reg_asym_us_avg", &reg_asym_us_avg);
      treg->SetBranchAddress("reg_asym_us_dd", &reg_asym_us_dd);
      treg->SetBranchAddress("reg_asym_ds_avg", &reg_asym_ds_avg);
      treg->SetBranchAddress("reg_asym_ds_dd", &reg_asym_ds_dd);
      tdit->SetBranchAddress("ErrorFlag", &ErrorFlag);
      tdit->SetBranchAddress("dit_asym_us_avg", &dit_asym_us_avg);
      tdit->SetBranchAddress("dit_asym_us_dd", &dit_asym_us_dd);
      tdit->SetBranchAddress("dit_asym_ds_avg", &dit_asym_ds_avg);
      tdit->SetBranchAddress("dit_asym_ds_dd", &dit_asym_ds_dd);

      const double ppm = 1e-6;
      const int nreg = treg->GetEntries();
      for(int j=0; j<nreg; j++) {
        treg->GetEntry(j);
        if (ok_cut == 1) {
          hist["raw"][0]->Fill(sign*raw_asym_us_avg/ppm);
          hist["raw"][1]->Fill(sign*raw_asym_us_dd/ppm);
          hist["raw"][2]->Fill(sign*raw_asym_ds_avg/ppm);
          hist["raw"][3]->Fill(sign*raw_asym_ds_dd/ppm);
          hist["reg"][0]->Fill(sign*reg_asym_us_avg/ppm);
          hist["reg"][1]->Fill(sign*reg_asym_us_dd/ppm);
          hist["reg"][2]->Fill(sign*reg_asym_ds_avg/ppm);
          hist["reg"][3]->Fill(sign*reg_asym_ds_dd/ppm);
        }
      }
      const int ndit = tdit->GetEntries();
      for(int j=0; j<ndit; j++) {
        tdit->GetEntry(j);
        if (ErrorFlag == 0) {
          hist["dit"][0]->Fill(sign*dit_asym_us_avg/ppm);
          hist["dit"][1]->Fill(sign*dit_asym_us_dd/ppm);
          hist["dit"][2]->Fill(sign*dit_asym_ds_avg/ppm);
          hist["dit"][3]->Fill(sign*dit_asym_ds_dd/ppm);
        }
      }
      fin.Close();
    }

    for(int i=0; i<3; i++) {
      for (int j=0; j<4; j++) {
        hist[prefix[i]][j]->Fit("gaus");
      }
    }

    TCanvas * c = new TCanvas("c", "c", 3200, 1400);
    c->Divide(4, 2);
    gStyle->SetOptStat("eMRuo");
    gStyle->SetOptFit(111);

    for(int i=0; i<3; i++) {
      for (int j=0; j<4; j++) {
        c->cd(j+1);
        hist[prefix[i]][j]->Draw();
        c->cd(j+5);
        gPad->SetLogy();
        hist[prefix[i]][j]->Draw();

        int bmin = hist[prefix[i]][j]->GetXaxis()->FindBin(-400);
        int bmax = hist[prefix[i]][j]->GetXaxis()->FindBin(400);
        double in = hist[prefix[i]][j]->Integral(bmin, bmax);
        double total = hist[prefix[i]][j]->Integral(0, 101);
        double out = total - in;
        cout << "for " << vars[j] << ": " << in << " (" << in/total*100. << "%)" << " out of " << total << " lies within gaussian fit" << endl;
        cout << "for " << vars[j] << ": " << out << " (" << out/total*100. << "%)" << " out of " << total << " lies out of gaussian fit" << endl << endl;
      }

      c->Print(Form("%s_ud_%s.png", name, prefix[i]));
    }
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

    int slugs[argc-optind];
    for(int i=optind; i<argc; i++)
      slugs[i-optind] = atoi(argv[i]);

    cout << "Info: you input " << argc-optind << " slug number" << endl;

    draw_mul_plots_sign_corrected(name, slugs, argc-optind);

    return(0);
}
