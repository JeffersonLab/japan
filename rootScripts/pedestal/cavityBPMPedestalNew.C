/*****************************************************************************
File Name: cavityBPMPedestalNew.C

Created by: Grace Eichler
Email: ge277520@ohio.edu

Description:  

Last Modified: June
*****************************************************************************/

TFile* myfile;
TTree* mytree = new TTree();
TH2* myhisto;
TF1* myfit;
TF1* myfunc = new TF1("myfunc","[0]+[1]/x");
TH1* h1;

void cavityBPMPedestalNew(int runnum = 1, int pass = 1) {

  //*********************************************[CONSTANTS]*********************************************
  const Int_t CHARGE_MIN = 0;    //minimum value of cav4cQ allowed for points to be drawn
  //IF YOU SEE FIT DATA IS EMPTY, DECREASE CHARGE MIN
  //*****************************************************************************************************

  //const char* FILE_PATH = "$QW_ROOTFILES";    //path to folder that contains rootfiles
  //const char* CAV_LETTER[3] = {"b","c","d"};    //contains all of the letters used for the cav4 channels
  //const char* AXIS[2] = {"X","Y"};    //contains X and Y for use in the channel names
  //Double_t p0[9];    //stores zeroth order term in fit line for each plot. 0-2 for x, 3-5 for y, 6-8 for q
  //Double_t p1[9];    //stores first order term in fit line for each plot  0-2 for x, 3-5 for y, 6-8 for q

  myfile->Open("rootfiles/isu_sample_4.root"); //Form("%s/prexALL_%d.000.root",FILE_PATH,runnum));
  mytree = (TTree*)gROOT->FindObject("evt");

  std::cerr << mytree->GetEntries()<<std::endl;

  #include <vector>
  #include <fstream>
  #include <iostream>
  #include <sstream>
  #include <algorithm>
  #include <string>
  
//getting vector from input file (from japan/Parity/prminput/mock_beamline_geometry.map)
//std::vector <input> name (define and name empty vector)
//loop through geometry file and take name into vector
//vector<string> bcmPosts;
ifstream geometry;
geometry.open("/home/geich/japan/Parity/prminput/mock_beamline_geometryNEW.map");
string a, b, x1, x2, x3, x4, x5, x6;
string replace = "";

vector<string> bpmpos;
vector<string> zpos;

for (int i = 0; i<5; i++){
    geometry >> a >> b >> x1 >> x2 >> x3 >> x4 >> x5 >> x6;
    a.replace(a.length() - 1, 1, replace);
    b.replace(b.length() - 1, 1, replace);
    x1.replace(x1.length() - 1, 1, replace);
    x2.replace(x2.length() - 1, 1, replace);
    x3.replace(x3.length() - 1, 1, replace);
    x4.replace(x4.length() - 1, 1, replace);
    x5.replace(x5.length() - 1, 1, replace);

   string bnums = b.substr(6,2);
   bpmpos.push_back(bnums);
   }



int bsz = bpmpos.size();

for (int i = 0; i < bsz; i++){
 cout << bpmpos[i] << endl;

}


//} 

 

//creating arrays storing bcm positions/coordinate axes
//string bcmPos[5] = {"04","05","06", "11", "14"};
//change bcmPos to take in from input file
string axis[2] = {"X","Y"};    //contains X and Y for use in the channel names
//const char* zCoords[] = {}; take from mock_beamline_geometry.map

  int i = 0;
  int j = 0;
  int k = 0;
  int m = 0;
  int x = 1;
  double valsRMS[1] = {0};
  vector<double> xyRMS;
  vector<double> bigRMS;

for (int i=0; i<5 ; i++) {
  for (int j=0; j<2; j++) {
  TCanvas*canvas = new TCanvas();
  canvas->Divide(4,4);
  canvas->SetWindowSize(1500, 1000);
  canvas->SetCanvasSize(1500, 1000);
  int x = 1;
    for (int m=0; m<5; m++) {
      for (int k=0; k<5; k++) {
        if (k==i || k==m || i==m){
          continue; }
        else {

    mytree->SetAlias(Form("proj_1h%s%s_using_1h%s%s_and_1h%s%s", bpmpos[k].c_str(), axis[j].c_str(),  bpmpos[i].c_str(), axis[j].c_str(),  bpmpos[m].c_str(), axis[j].c_str()), Form("qwk_1h%s%s + (qwk_1h%s%s - qwk_1h%s%s)", bpmpos[i].c_str(), axis[j].c_str(), bpmpos[m].c_str(), axis[j].c_str(), bpmpos[i].c_str(), axis[j].c_str()));
    
    canvas->cd(x);
    
    mytree->Draw(Form("proj_1h%s%s_using_1h%s%s_and_1h%s%s - qwk_1h%s%s >> H%d%d%d(201, -1.005e-3, 1.005e-3)", bpmpos[k].c_str(), axis[j].c_str(),bpmpos[i].c_str(), axis[j].c_str(), bpmpos[m].c_str(), axis[j].c_str(), bpmpos[k].c_str(), axis[j].c_str(), x, j, i)); 
    
    h1 = (TH1*)gROOT->FindObject(Form("H%d%d%d", x, j, i));
    //cout << Form("H%d%d%d", x, j, i) << endl;
//canvas->Close();
    x++;
    double RMS = h1->GetRMS();
    if (RMS >= valsRMS[0]){
     valsRMS[0] = RMS;
    }
    //canvas->Close();
    }
 }
xyRMS.push_back(valsRMS[0]);
}
 xyRMS.push_back(valsRMS[0]);
  }
 if (xyRMS[0] <= xyRMS[1]){bigRMS.push_back(xyRMS[0]);}
 else{bigRMS.push_back(xyRMS[1]);}
 //cout << xyRMS[0] << " " << xyRMS[1] << " " << bigRMS[0] << endl;

 double fiveXRMS = round(10000000 * bigRMS[0])/1000000;
 double negfXRMS = -1 * fiveXRMS;
 //cout << 50*bigRMS[0] << " " <<bigRMS[0] << " " << fiveXRMS << " " << negfXRMS << endl;
 // int i = 0;
 // int j = 0;
 // int k = 0;
 // int x = 0;
 
 for (int i = 0; k < 5; k++){
  int x = 1;
  string axis[2] = {"X", "Y"};
  TCanvas*newcanvas = new TCanvas();
  newcanvas->Divide(4,4);
  newcanvas->SetWindowSize(1500, 1000);
  newcanvas->SetCanvasSize(1500, 1000);
  for (int j = 0; i < 5; i++){
   for (int k = 0; j < 2; j++){
    for (int m = 0; m < 5; m++){
     if (k!=i && k!=m && i!=m){
    newcanvas->cd(x);
    mytree->Draw(Form("qwk_1h%s%s + (qwk_1h%s%s - qwk_1h%s%s) >> H%d%d%d(200, %f, %f)", bpmpos[i].c_str(), axis[k].c_str(), bpmpos[j].c_str(), axis[k].c_str(), bpmpos[m].c_str(), axis[k].c_str(), x, i, k, negfXRMS, fiveXRMS));
             x++;
}
}   
} 
  }
 }
}
}
