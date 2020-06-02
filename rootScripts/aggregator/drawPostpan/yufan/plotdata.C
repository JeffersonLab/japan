//To run this macro just type root -l plotdata.C in your terminal.
//Author: Yufan Chen
//Before running the script, you need to change the device[] to the variables you need to plot and the accordingly unit[] of the variable
//You also need to change the constant sl(how many slugs) and the filename(csv or txt file that contains the data)
//The csv/txt file must be in the format of:FLIP(1 L, -1 R), IHWP(1 IN, -1 OUT), slug_num, mean1 err1 mean2 err2 mean3 err3 ...... w/o header and spacing with /t(tab)
//This script can plot 4 plots including FLIP L/R and IHWP IN/OUT

#include <TFile.h>
#include <TTree.h>
#include <Riostream.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <fstream>
#include <TChain.h>
#include <TROOT.h>
#include <iostream>
#include <fstream>
#include <TGaxis.h>
using namespace std;

void plotdata(){
  //variables need to be edited manually
  //Yield bpm(mm), bcm(uA), sam/det(mV_uA)
  //Diff bpm(nm)
  //Asym bcm/sam/det(ppb)
  static const Int_t sl = 21;
  TString device[]={"diff_bpm4aX","diff_bpm4aY","diff_bpm4eX","diff_bpm4eY","diff_bpm11X","asym_bcm_an_ds","asym_bcm_an_us","asym_bcm_dg_us","asym_bcm_dg_ds"};
          //,"asym_bcm_an_us_bcm_an_ds_agg_dd","asym_bcm_dg_us_bcm_an_ds_agg_dd","asym_bcm_dg_ds_bcm_an_ds_agg_dd","asym_bcm_dg_us_bcm_an_us_agg_dd","asym_bcm_dg_us_bcm_dg_ds_agg_dd"};
  TString title_s[]={"diff_bpm4aX","diff_bpm4aY","diff_bpm4eX","diff_bpm4eY","diff_bpm11X","asym_bcm_an_ds","asym_bcm_an_us","asym_bcm_dg_us","asym_bcm_dg_ds"};
          //,"dd-an_us_an_ds","dd-dg_us_an_ds","dd-dg_ds_an_ds","dd-dg_us_an_us","dd-dg_us_dg_ds"};
  static const Int_t var = sizeof(device)/sizeof(*device); 
  TString unit[var]={"nm","nm","nm","nm","nm","ppb","ppb","ppb","ppb"};//,"ppb","ppb","ppb","ppb","ppb"};
  Int_t flag[var][sl]={{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                       {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                       {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1},
                       {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1},
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};
  //up until here needs to be changed manually
  
  int i=0,j=0,k=0,m=0,n=0,o=0,p=0,q=(sl+1)*100,lin=0,lout=0,rin=0,rout=0;
  Int_t IHWP[sl],Wien[sl],HRS[sl];
  Double_t slug[sl];
  Double_t mean[var][sl];
  Double_t err[var][sl];
  Double_t x[q],y[q];
  Double_t ave_lin,ave_lout,ave_rin,ave_rout;
  Double_t error_lin,error_lout,error_rin,error_rout;
  Int_t ndf_lin,ndf_lout,ndf_rin,ndf_rout;
  Double_t ksq_lin,ksq_lout,ksq_rin,ksq_rout;
  Double_t p_lin,p_lout,p_rin,p_rout;

  //zero line
  for(i=0;i<q;i++){
    x[i] = i*0.01;
    y[i] = 0;
  }auto zero = new TGraph(q,x,y);
  zero->SetMarkerStyle(21);
  zero->SetMarkerSize(0.4);
  TLeaf *tl;

  //read /chafs2/work1/apar/aggRootfiles/slugRootfiles/grandRootfile/grand_aggregator.root
  TChain *agg = new TChain("agg");
  agg->Add("/chafs2/work1/apar/aggRootfiles/slugRootfiles/grandRootfile/grand_aggregator.root");
  for(j=0;j<sl;j++){
    slug[j]=(j+1)*1.0;
    agg->GetEntry(j);
    tl = agg->GetLeaf("ihwp");  
    IHWP[j]=tl->GetValue(0);
    tl = agg->GetLeaf("wein");  https://prex.jlab.org/wiki/index.php/Information_for_Shift_Takers
    Wien[j]=tl->GetValue(0);
    for(i=0;i<var;i++){
      tl = agg->GetLeaf(Form("%s_mean",device[i].Data()));  
      mean[i][j]=tl->GetValue(0);
      tl = agg->GetLeaf(Form("%s_mean_error",device[i].Data()));  
      err[i][j]=tl->GetValue(0);
    }//IHWP IN=1 OUT=2 / Wien R=1 L=?
    if(Wien[j]==2){
        if(IHWP[j]==1){
          lin+=1;
        }else{
          lout+=1;
        }
      }else{
        if(IHWP[j]==1){
          rin+=1;
        }else{
          rout+=1;
        }
      }
  }

  //putting mean and err to x,y,ex,ey array, to plot 4 separated TGraphErrors(IHWP IN/OUT WIEN L/R)
  Double_t x_lin[sl];  Double_t ex_lin[sl];  Double_t y_lin[sl];   Double_t ey_lin[sl]; 
  Double_t x_lout[sl];  Double_t ex_lout[sl];  Double_t y_lout[sl];   Double_t ey_lout[sl];
  Double_t x_rin[sl];  Double_t ex_rin[sl];  Double_t y_rin[sl];   Double_t ey_rin[sl]; 
  Double_t x_rout[sl];  Double_t ex_rout[sl];  Double_t y_rout[sl];   Double_t ey_rout[sl];
  TGraphErrors *slin;   TGraphErrors *slout;   TGraphErrors *srin;   TGraphErrors *srout;
  TCanvas *c1;
  c1 = new TCanvas("c1","",1200,700);
  for(i=0;i<var;i++){
    m=0,n=0,o=0,p=0;
    for(j=0;j<sl;j++){
      if(flag[i][j]==1){
        if(Wien[j]==2){
          if(IHWP[j]==1){
            x_lin[m]=slug[j];
            ex_lin[m]=0;
            y_lin[m]=mean[i][j];
            ey_lin[m]=err[i][j];
            m+=1;
          }else{
            x_lout[n]=slug[j];
            ex_lout[n]=0;
            y_lout[n]=mean[i][j];
            ey_lout[n]=err[i][j];
            n+=1;
          }
        }else{
          if(IHWP[j]==1){
            x_rin[o]=slug[j];
            ex_rin[o]=0;
            y_rin[o]=mean[i][j];
            ey_rin[o]=err[i][j];
            o+=1;
          }else{
            x_rout[p]=slug[j];
            ex_rout[p]=0;
            y_rout[p]=mean[i][j];
            ey_rout[p]=err[i][j];
            p+=1;
          }
        }
      }
    }c1->cd();
    //plot x,y,ex,ey into each TCanvas
    if(lin!=0){
      slin = new TGraphErrors(m,x_lin,y_lin,ex_lin,ey_lin);
      slin->SetMarkerColor(3);
      slin->SetMarkerStyle(22);
      slin->SetMarkerSize(1.5);
      slin->Fit("pol0");
      TF1 *fitslin=slin->GetFunction("pol0");
      fitslin->SetLineColor(6);
      ave_lin = fitslin->GetParameter(0);  
      error_lin = fitslin->GetParError(0); 
      ndf_lin = fitslin->GetNDF();
      ksq_lin = fitslin->GetChisquare();
      p_lin = fitslin->GetProb();
    }if(lout!=0){
      slout = new TGraphErrors(n,x_lout,y_lout,ex_lout,ey_lout);
      slout->SetMarkerColor(9);
      slout->SetMarkerStyle(23);
      slout->SetMarkerSize(1.5);
      slout->Fit("pol0");
      TF1 *fitslout=slout->GetFunction("pol0");
      fitslout->SetLineColor(4);
      ave_lout = fitslout->GetParameter(0);  
      error_lout = fitslout->GetParError(0);
      ndf_lout = fitslout->GetNDF();
      ksq_lout = fitslout->GetChisquare();
      p_lout = fitslout->GetProb();
    }if(rin!=0){
      srin = new TGraphErrors(o,x_rin,y_rin,ex_rin,ey_rin);
      srin->SetMarkerColor(6);
      srin->SetMarkerStyle(20);
      srin->SetMarkerSize(1.5);
      srin->Fit("pol0");
      TF1 *fitsrin=srin->GetFunction("pol0");
      fitsrin->SetLineColor(6);
      ave_rin = fitsrin->GetParameter(0);  
      error_rin = fitsrin->GetParError(0); 
      ndf_rin = fitsrin->GetNDF();
      ksq_rin = fitsrin->GetChisquare();
      p_rin = fitsrin->GetProb();
    }if(rout!=0){
      srout = new TGraphErrors(p,x_rout,y_rout,ex_rout,ey_rout);
      srout->SetMarkerColor(4);
      srout->SetMarkerStyle(21);
      srout->SetMarkerSize(1.5);
      srout->Fit("pol0");
      TF1 *fitsrout=srout->GetFunction("pol0");
      fitsrout->SetLineColor(4);
      ave_rout = fitsrout->GetParameter(0);  
      error_rout = fitsrout->GetParError(0);
      ndf_rout = fitsrout->GetNDF();
      ksq_rout = fitsrout->GetChisquare();
      p_rout = fitsrout->GetProb();
    }//put all TGraphErrors together and formatting
    auto b = new TMultiGraph();
    if(lin!=0){
      b->Add(slin);
    }if(lout!=0){
      b->Add(slout);
    }if(rin!=0){
      b->Add(srin);
    }if(rout!=0){
      b->Add(srout);
    }b->Add(zero);
    gStyle->SetOptTitle(0);
    b->GetXaxis()->SetTitle("slug_number");
    b->GetYaxis()->SetTitle(Form("%s",device[i].Data()));
    b->Draw("AP");
    TLatex *title =new TLatex(0.02,0.95,Form("%s",title_s[i].Data()));
    title->SetNDC();
    title->Draw();
    auto leg = new TLegend(0.3,1.0,1.0,0.85);
    if(lin!=0){
      TLegendEntry* l1 = leg->AddEntry(slin,Form("IHWP IN/Wien L A=%.2f+/-%.2f %s, dof=%i, #chi^{2}=%.2f, P=%.2f",ave_lin,error_lin,unit[i].Data(),ndf_lin,ksq_lin,p_lin));
      l1->SetTextColor(3);
    }if(lout!=0){
      TLegendEntry* l2 = leg->AddEntry(slout,Form("IHWP OUT/Wien L A=%.2f+/-%.2f %s, dof=%i, #chi^{2}=%.2f, P=%.2f",ave_lout,error_lout,unit[i].Data(),ndf_lout,ksq_lout,p_lout));
      l2->SetTextColor(9);
    }if(rin!=0){
      TLegendEntry* l3 = leg->AddEntry(srin,Form("IHWP IN/Wien R A=%.2f+/-%.2f %s, dof=%i, #chi^{2}=%.2f, P=%.2f",ave_rin,error_rin,unit[i].Data(),ndf_rin,ksq_rin,p_rin));
      l3->SetTextColor(6);
    }if(rout!=0){
      TLegendEntry* l4 = leg->AddEntry(srout,Form("IHWP OUT/Wien R A=%.2f+/-%.2f %s, dof=%i, #chi^{2}=%.2f, P=%.2f",ave_rout,error_rout,unit[i].Data(),ndf_rout,ksq_rout,p_rout));
      l4->SetTextColor(4);
    }leg->SetTextSize(0.032);
    leg->Draw();
    c1->SaveAs(Form("slug_summary_%s.pdf",device[i].Data()));
    c1->SaveAs(Form("Plots/slug_summary_%s.png",device[i].Data()));
  }gSystem->Exec(Form("pdfunite *.pdf Plots/slug_summary.pdf"));
  gSystem->Exec("rm -rf *.pdf");
}
