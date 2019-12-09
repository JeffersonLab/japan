#include <iostream>
#include "TMath.h"
#include "TMatrixD.h"
#include "TMatrixDLazy.h"
#include "TVectorD.h"
#include "TDecompLU.h"
#include "TDecompSVD.h"
#include "TArrayD.h"
#include "TMatrixD.h"
#include "TMatrixDBase.h"
#include <TRandom.h>
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
#include <TChain.h>
#include <TString.h>
#include <TProfile.h>
#include <sstream>
#include <iostream>
#include <TGraphErrors.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <TEntryList.h>

//using namespace std; 

class Cycle{
  public:
    Int_t BPMvalid = 1;
    Int_t Detvalid = 1;
    Int_t slopesValid = 1;
    Double_t cycleNumber;
    Double_t supercycslope;
    Double_t supercyc_err;
    std::vector <TMatrixD> coilNames;
    std::vector <TMatrixD> BPMNames; 
    std::vector <TMatrixD> detNames; 
    std::vector <TMatrixD> coilData; // 7 coils
    std::vector <TMatrixD> BPMData; // N devices
    std::vector <TMatrixD> detData; // N devices
    std::vector <TMatrixD> detSlopes; // N devices
    std::vector<std::vector<Double_t>> BPMsens;
    std::vector<std::vector<Double_t>> BPMsens_err;
    std::vector<std::vector<Double_t>> BPMfNdata;
    std::vector<std::vector<Double_t>> Detsens;
    std::vector<std::vector<Double_t>> Detsens_err;
    std::vector<std::vector<Double_t>> DetfNdata;
};

class BMOD{
  public:
    Int_t runNumber = 0;
    Int_t nBPM = 0;
    Int_t nDet = 0;
    Int_t nCoil = 0;
    std::string infile = "NULL";
    TFile *file1;
    TChain *tree_R;
    ofstream outfile0;
    TString CoilListStr = "";
    TEntryList *elist;
    std::vector<Cycle> cycles;
    std::vector<Double_t> trim_base;
    std::map<std::string,std::vector<std::string>> parameterVectors;
    TString slopeFilename = "test.root";
    void parseTextFile(std::string);
    void calculateSensitivities();
    void invertMatrix();
    void saveSensitivityData();
    void saveSlopeData();
    void copytree(TString, Double_t);
    void edittree(TString);
    ~BMOD();
    Int_t getData(Int_t);
};

BMOD::~BMOD(){
  outfile0.close();
};

int QuerySlugNumber(int run_number){

  // Experimenting Function to Get slug number based on run number 
  // Author : Tao Ye

  TSQLResult* res;
  TSQLRow *row;
  cout << " -- Getting Slug Number from RCDB -- " << endl;
  cout << " -- Connecting to RCDB -- " << endl;
  TSQLServer *rcdb = TSQLServer::Connect("mysql://hallcdb.jlab.org:3306/a-rcdb","rcdb","");
  cout << " -- ServerInfo: " << rcdb->ServerInfo() << endl;
  cout << " -- Host : " << rcdb->GetHost() << endl;
  cout << " -- Query DataBase " << endl;
  TString select_q ="SELECT run_number,name,int_value "; 
  TString from_q =  "FROM `a-rcdb`.conditions,`a-rcdb`.condition_types ";
  TString where_q = Form("WHERE conditions.condition_type_id=condition_types.id and name='slug' and run_number='%d'",
			 run_number);
  res = rcdb->Query(select_q + from_q + where_q);
  if(res==NULL){
    cout << " -- Failed to Query " << endl;
    cout << " -- Bye-bye! " << endl;
    delete row;
    delete res;
    cout << " -- Closing Connection to RCDB " << endl;
    rcdb->Close();
    delete rcdb;
    return -1;
  }

  int nFields =res->GetFieldCount();
  row = res->Next();
  if(row==NULL){
    cout << " -- Failed to load slug number " << endl;
    cout << " -- Bye-bye! " << endl;
    delete row;
    delete res;
    cout << " -- Closing Connection " << endl;
    rcdb->Close();
    delete rcdb;
    return -1;
  }
  cout << " ----------------------------- " << endl;
  for(int j=0; j< nFields; j++)
    cout << "\t" << row->GetField(j) ;
  cout << endl;
  const char* slug_char = row->GetField(2);
  int slug_id = TString(slug_char).Atoi();
  delete row;
  cout << " ----------------------------- " << endl;
  cout << " -- Slug Number found  " << slug_id << endl;
  cout << " -- Closing Connection " << endl;
  rcdb->Close();
  delete res;
  delete rcdb;
  return slug_id;
}

Int_t BMOD::getData(Int_t runNo) { 
  Printf("Getting BMOD Data for run %d",runNo);
  runNumber = runNo; 
  if (parameterVectors.count("ROOT input path") != 0 ) {
    infile = parameterVectors["ROOT input path"].at(0);
    if (!((TString)parameterVectors["ROOT input path"].at(0)).Contains("#") || parameterVectors["ROOT input path"].at(0).size() == 0) {
      Printf("Error, please include a \"#\" symbol to be replaced by the run number in \"ROOT input path\" in %s",infile.c_str());
      return 1;
    }

    //Printf("Size of input file name %s = %d",infile.c_str(),infile.size());
    std::string infile1 = infile.substr(0,infile.find("#"));
    //Printf("Size of input file name part 1 %s = %d",infile1.c_str(),infile1.size());
    std::string infile2 = infile.substr(infile.find("#")+1,infile.size()-infile1.size()-1);
    //Printf("Size of input file name part 2 %s = %d",infile2.c_str(),infile2.size());
    //infile = Form("$QW_ROOTFILES/prexPrompt_pass1_%d.000.root",runNumber);
    infile = Form("%s%d%s",infile1.c_str(),runNumber,infile2.c_str());
    file1 = TFile::Open(infile.c_str());
    if(!file1){
      infile = Form("$QW_ROOTFILES/prexPrompt_pass1_%d.000.root",runNumber);
      file1 = TFile::Open(infile.c_str());
      if(!file1){
        std::cout << infile << " doesn't exist!!! Trying quick prompt" << std::endl;
        infile = Form("$QW_ROOTFILES/quick_%d.000.root",runNumber);
        file1=TFile::Open(infile.c_str());
        if(!file1){
          std::cout << infile << " doesn't exist!!!" << std::endl;
          return 1;
        }
      }
    }
  }
  else {
    Printf("ERROR: No \"ROOT input path\" parameter in %s",infile.c_str());
    return 1;
  }
  std::cout << "Using "<< infile << std::endl;
  return 0;
}

