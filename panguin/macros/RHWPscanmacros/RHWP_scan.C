
void RHWP_scan(TString device="bpm1i02", TString ucut ="1",Int_t myii=0) {
	gROOT->Reset();

  TPad *p1 = new TPad("p1", "p1",
  		    0,0,1,1);
     p1->Draw();
     p1->Divide(1,3);
     
	// define style here 
	// general parameters
	gStyle->SetOptDate(0);     gStyle->SetOptTitle(0);
	gStyle->SetStatColor(10);  gStyle->SetStatH(0.2);
	gStyle->SetStatW(0.3);     gStyle->SetOptStat(0); 
  

	//	TString finalcut = "cleandata==1 && ";
	//finalcut += ucut;

  
	TString *bpmNam = new TString(device);

	TString tmpname;
	TString plotcommand;
	Double_t aqpar[5];
	Double_t dxpar[5];
	Double_t dypar[5];


	// Open the file
  TTree* p = (TTree*)gROOT->FindObject("mul");
	// make cut
  TString scut = Form("cleandata==1 && %s",ucut.Data());

    // plot A_Q
    tmpname = bpmNam->Data();
	plotcommand = "1e6*asym_";
		plotcommand += tmpname.Data();
	plotcommand += "WS";
	plotcommand += Form(":scandata1*180./4000>>hAq%d",myii);
    p1->cd(1);
    p->SetMarkerStyle(7);
    p->SetMarkerColor(kBlue);
    p->SetLineColor(kBlue);
    p->Draw(plotcommand.Data(),scut.Data(),"prof");
    TH1F *hAq=(TH1F*)gDirectory->Get(Form("hAq%d",myii));
    // fit Aq vs theta
    TF1 *f1 = new TF1("f1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0.0,360.0);
    f1->SetLineColor(2);
    f1->SetLineWidth(1);
	f1->SetParLimits(2,-3.14159,3.14159);
	f1->SetParLimits(4,-3.14159,3.14159);
    // f2 and f4 are functions for plotting the 2theta and 4theta components of the fit.
    TF1 *f2 = new TF1("f2","[0]*sin(2*3.14159*x/180.+[1])",0.0,360.0);
    f2->SetLineColor(13);
    f2->SetLineWidth(1);
    f2->SetLineStyle(2);
    TF1 *f4 = new TF1("f4","[0]*sin(4*3.14159*x/180.+[1])",0.0,360.0);
    f4->SetLineColor(13);
    f4->SetLineWidth(1);
    f4->SetLineStyle(2);

    hAq->Fit("f1","QR");
    Double_t rectphs = f1->GetParameter(2);
    while (rectphs>3.14159) rectphs-=2*3.14159;
    while (rectphs<-3.14159) rectphs+=2*3.14159;
    if (rectphs<0) { 
		rectphs+=3.14159;
		f1->SetParameter(1,-1*f1->GetParameter(1));
    }
	f1->SetParameter(2,rectphs);
    rectphs = f1->GetParameter(4);
    while (rectphs>3.14159) rectphs-=2*3.14159;
    while (rectphs<-3.14159) rectphs+=2*3.14159;
    if (rectphs<0) { 
		rectphs+=3.14159;
		f1->SetParameter(3,-1*f1->GetParameter(3));
    }
    f1->SetParameter(4,rectphs);
    hAq->Fit("f1","QR");

    f2->SetParameters(f1->GetParameter(1),f1->GetParameter(2));
    f2->DrawCopy("same");
    f4->SetParameters(f1->GetParameter(3),f1->GetParameter(4));
    f4->DrawCopy("same");
    f1->Draw("same");
    hAq->GetXaxis()->CenterTitle();
    hAq->GetXaxis()->SetTitleSize(0.05)  ;
  hAq->GetYaxis()->SetTitleSize(0.065);
    TString tit = bpmNam->Data();
    tit += ", Asymmetry vs.  #theta";
    hAq->GetYaxis()->SetTitle(tit.Data());


    Double_t xmin, xmax;
    TLine *t1 = new TLine(xmin,0.0,xmax,1.0);
    xmin = f1->GetXaxis()->GetXmin();
    xmax = f1->GetXaxis()->GetXmax();
    t1->DrawLine(xmin,0,xmax,0);


    char linetxt[50];
    sprintf(linetxt,"Aq = %7.2f + ",f1->GetParameter(0));
    TString funcstr = linetxt;
    sprintf(linetxt," %7.2f sin (2#theta +%7.2f) +",
			f1->GetParameter(1),f1->GetParameter(2)*180.0/3.14159);
    funcstr += linetxt;
    sprintf(linetxt," %7.2f sin (4#theta +%7.2f)  (ppm)",
			f1->GetParameter(3),f1->GetParameter(4)*180.0/3.14159);
    funcstr += linetxt;
      hAq->GetXaxis()->SetTitle(funcstr.Data());

    for (Int_t i = 0; i<5; i++) {
		aqpar[i]=f1->GetParameter(i);
    }


  
    // plot Dx
    tmpname = bpmNam->Data();
    plotcommand = "diff_";
    plotcommand += tmpname.Data();
    plotcommand += "X";
    plotcommand += Form("*1e6:scandata1*180./4000 >>hDx%d",myii);
    p1->cd(2);
    p->Draw(plotcommand.Data(),scut.Data(),"prof");
    // fit Dx vs theta
    f1 = new TF1("f1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0.0,360.0);
    f1->SetLineColor(2);
    f1->SetLineWidth(1);

    TH1F *hDx=(TH1F*)gDirectory->Get(Form("hDx%d",myii));
	hDx->Fit("f1","QR");
    rectphs = f1->GetParameter(2);
    while (rectphs>3.14159) rectphs-=2*3.14159;
    while (rectphs<-3.14159) rectphs+=2*3.14159;
    if (rectphs<0) { 
		rectphs+=3.14159;
		f1->SetParameter(1,-1*f1->GetParameter(1));
    }
    f1->SetParameter(2,rectphs);
    rectphs = f1->GetParameter(4);
    while (rectphs>3.14159) rectphs-=2*3.14159;
    while (rectphs<-3.14159) rectphs+=2*3.14159;
    if (rectphs<0) { 
		rectphs+=3.14159;
		f1->SetParameter(3,-1*f1->GetParameter(3));
    }
    f1->SetParameter(4,rectphs);
    hDx->Fit("f1","QR");

    f2->SetParameters(f1->GetParameter(1),f1->GetParameter(2));
    f2->DrawCopy("same");
    f4->SetParameters(f1->GetParameter(3),f1->GetParameter(4));
    f4->DrawCopy("same");
    f1->Draw("same");
    hDx->GetXaxis()->CenterTitle();
    //    hDx->GetXaxis()->SetTitleOffset(1.2);
    hDx->GetXaxis()->SetTitleSize(0.05);
    hDx->GetYaxis()->SetTitleSize(0.065);
    tit = bpmNam->Data();
    tit += ",  Dx/2 vs. #theta";
    hDx->GetYaxis()->SetTitle(tit.Data());
    
    xmin = f1->GetXaxis()->GetXmin();
    xmax = f1->GetXaxis()->GetXmax();
    t1->DrawLine(xmin,0,xmax,0);

    //print fit eqn to canvas   
    sprintf(linetxt,"Dx/2 = %7.2f + ",f1->GetParameter(0));
    funcstr = linetxt;
    sprintf(linetxt," %7.2f sin (2#theta +%7.2f) +",
			f1->GetParameter(1),f1->GetParameter(2)*180.0/3.14159);
    funcstr += linetxt;
    sprintf(linetxt," %7.2f sin (4#theta +%7.2f)  (nm)",
			f1->GetParameter(3),f1->GetParameter(4)*180.0/3.14159);
   funcstr += linetxt;
      hDx->GetXaxis()->SetTitle(funcstr.Data());


    for (Int_t i = 0; i<5; i++) {
		dxpar[i]=f1->GetParameter(i);
    }

    // plot Dy
    plotcommand = "diff_";
    plotcommand += tmpname.Data();
    plotcommand += "Y";
    plotcommand += Form("*1e6:scandata1*180./4000>>hDy%d",myii);
   
    p1->cd(3);
    p->Draw(plotcommand.Data(),scut.Data(),"prof");
    // fit Dy vs theta
    f1 = new TF1("f1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0.0,360.0);
    f1->SetLineColor(2);
    f1->SetLineWidth(1);

    TH1F *hDy=(TH1F*)gDirectory->Get(Form("hDy%d",myii));
	hDy->Fit("f1","QR");
    rectphs = f1->GetParameter(2);
    while (rectphs>3.14159) rectphs-=2*3.14159;
    while (rectphs<-3.14159) rectphs+=2*3.14159;
    if (rectphs<0) { 
		rectphs+=3.14159;
		f1->SetParameter(1,-1*f1->GetParameter(1));
    }
    f1->SetParameter(2,rectphs);
    rectphs = f1->GetParameter(4);
    while (rectphs>3.14159) rectphs-=2*3.14159;
    while (rectphs<-3.14159) rectphs+=2*3.14159;
    if (rectphs<0) { 
		rectphs+=3.14159;
		f1->SetParameter(3,-1*f1->GetParameter(3));
    }
    f1->SetParameter(4,rectphs);
    hDy->Fit("f1","QR");

    f2->SetParameters(f1->GetParameter(1),f1->GetParameter(2));
    f2->DrawCopy("same");
    f4->SetParameters(f1->GetParameter(3),f1->GetParameter(4));
    f4->DrawCopy("same");
    f1->Draw("same");
    hDy->GetXaxis()->CenterTitle();
    //    hDy->GetXaxis()->SetTitleOffset(1.2);
    hDy->GetXaxis()->SetTitleSize(0.05);
    hDy->GetYaxis()->SetTitleSize(0.065);
    tit = bpmNam->Data();
    if(tmpname.Contains("lina1"))    tit += ",  diff_rms vs. #theta";
    else tit += ",  Dy/2 vs. #theta";
    hDy->GetYaxis()->SetTitle(tit.Data());
    
    xmin = f1->GetXaxis()->GetXmin();
    xmax = f1->GetXaxis()->GetXmax();
    t1->DrawLine(xmin,0,xmax,0);

    //print fit eqn to canvas   
    if(tmpname.Contains("lina1")) sprintf(linetxt,"Drms = %7.2f + ",f1->GetParameter(0));
    else sprintf(linetxt,"Dy/2 = %7.2f + ",f1->GetParameter(0));
    funcstr = linetxt;
    sprintf(linetxt," %7.2f sin (2#theta +%7.2f) +",
			f1->GetParameter(1),f1->GetParameter(2)*180.0/3.14159);
    funcstr += linetxt;
    sprintf(linetxt," %7.2f sin (4#theta +%7.2f)  (nm)",
			f1->GetParameter(3),f1->GetParameter(4)*180.0/3.14159);
    funcstr += linetxt;
    hDy->GetXaxis()->SetTitle(funcstr.Data());
    
    for (Int_t i = 0; i<5; i++) {
		dypar[i]=f1->GetParameter(i);
    }


    //   printf(" %7.1f  %7.1f  %5.1f  %7.1f  %5.1f \n",aqpar[0],
    //		   aqpar[1],aqpar[2]*180.0/3.14159,aqpar[3],
    //		   aqpar[4]*180.0/3.14159);
    //   printf(" %7.1f  %7.1f  %5.1f  %7.1f  %5.1f \n",1e3*dxpar[0],
    //		   1e3*dxpar[1],dxpar[2]*180.0/3.14159,1e3*dxpar[3],
    //		   dxpar[4]*180.0/3.14159);
    //  printf(" %7.1f  %7.1f  %5.1f  %7.1f  %5.1f \n",1e3*dypar[0],
    //		   1e3*dypar[1],dypar[2]*180.0/3.14159,1e3*dypar[3],
    //		   dypar[4]*180.0/3.14159);


    //  delete finalcut;
delete bpmNam, tmpname, plotcommand, aqpar,dxpar,dypar,p,scut,f1,f2,f4,rectphs,tit,xmin,xmax,linetxt,funcstr,t1;
// delete hAq, hDx,hDy;

}
