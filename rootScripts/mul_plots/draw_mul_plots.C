#include <iostream>
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

void draw_mul_plots(const char * name, const char * regress_files[], const int nfiles) {	// draw both raw and reg pair plot in normal and log scale
    TChain * treg = new TChain("reg");
    for(int i=0; i<nfiles; i++) {
      treg->AddFile(regress_files[i]);
    }
    TChain * tdit = new TChain("dit");
    for(int i=0; i<nfiles; i++) {
      tdit->AddFile(regress_files[i]);
    }

    TH1F * us_avg = new TH1F("us_avg", "us avg;ppm", 100, -3000, 3000);
    TH1F * us_dd = new TH1F("us_dd", "us dd;ppm", 100, -3000, 3000);
    TH1F * ds_avg = new TH1F("ds_avg", "ds avg;ppm", 100, -3000, 3000);
    TH1F * ds_dd = new TH1F("ds_dd", "ds dd;ppm", 100, -3000, 3000);
    TCanvas * c = new TCanvas("c", "c", 3200, 1400);
    c->Divide(4, 2);
    gStyle->SetOptStat("eMRuo");
    gStyle->SetOptFit(111);

    const char* prefix[] = {"raw", "reg"};
    TCut ok_cut("ok_cut==1");
    for(int i=0; i<2; i++) {
      c->cd(1);
      treg->Draw(Form("%s_asym_us_avg/ppm>>us_avg", prefix[i]), ok_cut);
      us_avg->Fit("gaus");
      c->cd(5);
      gPad->SetLogy();
      us_avg->Draw();

      c->cd(2);
      treg->Draw(Form("%s_asym_us_dd/ppm>>us_dd", prefix[i]), ok_cut);
      us_dd->Fit("gaus");
      c->cd(6);
      gPad->SetLogy();
      us_dd->Draw();

      c->cd(3);
      treg->Draw(Form("%s_asym_ds_avg/ppm>>ds_avg", prefix[i]), ok_cut);
      ds_avg->Fit("gaus");
      c->cd(7);
      gPad->SetLogy();
      ds_avg->Draw();

      c->cd(4);
      treg->Draw(Form("%s_asym_ds_dd/ppm>>ds_dd", prefix[i]), ok_cut);
      ds_dd->Fit("gaus");
      c->cd(8);
      gPad->SetLogy();
      ds_dd->Draw();
      c->Print(Form("%s_ud_%s.png", name, prefix[i]));
    }

    // dithering plot
    TCut dit_cut("ErrorFlag==0");
    {
      c->cd(1);
      treg->Draw("dit_asym_us_avg/ppm>>us_avg", dit_cut);
      us_avg->Fit("gaus");
      c->cd(5);
      gPad->SetLogy();
      us_avg->Draw();

      c->cd(2);
      treg->Draw("dit_asym_us_dd/ppm>>us_dd", dit_cut);
      us_dd->Fit("gaus");
      c->cd(6);
      gPad->SetLogy();
      us_dd->Draw();

      c->cd(3);
      treg->Draw("dit_asym_ds_avg/ppm>>ds_avg", dit_cut);
      ds_avg->Fit("gaus");
      c->cd(7);
      gPad->SetLogy();
      ds_avg->Draw();

      c->cd(4);
      treg->Draw("dit_asym_ds_dd/ppm>>ds_dd", dit_cut);
      ds_dd->Fit("gaus");
      c->cd(8);
      gPad->SetLogy();
      ds_dd->Draw();
      c->Print(Form("%s_ud_dit.png", name));
    }
}



int main(int argc, char *argv[]) {
    if (argc <= 2) {
      cout << "Error, at least two arguments needed." << endl;
      exit(1);
    }
    const char * name = argv[1];
    const char * regress_files[argc-2];
    for(int i=2; i<argc; i++)
      regress_files[i-2] = argv[i];

    draw_mul_plots(name, regress_files, argc-2);
    return(0);
}
