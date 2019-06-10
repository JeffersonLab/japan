/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Finds correlation and alphas for  QW event 
 *********************************************************************/

#include <cstdio>
#include <iostream>
using namespace std;

#include <TH2.h> 
#include <TProfile.h> 
#include <TList.h>
#include <TLine.h>
#include <TFile.h> 

#include <math.h>
#include <TMath.h>

#include "QwkRegBlueCorrelator.h"

//========================
//========================
QwkRegBlueCorrelator::QwkRegBlueCorrelator(const char *core) {
  mCore=core;
  h1iv=0;
  // printf("constr of QwkRegBlueCorrelator=%s\n",mCore.Data());
}

//========================
//========================
QwkRegBlueCorrelator::~QwkRegBlueCorrelator(){
  printf("destructor QwkRegBlueCorrelator=%s\n",mCore.Data());
  if(h1iv) { // only if previously allocated
    delete  [] h1iv;
    delete  [] h2iv;
    delete  [] h1dv;
    delete  [] h2dv;
  }
  printf("destructor QwkRegBlueCorrelator done\n");
}

//========================
//========================
void QwkRegBlueCorrelator::print() {

}


//========================
//========================
void
QwkRegBlueCorrelator::addEvent(double *Pvec, double *Yvec){
  linReg.accumulate(Pvec, Yvec);
  // .... monitoring 
  
  if (fDisableHistos == false) {
    for(int i=0;i<nP;i++) {    
      h1iv[i]->Fill(Pvec[i]);
      for(int j=i+1;j<nP;j++) h2iv[i*nP+j]->Fill(Pvec[i],Pvec[j]);
    }
    for(int j=0;j<nY;j++) {
      h1dv[j]->Fill(Yvec[j]);
      for(int i=0;i<nP;i++)  h2dv[i*nY+j]->Fill(Pvec[i],Yvec[j]);
    }
  }
  
}


//========================
//========================
void QwkRegBlueCorrelator::init(std::vector < TString > ivName, std::vector < TString > dvName) {
  nP=ivName.size();  
  nY=dvName.size();  

  initHistos(ivName,dvName); 
  linReg.setDims(nP, nY); 
  linReg.init();
}


//________________________________________________
//________________________________________________
void
QwkRegBlueCorrelator::initHistos(std::vector < TString > Pname, std::vector < TString > Yname){
  printf("QwkRegBlueCorrelator::initHistos()\n");
  TH1*h;

  //..... 1D,  iv
  h1iv=new TH1 *[nP];
  for(int i=0;i<nP;i++) {    
    h1iv[i]=h=new TH1D(Form(mCore+"P%d",i),Form("iv P%d=%s, pass=%s ;iv=%s (ppm)",i,Pname[i].Data(),mCore.Data(),Pname[i].Data()),128,0.,0.);
    h->GetXaxis()->SetNdivisions(4);
    //h->SetBit(TH1::kCanRebin);
  }
  
  double x1=0;
  //..... 2D,  iv correlations
  h2iv=new TH1 *[nP*nP]; // not all are used
  for(int i=0;i<nP;i++) {    
    for(int j=i+1;j<nP;j++) {    
      h2iv[i*nP+j]=h=new TH2D(Form(mCore+"P%d_P%d",i,j),Form("iv correlation  P%d_P%d, pass=%s ;P%d=%s (ppm);P%d=%s   (ppm)  ",i,j,mCore.Data(),i,Pname[i].Data(),j,Pname[j].Data()),64,-x1,x1,64,-x1,x1);
      h->GetXaxis()->SetTitleColor(kBlue);
      h->GetYaxis()->SetTitleColor(kBlue);
      h->GetXaxis()->SetNdivisions(4);
      h->GetYaxis()->SetNdivisions(4);
      //h->SetBit(TH1::kCanRebin); 
    }
  }
  
  //..... 1D,  dv
  h1dv=new TH1 *[nY];
  for(int i=0;i<nY;i++) {    
    h1dv[i]=h=new TH1D(Form(mCore+"Y%d",i),Form("dv Y%d=%s, pass=%s ;dv=%s (ppm)",i,Yname[i].Data(),mCore.Data(),Yname[i].Data()),128,0.,0.);
    h->GetXaxis()->SetNdivisions(4);
    //h->SetBit(TH1::kCanRebin);
  }
  
  
  double y1=0;
  //..... 2D,  dv-iv correlations
  h2dv=new TH1 *[nP*nY]; // not all are used
  for(int i=0;i<nP;i++) {    
    for(int j=0;j<nY;j++) {    
      h2dv[i*nY+j]=h=new TH2D(Form(mCore+"P%d_Y%d",i,j),Form("iv-dv correlation  P%d_Y%d, pass=%s ;P%d=%s (ppm);Y%d=%s   (ppm)  ",i,j,mCore.Data(),i,Pname[i].Data(),j,Yname[j].Data()),64,-x1,x1,64,-y1,y1);
      h->GetXaxis()->SetTitleColor(kBlue);
      h->GetYaxis()->SetTitleColor(kBlue);
      h->GetXaxis()->SetNdivisions(4);
      h->GetYaxis()->SetNdivisions(4);
      //h->SetBit(TH1::kCanRebin);
    }
  }
 
  // store list of names to be archived
  hA[0]=new TH1D(mCore+"NamesIV",Form("IV name list nIV=%d",nP),nP,0,1);
  for(int i=0;i<nP;i++)   hA[0]->Fill(Pname[i].Data(),1.*i); 
  hA[1]=new TH1D(mCore+"NamesDV",Form("DV name list nIV=%d",nY),nY,0,1);
  for(int i=0;i<nY;i++)   hA[1]->Fill(Yname[i].Data(),i*1.); 
}


