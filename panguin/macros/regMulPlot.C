///Panguin gui compatible diagnostic plots for shift taking online replay 
#include <TPad.h>
#include <TObjArray.h>
#include <TLatex.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TTree.h>
using namespace std;
void regMulPlot(TString type = "burst_lrb_std", TString det = "us_avg") {
  gStyle->SetOptStat(0);
  TTree* tree_C = (TTree*)gDirectory->Get(type);
  Int_t ind = 0;
  if (det=="usl") ind = 0;
  if (det=="usr") ind = 1;
  if (det=="us_avg") ind = 2;
  if (det=="us_dd") ind = 3;
  int num = tree_C->Draw(Form("A[0][%d]",ind),"","goff"); // Number of miniruns
  if(num<1) {
    TPad *CPlot = new TPad("CPlot","CPlot",0,0,1,1);
    CPlot->Divide(1,3);
    CPlot->Draw();
    TLatex line1;
    TLatex line2;
    TLatex line3;
    line1.SetTextSize(.3);
    line1.SetTextAlign(12);
    line2.SetTextSize(.2);
    line2.SetTextAlign(12);
    line3.SetTextSize(.15);
    line3.SetTextAlign(12);

    CPlot->cd(1);
    line1.DrawLatex(.25,.5,"No Miniruns");

    CPlot->cd(2);
    line2.DrawLatex(.25,.5,"Completed Yet");

    CPlot->cd(3);
    line3.DrawLatex(.12,.75,"(Wait for 5 minutes of good data)");
    return 0;
  }
  
  TPad *CPlot = new TPad("CPlot","CPlot",0,0,1,1);
  CPlot->Draw();
  tree_C->Draw(">>elist", "","entrylist");
  TEntryList *elist = (TEntryList*)gDirectory->Get("elist");
  tree_C->SetEntryList(elist);
  std::vector<std::vector<Double_t>> d_slopes;
  int nonzero = tree_C->Draw("A[0][0]","","goff");
  for (Int_t IVs = 0; IVs < 5; IVs++) {
    std::vector<Double_t> tmp_d_slopess;
    for (Int_t i = 0; i<nonzero; i++) {
      tmp_d_slopess.push_back(0.0);
    }
    d_slopes.push_back(tmp_d_slopess);
  }
  for (Int_t IVs = 0; IVs < 5; IVs++) {
    tree_C->Draw(Form("A[%d][%d]",IVs,ind),"","GOFF");
    std::vector<double> xVals(tree_C->GetV1(), tree_C->GetV1() + tree_C->GetSelectedRows() % tree_C->GetEstimate());
    for (Int_t j = 0; j<xVals.size(); j++) {
      d_slopes.at(IVs).at(j) = xVals.at(j);
    }
  }
  TString drawDiffs[5];
  TTree* tree_R;
  TTree* tree_Rp;
  TString draw;
  TString cut;
/*  if (ind > 1) {
    tree_R = (TTree*)gDirectory->Get("mulc");
    tree_Rp = (TTree*)gDirectory->Get("mul");
    tree_R->AddFriend(tree_Rp);
    draw = "asym_"+det+".hw_sum";
    drawDiffs[0]="mul.diff_bpm4aX";
    drawDiffs[1]="mul.diff_bpm4aY";
    drawDiffs[2]="mul.diff_bpm4eX";
    drawDiffs[3]="mul.diff_bpm4eY";
    drawDiffs[4]="mul.diff_bpm12X";
    cut = Form("mul.ErrorFlag==0&&asym_%s.Device_Error_Code==0",det.Data());
  }*/
  if (ind > 1) {
    tree_R = (TTree*)gDirectory->Get("mul");
    if (det.Contains("dd")) {
      draw = "(asym_usl.hw_sum-asym_usr.hw_sum)/2";
    }
    else {
      draw = "(asym_usl.hw_sum+asym_usr.hw_sum)/2";
    }
    drawDiffs[0]="diff_bpm4aX";
    drawDiffs[1]="diff_bpm4aY";
    drawDiffs[2]="diff_bpm4eX";
    drawDiffs[3]="diff_bpm4eY";
    drawDiffs[4]="diff_bpm12X";
    cut = Form("ErrorFlag==0&&asym_usl.Device_Error_Code==0&&asym_usr.Device_Error_Code==0");
  }
  else {
    tree_R = (TTree*)gDirectory->Get("mul");
    tree_R->AddFriend(tree_R);
    draw = "asym_"+det+".hw_sum";
    drawDiffs[0]="diff_bpm4aX";
    drawDiffs[1]="diff_bpm4aY";
    drawDiffs[2]="diff_bpm4eX";
    drawDiffs[3]="diff_bpm4eY";
    drawDiffs[4]="diff_bpm12X";
    cut = Form("ErrorFlag==0&&asym_%s.Device_Error_Code==0",det.Data());
  }
  gStyle->SetOptStat(1111);
  TString bdraw = draw;
  TString bcut = cut;
  for (Int_t n = 0; n<nonzero; n++) {
    if (n == nonzero-1) {
      bcut = cut+Form("&&mul.BurstCounter>=%d",n);
    }
    else {
      bcut = cut+Form("&&mul.BurstCounter==%d",n);
    }
    for (int i=0;i<5; i++){
      bdraw = Form("%s-1.0*%f*%s",bdraw.Data(),(Double_t)(d_slopes.at(i).at(n)),drawDiffs[i].Data());
    }
    if (n == 0) {
      tree_R->Draw(Form("(%s)/ppm>>%s",bdraw.Data(),det.Data()),bcut.Data());
    }
    else { 
      tree_R->Draw(Form("(%s)/ppm>>+%s",bdraw.Data(),det.Data()),bcut.Data());
    }
    bdraw = draw;
  }
  TH1 *h1 = (TH1*)gROOT->FindObject(Form("%s",det.Data()));
  h1->SetTitle(Form("Regressed %s",det.Data()));
  h1->GetYaxis()->SetTitle(Form("Regressed %s",det.Data()));
  h1->GetXaxis()->SetTitle(Form("ppm"));
  h1->Draw();
}
