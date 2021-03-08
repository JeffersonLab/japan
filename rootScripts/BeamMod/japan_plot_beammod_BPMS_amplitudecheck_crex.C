#include <TMath.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TLine.h>
#include <TLegend.h>
#include <TVector2.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TProfile.h>
#include <sstream>
#include <iostream>
#include <TGraphErrors.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string.h>
#include <TSystem.h>
#include <TEntryList.h>
#include <TTreeReader.h>
using namespace std; 
class TF1 *f1trap[42];
void japan_plot_beammod_BPMS_amplitudecheck_crex(int runNo=0, int segNo=0) { 
  gStyle->SetOptStat(0); 
  /*int filenum=1;
    if (inputfile_name.find("_",0) != string::npos) {
    filenum=atof(inputfile_name.substr(inputfile_name.find("_")+1,inputfile_name.find("_")).c_str());
    cout << "run num " << filenum <<"  "<< "make sure this is run for the beam modulation" <<  endl;
    }else{
    cout << "this file has no run number, please check the file name" << endl;
    }*/
  // ostringstream sstr0;
  // sstr0<<"BPMs_sensitivity_run"<<runNo<<"_check.txt";
  // ofstream outfile0(sstr0.str().c_str());
  // sstr0.str("");
  char infile[300];
  sprintf(infile,"$QW_ROOTFILES/prexPrompt_pass2_%d.%03d.root",runNo,segNo);
  TFile *file1=new TFile(infile);



  TGraphErrors *test;
  TTree *tree_R = (TTree*) file1->Get("evt");

  tree_R->Draw(">>elist","bmwcycnum>0","entrylist");  //picks out unique cycle numbers
  TEntryList *elist = (TEntryList*)gDirectory->Get("elist");
  tree_R->SetEntryList(elist);
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  int nonzero = tree_R->Draw("bmwcycnum","bmwcycnum>0");
  vector<Double_t> cycles;
  for(int i=0;i<nonzero;i++){
    l_bmwcycnum->GetBranch()->GetEntry(elist->GetEntry(i));
    Double_t cyclenum = l_bmwcycnum->GetValue();
    if(i==0){
      cycles.push_back(cyclenum);
    }   
    else{
      int sizeVec = cycles.size();
      if(cyclenum != cycles[sizeVec-1]){
        cycles.push_back(cyclenum);
      }
    }   
  }   
  const int n=cycles.size();//number of cycle
  int supercyc[n];
  int Is_fill[n];
  int tmpgood=0;
  int Is_good[n];
  for(int i=0;i<n;i++){
    Is_fill[i] = 0;
    Is_good[i] = 0;
    supercyc[i]=cycles[i]; 
    cout<<"i="<<i<<"  "<<"supercyc="<<supercyc[i]<<endl;
  }    

  const double trimmin=0.38;
  const double trimmax=0.7;
  const double bpmmax=3;
  const double bpmmin=1;
  const double chtov=1.0e-3;//7.62939453125000000e-05; //10V*(1/pow(2,17))
  const double factor=1.0;

  const int nCoil =7;
  const int nBPM = 8;

  TH2D * hist_BPM_coil[nBPM][nCoil][n];
  TGraphErrors *tgraph_BPM_coil[nBPM][nCoil][n];
  TLine * avg_line_BPM_coil[nBPM][nCoil][n];
  TLine * high_line_BPM_coil[nBPM][nCoil][n];
  TLine * low_line_BPM_coil[nBPM][nCoil][n];
  TText * avg_text_BPM_coil[nBPM][nCoil][n];
  TText * high_text_BPM_coil[nBPM][nCoil][n];
  TText * low_text_BPM_coil[nBPM][nCoil][n];
  TH2F * hist_test_coil;
  TCanvas *c[n][nCoil];
  TF1 *fun = new TF1("fun","[0]+[1]*x",0,1);
  int xnbins=0.0;
  int ynbins=0;
  double ibincon=0;
  int ibin=0;
  double xbincon=0.0;
  const int nbins = 25;
  int coilnum=0;
  TF1 *fun1=new TF1("fun1","[0]*sin((2*TMath::Pi()*[1])*x+[2])+[3]",100,2000);
  cout<<"Drawing the coil response plot"<<endl;
  TString wire[nCoil]={"bmod_trim1","bmod_trim2","bmod_trim3","bmod_trim4","bmod_trim5","bmod_trim6","bmod_trim7"};
  TString BPMS[nBPM]={"bpm1X","bpm4eX","bpm4aY","bpm4eY","bpm11X","bpm12X","bpm4aX","bpm16X"};
  int totalEntry[8][nCoil][n];
  TH1F * hist_trim[nCoil];
  Double_t trim_base[nCoil]={0};
  for(int icoil=0;icoil<nCoil;icoil++){
    tree_R->Draw(Form("%s>>hist_trim%d",wire[icoil].Data(),icoil),"bmod_ramp<0","");
    hist_trim[icoil] = (TH1F *)gDirectory->Get(Form("hist_trim%d",icoil));
    int maxbin = hist_trim[icoil]->GetMaximumBin();
    trim_base[icoil] = hist_trim[icoil]->GetXaxis()->GetBinCenter(maxbin);
    cout<<"trim_base="<<trim_base[icoil]<<endl;  
  }
  double rampentry[nbins]={0} ;
  double bpm_mean[nbins]={0};
  double bpm_rms[nbins]={0};
  double rampvalue[nbins]={0};
  double bpm_error[nbins]={0};
  char name_f[50];
  for(int t=0;t<n;t++){ 
    for(int j=0;j<nBPM;j++){
      for(int i=0;i<nCoil;i++){
        tree_R->Draw(Form("(%lf*%s):bmod_ramp>>hist_BPM_coil%d%d%d(25,0,1500)",factor,BPMS[j].Data(),i,j,t),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d&& bmwcycnum==%d",i+1,supercyc[t]),"goff");
        hist_BPM_coil[j][i][t] = (TH2D *)gDirectory->Get(Form("hist_BPM_coil%d%d%d",i,j,t));
        hist_BPM_coil[j][i][t]->GetYaxis()->SetTitle(Form("%s",BPMS[j].Data()));
        hist_BPM_coil[j][i][t]->GetYaxis()->SetTitleOffset(0.77);
        hist_BPM_coil[j][i][t]->GetXaxis()->SetTitle("ramp");
        hist_BPM_coil[j][i][t]->GetXaxis()->SetTitleSize(0.05);
        hist_BPM_coil[j][i][t]->GetYaxis()->SetTitleSize(0.05);
        hist_BPM_coil[j][i][t]->GetXaxis()->SetLabelSize(0.05);
        hist_BPM_coil[j][i][t]->GetYaxis()->SetLabelSize(0.05);
        hist_BPM_coil[j][i][t]->SetMarkerSize(1);  
        hist_BPM_coil[j][i][t]->SetMarkerStyle(20);  
        hist_BPM_coil[j][i][t]->SetTitle(Form("%s,cyc=%d",wire[i].Data(),supercyc[t]));  
        // double y_avg =  
        // TLine
        totalEntry[j][i][t]=hist_BPM_coil[j][i][t]->GetEntries(); 
        int a=0;
        double bpm_sum = 0;
        int entry_sum = 0;
        double bpm_avg = 0;
        double bpm_high, y_max;
        double bpm_low, y_min;
        for (int tbn = 0; tbn < nbins; tbn++) {
          sprintf(name_f,"ramp_bin%d",tbn);
          TH1D *hramp = hist_BPM_coil[j][i][t]->ProjectionY(name_f,tbn,tbn);
          rampentry[tbn] =hramp->GetEntries();
          // if(rampentry[tbn]>5){}
          bpm_mean[tbn]= hramp->GetMean();
          bpm_rms[tbn]= hramp->GetRMS();
          rampvalue[tbn] = hist_BPM_coil[j][i][t]->GetXaxis()->GetBinCenter(tbn);
          bpm_sum += rampentry[tbn]*bpm_mean[tbn];
          entry_sum += rampentry[tbn];
        }

        tgraph_BPM_coil[j][i][t]= new TGraphErrors(nbins,rampvalue,bpm_mean,bpm_error,bpm_rms);
        tgraph_BPM_coil[j][i][t]->GetYaxis()->SetTitle(Form("%s [mm]",BPMS[j].Data()));
        tgraph_BPM_coil[j][i][t]->GetXaxis()->SetTitle("ramp");
        tgraph_BPM_coil[j][i][t]->GetXaxis()->SetTitleSize(0.05);
        tgraph_BPM_coil[j][i][t]->GetYaxis()->SetTitleSize(0.04);
        tgraph_BPM_coil[j][i][t]->GetXaxis()->SetLabelSize(0.05);
        tgraph_BPM_coil[j][i][t]->GetYaxis()->SetLabelSize(0.05);
        tgraph_BPM_coil[j][i][t]->SetMarkerSize(1);  
        tgraph_BPM_coil[j][i][t]->SetTitle(Form("%s,cyc=%d",wire[i].Data(),supercyc[t]));  

        bpm_avg = bpm_sum / entry_sum;
        y_max = hist_BPM_coil[j][i][t]->GetYaxis()->GetXmax();
        y_min = hist_BPM_coil[j][i][t]->GetYaxis()->GetXmin();
        double y_step = (y_max - y_min) / 25;
        bpm_high = bpm_avg + 0.4;
        bpm_high = (bpm_high < y_max) ? bpm_high : (y_max-y_step);
        bpm_low = bpm_avg - 0.4;
        bpm_low = (bpm_low > y_min) ? bpm_low : (y_min+y_step);
        avg_line_BPM_coil[j][i][t] =  new TLine(0, bpm_avg, 1300, bpm_avg);
        avg_line_BPM_coil[j][i][t]->SetLineStyle(2);
        avg_line_BPM_coil[j][i][t]->SetLineColor(kRed);
        avg_text_BPM_coil[j][i][t] = new TText(1305, bpm_avg-0.28*y_step, Form("%.2f", bpm_avg));
        high_line_BPM_coil[j][i][t] =  new TLine(0, bpm_high, 1300, bpm_high);
        high_line_BPM_coil[j][i][t]->SetLineStyle(2);
        high_line_BPM_coil[j][i][t]->SetLineColor(kRed);
        high_text_BPM_coil[j][i][t] = new TText(1305, bpm_high-0.28*y_step, Form("%.2f", bpm_high));
        low_line_BPM_coil[j][i][t] =  new TLine(0, bpm_low, 1300, bpm_low);
        low_line_BPM_coil[j][i][t]->SetLineStyle(2);
        low_line_BPM_coil[j][i][t]->SetLineColor(kRed);
        low_text_BPM_coil[j][i][t] = new TText(1305, bpm_low-0.28*y_step, Form("%.2f", bpm_low));

        for (Int_t tbn = 0; tbn < nbins; tbn++) {
          if(bpm_mean[tbn]==0 ||rampentry[tbn]<5 ){
            tgraph_BPM_coil[j][i][t]->RemovePoint(tbn-a);
            a=a+1;
          }
        }
      }  
    }
  }

  char name_canvas[50];
  //const char prefix[] = "./";
  //const char prefix[] = "/chafs2/work1/apar/DitherResponse/";
  const char prefix[] = "./amplitude-check/";

  for(int t=0;t<n;t++){
    for(int i=0;i<nCoil;i++){
      sprintf(name_canvas,"%s_cyc%d",wire[i].Data(),t);
      c[t][i]= new TCanvas(name_canvas,name_canvas,1000,1000);
      c[t][i]->Divide(2,4);
    }
  } 
  for(int t=0;t<n;t++){
    for(int i=0;i<nCoil;i++){
      for(int j=0;j<nBPM;j++){
        c[t][i]->cd(j+1);
        if(totalEntry[j][i][t]>50){
          hist_BPM_coil[j][i][t]->Draw(" p");
          tgraph_BPM_coil[j][i][t]->SetMarkerSize(2);
          tgraph_BPM_coil[j][i][t]->SetMarkerStyle(20);
          tgraph_BPM_coil[j][i][t]->SetMarkerColor(2);
          tgraph_BPM_coil[j][i][t]->SetLineColor(2);
          tgraph_BPM_coil[j][i][t]->SetLineWidth(2);
          tgraph_BPM_coil[j][i][t]->Draw("p same");
          avg_line_BPM_coil[j][i][t]->Draw(" same");
          avg_text_BPM_coil[j][i][t]->Draw(" same");
          high_line_BPM_coil[j][i][t]->Draw(" same");
          high_text_BPM_coil[j][i][t]->Draw(" same");
          low_line_BPM_coil[j][i][t]->Draw(" same");
          low_text_BPM_coil[j][i][t]->Draw(" same");
          Is_fill[t]=1; 
          tmpgood+=1;
        }
        c[t][i]->SaveAs(Form("%s/dithering_amplitudes_coil%d_cyc%d.pdf)", prefix, i+1, supercyc[t]));
        //fun1->SetParameters(0.2,50,0.011,1.3);
        //hist_BPM_coil[j][i]->Fit("fun1");
        //for(int i=0;i<nCoil;i++){}
        // hist_BPM_coil[j][i]->Draw("same p*");

        // {} 
      }
      if (tmpgood == nBPM) {
        Is_good[t]+=1;
      }
      tmpgood=0;
      //c[t][i]->SaveAs(Form("dithering_amplitudes_coil%d_cyc%d.pdf)",i+1,supercyc[t]));
    }
    if(Is_fill[t]){
      if (Is_good[t] == nCoil) {
        gSystem->Exec(Form("pdfunite %s/*coil*_cyc%d.pdf %s/run%d_full_cyc%d.pdf", prefix, supercyc[t], prefix, runNo, supercyc[t]));
      }
      else {
        gSystem->Exec(Form("pdfunite %s/*coil*_cyc%d.pdf %s/run%d_incomplete_cyc%d.pdf", prefix, supercyc[t], prefix, runNo, supercyc[t]));
      }
    }
    else {
      gSystem->Exec(Form("pdfunite %s/*coil*_cyc%d.pdf %s/run%d_nodata_cyc%d.pdf", prefix, supercyc[t], prefix, runNo, supercyc[t]));
    }
    gSystem->Exec(Form("rm -rf %s/*coil*_cyc%d.pdf", prefix,runNo,supercyc[t]));
  }
}