//________________________________________________
//________________________________________________
void
QwkRegBlueCorrelator::finish(){

  printf("::::::::::::::::QwkRegBlueCorrelator::finish(%s) :::::::::::START\n",mCore.Data());
  if(linReg.failed()) {
    printf(" abnormal finish of linReg\n");
    return;
  }

  linReg.printSummaryP();
  linReg.printSummaryY();
  linReg.solve();
  linReg.printSummaryAlphas();

  printf("::::::::::::::::QwkRegBlueCorrelator::finish(%s) :::::::::::END\n",mCore.Data());
}


//________________________________________________
//________________________________________________
void
QwkRegBlueCorrelator::exportAlphas(TString outName, std::vector < TString > ivName, std::vector < TString > dvName){
  printf("::::::::::::::::QwkRegBlueCorrelator::exportAlphas(%s) :::::::::::\n",outName.Data());  
  TFile*  hFile=new TFile(outName,"RECREATE","correlation coefficents");

  linReg.mA.Write("slopes");
  linReg.mAsig.Write("sigSlopes");
  linReg.mRjk.Write("IV_IV_correlation");
  linReg.mRky.Write("IV_DV_correlation");
  linReg.mRyy.Write("DV_DV_correlation");
  linReg.mMP.Write("IV_mean");
  linReg.mMY.Write("DV_mean");
 
  //add processed  matrices
  double usedEve=linReg.getUsedEve();
  TMatrixD Mstat(1,1);
  Mstat(0,0)=usedEve;
  Mstat.Write("MyStat");

  //... IVs
  TMatrixD MsigIV(nP,1);
  TH1D hiv("IVname","names of IVs",nP,-0.5,nP-0.5); 
  double val;
  for(int i=0;i<nP;i++){
    Int_t testval = 0;
    testval = linReg.getSigmaP(i,val);
    assert(testval==0);
    MsigIV(i,0)=val;
    hiv.Fill(ivName[i],i);
  }
  MsigIV.Write("IV_sigma"); // of distribution
  hiv.Write();

  //... DVs
  TMatrixD MsigDV(nY,1);
  TH1D hdv("DVname","names of IVs",nY,-0.5,nY-0.5); 
  for(int i=0;i<nY;i++){
    Int_t testval = 0;
    testval = linReg.getSigmaY(i,val);
    assert(testval==0);
    MsigDV(i,0)=val;
    hdv.Fill(dvName[i],i);
  }
  MsigDV.Write("DV_sigma"); // of distribution
  hdv.Write();

  //raw matrices
  linReg.mVPP.Write("IV_IV_rawVariance");
  linReg.mVPY.Write("IV_DV_rawVariance");
  linReg.mVYY.Write("DV_DV_rawVariance");
  linReg.mVY2.Write("DV_rawVariance");
  linReg.mVP2.Write("IV_rawVariance");

  //  TMatrixD Mstats(1,0);


  hFile->Close();
  printf("saved %s\n",hFile->GetName());
}



//________________________________________________
//________________________________________________
void
QwkRegBlueCorrelator::exportAlias(TString outPath, TString macroName,std::vector < TString > Pname, std::vector < TString > Yname){

  printf("::::::::::::::::QwkRegBlueCorrelator::exportAlias(%s) :::::::::::\n",macroName.Data());

 
  FILE *fd=fopen(outPath+macroName+".C","w");
  fprintf(fd,"void %s() {\n",macroName.Data());
  fprintf(fd,"  TTree* tree = (TTree*) gDirectory->Get(\"mul\");\n");
  for (int iy = 0; iy <nY; iy++) {
    fprintf(fd,"  tree->SetAlias(\"reg_%s\",\n         \"%s",Yname[iy].Data(),Yname[iy].Data());
    for (int j = 0; j < nP; j++) {
      double val= -linReg.mA(j,iy);
      if(val>0)  fprintf(fd,"+");
      fprintf(fd,"%.4e*%s",val,Pname[j].Data());
    }
    fprintf(fd,"\");\n");

  }


#if 0 // format
regalias_r5848() {
	Hel_Tree->SetAlias("reg_<dv1>","<dv1> +<slp_dv1_iv1>*<iv1>+<slp_dv1_iv2>*<iv2>+...");
	Hel_Tree->SetAlias("reg_<dv2>","<dv2> +<slp_dv2_iv1>*<iv1>+<slp_dv2_iv2>*<iv2>+...");
	.
	.
}

#endif 
  fprintf(fd,"}\n");
  fclose(fd);
  printf("saved %s\n",macroName.Data());
}