void BMOD::parseTextFile(std::string fileName = "input.txt"){
  Printf("Reading input config file");
  std::string line;
  std::vector<std::string> tokens;
  std::string token1;
  std::string token2;
  std::vector<std::string> details;
  ifstream ifinfile(fileName);
  if (ifinfile.is_open()){
    while(getline(ifinfile,line)){
//      Printf("Reading line %s",line.c_str());
      std::string token;
      std::istringstream tokenStream(line);
      Int_t tokenizer = 0;
      while(getline(tokenStream,token,'=')){
//        Printf("Reading token %s",token.c_str());
        if (tokenizer == 0) {
//          Printf("Token 1 = %s",token.c_str());
          token1 = (std::string)token;
        }
        else {
//          Printf("Token 2 = %s",token.c_str());
          token2 = (std::string)token;
        }
        tokenizer++;
        tokens.push_back(token);
      }
      if (tokens.size() < 2){
        Printf("Error: invalid input file");
        continue;
      }
      if (tokens.size() == 2){
        std::string detail;
        std::istringstream detailsStream(tokens.at(1));
        while(getline(detailsStream,detail,',')){
//          Printf("Reading detail %s",detail.c_str());
          details.push_back(detail);
        }
        if (details.size() > 0) {
//          Printf("Adding details to parameter vector %s",tokens[0].c_str());
          parameterVectors.insert(std::pair<std::string,std::vector<std::string>>(tokens.at(0),details));
        }
      }
      else {
        Printf("Invalid entry in input file: %s",line.c_str());
      }
      tokens.clear();
      details.clear();
    }
    ifinfile.close();
  }
}

