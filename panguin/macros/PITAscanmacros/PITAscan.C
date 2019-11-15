//Caryns macro
void PITAscan(TString device="det1", TString ucut ="1", Int_t myii =0){
  gROOT->Reset();
gStyle->SetOptFit(0001);
 gStyle -> SetOptStat(0);
  // define style here 
  // general parameters 

   TPad *a1 = new TPad("a1", "a1",
  		    0,0,1,1);
     a1->Draw();

     //static Int_t myii;
     //new (Form(...",)) = 
     //Nr++
  TString *bpmNam13 = new TString(device);
  TString tmpname13;
  TString plotcommand13;

  TTree* p = (TTree*)gROOT->FindObject("mul");


     TString scut13 = "cleandata==1&&" + ucut;


     tmpname13 = bpmNam13->Data();
     TString titpre13 = "asym_";
     TString titsum13 = Form(" (ppm)  [%d]",myii);
          if (tmpname13.Contains("x") || tmpname13.Contains("y")) {
     //  plotcommand13 = "1e6*diff_";
     //  titpre13 = "1e6*diff_";
     //  titsum13 = " (nm)  ";
      plotcommand13 = "1e6*asym_";
     } else {
       plotcommand13 = "1e6*asym_";
       }
     plotcommand13 += tmpname13.Data();
     plotcommand13 += Form(":scandata1>>hAq%d",myii);
     //cout<<plotcommand13.Data()<<","<<scut13.Data()<<endl;
    a1->cd();
     p->Draw(plotcommand13.Data(),scut13.Data(),"prof");
    
      TH1D* thishist13;
      thishist13 = (TH1D*)gDirectory->FindObject(Form("hAq%d",myii));

      thishist13->Fit("pol1","Q");
     TF1 *f1_13 = thishist13->GetFunction("pol1");
    f1_13->SetLineColor(2);
    f1_13->SetLineWidth(1);

    thishist13->SetMarkerColor(12);
    thishist13->SetMarkerSize(1.0);
    thishist13->SetMarkerStyle(22);
    //thishist13->GetXaxis()->CenterTitle();
    //    thishist13->SetTitleOffset(1.2);
    // thishist13->SetTitleSize(0.06);
    thishist13->Draw();
    TString tit13 = titpre13.Data();
    tit13 += tmpname13.Data();
    tit13 += titsum13;
    //       thishist13->GetXaxis()->SetTitle(tit13.Data());
    thishist13->SetTitle(tit13.Data());

        delete bpmNam13,tmpname13, plotcommand13, scut13, titpre13, titsum13, thishist13, f1_13, tit13;

    //    myii++;
}


