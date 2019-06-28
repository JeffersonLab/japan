
// Macro for SAM plots
//Auther: Devi Adhikari
//I have commented out the pdf outputs.
//You can uncomment if you want to produce plots as pdf file.
//Or you can also produce the plots into any other file format.
//To run this macro just type root-l SAM_pdf.C in your terminal.
//Then it will ask you to enter run number(only number).
//Type in run number you want to see and hit enter.
//This macro stands on its own.
//In order to configure to panguin some modifications are needed.
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <fstream>
 
void DET_noise(){
         gStyle->SetTitleYOffset(1.5);
//         gStyle->SetOptStat(0);
         TGaxis::SetMaxDigits(3);
         gStyle->SetStatX(0.9);
         gStyle->SetStatY(0.9);
         gStyle->SetStatW(0.32);
         gStyle->SetStatH(0.18);
         int run1=0;
         char buf[300];
         const int DET_num=8;
         TH1D *h_uV[DET_num];
         TH1D *h_ppm[DET_num];
   if(run1<=0){
	    cout << "\nPlease enter a Run Number (-1 to exit):";
	    cin >> run1;
    }
        ofstream outfile(Form("TextFiles/diff_run%d.txt",run1));
        TString root_file1 =Form("$QW_ROOTFILES/prexALL_%d.000.root",run1);
        TFile *rootfile1 = TFile::Open(root_file1);
        TTree *evtTree1 =(TTree*)rootfile1->Get("evt");
        TTree *mulTree1 =(TTree*)rootfile1->Get("mul");

        mulTree1->Draw(Form("1e6*asym_usr*yield_usr>>huV0"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_dsr*yield_dsr>>huV1"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_atr1*yield_atr1>>huV2"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_atr2*yield_atr2>>huV3"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_usl*yield_usl>>huV4"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_dsl*yield_dsl>>huV5"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_atl1*yield_atl1>>huV6"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_atl2*yield_atl2>>huV7"),"pattern_number>1",".");

        mulTree1->Draw(Form("1e6*asym_usr*yield_usr/5>>hppm0"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_dsr*yield_dsr/5>>hppm1"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_atr1*yield_atr1/5>>hppm2"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_atr2*yield_atr2/5>>hppm3"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_usl*yield_usl/5>>hppm4"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_dsl*yield_dsl/5>>hppm5"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_atl1*yield_atl1/5>>hppm6"),"pattern_number>1",".");
        mulTree1->Draw(Form("1e6*asym_atl2*yield_atl2/5>>hppm7"),"pattern_number>1",".");

   for(int i=0;i<DET_num;i++){
	h_uV[i] =(TH1D*)gDirectory->FindObject(Form("huV%d",i));
        h_uV[i]->SetDirectory(gROOT);
    }
    for(int i=0;i<DET_num;i++){
	h_ppm[i] =(TH1D*)gDirectory->FindObject(Form("hppm%d",i));
        h_ppm[i]->SetDirectory(gROOT);
    }
        TCanvas *c1 = new TCanvas("c1","",1200,700);
        c1->Clear();
        c1->Divide(4,2);
        c1->cd(1);
        gPad->SetLogy(1);
        h_uV[0]->SetTitle(Form("1e6*diff_usr(run%d)",run1));
        h_uV[0]->SetXTitle(Form("1e6*diff_usr(uV)"));
        h_uV[0]->Fit("gaus");
        c1->cd(2);
        gPad->SetLogy(1);
        h_uV[1]->SetTitle(Form("1e6*diff_dsr(run%d)",run1));
        h_uV[1]->SetXTitle(Form("1e6*diff_dsr(uV)"));
        h_uV[1]->Fit("gaus");
        c1->cd(3);
        gPad->SetLogy(1);
        h_uV[2]->SetTitle(Form("1e6*diff_atr1(run%d)",run1));
        h_uV[2]->SetXTitle(Form("1e6*diff_atr1(uV)"));
        h_uV[2]->Fit("gaus");
        c1->cd(4);
        gPad->SetLogy(1);
        h_uV[3]->SetTitle(Form("1e6*diff_atr2(run%d)",run1));
        h_uV[3]->SetXTitle(Form("1e6*diff_atr2(uV)"));
        h_uV[3]->Fit("gaus");
        c1->cd(5);
        gPad->SetLogy(1);
        h_uV[4]->SetTitle(Form("1e6*diff_usl(run%d)",run1));
        h_uV[4]->SetXTitle(Form("1e6*diff_usl(uV)"));
        h_uV[4]->Fit("gaus");
        c1->cd(6);
        gPad->SetLogy(1);
        h_uV[5]->SetTitle(Form("1e6*diff_dsl(run%d)",run1));
        h_uV[5]->SetXTitle(Form("1e6*diff_dsl(uV)"));
        h_uV[5]->Fit("gaus");
        c1->cd(7);
        gPad->SetLogy(1);
        h_uV[6]->SetTitle(Form("1e6*diff_atl1(run%d)",run1));
        h_uV[6]->SetXTitle(Form("1e6*diff_atl1(uV)"));
        h_uV[6]->Fit("gaus");
        c1->cd(8);
        gPad->SetLogy(1);
        h_uV[7]->SetTitle(Form("1e6*diff_atl2(run%d)",run1));
        h_uV[7]->SetXTitle(Form("1e6*diff_atl2(uV)"));
        h_uV[7]->Fit("gaus");
   for(int i=0;i<DET_num;i++){
    cout << h_uV[i]->GetMean() <<"	"<< h_uV[i]->GetRMS() << endl;
    outfile << h_uV[i]->GetMean() <<"	"<< h_uV[i]->GetRMS() << endl;
    }
    c1->SaveAs(Form("Plots/det_diff_uV_run%d.pdf",run1));

        TCanvas *c2 = new TCanvas("c2","",1200,700);
        c2->Clear();
        c2->Divide(4,2);
        c2->cd(1);
        gPad->SetLogy(1);
        h_ppm[0]->SetTitle(Form("1e6*diff_usr/5(run%d)",run1));
        h_ppm[0]->SetXTitle(Form("1e6*diff_usr/5(ppm)"));
        h_ppm[0]->Fit("gaus");
        c2->cd(2);
        gPad->SetLogy(1);
        h_ppm[1]->SetTitle(Form("1e6*diff_dsr/5(run%d)",run1));
        h_ppm[1]->SetXTitle(Form("1e6*diff_dsr/5(ppm)"));
        h_ppm[1]->Fit("gaus");
        c2->cd(3);
        gPad->SetLogy(1);
        h_ppm[2]->SetTitle(Form("1e6*diff_atr1/5(run%d)",run1));
        h_ppm[2]->SetXTitle(Form("1e6*diff_atr1/5(ppm)"));
        h_ppm[2]->Fit("gaus");
        c2->cd(4);
        gPad->SetLogy(1);
        h_ppm[3]->SetTitle(Form("1e6*diff_atr2/5(run%d)",run1));
        h_ppm[3]->SetXTitle(Form("1e6*diff_atr2/5(ppm)"));
        h_ppm[3]->Fit("gaus");
        c2->cd(5);
        gPad->SetLogy(1);
        h_ppm[4]->SetTitle(Form("1e6*diff_usl/5(run%d)",run1));
        h_ppm[4]->SetXTitle(Form("1e6*diff_usl/5(ppm)"));
        h_ppm[4]->Fit("gaus");
        c2->cd(6);
        gPad->SetLogy(1);
        h_ppm[5]->SetTitle(Form("1e6*diff_dsl/5(run%d)",run1));
        h_ppm[5]->SetXTitle(Form("1e6*diff_dsl/5(ppm)"));
        h_ppm[5]->Fit("gaus");
        c2->cd(7);
        gPad->SetLogy(1);
        h_ppm[6]->SetTitle(Form("1e6*diff_atl1/5(run%d)",run1));
        h_ppm[6]->SetXTitle(Form("1e6*diff_atl1/5(ppm)"));
        h_ppm[6]->Fit("gaus");
        c2->cd(8);
        gPad->SetLogy(1);
        h_ppm[7]->SetTitle(Form("1e6*diff_atl2/5(run%d)",run1));
        h_ppm[7]->SetXTitle(Form("1e6*diff_atl2/5(ppm)"));
        h_ppm[7]->Fit("gaus");
    c2->SaveAs(Form("Plots/det_diff_ppm_run%d.pdf",run1));

        rootfile1->Close();
    }