void BMOD::calculateSensitivities(){
  gStyle->SetOptStat(0); 
  if (parameterVectors.count("Tree") == 0){
    Printf("Looking in \"evt\" tree for BMOD data");
    std::string tmpTreeStr = "evt";
    std::vector<std::string> tmpTreeStrVec;
    tmpTreeStrVec.push_back(tmpTreeStr);
    parameterVectors["Tree"] = tmpTreeStrVec;
  }
  tree_R = (TChain*) file1->Get(parameterVectors["Tree"].at(0).c_str());
  tree_R->Draw(">>elist","bmwcycnum>0","entrylist");  //picks out unique cycle numbers
  elist = (TEntryList*)gDirectory->Get("elist");
  tree_R->SetEntryList(elist);
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  int nonzero = tree_R->Draw("bmwcycnum","bmwcycnum>0","goff");
  vector<Double_t> cycleNumbers;
  for(int i=0;i<nonzero;i++){
    l_bmwcycnum->GetBranch()->GetEntry(elist->GetEntry(i));
    Double_t cyclenum = (Double_t)l_bmwcycnum->GetValue();
    if(i==0){
      cycleNumbers.push_back(cyclenum);
    }
    else{
      int sizeVec = cycleNumbers.size();
      if(cyclenum != cycleNumbers[sizeVec-1]){
        cycleNumbers.push_back(cyclenum);
      }
    }
  }
  if (cycleNumbers.size()==0) { 
    Printf("No good cycles this run, ERROR (for now)");
    return; 
  }
  /* OLD
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  for(int i=0;i<nonzero;i++){
    l_bmwcycnum->GetBranch()->GetEntry(elist->GetEntry(i));
    Double_t cyclenum = l_bmwcycnum->GetValue();
    if(i==0){
      cycleNumbers.push_back(cyclenum);
    }   
    else{
      int sizeVec = cycleNumbers.size();
      if(cyclenum != cycleNumbers[sizeVec-1]){
        cycleNumbers.push_back(cyclenum);
      }
    }   
  }
  */  
  //const int nCoil =7;
  //TString wire[nCoil]={"bmod_trim1","bmod_trim2","bmod_trim3","bmod_trim4","bmod_trim5","bmod_trim6","bmod_trim7"};
  // Guarantee device vectors is full of contents from parameter vectors
  if (parameterVectors.count("Coils") == 0 || parameterVectors["Coils"].size() == 0) {
    Printf("ERROR: No Coils listed for Dithering Analysis. Please add a line to input file with an entry \"Coils=comma separated list of bmod_trim# numbers\"");
    return 0;
  }
  nCoil = parameterVectors["Coils"].size();
  for (int l = 0 ; l<nCoil ; l++){
    CoilListStr = Form("%s%s",CoilListStr.Data(),parameterVectors["Coils"].at(l).c_str());
  }

  std::vector<Double_t> tmp1;
  std::vector<Double_t> tmp2;
  std::vector<Double_t> tmp3;
  for(int icoil=0;icoil<nCoil;icoil++){
    tmp1.push_back(0.0);
    tmp2.push_back(-1.0);
    tmp3.push_back(0.0);
  }

  TString name; // A BUFF
  //TString bpmName; // A BUFF
  //TString bpm_array[] = {"bpm4aX","bpm4eX","bpm4aY","bpm4eY","bpm11X+0.4*bpm12X"};
  // Guarantee device vectors is full of contents from parameter vectors
  if (parameterVectors.count("BPMs") == 0 || parameterVectors["BPMs"].size() == 0) {
    Printf("ERROR: No BPMs listed for Dithering Analysis. Please add a line to input file with an entry \"BPMs=comma separated list of BPM device names\"");
    return 0;
  }
  //const int nBPM = 5;
  nBPM = parameterVectors["BPMs"].size();

  TString detname; // A BUFF
  if (parameterVectors.count("Detectors") == 0 || parameterVectors["Detectors"].size() == 0) {
    Printf("Warning: No Detectors listed for Dithering Analysis. Please add a line to input file with an entry \"Detectors=comma separated list of Detector device names\"");
  }
  nDet = parameterVectors["Detectors"].size();

  if (parameterVectors.count("Cut") == 0) {
    Printf("Warning: no parameter \"Cut\" given, assuming User cut = bcm_dg_ds>60");
    std::string tmpCutStr = "bcm_dg_ds>60";
    std::vector<std::string> tmpCutStrVec;
    tmpCutStrVec.push_back(tmpCutStr);
    parameterVectors["Cut"] = tmpCutStrVec;
  }

  //const int n=cycleNumbers.size();//number of cycle
  //Double_t cycleNumbers[n];
  //Double_t supercycslope[n];
  //Double_t supercyc_err[n];
  //Int_t Is_fill[n];
  for(int i=0;i<cycleNumbers.size();i++){
    Cycle tmpCycle;
    tmpCycle.cycleNumber = cycleNumbers[i];
    tmpCycle.supercycslope = cycleNumbers[i];
    tmpCycle.supercyc_err = 0;
    for(int ibpm=0;ibpm<nBPM;ibpm++){
      tmpCycle.BPMsens.push_back(tmp1);
      tmpCycle.BPMsens_err.push_back(tmp2);
      tmpCycle.BPMfNdata.push_back(tmp3);
    }
    for(int idet=0;idet<nDet;idet++){
      tmpCycle.Detsens.push_back(tmp1);
      tmpCycle.Detsens_err.push_back(tmp2);
      tmpCycle.DetfNdata.push_back(tmp3);
    }
    cycles.push_back(tmpCycle);
    Printf("i = %d, supercyc = %f",i,cycleNumbers[i]);
  }

  //Double_t trim_base[7] = {1683,1582,1708,1670,1662,1709,1686};
  // NEW bases:
  // Double_t trim_base[7] = {833,782,845,827,822,844,832};
  /*
  if (parameterVectors.count("Trim Bases")) {
    for (std::string &num:parameterVectors["Trim Bases"]){
      trim_base.push_back((Double_t)std::atof(num.c_str()));
    }
  }*/
  TH1F* hist_trim;
  for (Int_t k = 0; k<nCoil; k++){
    tree_R->Draw(Form("bmod_trim%s>>hist_trim%d",parameterVectors["Coils"][k].c_str(),k),"bmod_ramp<0","");
    hist_trim = (TH1F *)gDirectory->Get(Form("hist_trim%d",k));
    trim_base.push_back(hist_trim->GetXaxis()->GetBinCenter(hist_trim->GetMaximumBin()));
  }
  
  const double trimmin=0.38;
  const double trimmax=0.7;
  const double bpmmax=3;
  const double bpmmin=1;
  const double chtov=1.0e-3;//7.62939453125000000e-05; //10V*(1/pow(2,17))
  double factor=1.0e+3;
  TF1 *fun = new TF1("fun","[0]+[1]*x",-1e6,1e6);
  Double_t init_par[2] = {0,0};
  fun->SetParameters(init_par);

  int xnbins=0.0;
  int ynbins=0;
  double ibincon=0;
  int ibin=0;
  double xbincon=0.0;
  int coilnum=0;


  for(Int_t i=0;i<cycles.size();i++){
    //for(int ibpm=0;ibpm<nBPM;ibpm++)
    int j = 0;
    //for(int j=0;j<(nBPM+nDet);j++)
    while(j<(nBPM+nDet) && cycles[i].BPMvalid!=0){
      //bpmName= bpm_array[ibpm];
      //bpmName=parameterVectors["BPMs"][ibpm];
      if (j<nBPM){
        name=parameterVectors["BPMs"][j];
        factor=1.0e+3;
      }
      else {
        name=parameterVectors["Detectors"][j-nBPM];
        factor=1.0e+6;
      }
      if (parameterVectors.count("Coils") == 0 || parameterVectors["Coils"].size() == 0) {
        Printf("Error: No Trim Card Coils listed for Dithering Analysis. Please add a line to input file with an entry \"Coils=comma separated list of bmod_trim# numbers\"");
        return 1;
      }
      for(int icoil=0;icoil<nCoil;icoil++){
        //int ndata = tree_R->Draw(Form("%lf*(%s):(%s*%lf)",factor,bpmName.Data(),parameterVectors["Coils"][icoil].c_str(),chtov),
        // Get Mean for normalized sensitivity calculation
        int ndata = tree_R->Draw(Form("(%s):(bmod_trim%s*%lf)",name.Data(),parameterVectors["Coils"][icoil].c_str(),chtov),
            Form("%s && bmod_ramp>0 && bmwobj==%s && abs(bmod_trim%s-%f)>20 && bmwcycnum==%f",
              parameterVectors["Cut"].at(0).c_str(),parameterVectors["Coils"][icoil].c_str(),parameterVectors["Coils"][icoil].c_str(),trim_base[icoil],cycles[i].cycleNumber));
        if(ndata<50){
          Printf("-- Sensitivity for Device: %s",name.Data());
          Printf("-- CycleNumber: %f",cycles[i].cycleNumber);
          Printf("-- Coil ID: %d",icoil+1);
          Printf("-- NData: %d",ndata);
          Printf("-- Not enough data Discarding this cycle ");
          if (j<nBPM) {
            cycles[i].BPMvalid = 0;
            icoil = nCoil;
            j=nBPM+nDet;
            continue;
          }
          else if (j>=nBPM) {
            cycles[i].Detvalid = 0;
            icoil = nCoil;
            j=nBPM+nDet;
            continue;
          }
        }
        else if(ndata>=50 && cycles[i].BPMvalid==1){
          TGraph *g1 = new TGraph(ndata, tree_R->GetV2(),tree_R->GetV1());
          g1->GetXaxis()->SetTitle(Form("bmod_trim%s",parameterVectors["Coils"][icoil].c_str()));
          g1->GetYaxis()->SetTitle(name);
          g1->GetXaxis()->SetTitleSize(0.05);
          g1->GetYaxis()->SetTitleSize(0.05);

          double this_mean = g1->GetMean(2);
          if (j<nBPM){
            this_mean = 1.0;
          }
          ndata = tree_R->Draw(Form("(%lf/%lf)*(%s):(bmod_trim%s*%lf)",factor,this_mean,name.Data(),parameterVectors["Coils"][icoil].c_str(),chtov),
              Form("%s && bmod_ramp>0 && bmwobj==%s && abs(bmod_trim%s-%f)>20 && bmwcycnum==%f",
                parameterVectors["Cut"].at(0).c_str(),parameterVectors["Coils"][icoil].c_str(),parameterVectors["Coils"][icoil].c_str(),trim_base[icoil],cycles[i].cycleNumber));

          double this_slope,this_error;
          Printf("-- Sensitivity for Device: %s",name.Data());
          Printf("-- CycleNumber: %f",cycles[i].cycleNumber);
          Printf("-- Coil ID: %d",icoil+1);
          Printf("-- NData: %d",ndata);

          TGraph* g2 = new TGraph(ndata, tree_R->GetV2(),tree_R->GetV1());
          fun->SetParameters(init_par);
          g2->Fit("fun","Q0");

          this_slope = fun->GetParameter(1);
          this_error = fun->GetParError(1);

          if (j<nBPM) {
            cycles.at(i).BPMfNdata.at(j).at(icoil)   += ndata;
            cycles.at(i).BPMsens[j][icoil]     =  this_slope;
            cycles.at(i).BPMsens_err[j][icoil] =  this_error;
          }
          else {
            cycles.at(i).DetfNdata[j-nBPM][icoil]   += ndata;
            cycles.at(i).Detsens[j-nBPM][icoil]     =  this_slope;
            cycles.at(i).Detsens_err[j-nBPM][icoil] =  this_error;
          }
        }
      } // end of coil loop
      j++;
    } // end of Det loop
/*
    for(int idev=0;idev<(nBPM+nDet);idev++) {
      for(int icoil=0;icoil<nCoil;icoil++) {
        if (idev<nBPM){
          if(cycles.at(i).BPMsens_err[idev][icoil]==-1.0){
            //Printf("-- Incomplete dithering coil %d is found. No BPM sensitivity output for this cycle",icoil);
            cycles[i].BPMvalid = 0;
            //	return 1;
          }
        }
        else {
          if(cycles.at(i).Detsens_err[idev-nBPM][icoil]==-1.0){
            //Printf("-- Incomplete dithering coil %d is found. No detector sensitivity output for this cycle",icoil);
            cycles[i].Detvalid = 0;
            //	return 1;
          }
        }
      }
    }*/
  } //end of cyc loop
}

