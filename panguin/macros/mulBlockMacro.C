void mulBlockMacro(char* ch, int a, int b){

  mul->Draw(Form("%s.block%i-%s.block%i",ch,a,ch,b),
  	Form("ErrorFlag==0 && %s.Device_Error_Code == 0",ch));
  TH1* htemp = (TH1*)gROOT->FindObject("htemp");

  htemp->SetTitle(Form("%s Block %i - Block %i",ch,a,b));

  gStyle->SetStatH(0.35);
  gStyle->SetStatW(0.22);

  if (htemp != NULL) {
    htemp->SetNameTitle(Form("htemp%zx",(size_t)htemp),htemp->GetTitle());
    double n = (htemp->GetMean())/(htemp->GetMeanError());
    double m = htemp->GetRMS();

    double x = htemp->GetXaxis()->GetXmin();
    double y = htemp->GetMaximum();
    double ratio_x = x * 0.9;
    double ratio_y = y * 0.9;
    double ratioVal_x = x * 0.9;
    double ratioVal_y = y * 0.8;
    double rms_x = x * 0.9;
    double rms_y = y * 0.7;
    double rmsVal_x = x * 0.9;
    double rmsVal_y = y * 0.6;

    TText* ratio = new TText(ratio_x,ratio_y,Form("Mean/Error:"));
    TText* ratioVal = new TText(ratioVal_x,ratioVal_y,Form("%f",n));
    TText* rms = new TText(rms_x,rms_y,Form("RMS:"));
    TText* rmsVal = new TText(rmsVal_x,rmsVal_y,Form("%f",m));

    ratio->SetTextSize(.07);
    ratioVal->SetTextSize(.07);
    rms->SetTextSize(.07);
    rmsVal->SetTextSize(.07);
    ratio->Draw();
    ratioVal->Draw();
    rms->Draw();
    rmsVal->Draw();
  }

}
