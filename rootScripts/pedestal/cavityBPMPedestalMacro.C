/*****************************************************************************
File Name: cavityBPMPedestalMacro.C

Created by: Michael Vallee
Email: mv836315@ohio.edu

Description:  This macro is intended to calibrate the cavity BPMs relative
              to an already calibrated bpm4eX, bpm4eY, or bcm_an_ds3.

Last Modified: June 12, 2019
*****************************************************************************/

TFile* myfile;
TTree* mytree;
TH2* myhisto;
TF1* myfit;

void cavityBPMPedestalMacro(int runnum, bool docharge = false) {

  const char* FILE_PATH = "/chafs1/work1/apar/mrvallee/rootFiles";    //path to folder the contains rootfiles
  const char* CAV_LETTER[3] = {"b","c","d"};    //contains all of the letters used for the cav4 channels
  const char* AXIS[2] = {"X","Y"};    //contains X and Y for use in the channel names
  const Int_t CHARGE_MIN = 600;    //minimum value of cav4cQ allowed for points to be drawn
  const Int_t TOLERANCE = 10;    //maximum absolute distance from residual ploted for subsequent fits
  Double_t p0[9];    //stores zeroth order term in fit line for each plot. 0-2 for x, 3-5 for y, 6-8 for q
  Double_t p1[9];    //stores first order term in fit line for each plot  0-2 for x, 3-5 for y, 6-8 for q

  myfile->Open(Form("%s/prexALL_%d.root",FILE_PATH,runnum));
  mytree = (TTree*)gROOT->FindObject("evt");

  //loop over all cav4XI and cav4YI plots
  for(size_t i = 0; i < 6; ++i) {
    //draw inital plot
    mytree->Draw(Form("cav4%s%sI.hw_sum_raw/cav4%s%sI.num_samples:bpm4e%s>>myhisto",CAV_LETTER[i%3],AXIS[i/3],
                      CAV_LETTER[i%3],AXIS[i/3],AXIS[i/3]),Form("ErrorFlag==0 && cav4cQ>%d",CHARGE_MIN));
    myhisto = (TH2*)gROOT->FindObject("myhisto");
    //fit plot
    myhisto->Fit("pol1");
    //load fit line parameters into array
    myfit = myhisto->GetFunction("pol1");
    p0[i] = myfit->GetParameter(0);
    p1[i] = myfit->GetParameter(1);
    myhisto->Delete();
    //redraw plot with condition that points have to be close to original fit line
    mytree->Draw(Form("cav4%s%sI.hw_sum_raw/cav4%s%sI.num_samples:bpm4e%s>>myhisto",CAV_LETTER[i%3],AXIS[i/3],
                       CAV_LETTER[i%3],AXIS[i/3],AXIS[i/3]),
                       Form("ErrorFlag==0 && cav4cQ>%d && abs(cav4%s%sI.hw_sum_raw/cav4%s%sI.num_samples - (bpm4e%s*%f + %f))<%d",
                             CHARGE_MIN,CAV_LETTER[i%3],AXIS[i/3],CAV_LETTER[i%3],AXIS[i/3],AXIS[i/3],p1[i],p0[i],TOLERANCE));
    myhisto = (TH2*)gROOT->FindObject("myhisto");
    //fit plot again
    myhisto->Fit("pol1");
    //overwrite old fit line parameters with new fit line parameters
    myfit = myhisto->GetFunction("pol1");
    p0[i] = myfit->GetParameter(0);
    p1[i] = myfit->GetParameter(1);
    myhisto->Delete();
    //repeat the same process as above to get more accurate fit
    mytree->Draw(Form("cav4%s%sI.hw_sum_raw/cav4%s%sI.num_samples:bpm4e%s>>myhisto",CAV_LETTER[i%3],AXIS[i/3],
                       CAV_LETTER[i%3],AXIS[i/3],AXIS[i/3]),
                       Form("ErrorFlag==0 && cav4cQ>%d && abs(cav4%s%sI.hw_sum_raw/cav4%s%sI.num_samples - (bpm4e%s*%f + %f))<%d",
                             CHARGE_MIN,CAV_LETTER[i%3],AXIS[i/3],CAV_LETTER[i%3],AXIS[i/3],AXIS[i/3],p1[i],p0[i],TOLERANCE));
    myhisto = (TH2*)gROOT->FindObject("myhisto");
    myhisto->Fit("pol1");
    myfit = myhisto->GetFunction("pol1");
    p0[i] = myfit->GetParameter(0);
    p1[i] = myfit->GetParameter(1);
    myhisto->Delete();
  }

  //loop over all cav4Q plots
  if (docharge == true) {
    for (size_t i = 6; i < 9; ++i) {

      //draw inital plot
      mytree->Draw(Form("cav4%sQ.hw_sum_raw/cav4%sQ.num_samples:bcm_an_ds3>>myhisto",CAV_LETTER[i%3],CAV_LETTER[i%3]),
                        Form("ErrorFlag==0 && cav4cQ>%d",CHARGE_MIN));
      myhisto = (TH2*)gROOT->FindObject("myhisto");
      //fit plot
      myhisto->Fit("pol1");
      //load fit line parameters into array
      myfit = myhisto->GetFunction("pol1");
      p0[i] = myfit->GetParameter(0);
      p1[i] = myfit->GetParameter(1);
      myhisto->Delete();
      //redraw plot with condition that points have to be close to original fit line
      mytree->Draw(Form("cav4%sQ.hw_sum_raw/cav4%sQ.num_samples:bcm_an_ds3>>myhisto",CAV_LETTER[i%3],CAV_LETTER[i%3]),
                        Form("ErrorFlag==0 && cav4cQ>%d && abs(cav4%sQ.hw_sum_raw/cav4%sQ.num_samples - (bcm_an_ds3*%f + %f))<%d",
                             CHARGE_MIN,CAV_LETTER[i%3],CAV_LETTER[i%3],p1[i],p0[i],TOLERANCE));
      myhisto = (TH2*)gROOT->FindObject("myhisto");
        //fit plot again
      myhisto->Fit("pol1");
      //overwrite old fit line parameters with new fit line parameters
      myfit = myhisto->GetFunction("pol1");
      p0[i] = myfit->GetParameter(0);
      p1[i] = myfit->GetParameter(1);
      myhisto->Delete();
      //repeat the same process as above to get more accurate fit
      mytree->Draw(Form("cav4%sQ.hw_sum_raw/cav4%sQ.num_samples:bcm_an_ds3>>myhisto",CAV_LETTER[i%3],CAV_LETTER[i%3]),
                        Form("ErrorFlag==0 && cav4cQ>%d && abs(cav4%sQ.hw_sum_raw/cav4%sQ.num_samples - (bcm_an_ds3*%f + %f))<%d",
                             CHARGE_MIN,CAV_LETTER[i%3],CAV_LETTER[i%3],p1[i],p0[i],TOLERANCE));
      myhisto = (TH2*)gROOT->FindObject("myhisto");
      //fit plot again
      myhisto->Fit("pol1");
      //overwrite old fit line parameters with new fit line parameters
      myfit = myhisto->GetFunction("pol1");
      p0[i] = myfit->GetParameter(0);
      p1[i] = myfit->GetParameter(1);
      myhisto->Delete();

    }
  }

  //print all fit line parameters to screen in format of map file
  if (docharge == true) {
    cout << endl;
    cout << "  cav4bq,   " << p0[6] << ",     " << 1/p1[6] << endl;
    cout << "  cav4bxi,  " << p0[0] << ",    " << 1/p1[0] << endl;
    cout << "  cav4byi,  " << p0[3] << ",    " << 1/p1[3] << endl;
    cout << endl;
    cout << "  cav4cq,   " << p0[7] << ",     " << 1/p1[7] << endl;
    cout << "  cav4cxi,  " << p0[1] << ",    " << 1/p1[1] << endl;
    cout << "  cav4cyi,  " << p0[4] << ",    " << 1/p1[4] << endl;
    cout << endl;
    cout << "  cav4dq,   " << p0[8] << ",     " << 1/p1[8] << endl;
    cout << "  cav4dxi,  " << p0[2] << ",    " << 1/p1[2] << endl;
    cout << "  cav4dyi,  " << p0[5] << ",    " << 1/p1[5] << endl;
    cout << endl;
  }
  else {
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
  }

}