void BMOD::saveSensitivityData() {
  // Text file output
  if (parameterVectors.count("Output path") == 0) {
    Printf("No output path given, using: ./BPMs_sensitivity_run%d.txt",runNumber);
    outfile0.open(Form("./BMOD_sensitivities_%s_run%d.txt",CoilListStr.Data(),runNumber));
  }
  else {
    outfile0.open(Form("%s/BMOD_sensitivities_%s_run%d.txt",parameterVectors["Output path"].at(0).c_str(),CoilListStr.Data(),runNumber));
  }

  double tab_width = 20;
  const int cutflag=1;

  for(Int_t i=0;i<cycles.size();i++){
    if(cycles[i].BPMvalid==1){
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        outfile0
          << setw(tab_width) << setiosflags(ios::left) << runNumber 
          << setw(tab_width) << setiosflags(ios::left) << cycles[i].cycleNumber 
          << setw(tab_width) << setiosflags(ios::left) << cutflag 
          << setw(tab_width) << setiosflags(ios::left) << parameterVectors["BPMs"][ibpm];
        for(int icoil=0;icoil<nCoil;icoil++){
          outfile0
            << setw(tab_width) << setiosflags(ios::left) 
            << setprecision(5) << cycles[i].BPMsens[ibpm][icoil]
            << setw(tab_width) << setiosflags(ios::left)
            << setprecision(3) << cycles[i].BPMsens_err[ibpm][icoil]
            << setw(tab_width) << setiosflags(ios::left) 
            << setprecision(4) << cycles[i].BPMfNdata[ibpm][icoil];
        } // end of coil loop
        outfile0 << std::endl;
      } // end of detector loop
      if(cycles[i].Detvalid==1){
        for(int idet=0;idet<nDet;idet++){
          outfile0
            << setw(tab_width) << setiosflags(ios::left) << runNumber
            << setw(tab_width) << setiosflags(ios::left) << cycles[i].cycleNumber 
            << setw(tab_width) << setiosflags(ios::left) << cutflag // Now , just one o   
            << setw(tab_width) << setiosflags(ios::left) << parameterVectors["Detectors"][idet];
          for(int icoil=0;icoil<nCoil;icoil++){
            outfile0
              << setw(tab_width) << setiosflags(ios::left)
              << setprecision(5) << cycles[i].Detsens[idet][icoil]
              << setw(tab_width) << setiosflags(ios::left)
              << setprecision(3) << cycles[i].Detsens_err[idet][icoil]
              << setw(tab_width) << setiosflags(ios::left)
              << setprecision(4) << cycles[i].DetfNdata[idet][icoil];
          } // end of coil loop
          outfile0 << std::endl;
        } // end of detector loop
      }
    }
  }
  outfile0.close();
}

void BMOD::invertMatrix() {
  //  Loop over cycles
  for(Int_t i=0;i<cycles.size();i++){
    if(cycles[i].BPMvalid==1 && cycles[i].Detvalid==1){
      TMatrixD tmpBPMmat(nCoil,nBPM);
      for(int icoil=0;icoil<nCoil;icoil++){
        for(int ibpm=0;ibpm<nBPM;ibpm++){
          tmpBPMmat[icoil][ibpm] = cycles[i].BPMsens[ibpm][icoil];// BPM data
          cycles[i].BPMData.push_back(tmpBPMmat);
        }
      }
      TMatrixD BPM_inverse = tmpBPMmat;
      Double_t det2 = 0.0;
      BPM_inverse.Invert(&det2);
      // FIXME do some checks on det2 here

      for(int idet=0;idet<nDet;idet++){
        TMatrixD tmpDetVec(nCoil,1);
        for(int icoil=0;icoil<nCoil;icoil++){
          tmpDetVec[icoil][0] = cycles[i].Detsens[idet][icoil];// BPM data
          cycles[i].detData.push_back(tmpDetVec);
        }
        for(int idet=0;idet<nDet;idet++){
          TMatrixD tmpDetSlopes(nBPM,1);
          tmpDetSlopes.Mult(BPM_inverse,tmpDetVec);
          cycles[i].detSlopes.push_back(tmpDetSlopes);
        }
      }

    }
  }
}

