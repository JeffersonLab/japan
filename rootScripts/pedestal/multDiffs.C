/*****************************************************************************
File name: multDiffs.C

Created by: Grace Eichler
Email: ge277520@ohio.edu

Description: Cycles through beamline positions in file 
mock_beamline_geometryNEW.C and subtracts the diff values of each position
from each other. Plots resultant histograms.

Last Modified: 12 June 2023
*****************************************************************************/

TFile* myfile1;
TTree* mytree1 = new TTree();
TH1* h1;// = new TH1D("h", "h", 200, -0.001, 0.001);

void multDiffs(){
 myfile1->Open("rootfiles/isu_sample_4.root");
 mytree1 = (TTree*)gROOT->FindObject("mul");

 //take beamlines from file and put names into vectors bpmpos
 fstream geometry;
 geometry.open("/home/geich/japan/Parity/prminput/mock_beamline_geometryNEW.map");
 string a, b, x1, x2, x3, x4, x5, x6;
 string replace = "";

 //vector to store beamline positions
 vector<string> bpmpos;
 
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
    
 // declaring integers to use as loop variables
 int i = 0;
 int j = 0;
 int k = 0;
 int l = 0;
 double valsRMS[1] = {0};
 vector<double> xyRMS;
 vector<double> bigRMS;
 //loop through beamlines to subtract and plot
 for (int k = 0; k < 2; k++){
  int x = 1;
  string axis[2] = {"X", "Y"};
  TCanvas*canvas = new TCanvas();
  canvas->Divide(3,4);
  canvas->SetWindowSize(1500, 1000);
  canvas->SetCanvasSize(1500, 1000);
//  double valsRMS[1] = {0};
 // vector<double> xyRMS; 
  for (int i = 0; i < 5; i++){
   for (int j = i+1; j < 5; j++){
    canvas->cd(x);
    mytree1->Draw(Form("diff_qwk_1h%s%s - diff_qwk_1h%s%s >> H%d%d%d(200, -0.005, 0.005)", bpmpos[i].c_str(), axis[k].c_str(), bpmpos[j].c_str(), axis[k].c_str(), x, i, k));
    
    
    h1 = (TH1*)gROOT->FindObject(Form("H%d%d%d", x, i, k)); 
    //cout << Form("H%d%d%d", x, i, k) << ":" << h1 << endl;
    //h1->Draw(Form("diff_qwk_1h%s%s - diff_qwk_1h%s%s)", bpmpos[i].c_str(), axis[k].c_str(), bpmpos[j].c_str(), axis[k].c_str()));
    //h1->SetBinContent(mytree1->diff);
    x++;
    double RMS = h1->GetRMS();
    //double valsRMS[1] = {0};
    if (RMS >= valsRMS[0]){
     valsRMS[0] = RMS;
    } 
    //cout << valsRMS[0] << endl;
  // store first RMS value
  // if (RMS >= (stored RMS)
  //   rmsName = rmsCurrent
  // replace initial RMS
     
  //Remake histograms using 5x greatest rms as bounds
  //draw
    }
   }
  //cout << valsRMS[0] << endl;
  xyRMS.push_back(valsRMS[0]);
  }
 if (xyRMS[0] <= xyRMS[1]){bigRMS.push_back(xyRMS[0]);} 
 else{bigRMS.push_back(xyRMS[1]);}
 cout << xyRMS[0] << " " << xyRMS[1] << " " << bigRMS[0] << endl;
 //string times = strtod(5 * bigRMS[0]);
 //string round = Form(times, "%.2f");
 double fiveXRMS = round(10000000 * bigRMS[0])/1000000;
 double negfXRMS = -1 * fiveXRMS;
 cout << 50*bigRMS[0] << " " <<bigRMS[0] << " " << fiveXRMS << " " << negfXRMS << endl; 
// int i = 0;
// int j = 0;
// int k = 0;
// int x = 0;

 for (int k = 0; k < 2; k++){
  int x = 1;
  string axis[2] = {"X", "Y"};
  TCanvas*canvas = new TCanvas();
  canvas->Divide(3,4);
  canvas->SetWindowSize(1500, 1000);
  canvas->SetCanvasSize(1500, 1000);
   for (int i = 0; i < 5; i++){
    for (int j = i+1; j < 5; j++){
     canvas->cd(x);
     mytree1->Draw(Form("diff_qwk_1h%s%s - diff_qwk_1h%s%s >> H%d%d%d(200, %f, %f)", bpmpos[i].c_str(), axis[k].c_str(), bpmpos[j].c_str(), axis[k].c_str(), x, i, k, negfXRMS, fiveXRMS));
     x++;
   }
  }
 }

}
