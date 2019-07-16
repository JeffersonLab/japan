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
#include <TEntryList.h>
#include <TTreeReader.h>
using namespace std; 
class TF1 *f1trap[42];
int japan_plot_beammod_quartz_cyc(int runNo=0) { 
  gStyle->SetOptStat(0); 
/*int filenum=1;
 if (inputfile_name.find("_",0) != string::npos) {
      filenum=atof(inputfile_name.substr(inputfile_name.find("_")+1,inputfile_name.find("_")).c_str());
      cout << "run num " << filenum <<"  "<< "make sure this is run for the beam modulation" <<  endl;
 }else{
    cout << "this file has no run number, please check the file name" << endl;
  }*/
  ostringstream sstr0;
  sstr0<<"Quartz_sensitivity_run"<<runNo<<".txt";
  ofstream outfile0(sstr0.str().c_str());
  sstr0.str("");
  char infile[300];
 // sprintf(infile,"/chafs2/work1/apar/japanOutput/prexALL_%d.000.root",runNo);
  sprintf(infile,"/chafs2/work1/apar/japanOutput/prexPrompt_pass2_%d.000.root",runNo);
  TFile *file1=new TFile(infile);
  
  TGraphErrors *test;
  TTree *tree_R = (TTree*) file1->Get("evt");
  Double_t        CodaEventNumber;
  Double_t        bpm4eX_hw_sum;
  Double_t        bpm4eY_hw_sum;
  Double_t        bpm4aX_hw_sum;
  Double_t        bpm4aY_hw_sum;
  Double_t        bpm12X_hw_sum;
  Double_t        bpm12Y_hw_sum;
  Double_t        bcm_an_us_hw_sum;
  Double_t        bcm_an_ds3_hw_sum;
  Double_t        bcm_an_ds_hw_sum;
  Double_t        ErrorFlag;
  Double_t        sam1_hw_sum;
  Double_t        sam2_hw_sum;
  Double_t        sam3_hw_sum;
  Double_t        sam4_hw_sum;
  Double_t        sam5_hw_sum;
  Double_t        sam6_hw_sum;
  Double_t        sam7_hw_sum;
  Double_t        sam8_hw_sum;
  Double_t        bmwphase;
  Double_t        bmwperiod;
  Double_t        bmwobj;
  Double_t        bmwfreq;
  Double_t        bmwcycnum;
  Double_t        bmod_trim1_value;
  Double_t        bmod_trim2_value;
  Double_t        bmod_trim3_value;
  Double_t        bmod_trim4_value;
  Double_t        bmod_trim5_value;
  Double_t        bmod_trim6_value;
  Double_t        bmod_trim7_value;
   Double_t        bmod_ramp_hw_sum;
   Double_t        bmod_ramp_block0;
   Double_t        bmod_ramp_block1;
   Double_t        bmod_ramp_block2;
   Double_t        bmod_ramp_block3;
   Double_t        bmod_ramp_num_samples;
   Double_t        bmod_ramp_Device_Error_Code;
   Double_t        bmod_ramp_hw_sum_raw;
   Double_t        bmod_ramp_block0_raw;
   Double_t        bmod_ramp_block1_raw;
   Double_t        bmod_ramp_block2_raw;
   Double_t        bmod_ramp_block3_raw;
   Double_t        bmod_ramp_sequence_number;

   Double_t        usr_hw_sum;
   Double_t        usr_block0;
   Double_t        usr_block1;
   Double_t        usr_block2;
   Double_t        usr_block3;
   Double_t        usr_num_samples;
   Double_t        usr_Device_Error_Code;
   Double_t        usr_hw_sum_raw;
   Double_t        usr_block0_raw;
   Double_t        usr_block1_raw;
   Double_t        usr_block2_raw;
   Double_t        usr_block3_raw;
   Double_t        usr_sequence_number;
   Double_t        dsr_hw_sum;
   Double_t        dsr_block0;
   Double_t        dsr_block1;
   Double_t        dsr_block2;
   Double_t        dsr_block3;
   Double_t        dsr_num_samples;
   Double_t        dsr_Device_Error_Code;
   Double_t        dsr_hw_sum_raw;
   Double_t        dsr_block0_raw;
   Double_t        dsr_block1_raw;
   Double_t        dsr_block2_raw;
   Double_t        dsr_block3_raw;
   Double_t        dsr_sequence_number;
   Double_t        usl_hw_sum;
   Double_t        usl_block0;
   Double_t        usl_block1;
   Double_t        usl_block2;
   Double_t        usl_block3;
   Double_t        usl_num_samples;
   Double_t        usl_Device_Error_Code;
   Double_t        usl_hw_sum_raw;
   Double_t        usl_block0_raw;
   Double_t        usl_block1_raw;
   Double_t        usl_block2_raw;
   Double_t        usl_block3_raw;
   Double_t        usl_sequence_number;
   Double_t        dsl_hw_sum;
   Double_t        dsl_block0;
   Double_t        dsl_block1;
   Double_t        dsl_block2;
   Double_t        dsl_block3;
   Double_t        dsl_num_samples;
   Double_t        dsl_Device_Error_Code;
   Double_t        dsl_hw_sum_raw;
   Double_t        dsl_block0_raw;
   Double_t        dsl_block1_raw;
   Double_t        dsl_block2_raw;
   Double_t        dsl_block3_raw;
   Double_t        dsl_sequence_number;
  
   TBranch        *b_usr;   //!
   TBranch        *b_dsr;   //!
   TBranch        *b_usl;   //!
   TBranch        *b_dsl;   //!
    TBranch        *b_bmod_ramp;   //!



  TBranch        *b_CodaEventNumber;
  TBranch        *b_bpm4aX;   //!
  TBranch        *b_bpm4aY;   //!
  TBranch        *b_bpm4eX;   //!
  TBranch        *b_bpm4eY;   //!
  TBranch        *b_bpm12X;   //!
  TBranch        *b_bpm12Y;   //!
  TBranch        *b_bcm_an_us;   //!
  TBranch        *b_bcm_an_ds;   //!
  TBranch        *b_bcm_an_ds3;   //!
  TBranch        *b_sam1;   //!
  TBranch        *b_sam2;   //!
  TBranch        *b_sam3;   //!
  TBranch        *b_sam4;   //!
  TBranch        *b_sam5;   //!
  TBranch        *b_sam6;   //!
  TBranch        *b_sam7;   //!
  TBranch        *b_sam8;   //
  TBranch        *b_bmod_trim1;   //!
  TBranch        *b_bmod_trim2;   //!
  TBranch        *b_bmod_trim3;   //!
  TBranch        *b_bmod_trim4;   //!
  TBranch        *b_bmod_trim5;   //!
  TBranch        *b_bmod_trim6;   //!
  TBranch        *b_bmod_trim7;   //!
  
  TBranch        *b_bmwphase;   //!
  TBranch        *b_bmwperiod;   //!
  TBranch        *b_bmwobj;   //!
  TBranch        *b_bmwfreq;   //!
  TBranch        *b_bmwcycnum;   //!
  
  TBranch        *b_ErrorFlag;
  tree_R->SetBranchAddress("CodaEventNumber", &CodaEventNumber, &b_CodaEventNumber);
  tree_R->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);
  tree_R->SetBranchAddress("bpm4aX", &bpm4aX_hw_sum, &b_bpm4aX);
  tree_R->SetBranchAddress("bpm4aY", &bpm4aY_hw_sum, &b_bpm4aY);
  tree_R->SetBranchAddress("bpm4eX", &bpm4eX_hw_sum, &b_bpm4eX);
  tree_R->SetBranchAddress("bpm4eY", &bpm4eY_hw_sum, &b_bpm4eY);
  tree_R->SetBranchAddress("bpm12X", &bpm12X_hw_sum, &b_bpm12X);
  tree_R->SetBranchAddress("bpm12Y", &bpm12Y_hw_sum, &b_bpm12X);
  tree_R->SetBranchAddress("bcm_an_us", &bcm_an_us_hw_sum, &b_bcm_an_us);
  tree_R->SetBranchAddress("bcm_an_ds", &bcm_an_ds_hw_sum, &b_bcm_an_ds);
  tree_R->SetBranchAddress("bcm_an_ds3", &bcm_an_ds3_hw_sum, &b_bcm_an_ds3);
  tree_R->SetBranchAddress("sam1", &sam1_hw_sum, &b_sam1);
  tree_R->SetBranchAddress("sam2", &sam2_hw_sum, &b_sam2);
  tree_R->SetBranchAddress("sam3", &sam3_hw_sum, &b_sam3);
  tree_R->SetBranchAddress("sam4", &sam4_hw_sum, &b_sam4);
  tree_R->SetBranchAddress("sam5", &sam5_hw_sum, &b_sam5);
  tree_R->SetBranchAddress("sam6", &sam6_hw_sum, &b_sam6);
  tree_R->SetBranchAddress("sam7", &sam7_hw_sum, &b_sam7);
  tree_R->SetBranchAddress("sam8", &sam8_hw_sum, &b_sam8);
  tree_R->SetBranchAddress("usl", &usl_hw_sum, &b_usl);
  tree_R->SetBranchAddress("dsl", &dsl_hw_sum, &b_dsl);
  
  tree_R->SetBranchAddress("usr", &usr_hw_sum, &b_usr);
  tree_R->SetBranchAddress("dsr", &dsr_hw_sum, &b_dsr);
  
  tree_R->SetBranchAddress("bmwphase", &bmwphase, &b_bmwphase);
  tree_R->SetBranchAddress("bmwperiod", &bmwperiod, &b_bmwperiod);
  tree_R->SetBranchAddress("bmwobj", &bmwobj, &b_bmwobj);
  tree_R->SetBranchAddress("bmwfreq", &bmwfreq, &b_bmwfreq);
  tree_R->SetBranchAddress("bmwcycnum", &bmwcycnum, &b_bmwcycnum);
  tree_R->SetBranchAddress("bmod_trim1", &bmod_trim1_value, &b_bmod_trim1);
  tree_R->SetBranchAddress("bmod_trim2", &bmod_trim2_value, &b_bmod_trim2);
  tree_R->SetBranchAddress("bmod_trim3", &bmod_trim3_value, &b_bmod_trim3);
  tree_R->SetBranchAddress("bmod_trim4", &bmod_trim4_value, &b_bmod_trim4);
  tree_R->SetBranchAddress("bmod_trim5", &bmod_trim5_value, &b_bmod_trim5);
  tree_R->SetBranchAddress("bmod_trim6", &bmod_trim6_value, &b_bmod_trim6);
  tree_R->SetBranchAddress("bmod_trim7", &bmod_trim7_value, &b_bmod_trim7);
  tree_R->SetBranchAddress("bmod_ramp", &bmod_ramp_hw_sum, &b_bmod_ramp);


  tree_R->Draw(">>elist","bmwcycnum>0","entrylist");  //picks out unique cycle numbers
  TEntryList *elist = (TEntryList*)gDirectory->Get("elist");
  tree_R->SetEntryList(elist);
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  int nonzero = tree_R->Draw("bmwcycnum","bmwcycnum>0","goff");
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

  int n=cycles.size();//number of cycle
  
                  Double_t supercyc[n];
                     for(int i=0;i<n;i++){
                           supercyc[i]=cycles[i]; 
                               cout<<"i="<<i<<"  "<<"supercyc="<<supercyc[i]<<endl;
                                  }    
                       Double_t supercycslope[n];
                         Double_t supercyc_err[n];
                           for(int i=0;i<n;i++ ){
                                supercycslope[i]=/*filenum*100+*/supercyc[i];
                                   supercyc_err[i]=0;
                                      cout<<"supercycslope="<<supercycslope[i]<<endl;
                                        }

  Double_t trim_base[7] = {1683,1582,1708,1670,1662,1709,1686};
  const double trimmin=0.38;
  const double trimmax=0.7;
  const double bpmmax=3;
  const double bpmmin=1;
  const double chtov=1.0e-3;//7.62939453125000000e-05; //10V*(1/pow(2,17))
  const double factor=1.0e+6;
  Double_t b4ax_rslop[7][n];
  Double_t b4ax_rslop_err[7][n];
  Double_t b4ex_rslop[7][n];
  Double_t b4ex_rslop_err[7][n];
  Double_t b4ay_rslop[7][n];
  Double_t b4ay_rslop_err[7][n];
  Double_t b4ey_rslop[7][n];
  Double_t b4ey_rslop_err[7][n];
  Double_t b12x_rslop[7][n];
  Double_t b12x_rslop_err[7][n];

  TGraph * hist_4ax_coil[7][n];
  TGraph * hist_4ex_coil[7][n];
  TGraph * hist_4ay_coil[7][n];
  TGraph * hist_4ey_coil[7][n];
  TGraph * hist_12x_coil[7][n];


  TGraph * hist_4ax_coil_nor[7][n];
  TGraph * hist_4ex_coil_nor[7][n];
  TGraph * hist_4ay_coil_nor[7][n];
  TGraph * hist_4ey_coil_nor[7][n];
  TGraph * hist_14x_coil_nor[7][n];
  TGraph * hist_18x_coil_nor[7][n];


  TGraph * gcoil5_ax[n];
  TGraph * gcoil5_ex[n];
  TGraph * gcoil3_ax[n];
  TGraph * gcoil3_ex[n];
  TGraph * gcoil6_ax[n];
  TGraph * gcoil6_ex[n];
  TGraph * gcoil1_ax[n];
  TGraph * gcoil1_ex[n];
  TGraph * gcoil2_ay[n];
  TGraph * gcoil2_ey[n];
  TGraph * gcoil4_ay[n];
  TGraph * gcoil4_ey[n];
  TGraph * gcoil7_ay[n];
  TGraph * gcoil7_ey[n];
  TGraph * gcoil8_14x[n];
  TGraph * gcoil8_18x[n];
  TCanvas *c[4];
  TF1 *fun = new TF1("fun","[0]+[1]*x",0,1);
  TF1 *funslopax[7][n];

  TF1 *funslopex[7][n];
  TF1 *funslopay[7][n];
  TF1 *funslopey[7][n];

  TF1 *funslop12x[7][n];
  char name_ax[50];
  char name_ex[50];

  
  char name_ey[50];
  char name_ay[50];
  
  char name_r12x[50];

  
  for(int j=0;j<7;j++){
   for(int i=0;i<n;i++){

    sprintf(name_ax,"funslopax_coil%d_cyc%d",j,i);
    sprintf(name_ex,"funslopex_coil%d_cyc%d",j,i);
    sprintf(name_ay,"funslopay_coil%d_cyc%d",j,i);
    sprintf(name_ey,"funslopey_coil%d_cyc%d",j,i);
    sprintf(name_r12x,"funslop12x_coil%d_cyc%d",j,i);
    
    funslopax[j][i]= new TF1(name_ax,"[0]+[1]*x",0,10000);
    funslopex[j][i]= new TF1(name_ex,"[0]+[1]*x",0,10000);
    funslopay[j][i]= new TF1(name_ay,"[0]+[1]*x",0,10000);
    funslopey[j][i]= new TF1(name_ey,"[0]+[1]*x",0,10000);
    funslop12x[j][i]= new TF1(name_r12x,"[0]+[1]*x",0,10000);
   }
  }

  double mean_coil4ax[7][n];
  double mean_coil4ex[7][n];
  double mean_coil4ay[7][n];
  double mean_coil4ey[7][n];
  int entry_bpm4ax[7][n];
  int entry_bpm4ex[7][n];
  int entry_bpm4ay[7][n];
  int entry_bpm4ey[7][n];
  int entry_bpm12x[7][n];

  int xnbins=0.0;
  int ynbins=0;
  double ibincon=0;
  int ibin=0;
  double xbincon=0.0;
  int coilnum=0;
  // tree_R->Draw("bpm4ax.block0:Entry$","ErrorFlag==0 && bmwobj==5 && bmwcycnum==16");
  cout<<"Drawing the coil response plot"<<endl;
  TString wire[7]={"bmod_trim1","bmod_trim2","bmod_trim3","bmod_trim4","bmod_trim5","bmod_trim6","bmod_trim7"};


  for(int j=0;j<7;j++){
    for(int i=0;i<n;i++){
    mean_coil4ax[j][i]=0;
    mean_coil4ex[j][i]=0;
    mean_coil4ay[j][i]=0;
    mean_coil4ey[j][i]=0;
    int num_4ax_nor = tree_R->Draw(Form("(usl/bcm_an_ds3):(%s*%lf)",wire[j].Data(),chtov),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d && abs(%s-%f)>20 && bmwcycnum==%f",j+1,wire[j].Data(),trim_base[j],supercyc[i]));
    hist_4ax_coil_nor[j][i] = new TGraph(num_4ax_nor, tree_R->GetV2(),tree_R->GetV1());
    hist_4ax_coil_nor[j][i]->GetXaxis()->SetTitle(Form("%s",wire[j].Data()));
    hist_4ax_coil_nor[j][i]->GetYaxis()->SetTitle("usl");
    hist_4ax_coil_nor[j][i]->GetXaxis()->SetTitleSize(0.05);
    hist_4ax_coil_nor[j][i]->GetYaxis()->SetTitleSize(0.05);
    mean_coil4ax[j][i]= hist_4ax_coil_nor[j][i]->GetMean(2);
  
    int num_4ex_nor = tree_R->Draw(Form("(dsl/bcm_an_ds3):(%s*%lf)",wire[j].Data(),chtov),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d && abs(%s-%f)>20&& bmwcycnum==%f",j+1,wire[j].Data(),trim_base[j],supercyc[i]));
    hist_4ex_coil_nor[j][i] = new TGraph(num_4ex_nor, tree_R->GetV2(),tree_R->GetV1());
    hist_4ex_coil_nor[j][i]->GetXaxis()->SetTitle(Form("%s",wire[j].Data()));
    hist_4ex_coil_nor[j][i]->GetYaxis()->SetTitle("dsl");
    hist_4ex_coil_nor[j][i]->GetXaxis()->SetTitleSize(0.05);
    hist_4ex_coil_nor[j][i]->GetYaxis()->SetTitleSize(0.05);
    mean_coil4ex[j][i]= hist_4ex_coil_nor[j][i]->GetMean(2);
    int num_4ay_nor =tree_R->Draw(Form("(usr/bcm_an_ds3):(%s*%lf)",wire[j].Data(),chtov),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d && abs(%s-%f)>20&& bmwcycnum==%f",j+1,wire[j].Data(),trim_base[j],supercyc[i]));
    hist_4ay_coil_nor[j][i] = new TGraph(num_4ay_nor, tree_R->GetV2(),tree_R->GetV1());
    hist_4ay_coil_nor[j][i]->GetXaxis()->SetTitle(Form("%s",wire[j].Data()));
    hist_4ay_coil_nor[j][i]->GetYaxis()->SetTitle("usr");
    hist_4ay_coil_nor[j][i]->GetXaxis()->SetTitleSize(0.05);
    hist_4ay_coil_nor[j][i]->GetYaxis()->SetTitleSize(0.05);
    mean_coil4ay[j][i]= hist_4ay_coil_nor[j][i]->GetMean(2);
    
    int num_4ey_nor =tree_R->Draw(Form("(dsr/bcm_an_ds3):(%s*%lf)",wire[j].Data(),chtov),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d && abs(%s-%f)>20&& bmwcycnum==%f",j+1,wire[j].Data(),trim_base[j],supercyc[i]));
    hist_4ey_coil_nor[j][i] = new TGraph(num_4ey_nor, tree_R->GetV2(),tree_R->GetV1());
    hist_4ey_coil_nor[j][i]->GetXaxis()->SetTitle(Form("%s",wire[j].Data()));
    hist_4ey_coil_nor[j][i]->GetYaxis()->SetTitle("dsr");
    hist_4ey_coil_nor[j][i]->GetXaxis()->SetTitleSize(0.05);
    hist_4ey_coil_nor[j][i]->GetYaxis()->SetTitleSize(0.05);
    mean_coil4ey[j][i]= hist_4ey_coil_nor[j][i]->GetMean(2);

    int num_4ax = tree_R->Draw(Form("(%lf/%lf)*(usl/bcm_an_ds3):(%s*%lf)",factor,mean_coil4ax[j][i], wire[j].Data(),chtov),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d && abs(%s-%f)>20 && bmwcycnum==%f",j+1,wire[j].Data(),trim_base[j],supercyc[i]));
    hist_4ax_coil[j][i] = new TGraph(num_4ax, tree_R->GetV2(),tree_R->GetV1());
    hist_4ax_coil[j][i]->GetXaxis()->SetTitle(Form("%s",wire[j].Data()));
    hist_4ax_coil[j][i]->GetYaxis()->SetTitle("usl");
    hist_4ax_coil[j][i]->GetXaxis()->SetTitleSize(0.05);
    hist_4ax_coil[j][i]->GetYaxis()->SetTitleSize(0.05);

  
    int num_4ex = tree_R->Draw(Form("(%lf/%lf)*(dsl/bcm_an_ds3):(%s*%lf)",factor,mean_coil4ex[j][i],wire[j].Data(),chtov),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d && abs(%s-%f)>20&& bmwcycnum==%f",j+1,wire[j].Data(),trim_base[j],supercyc[i]));
    hist_4ex_coil[j][i] = new TGraph(num_4ex, tree_R->GetV2(),tree_R->GetV1());
    hist_4ex_coil[j][i]->GetXaxis()->SetTitle(Form("%s",wire[j].Data()));
    hist_4ex_coil[j][i]->GetYaxis()->SetTitle("dsl");
    hist_4ex_coil[j][i]->GetXaxis()->SetTitleSize(0.05);
    hist_4ex_coil[j][i]->GetYaxis()->SetTitleSize(0.05);

    int num_4ay =tree_R->Draw(Form("(%lf/%lf)*(usr/bcm_an_ds3):(%s*%lf)",factor,mean_coil4ay[j][i],wire[j].Data(),chtov),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d && abs(%s-%f)>20&& bmwcycnum==%f",j+1,wire[j].Data(),trim_base[j],supercyc[i]));
    hist_4ay_coil[j][i] = new TGraph(num_4ay, tree_R->GetV2(),tree_R->GetV1());
    hist_4ay_coil[j][i]->GetXaxis()->SetTitle(Form("%s",wire[j].Data()));
    hist_4ay_coil[j][i]->GetYaxis()->SetTitle("usr");
    hist_4ay_coil[j][i]->GetXaxis()->SetTitleSize(0.05);
    hist_4ay_coil[j][i]->GetYaxis()->SetTitleSize(0.05);

    
    int num_4ey =tree_R->Draw(Form("(%lf/%lf)*(dsr/bcm_an_ds3):(%s*%lf)",factor,mean_coil4ey[j][i],wire[j].Data(),chtov),Form("(ErrorFlag & 0x7bfe6fff)==0 && bmod_ramp>0 && bmwobj==%d && abs(%s-%f)>20&& bmwcycnum==%f",j+1,wire[j].Data(),trim_base[j],supercyc[i]));
    hist_4ey_coil[j][i] = new TGraph(num_4ey, tree_R->GetV2(),tree_R->GetV1());
    hist_4ey_coil[j][i]->GetXaxis()->SetTitle(Form("%s",wire[j].Data()));
    hist_4ey_coil[j][i]->GetYaxis()->SetTitle("dsr");
    hist_4ey_coil[j][i]->GetXaxis()->SetTitleSize(0.05);
    hist_4ey_coil[j][i]->GetYaxis()->SetTitleSize(0.05);


    
   }
  }



//Fit the plots to get the slope
   outfile0<<setw(20)<<setiosflags(ios::left)<<"cyc_number"<<setw(20)<<setiosflags(ios::left)<<"monitors"<<setw(20)<<setiosflags(ios::left)<<"coil1(ppm/count)"<<setw(20)<<setiosflags(ios::left)<<"coil1_error"<<setw(20)<<setiosflags(ios::left)<<"entries"<<setw(20)<<setiosflags(ios::left)<<"coil2(ppm/count)"<<setw(20)<<setiosflags(ios::left)<<"coil2_error"<<setw(20)<<setiosflags(ios::left)<<"entries"<<setw(20)<<setiosflags(ios::left)<<"coil3(ppm/count)"<<setw(20)<<setiosflags(ios::left)<<"coil3_error"<<setw(20)<<setiosflags(ios::left)<<"entries"<<setw(20)<<setiosflags(ios::left)<<"coil4(ppm/count)"<<setw(20)<<setiosflags(ios::left)<<"coil4_error"<<setw(20)<<setiosflags(ios::left)<<"entries"<<setw(20)<<setiosflags(ios::left)<<"coil5(ppm/count)"<<setw(20)<<setiosflags(ios::left)<<"coil5_error"<<setw(20)<<setiosflags(ios::left)<<"entries"<<setw(20)<<setiosflags(ios::left)<<"coil6(ppm/count)"<<setw(20)<<setiosflags(ios::left)<<"coil6_error"<<setw(20)<<setiosflags(ios::left)<<"entries"<<setw(20)<<setiosflags(ios::left)<<"coil7(ppm/count)"<<setw(20)<<setiosflags(ios::left)<<"coil7_error"<<setw(20)<<setiosflags(ios::left)<<"entries"<<endl;
  char name_canvas[50];
  for(int i=0;i<4;i++){
  sprintf(name_canvas,"c%d",i);
  c[i]= new TCanvas(name_canvas,name_canvas,1000,1000);
  c[i]->Divide(7,n);
  }
 for(int i=0;i<n;i++){
  for(int j=0;j<7;j++){
    c[0]->cd(i*7+j+1);
    hist_4ax_coil[j][i]->Draw("A p*");
    hist_4ax_coil[j][i]->Fit(funslopax[j][i],"R+");
    b4ax_rslop[j][i]=funslopax[j][i]->GetParameter(1);
    b4ax_rslop_err[j][i]=funslopax[j][i]->GetParError(1);
    entry_bpm4ax[j][i]=hist_4ax_coil[j][i]->GetN();
 
    c[1]->cd(i*7+j+1);
    hist_4ex_coil[j][i]->Draw("A p*");
    hist_4ex_coil[j][i]->Fit(funslopex[j][i],"R+");
    b4ex_rslop[j][i]=funslopex[j][i]->GetParameter(1);
    b4ex_rslop_err[j][i]=funslopex[j][i]->GetParError(1);
    entry_bpm4ex[j][i]=hist_4ex_coil[j][i]->GetN();
    
    c[2]->cd(i*7+j+1);
    hist_4ay_coil[j][i]->Draw("A p*");
    hist_4ay_coil[j][i]->Fit(funslopay[j][i],"R+");
    b4ay_rslop[j][i]=funslopay[j][i]->GetParameter(1);
    b4ay_rslop_err[j][i]=funslopay[j][i]->GetParError(1);
    entry_bpm4ay[j][i]=hist_4ay_coil[j][i]->GetN();
    c[3]->cd(i*7+j+1);
    hist_4ey_coil[j][i]->Draw("A p*");
    hist_4ey_coil[j][i]->Fit(funslopey[j][i],"R+");
    b4ey_rslop[j][i]=funslopey[j][i]->GetParameter(1);
    b4ey_rslop_err[j][i]=funslopey[j][i]->GetParError(1);
    entry_bpm4ey[j][i]=hist_4ey_coil[j][i]->GetN();

     } 
    outfile0<<setw(20)<<setiosflags(ios::left)<<supercyc[i]<<setw(20)<<setiosflags(ios::left)<<"usl"<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop[0][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop_err[0][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ax[0][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop[1][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop_err[1][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ax[1][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop[2][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop_err[2][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ax[2][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop[3][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop_err[3][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ax[3][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop[4][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop_err[4][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ax[4][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop[5][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop_err[5][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ax[5][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop[6][i]<<setw(20)<<setiosflags(ios::left)<<b4ax_rslop_err[6][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ax[6][i]<<endl;  

    outfile0<<setw(20)<<setiosflags(ios::left)<<supercyc[i]<<setw(20)<<setiosflags(ios::left)<<"dsl"<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop[0][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop_err[0][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ex[0][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop[1][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop_err[1][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ex[1][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop[2][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop_err[2][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ex[2][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop[3][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop_err[3][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ex[3][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop[4][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop_err[4][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ex[4][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop[5][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop_err[5][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ex[5][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop[6][i]<<setw(20)<<setiosflags(ios::left)<<b4ex_rslop_err[6][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ex[6][i]<<endl;
    
  outfile0<<setw(20)<<setiosflags(ios::left)<<supercyc[i]<<setw(20)<<setiosflags(ios::left)<<"usr"<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop[0][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop_err[0][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ay[0][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop[1][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop_err[1][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ay[1][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop[2][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop_err[2][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ay[2][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop[3][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop_err[3][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ay[3][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop[4][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop_err[4][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ay[4][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop[5][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop_err[5][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ay[5][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop[6][i]<<setw(20)<<setiosflags(ios::left)<<b4ay_rslop_err[6][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ay[6][i]<<endl;


   outfile0<<setw(20)<<setiosflags(ios::left)<<supercyc[i]<<setw(20)<<setiosflags(ios::left)<<"dsr"<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop[0][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop_err[0][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ey[0][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop[1][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop_err[1][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ey[1][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop[2][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop_err[2][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ey[2][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop[3][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop_err[3][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ey[3][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop[4][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop_err[4][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ey[4][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop[5][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop_err[5][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ey[5][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop[6][i]<<setw(20)<<setiosflags(ios::left)<<b4ey_rslop_err[6][i]<<setw(20)<<setiosflags(ios::left)<<entry_bpm4ey[6][i]<<endl;



   }

  }
  