void BMOD::copytree(TString oldFileName = "test.root", Double_t cyclenumber = -1)
{
  // Get old file, old tree and set top branch address
  if(cyclenumber==-1){
    cout << "No cyclenumber passed with macro call!" << endl;
    return;
  }
  cout << "ignoring cyclenumber " << cyclenumber << endl;
  Bool_t newFile = gSystem->AccessPathName(oldFileName);
  if (!newFile){
    TFile* oldfile = TFile::Open(oldFileName);
    TTree *oldtree;
    oldfile->GetObject("dit", oldtree);
    const auto nentries = oldtree->GetEntries();

    TFile newfile(Form("localTmp_%d_%f.root",runNumber,cyclenumber), "recreate");
    auto newtree = oldtree->CloneTree(0);
    TLeaf* cycNumL = oldtree->GetLeaf("cyclenum");
    Double_t tmpCycNum = 0.0;
    for (auto i : ROOT::TSeqI(nentries)) {
      cycNumL->GetBranch()->GetEntry(i);
      tmpCycNum = cycNumL->GetValue();
      if (tmpCycNum==cyclenumber) continue;
      oldtree->GetEntry(i);
      newtree->Fill();
    }
    newtree->Write("dit",TObject::kOverwrite);
    newfile.Close();
    gSystem->Exec(Form("mv localTmp_%d_%f.root %s",runNumber,cyclenumber,oldFileName.Data()));
  }
}

void BMOD::edittree(TString oldFileName = "test.root")
{
  // Get old file, old tree and set top branch address
  Bool_t newFile = gSystem->AccessPathName(oldFileName);
  if (!newFile){
    TFile* oldfile = TFile::Open(oldFileName);
    TTree *oldtree;
    oldfile->GetObject("dit", oldtree);
    const auto nentries = oldtree->GetEntries();

    TFile newfile(Form("localTmp_%d.root",runNumber), "recreate");
    auto newtree = oldtree->CloneTree(0);
    TLeaf* cycNumL = oldtree->GetLeaf("cyclenum");
    Double_t tmpCycNum = 0.0;

    Int_t flag = 1;
    newtree->SetBranchAddress("flag",&flag);

    std::vector<std::vector<std::vector<Double_t>>> tmpVecss;
    std::vector<std::vector<Double_t>> tmpVecs;
    std::vector<std::vector<Double_t>> tmpMeans;
    std::vector<std::vector<Double_t>> tmpRMSs;
    std::vector<Double_t> tmpVec;
    std::vector<Double_t> tmpVec2;
    // FIXME Technically nDet and nBPM are 0 unless you've executed "calculateSensitivities" at least once...
    for(int idet=0;idet<nDet;idet++){
      tmpVecs.clear();
      tmpVec2.clear();
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        tmpVec.clear();
        for (Int_t j = 0; j<nentries ; j++){
          tmpVec.push_back(0.0);
        }
        tmpVecs.push_back(tmpVec);
        tmpVec2.push_back(0.0);
      }
      tmpVecss.push_back(tmpVecs);
      tmpMeans.push_back(tmpVec2);
      tmpRMSs.push_back(tmpVec2);
    }
    for (auto i : ROOT::TSeqI(nentries)) {
      cycNumL->GetBranch()->GetEntry(i);
      tmpCycNum = cycNumL->GetValue();
      oldtree->GetEntry(i);
      //{do stuff}
      if (parameterVectors.count("BPMs Slope Names") == 0 || parameterVectors["BPMs Slope Names"].size() == 0) {
        Printf("ERROR: No \"BPMs Slope Names\" listed for Dithering Analysis. Please add a line to input file with an entry \"BPMs Slope Names\"=comma separated list of BPM slope names\"");
        return;
      }
      if (parameterVectors.count("Detectors") == 0 || parameterVectors["Detectors"].size() == 0) {
        Printf("ERROR: No \"Detectors\" listed for Dithering Analysis. Please add a line to input file with an entry \"Detectors=comma separated list of Detector device names\"");
        return;
      }
      for(int idet=0;idet<nDet;idet++){
        for(int ibpm=0;ibpm<nBPM;ibpm++){
          TLeaf* tmpLeaf = oldtree->GetLeaf(Form("%s_%s",parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm).c_str()));
          tmpLeaf->GetBranch()->GetEntry(i);
          tmpVecss.at(idet).at(ibpm).at(i) = tmpLeaf->GetValue();
        }
      }
    }
    for(int idet=0;idet<nDet;idet++){
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        tmpMeans.at(idet).at(ibpm) = TMath::Mean(tmpVecss.at(idet).at(ibpm).begin(),tmpVecss.at(idet).at(ibpm).end());
        tmpRMSs.at(idet).at(ibpm)  = TMath::RMS(tmpVecss.at(idet).at(ibpm).begin(),tmpVecss.at(idet).at(ibpm).end());
      }
    }
    for (Int_t i=0 ; i<nentries ; i++) {
      cycNumL->GetBranch()->GetEntry(i);
      tmpCycNum = cycNumL->GetValue();
      oldtree->GetEntry(i);
      // FIXME how many detectors' slopes we actually look at is an important question: ignore ATs? Only look at SAMs for CREX?
      for(int idet=0;idet<nDet;idet++){
        for(int ibpm=0;ibpm<nBPM;ibpm++){
          if ( parameterVectors.count("Flag Sigma Cut") != 0 && abs(tmpVecss.at(idet).at(ibpm).at(i)-tmpMeans.at(idet).at(ibpm)) > atof((parameterVectors["Flag Sigma Cut"].at(0).c_str()))*tmpRMSs.at(idet).at(ibpm)) {
          //  Printf("Cycle hit %f sigma cut, flag = 0",atof((parameterVectors["Flag Sigma Cut"].at(0).c_str())));
          //  Printf("Slope %f doesn't fit within %f of mean %f",tmpVecss.at(idet).at(ibpm).at(i),atof((parameterVectors["Flag Sigma Cut"].at(0).c_str()))*tmpRMSs.at(idet).at(ibpm),tmpMeans.at(idet).at(ibpm));
            flag = 0;
          }
          //else {
          //  Printf("Slope OK");
          //  Printf("Slope %f fits within %f of mean %f",tmpVecss.at(idet).at(ibpm).at(i),atof((parameterVectors["Flag Sigma Cut"].at(0).c_str()))*tmpRMSs.at(idet).at(ibpm),tmpMeans.at(idet).at(ibpm));
          //}
        }
      }
      newtree->Fill();
    }
    newtree->Write("dit",TObject::kOverwrite);
    newfile.Close();
    gSystem->Exec(Form("mv localTmp_%d.root %s",runNumber,oldFileName.Data()));
  }
  else {
    Printf("Error, %s doesn't exist",oldFileName.Data());
  }
}

