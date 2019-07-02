/*****************************************************************************
File Name: cavityBPMPedestalMacro.C

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This macro is intended to calibrate the cavity BPMs relative
              to an already calibrated bpm4eX, bpm4eY, or bcm_an_ds3.

Last Modified: June 24, 2019
*****************************************************************************/

TFile* myfile;
TTree* mytree;
TH2* myhisto;
TF1* myfit;
TF1* myfunc = new TF1("myfunc","[0]+[1]/x");

void cavityBPMPedestalMacro(int runnum, int pass = 1) {

  //*********************************************[CONSTANTS]*********************************************
  const Int_t CHARGE_MIN = 0;    //minimum value of cav4cQ allowed for points to be drawn
  //IF YOU SEE FIT DATA IS EMPTY, DECREASE CHARGE MIN
  //*****************************************************************************************************

  const char* FILE_PATH = "$QW_ROOTFILES";    //path to folder that contains rootfiles
  const char* CAV_LETTER[3] = {"b","c","d"};    //contains all of the letters used for the cav4 channels
  const char* AXIS[2] = {"X","Y"};    //contains X and Y for use in the channel names
  Double_t p0[9];    //stores zeroth order term in fit line for each plot. 0-2 for x, 3-5 for y, 6-8 for q
  Double_t p1[9];    //stores first order term in fit line for each plot  0-2 for x, 3-5 for y, 6-8 for q

  myfile->Open(Form("%s/prexALL_%d.000.root",FILE_PATH,runnum));
  mytree = (TTree*)gROOT->FindObject("evt");

  mytree->SetAlias("X_cav_b","bpm4aX +(bpm4eX-bpm4aX)*0.13424");
  mytree->SetAlias("Y_cav_b","bpm4aY +(bpm4eY-bpm4aY)*0.19261");
  mytree->SetAlias("X_cav_c","bpm4aX +(bpm4eX-bpm4aX)*0.42607");
  mytree->SetAlias("Y_cav_c","bpm4aY +(bpm4eY-bpm4aY)*0.48444");
  mytree->SetAlias("X_cav_d","bpm4aX +(bpm4eX-bpm4aX)*0.72763");
  mytree->SetAlias("Y_cav_d","bpm4aY +(bpm4eY-bpm4aY)*0.78405");

  //loop over all cav4Q plots
  if (pass == 1) {
    for (size_t i = 6; i < 9; ++i) {

      //draw inital plot
      mytree->Draw(Form("cav4%sQ.hw_sum_raw/cav4%sQ.num_samples:bcm_an_ds3>>myhisto",CAV_LETTER[i%3],CAV_LETTER[i%3]),
                        Form("ErrorFlag==0 && bcm_an_ds3>%d",CHARGE_MIN));
      myhisto = (TH2*)gROOT->FindObject("myhisto");
      //fit plot
      myhisto->Fit("pol1");
      //load fit line parameters into array
      myfit = myhisto->GetFunction("pol1");
      p0[i] = myfit->GetParameter(0);
      p1[i] = myfit->GetParameter(1);
      myhisto->Delete();

    }
  }

  //loop over all cav4XI and cav4YI plots
  else if (pass == 2) {
    for(size_t i = 0; i < 6; ++i) {
      //draw first plot
      mytree->Draw(Form("(cav4%s%sI.hw_sum_raw/cav4%s%sI.num_samples)/cav4%sQ:cav4%sQ>>myhisto",CAV_LETTER[i%3],AXIS[i/3],
                        CAV_LETTER[i%3],AXIS[i/3],CAV_LETTER[i%3],CAV_LETTER[i%3]),
                        Form("ErrorFlag==0 && bcm_an_ds3>%d",CHARGE_MIN));
      myhisto = (TH2*)gROOT->FindObject("myhisto");
      //fit plot
      myhisto->Fit("myfunc");
      //load fit line parameters into array
      myfit = myhisto->GetFunction("myfunc");
      p0[i] = myfit->GetParameter(1); //Due to the way this code was changed to function, p1 of this function is stored in p0
      myhisto->Delete();

      //draw second plot
      mytree->Draw(Form("((cav4%s%sI.hw_sum_raw/cav4%s%sI.num_samples)-%f)/cav4%sQ:%s_cav_%s>>myhisto",CAV_LETTER[i%3],AXIS[i/3],
                        CAV_LETTER[i%3],AXIS[i/3],p0[i],CAV_LETTER[i%3],AXIS[i/3],CAV_LETTER[i%3]),
                        Form("ErrorFlag==0 && bcm_an_ds3>%d",CHARGE_MIN));
      myhisto = (TH2*)gROOT->FindObject("myhisto");
      //fit plot
      myhisto->Fit("pol1");
      //load fit line parameters into array
      myfit = myhisto->GetFunction("pol1");
      p1[i] = myfit->GetParameter(1);  //store p1 of this function in p1 so that both p1s can be used at the same time
      myhisto->Delete();
    }
  }

  //print all fit line parameters to screen in format of map file
  if (pass == 1) {
    cout << endl;
    cout << "Replace the following lines in the pedestal mapfile:" << endl; 
    cout << endl;
    cout << "  cav4bq,   " << p0[6] << ",    " << 1/p1[6] << endl;
    cout << endl;
    cout << "  cav4cq,   " << p0[7] << ",    " << 1/p1[7] << endl;
    cout << endl;
    cout << "  cav4dq,   " << p0[8] << ",    " << 1/p1[8] << endl;
    cout << endl;
    cout << "After this change is made, run qwparity again on the same run number." << endl
         << "Then, run this macro again with 2 as a parameter after the run number." << endl << endl;
    cout << "Example: cavityBPMPedestalMacro(1000,2)" << endl << endl;
  }
  else if (pass == 2) {
    cout << endl;
    cout << "Replace the following lines in the pedestal mapfile:" << endl; 
    cout << endl;
    cout << "  cav4bxi,  " << p0[0] << ",    " << 1/p1[0] << endl;
    cout << "  cav4byi,  " << p0[3] << ",    " << 1/p1[3] << endl;
    cout << endl;
    cout << "  cav4cxi,  " << p0[1] << ",    " << 1/p1[1] << endl;
    cout << "  cav4cyi,  " << p0[4] << ",    " << 1/p1[4] << endl;
    cout << endl;
    cout << "  cav4dxi,  " << p0[2] << ",    " << 1/p1[2] << endl;
    cout << "  cav4dyi,  " << p0[5] << ",    " << 1/p1[5] << endl;
    cout << endl;
    cout << "Run qwparity again and the calibration will be complete" << endl << endl;
  }

  if (pass != 1 && pass != 2) {
    cout << endl << "Invalid Pass Number Provided" << endl << endl;
  }

}
