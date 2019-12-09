void plot_element(TPad *p1, TPad* p2, TString devnam, TString lcut, Int_t dset);

void PITAscan_Caryn(Int_t dset=1,TString device="det1", TString ucut ="1"){
  gROOT->Reset();
  //  gROOT->LoadMacro("macro/open.macro");  
  
    TString scantype;

    TString gtitle;
    TString pstit;

    
    gtitle = "PITA Scan, Run ";
    gtitle += dset;
    pstit = "PITAscan_run";
    pstit += dset;
    
  // define style here 
  // general parameters
  gStyle->SetOptDate(0);     gStyle->SetOptTitle(0);
  gStyle->SetStatColor(10);  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.3);     gStyle->SetOptStat(0); 

  gStyle->SetPaperSize(12,15);  
  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);
  // pads parameters
  //  gStyle->SetPadColor(39); 
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gROOT->ForceStyle();  

  TPad* a1_p[7];

  TCanvas *a1 = new TCanvas("a1", "Scan",
			    0,0,900,600);



  a1_p[0]  = new TPad( "a1_1", "a1_1",0.00,0.31,0.75,0.92);    
 a1_p[1]  = new TPad( "a1_7", "a1_7",0.75,0.31,1.00,0.92);    

  a1_p[2]  = new TPad( "a1_1", "a1_1",0.00,0.31,0.75,0.30);    
  a1_p[3]  = new TPad( "a1_7", "a1_7",0.75,0.31,1.00,0.3);    

  a1_p[4]  = new TPad( "a1_1", "a1_1",0.00,0.01,0.75,0.30);    
  a1_p[5]  = new TPad( "a1_7", "a1_7",0.75,0.01,1.00,0.30);    

  TPad* a1_title = new TPad("a1_title","a1_title",0.0,0.92,1.0,1.0);    

  a1->cd();
  for (Int_t i =0; i<6; i++) a1_p[i]->Draw();
  a1_title->Draw();


  a1_title->cd();
  TPaveText* pt2 = new TPaveText(0.04,0.04,0.96,0.90,"brNDC");
  pt2->SetBorderSize(1);pt2->SetFillColor(10);
  pt2->SetTextFont(20);
  pt2->SetTextSize(0.55);
  pt2->AddText(0.5,0.7,gtitle.Data());
  pt2->Draw();


  plot_element(a1_p[0],a1_p[1],""+device,ucut,dset);
  // plot_element(a1_p[0],a1_p[1],"bcm1",ucut,dset);

  // plot_element(a1_p[2],a1_p[3],""+device+"x",ucut,dset);

  //  plot_element(a1_p[4],a1_p[5],""+device+"y",ucut,dset);

  a1->cd();
  TString psnam = "/adaqfs/home/apar/PREX/japan/plots/parityOutputs/";
  psnam += pstit.Data();
  psnam += ".png";
  a1->Print(psnam.Data());
  
}

void plot_element(TPad *p1, TPad* p2, TString devnam, TString lcut, 
		  Int_t dset) 
{

  TString *bpmNam = new TString(devnam);

  Int_t ifnd = 0;
  TString tmpname;
  TString plotcommand;

  Int_t icurRun=0;
  char buff[170];

    Int_t irun = dset;
    //   if (irun!=icurRun) {
      // Int_t run = open(irun,"standard");
      
      //TTree *p = (TTree*)gROOT->FindObject("P");
      //TTree *r = (TTree*)gROOT->FindObject("R");

   TFile *f = TFile::Open(Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",irun));
   TTree* p=(TTree*)f->Get("mul");
  TTree* r=(TTree*)f->Get("evt");

  //    } 

    //    if(dset==1314) lcut="m_ev_num<11700";

    // make cut
    TString scut = "cleandata==1&&" + lcut;


    // plot desired  Aq
    tmpname = bpmNam->Data();
    //cout<<tmpname.Data()<<endl;
    TString titpre = "asym_";
    //    TString titpre = "asym_";
    TString titsum = " (ppm) vs Scandata1";
    //    tmpname += "1sum";
    if (tmpname.Contains("x") || tmpname.Contains("y")) {
      plotcommand = "1e6*diff_";
      titpre = "1e6*diff_";
      titsum = " (nm) vs Scandata1";
    } else {
      plotcommand = "1e6*asym_";
      }
    plotcommand += tmpname.Data();
    plotcommand += ":scandata1>>hAq";
    cout<<plotcommand.Data()<<","<<scut.Data()<<endl;
    p1->cd();
    p->Draw(plotcommand.Data(),scut.Data(),"prof");

     TH1D* thishist;
    //    objnam = Form("hista%d",dd);
    thishist = (TH1D*)gDirectory->FindObject("hAq");

    // fit Aq linearity
    thishist->Fit("pol1");
    TF1 *f1 = thishist->GetFunction("pol1");
    f1->SetLineColor(2);
    f1->SetLineWidth(1);

    thishist->SetMarkerColor(12);
    thishist->SetMarkerSize(1.0);
    thishist->SetMarkerStyle(22);
    thishist->GetXaxis()->CenterTitle();
    thishist->GetXaxis()->SetTitleOffset(1.2);
    thishist->GetXaxis()->SetTitleSize(0.06);
    thishist->Draw();
    TString tit = titpre.Data();
    tit += tmpname.Data();
    tit += titsum;
    thishist->GetXaxis()->SetTitle(tit.Data());

    Int_t factor;
    p2->cd();
    char linetxt[50];
    if (tmpname.Contains("x") || tmpname.Contains("y")) {
      factor = 1000;
    } else factor = 1;
    sprintf(linetxt,"A = %7.2f + ",f1->GetParameter(0)*factor);
    TString funcstr = linetxt;
    sprintf(linetxt,"          %7.2f * x", f1->GetParameter(1)*factor);
    TString funcstr2 = linetxt;
    TPaveText* pt = new TPaveText(0.0,0.3,0.9,0.7,"brNDC");
    pt->SetBorderSize(1);pt->SetFillColor(10);
    pt->SetTextAlign(12);pt->SetTextFont(22);
    //     pt->SetTextSize(0.06);
    pt->AddText(funcstr.Data());
    pt->AddText(funcstr2.Data());
    pt->Draw();

}