void BMOD::saveSlopeData() {
  if (parameterVectors.count("BPMs Slope Names") == 0 || parameterVectors["BPMs Slope Names"].size() == 0) {
    Printf("ERROR: No \"BPMs Slope Names\" listed for Dithering Analysis. Please add a line to input file with an entry \"BPMs Slope Names\"=comma separated list of BPM slope names\"");
    return;
  }
  if (parameterVectors.count("Detectors") == 0 || parameterVectors["Detectors"].size() == 0) {
    Printf("ERROR: No \"Detectors\" listed for Dithering Analysis. Please add a line to input file with an entry \"Detectors=comma separated list of Detector device names\"");
    return;
  }
  std::vector<std::vector<Double_t>> slopes;
  std::vector<std::vector<Double_t>> localBPMsens;
  std::vector<std::vector<Double_t>> localBPMsens_err;
  std::vector<std::vector<Double_t>> localBPMfNdata;
  std::vector<std::vector<Double_t>> localDetsens;
  std::vector<std::vector<Double_t>> localDetsens_err;
  std::vector<std::vector<Double_t>> localDetfNdata;
  std::vector<std::vector<std::vector<std::vector<Double_t>>>> localAlphas;
  std::vector<std::vector<std::vector<std::vector<Double_t>>>> localBetas;
  std::vector<std::vector<std::vector<std::vector<Double_t>>>> localDeltas;

  // Prepare empty vectors and matrices
  std::vector<Double_t> tmpVec;
  for (int j = 0; j < nDet; j++){
    for (int k = 0; k < nBPM; k++){
      tmpVec.push_back(0.0);
    }
    // Empty slopes per monitor/detector combination
    slopes.push_back(tmpVec);
    tmpVec.clear();
  }

  // Also for the sensitivities
  for(int icoil=0;icoil<nCoil;icoil++){
    for(int ibpm=0;ibpm<nBPM;ibpm++){
      tmpVec.push_back(0.0);
    }
    localBPMsens.push_back(tmpVec);
    localBPMsens_err.push_back(tmpVec);
    localBPMfNdata.push_back(tmpVec);
    tmpVec.clear();

    for(int idet=0;idet<nDet;idet++){
      tmpVec.push_back(0.0);
    }
    localDetsens.push_back(tmpVec);
    localDetsens_err.push_back(tmpVec);
    localDetfNdata.push_back(tmpVec);
    tmpVec.clear();
  }

  // Also prep the alphas, etc.
  std::vector<std::vector<Double_t>> tmpVec2;
  std::vector<std::vector<std::vector<Double_t>>> tmpVec3;
  for(int icoil1=0;icoil1<nCoil;icoil1++){
    for(int icoil2=0;icoil2<nCoil;icoil2++){
      for(int ibpm1=0;ibpm1<nBPM;ibpm1++){
        for(int ibpm2=0;ibpm2<nBPM;ibpm2++){
          tmpVec.push_back(0.0);
        }
        tmpVec2.push_back(tmpVec);
        tmpVec.clear();
      }
      tmpVec3.push_back(tmpVec2);
      tmpVec2.clear();
    }
    localAlphas.push_back(tmpVec3);
    localBetas.push_back(tmpVec3);
    localDeltas.push_back(tmpVec3);
  }

  Int_t slug_number = QuerySlugNumber(runNumber);

  if (parameterVectors.count("Rootfile Output Path") == 0) {
    Printf("ERROR: No \"Rootfile Output Path\" listed for Dithering Analysis. Using \"../rootfiles_alldet_pass1\" instead");
    slopeFilename = Form("dit_alldet_slopes_slug%d.root",slug_number);
  }
  else {
    if (parameterVectors.count("Rootfile Output Runwise") != 0 && parameterVectors["Rootfile Output Runwise"].at(0) == "True") {
      slopeFilename = Form("%s/dithering_slopes_slug%d_run%d.root",parameterVectors["Rootfile Output Path"].at(0).c_str(),slug_number,runNumber);
    }
    else {
      slopeFilename = Form("%s/dithering_slopes_slug%d.root",parameterVectors["Rootfile Output Path"].at(0).c_str(),slug_number);
    }
  }

  for(int i=0;i<cycles.size();i++){
    copytree(slopeFilename,cycles.at(i).cycleNumber);
  }
  TFile* ditfile = TFile::Open(Form("%s",slopeFilename.Data()),"UPDATE");
  if(ditfile==NULL){
    ditfile = TFile::Open(Form("%s",slopeFilename.Data()),"RECREATE");
  }
  TTree* dit_tree = (TTree*)ditfile->Get("dit");

  Int_t runNum = runNumber;
  Int_t flag = 1;
  Double_t cycleNum = 0;
  if(dit_tree==NULL){
    dit_tree = new TTree("dit","dit");
    for(int idet=0;idet<nDet;idet++){
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        dit_tree->Branch(Form("%s_%s",parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm).c_str()),&slopes[idet][ibpm]);
      }
    }    
    for(int icoil=0;icoil<nCoil;icoil++){
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        dit_tree->Branch(Form("%s_coil%s",      parameterVectors["BPMs Slope Names"].at(ibpm).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localBPMsens[icoil][ibpm]);
        dit_tree->Branch(Form("%s_coil%s_err",  parameterVectors["BPMs Slope Names"].at(ibpm).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localBPMsens_err[icoil][ibpm]);
        dit_tree->Branch(Form("%s_coil%s_Ndata",parameterVectors["BPMs Slope Names"].at(ibpm).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localBPMfNdata[icoil][ibpm]);
      }
      for(int idet=0;idet<nDet;idet++){
        dit_tree->Branch(Form("%s_coil%s",      parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localDetsens[icoil][idet]);
        dit_tree->Branch(Form("%s_coil%s_err",  parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localDetsens_err[icoil][idet]);
        dit_tree->Branch(Form("%s_coil%s_Ndata",parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localDetfNdata[icoil][idet]);
      }
    }
    for(int icoil1=0;icoil1<nCoil;icoil1++){
      for(int icoil2=0;icoil2<nCoil;icoil2++){
        for(int ibpm1=0;ibpm1<nBPM;ibpm1++){
          for(int ibpm2=0;ibpm2<nBPM;ibpm2++){
            if (ibpm1 == ibpm2 || icoil1 == icoil2) {
              continue;
            }
            else {
              dit_tree->Branch(Form("alpha%s%s_%s%s",parameterVectors["Coils"].at(icoil1).c_str(),parameterVectors["Coils"].at(icoil2).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm1).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm2).c_str()),&localAlphas[icoil1][icoil2][ibpm1][ibpm2]);
              dit_tree->Branch(Form("beta%s%s_%s%s",parameterVectors["Coils"].at(icoil1).c_str(),parameterVectors["Coils"].at(icoil2).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm1).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm2).c_str()),&localBetas[icoil1][icoil2][ibpm1][ibpm2]);
              dit_tree->Branch(Form("delta%s%s_%s%s",parameterVectors["Coils"].at(icoil1).c_str(),parameterVectors["Coils"].at(icoil2).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm1).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm2).c_str()),&localDeltas[icoil1][icoil2][ibpm1][ibpm2]);
            }
          }
        }
      }
    }
    dit_tree->Branch("run",
        &runNum,
        "run/I");
    dit_tree->Branch("cyclenum",
        &cycleNum,
        "cyclenum/D");
    dit_tree->Branch("flag",
        &flag,
        "flag/I");
  }    
  else{
    for(int idet=0;idet<nDet;idet++){
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        dit_tree->SetBranchAddress(Form("%s_%s",parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm).c_str()),&slopes[idet][ibpm]);
      }
    }
    for(int icoil=0;icoil<nCoil;icoil++){
      for(int ibpm=0;ibpm<nBPM;ibpm++){
        dit_tree->SetBranchAddress(Form("%s_coil%s",      parameterVectors["BPMs Slope Names"].at(ibpm).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localBPMsens[icoil][ibpm]);
        dit_tree->SetBranchAddress(Form("%s_coil%s_err",  parameterVectors["BPMs Slope Names"].at(ibpm).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localBPMsens_err[icoil][ibpm]);
        dit_tree->SetBranchAddress(Form("%s_coil%s_Ndata",parameterVectors["BPMs Slope Names"].at(ibpm).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localBPMfNdata[icoil][ibpm]);
      }
      for(int idet=0;idet<nDet;idet++){
        dit_tree->SetBranchAddress(Form("%s_coil%s",      parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localDetsens[icoil][idet]);
        dit_tree->SetBranchAddress(Form("%s_coil%s_err",  parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localDetsens_err[icoil][idet]);
        dit_tree->SetBranchAddress(Form("%s_coil%s_Ndata",parameterVectors["Detectors"].at(idet).c_str(),parameterVectors["Coils"].at(icoil).c_str()),&localDetfNdata[icoil][idet]);
      }
    }
    for(int icoil1=0;icoil1<nCoil;icoil1++){
      for(int icoil2=0;icoil2<nCoil;icoil2++){
        for(int ibpm1=0;ibpm1<nBPM;ibpm1++){
          for(int ibpm2=0;ibpm2<nBPM;ibpm2++){
            if (ibpm1 == ibpm2 || icoil1 == icoil2) {
              continue;
            }
            else {
              dit_tree->SetBranchAddress(Form("alpha%s%s_%s%s",parameterVectors["Coils"].at(icoil1).c_str(),parameterVectors["Coils"].at(icoil2).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm1).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm2).c_str()),&localAlphas[icoil1][icoil2][ibpm1][ibpm2]);
              dit_tree->SetBranchAddress(Form("beta%s%s_%s%s",parameterVectors["Coils"].at(icoil1).c_str(),parameterVectors["Coils"].at(icoil2).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm1).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm2).c_str()),&localBetas[icoil1][icoil2][ibpm1][ibpm2]);
              dit_tree->SetBranchAddress(Form("delta%s%s_%s%s",parameterVectors["Coils"].at(icoil1).c_str(),parameterVectors["Coils"].at(icoil2).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm1).c_str(),parameterVectors["BPMs Slope Names"].at(ibpm2).c_str()),&localDeltas[icoil1][icoil2][ibpm1][ibpm2]);
            }
          }
        }
      }
    }
    dit_tree->SetBranchAddress("run",
        &runNum);
    dit_tree->SetBranchAddress("cyclenum",
        &cycleNum);
    dit_tree->SetBranchAddress("flag",
        &flag);
  }

  for(int i=0;i<cycles.size();i++){
    if(cycles.at(i).BPMvalid==1 && cycles.at(i).Detvalid==1){
      runNum   = runNumber;
      cycleNum = cycles.at(i).cycleNumber;
      Printf("Adding entry for cycle %f",cycleNum);
      // The flag variable is determined by whether there is sufficient data for BPM sensitivity calculation
      flag     = cycles.at(i).BPMvalid;
      for(int idet=0;idet<nDet;idet++){
        for(int ibpm=0;ibpm<nBPM;ibpm++){
            // We found a null slope
          if(cycles.at(i).detSlopes.at(idet)[ibpm][0]==0){
            cycles.at(i).slopesValid = 0;
          }
        }
      }
      // Right here is ignoring data if the slopes == 0
      if (cycles.at(i).slopesValid == 1) {
        for(int idet=0;idet<nDet;idet++){
          for(int ibpm=0;ibpm<nBPM;ibpm++){
            slopes[idet][ibpm] = cycles.at(i).detSlopes.at(idet)[ibpm][0];
          }
          //cout<<"slope="<< slope[0][ibpm]<<"  "<<slope[1][ibpm]<<"  "<<slope[2][ibpm]<<"  "<<slope[3][ibpm]<<"  "<< slope[4][ibpm]<<"  "<<slope[5][ibpm] <<"  "<<slope[6][ibpm]<<"  " << slope[7][ibpm]<<" flag="<< bpm4ax_flag[i]<<endl;
        }
        for(int icoil=0;icoil<nCoil;icoil++){
          for(int ibpm=0;ibpm<nBPM;ibpm++){
            localBPMsens[icoil][ibpm] = cycles.at(i).BPMsens.at(icoil)[ibpm];
            localBPMsens_err[icoil][ibpm] = cycles.at(i).BPMsens_err.at(icoil)[ibpm];
            localBPMfNdata[icoil][ibpm] = cycles.at(i).BPMfNdata.at(icoil)[ibpm];
          }
          for(int idet=0;idet<nDet;idet++){
            localDetsens[icoil][idet] = cycles.at(i).Detsens.at(icoil)[idet];
            localDetsens_err[icoil][idet] = cycles.at(i).Detsens_err.at(icoil)[idet];
            localDetfNdata[icoil][idet] = cycles.at(i).DetfNdata.at(icoil)[idet];
          }
        }
        for(int icoil1=0;icoil1<nCoil;icoil1++){
          for(int icoil2=0;icoil2<nCoil;icoil2++){
            for(int ibpm1=0;ibpm1<nBPM;ibpm1++){
              for(int ibpm2=0;ibpm2<nBPM;ibpm2++){
                if ( localBPMsens[icoil1][ibpm2] != 0 && localBPMsens[icoil2][ibpm1] != 0 ) {
                  localAlphas[icoil1][icoil2][ibpm1][ibpm2] = 1.0 - (localBPMsens[icoil1][ibpm1]/localBPMsens[icoil1][ibpm2])*(localBPMsens[icoil2][ibpm2]/localBPMsens[icoil2][ibpm1]);
                  // There are a bunch of alphas that can be calculated, but the ones Ye Tian cares about are:
                  //
                  // alpha15 = 1-(bpm4ex_coil1*bpm4ax_coil5)/(bpm4ax_coil1*bpm4ex_coil5) = alpha15_4ex4ax
                  // alpha35 = 1-(bpm4ex_coil3*bpm4ax_coil5)/(bpm4ax_coil3*bpm4ex_coil5) = alpha35_4ex4ax
                  // alpha13 = 1-(bpm4ex_coil1*bpm4ax_coil3)/(bpm4ax_coil1*bpm4ex_coil3) = alpha13_4ex4ax
                  // alpha24 = 1-(bpm4ey_coil2*bpm4ay_coil4)/(bpm4ay_coil2*bpm4ey_coil4) = alpha24_4ey4ay
                  // alpha26 = 1-(bpm4ey_coil2*bpm4ay_coil6)/(bpm4ay_coil2*bpm4ey_coil6) = alpha26_4ey4ay
                  // alpha46 = 1-(bpm4ey_coil4*bpm4ay_coil6)/(bpm4ay_coil4*bpm4ey_coil6) = alpha46_4ey4ay
                  //
                  // delta15 = (bpm12x_coil5/bpm4ax_coil5)-(bpm12x_coil1/bpm4ax_coil1) = delta15_12x4ax
                  // delta35 = (bpm12x_coil5/bpm4ax_coil5)-(bpm12x_coil3/bpm4ax_coil3) = delta35_12x4ax
                  // delta13 = (bpm12x_coil3/bpm4ax_coil3)-(bpm12x_coil1/bpm4ax_coil1) = delta13_12x4ax
                  // delta26 = (bpm12x_coil6/bpm4ay_coil6)-(bpm12x_coil2/bpm4ay_coil2) = delta26_12y4ay
                  // delta46 = (bpm12x_coil6/bpm4ay_coil6)-(bpm12x_coil4/bpm4ay_coil4) = delta46_12y4ay
                  // delta24 = (bpm12x_coil4/bpm4ay_coil4)-(bpm12x_coil2/bpm4ay_coil2) = delta24_12y4ay
                }
                else {
                  localAlphas[icoil1][icoil2][ibpm1][ibpm2] = 0.0;
                }
                if ( localBPMsens[icoil1][ibpm2] != 0 && localBPMsens[icoil2][ibpm2] != 0 ) {
                  localDeltas[icoil1][icoil2][ibpm1][ibpm2] = (localBPMsens[icoil1][ibpm1]/localBPMsens[icoil1][ibpm2])-(localBPMsens[icoil2][ibpm1]/localBPMsens[icoil2][ibpm2]);
                }
                else {
                  localDeltas[icoil1][icoil2][ibpm1][ibpm2] = 0.0;
                }
                if ( localAlphas[icoil1][icoil2][ibpm1][ibpm2] != 0 ){
                  localBetas[icoil1][icoil2][ibpm1][ibpm2] = localDeltas[icoil1][icoil2][ibpm1][ibpm2]/localAlphas[icoil1][icoil2][ibpm1][ibpm2];
                }
                else {
                  localBetas[icoil1][icoil2][ibpm1][ibpm2] = 0;
                }
              }
            }
          }
        }
        if(cycleNum>1){
          //cout<<"cyc="<<cycleNum<<" flag="<<flag<<endl;
          dit_tree->Fill();
        }
      }
    }
  }
  if(dit_tree->GetEntries()==1) {
    dit_tree->Write();
  }
  else {
    dit_tree->Write(0,TObject::kOverwrite);
  }
  ditfile->Close();
}

int bmodAna(Int_t runNo = 4199, std::string inputFile = "input.txt", TString sluglist = "NULL"){
  Printf("Starting BMOD Analysis");
  TStopwatch tsw;
  tsw.Start();
  if (runNo < 1000 && sluglist != "NULL") { // Assume its a slug
    std::string runnum;
    ifstream ifinfile(sluglist);
    if (ifinfile.is_open()){
      while(getline(ifinfile,runnum)){
        BMOD * bmod = new BMOD();
        bmod->parseTextFile(inputFile);
        bmod->getData(std::atoi(runnum.c_str()));
        bmod->calculateSensitivities();
        bmod->saveSensitivityData();
        bmod->invertMatrix();
        bmod->saveSlopeData();
        bmod->edittree(bmod->slopeFilename);
        // FIXME also append this run's data into the combo/slugwise input file
      }
    }
  }
  else {
    BMOD * bmod = new BMOD();
    bmod->parseTextFile(inputFile);
    bmod->getData(runNo);
    bmod->calculateSensitivities();
    bmod->saveSensitivityData();
    bmod->invertMatrix();
    bmod->saveSlopeData();
    bmod->edittree(bmod->slopeFilename);
  }
  cout << "-- BMOD Analysis done in "; tsw.Print(); cout << endl;
  return 0;
}
