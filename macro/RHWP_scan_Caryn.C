void plot_element(TPad *p1, TPad *p2, TPad *p3, TPad *p4, TPad *p5, TPad *p6,
		  TString devnam, TString lcut, Int_t dset);

void RHWP_scan_Caryn(Int_t runnumber=1, TString device="bpm1i02", int IHWPstate=0, Int_t PITA=0, Int_t PITAposU=0,Int_t PITAposV=0,TString ucut ="1") {
	if (runnumber==1) {
		printf("Usage:\n\t.x RHWPscan.C(runnum,[device],[cut])\n\n");
		return;
	}
	gROOT->Reset();

	TString ihwpstate;
	Int_t orientation;
    
	if (IHWPstate==1){
		ihwpstate = "IHWP IN";
		cout << endl << "RHWP scan with IHWP IN" << endl;
	} else {
		ihwpstate = "IHWP OUT";
		cout << endl << "RHWP scan with IHWP OUT" << endl;
	}
  
	TString gtitle;
	TString pstit;
	TString psnam;
	TString dettype = device;

	if (runnumber != 1) {
		psnam = Form("/adaqfs/home/apar/PREX/japan/plots/parityOutputs/RHWP_%i", runnumber);
		gtitle = "RHWP scan, Run ";
		gtitle += runnumber;
		if(IHWPstate==1){
			psnam += "_IN";		gtitle += ", IHWP IN, ";
		} else {
			psnam += "_OUT";	gtitle += ", IHWP OUT, ";
		}

		gtitle+=  "PITA=";
		gtitle+= Form("%d",PITA);
		//		cout<<PITA<<endl;

        	gtitle+=  ", AposU=";
	      	gtitle+= Form("%d",PITAposU);



	       gtitle+=  ", AposV=";
	      
		gtitle+= Form("%d",PITAposV);


		psnam += Form("_PITA%d",PITA);
		psnam += Form("_AposU%d",PITAposU);
		psnam += Form("_AposV%d",PITAposV);

	      
			psnam += Form("_%s",device.Data());
			gtitle += Form(", %s",device.Data());
	  
		psnam += ".png";

		pstit = "rhwpscan_run";
		pstit += runnumber;
		if(IHWPstate==1){
			pstit += "_ihwp_";
		} else {
			pstit += "_noihwp_";
		}
		pstit += dettype;
	} else {
		return;
	}
    
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
	gStyle->SetPadBottomMargin(0.16);
	gStyle->SetPadRightMargin(0.01);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetLabelSize(0.05,"x");
	gStyle->SetLabelSize(0.05,"y");
	gROOT->ForceStyle();  

	TCanvas *a1 = new TCanvas("a1", "Scan plate scan",
							  0,0,800,800);

	TPad* a1_p[6];

	a1_p[0]  = new TPad("a1_0","a1_0",0.00,0.727,1.0,0.940);    
	a1_p[1]  = new TPad("a1_1","a1_1",0.00,0.624,1.0,0.727);
	a1_p[2]  = new TPad("a1_2","a1_2",0.00,0.415,1.0,0.624);    
	a1_p[3]  = new TPad("a1_3","a1_3",0.00,0.312,1.0,0.415);
	a1_p[4]  = new TPad("a1_4","a1_4",0.00,0.103,1.0,0.312);    
	a1_p[5]  = new TPad("a1_5","a1_5",0.00,0.000,1.0,0.103);

	TPad* a1_title = new TPad("a1_title","a1_title",0.0,0.94,1.0,1.0);    

	a1->cd();
	for (Int_t i =0; i<6; i++) a1_p[i]->Draw();
	a1_title->Draw();


	a1_title->cd();
	TPaveText* pt2 = new TPaveText(0.04,0.04,0.96,0.5,"brNDC");
	pt2->SetBorderSize(0);pt2->SetFillColor(10);
	pt2->SetTextFont(20);
	pt2->SetTextSize(0.45);
	pt2->SetTextAlign(22);
	pt2->AddText(0.5,0.9,gtitle.Data());
	pt2->Draw();

	TString finalcut = "cleandata==1 && ";
	finalcut += ucut;

	plot_element(a1_p[0],a1_p[1],a1_p[2],a1_p[3],a1_p[4],a1_p[5],dettype,finalcut,runnumber);

    a1->Print(psnam.Data());

}

void plot_element(TPad *p1, TPad *p2, TPad *p3, TPad *p4, TPad *p5, TPad *p6,
				  TString devnam, TString lcut, Int_t runnumber) 
{
  
	TString *bpmNam = new TString(devnam);

	Int_t ifnd = 0;
	TString tmpname;
	TString plotcommand;
	Double_t aqpar[5];
	Double_t dxpar[5];
	Double_t dypar[5];

	// TCanvas *c1 = new TCanvas("c1", "Beam Element Responses",0,0,300,200);

	ifnd =0;
	Int_t icurRun=0;
	char buff[170];

	// Open the file

	TFile *fin=TFile::Open(Form("$QW_ROOTFILES/prexALL_%d.000.root",runnumber),"READ");
	TTree *p=(TTree*)fin->Get("mul");

	// make cut
	TString scut = lcut;

    // plot A_Q
    tmpname = bpmNam->Data();
	plotcommand = "1e6*asym_";
		plotcommand += tmpname.Data();
	plotcommand += "WS";
    plotcommand += ":scandata1*180./4000>>hAq";
	cout<<plotcommand.Data()<<endl;
    p1->cd();
    p1->SetLeftMargin(0.06);
    p1->SetRightMargin(0.04);
    p->SetMarkerStyle(7);
    p->SetMarkerColor(kBlue);
    p->SetLineColor(kBlue);
    p->Draw(plotcommand.Data(),scut.Data(),"prof");
	TH1F *hAq=(TH1F*)gDirectory->Get("hAq");
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
    //    cout << "I'm trying to fit the charge asymmetry" << endl;

    hAq->Fit("f1");
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
    hAq->Fit("f1");

    f2->SetParameters(f1->GetParameter(1),f1->GetParameter(2));
    f2->DrawCopy("same");
    f4->SetParameters(f1->GetParameter(3),f1->GetParameter(4));
    f4->DrawCopy("same");
    f1->Draw("same");
    hAq->GetXaxis()->CenterTitle();
    hAq->GetXaxis()->SetTitleOffset(1.2);
    hAq->GetXaxis()->SetTitleSize(0.06);
    TString tit = bpmNam->Data();
    tit += ", Asymmetry vs.  #theta";
    hAq->GetXaxis()->SetTitle(tit.Data());


    Double_t padmaxx,padmaxy, padminx,padminy;
    //gPad->GetRange(padminx,padminy,padmaxx,padmaxy);
    //  padmaxx= gPad->GetLast();
    //cout << "padminx::" << padminx << endl;
    //cout << "padmaxx::" << padmaxx << endl;
    //cout << "padminy::" << padminy << endl;
    //cout << "padmaxy::" << padmaxy << endl;

    Double_t xmin, xmax;
    TLine *t1 = new TLine(xmin,0.0,xmax,1.0);
    xmin = f1->GetXaxis()->GetXmin();
    xmax = f1->GetXaxis()->GetXmax();
    t1->DrawLine(xmin,0,xmax,0);
    //    t1->Draw();

    
    //print fit eqn to canvas   
    p2->cd();
    char linetxt[50];
    sprintf(linetxt,"Aq = %7.2f + ",f1->GetParameter(0));
    TString funcstr = linetxt;
    sprintf(linetxt," %7.2f sin (2#theta +%7.2f) +",
			f1->GetParameter(1),f1->GetParameter(2)*180.0/3.14159);
    funcstr += linetxt;
    sprintf(linetxt," %7.2f sin (4#theta +%7.2f)  (ppm)",
			f1->GetParameter(3),f1->GetParameter(4)*180.0/3.14159);
    funcstr += linetxt;
    TPaveText* pt = new TPaveText(0.1,0.2,0.9,0.9,"brNDC");
    pt->SetBorderSize(1);pt->SetFillColor(10);
    pt->SetTextAlign(12);pt->SetTextFont(22);
    //     pt->SetTextSize(0.06);
    pt->AddText(funcstr.Data());
    //    pt->Draw();
    for (Int_t i = 0; i<5; i++) {
		aqpar[i]=f1->GetParameter(i);
    }


//     char linetxt2[50];
//     sprintf(linetxt2,"        +/- %5.1f",f1->GetParError(0));
//     TString efuncstr = linetxt2;
//     sprintf(linetxt2,"    +/- %5.1f           +/- %5.1f ",
//  	    f1->GetParError(1),f1->GetParError(2)*180.0/3.14159);
//      efuncstr += linetxt2;
//      sprintf(linetxt2,"     +/- %5.1f           +/- %5.1f)",
//  	    f1->GetParError(3),f1->GetParError(4)*180.0/3.14159);
//      efuncstr += linetxt2;
//     pt = new TPaveText(0.1,0.3,0.9,0.7,"brNDC");
//     pt->SetBorderSize(1);pt->SetFillColor(10);
//     pt->SetTextAlign(12);pt->SetTextFont(22);
    //     pt->SetTextSize(0.06);
//     pt->AddText(efuncstr.Data());
    pt->Draw();

    // plot Dx
    tmpname = bpmNam->Data();
    plotcommand = "diff_";
    plotcommand += tmpname.Data();
    plotcommand += "X";
    plotcommand += "*1e6:scandata1*180./4000 >>hDx";
    p3->cd();
    p3->SetLeftMargin(0.06);
    p3->SetRightMargin(0.04);
    p->Draw(plotcommand.Data(),scut.Data(),"prof");
    // fit Dx vs theta
    f1 = new TF1("f1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0.0,360.0);
    f1->SetLineColor(2);
    f1->SetLineWidth(1);

	TH1F *hDx=(TH1F*)gDirectory->Get("hDx");
    hDx->Fit("f1");
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
    hDx->Fit("f1");

    f2->SetParameters(f1->GetParameter(1),f1->GetParameter(2));
    f2->DrawCopy("same");
    f4->SetParameters(f1->GetParameter(3),f1->GetParameter(4));
    f4->DrawCopy("same");
    f1->Draw("same");
    hDx->GetXaxis()->CenterTitle();
    hDx->GetXaxis()->SetTitleOffset(1.2);
    hDx->GetXaxis()->SetTitleSize(0.06);
    tit = bpmNam->Data();
    tit += ",  Dx/2 vs. #theta";
    hDx->GetXaxis()->SetTitle(tit.Data());
    
    xmin = f1->GetXaxis()->GetXmin();
    xmax = f1->GetXaxis()->GetXmax();
    t1->DrawLine(xmin,0,xmax,0);

    //print fit eqn to canvas   
    p4->cd();
    //    linetxt[50];
    sprintf(linetxt,"Dx/2 = %7.2f + ",f1->GetParameter(0));
    funcstr = linetxt;
    sprintf(linetxt," %7.2f sin (2#theta +%7.2f) +",
			f1->GetParameter(1),f1->GetParameter(2)*180.0/3.14159);
    funcstr += linetxt;
    sprintf(linetxt," %7.2f sin (4#theta +%7.2f)  (nm)",
			f1->GetParameter(3),f1->GetParameter(4)*180.0/3.14159);
    funcstr += linetxt;
    pt = new TPaveText(0.1,0.3,0.9,0.7,"brNDC");
    pt->SetBorderSize(1);pt->SetFillColor(10);
    pt->SetTextAlign(12);pt->SetTextFont(22);
    //     pt->SetTextSize(0.06);
    pt->AddText(funcstr.Data());
    pt->Draw();

    cout<<"X 4theta err "<<f1->GetParError(3)<<endl;
      cout<<"X phase err "<<f1->GetParError(4)*180/3.14159<<endl;
      cout<<"X offs err "<<f1->GetParError(0)<<endl;


    for (Int_t i = 0; i<5; i++) {
		dxpar[i]=f1->GetParameter(i);
    }

    // plot Dy
    plotcommand = "diff_";
    plotcommand += tmpname.Data();
    plotcommand += "Y";
    plotcommand += "*1e6:scandata1*180./4000>>hDy";
    p5->cd();
    p5->SetLeftMargin(0.06);
    p5->SetRightMargin(0.04);
    p->Draw(plotcommand.Data(),scut.Data(),"prof");
    // fit Dy vs theta
    f1 = new TF1("f1","[0] + [1]*sin(2*3.14159*x/180.+[2]) + [3]*sin(4*3.14159*x/180.+[4])",0.0,360.0);
    f1->SetLineColor(2);
    f1->SetLineWidth(1);
//     f1->SetParLimits(2,-3.14159,3.14159);
//     f1->SetParLimits(4,0,3.14159);

	TH1F *hDy=(TH1F*)gDirectory->Get("hDy");
    hDy->Fit("f1");
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
    hDy->Fit("f1");

    f2->SetParameters(f1->GetParameter(1),f1->GetParameter(2));
    f2->DrawCopy("same");
    f4->SetParameters(f1->GetParameter(3),f1->GetParameter(4));
    f4->DrawCopy("same");
    f1->Draw("same");
    hDy->GetXaxis()->CenterTitle();
    hDy->GetXaxis()->SetTitleOffset(1.2);
    hDy->GetXaxis()->SetTitleSize(0.06);
    tit = bpmNam->Data();
    if(tmpname.Contains("lina1"))    tit += ",  diff_rms vs. #theta";
    else tit += ",  Dy/2 vs. #theta";
    hDy->GetXaxis()->SetTitle(tit.Data());
    
    xmin = f1->GetXaxis()->GetXmin();
    xmax = f1->GetXaxis()->GetXmax();
    t1->DrawLine(xmin,0,xmax,0);

    //print fit eqn to canvas   
    p6->cd();
    //linetxt[50];
    if(tmpname.Contains("lina1")) sprintf(linetxt,"Drms = %7.2f + ",f1->GetParameter(0));
    else sprintf(linetxt,"Dy/2 = %7.2f + ",f1->GetParameter(0));
    funcstr = linetxt;
    sprintf(linetxt," %7.2f sin (2#theta +%7.2f) +",
			f1->GetParameter(1),f1->GetParameter(2)*180.0/3.14159);
    funcstr += linetxt;
    sprintf(linetxt," %7.2f sin (4#theta +%7.2f)  (nm)",
			f1->GetParameter(3),f1->GetParameter(4)*180.0/3.14159);
    funcstr += linetxt;
    pt = new TPaveText(0.1,0.3,0.9,0.7,"brNDC");
    pt->SetBorderSize(1);pt->SetFillColor(10);
    pt->SetTextAlign(12);pt->SetTextFont(22);
    //     pt->SetTextSize(0.06);
    pt->AddText(funcstr.Data());
    pt->Draw();

    cout<<"Y 4theta err "<<f1->GetParError(3)<<endl;
      cout<<"Y phase err "<<f1->GetParError(4)*180/3.14159<<endl;
      cout<<"Y offs err "<<f1->GetParError(0)<<endl;

    
    for (Int_t i = 0; i<5; i++) {
		dypar[i]=f1->GetParameter(i);
    }


    printf(" %7.1f  %7.1f  %5.1f  %7.1f  %5.1f \n",aqpar[0],
		   aqpar[1],aqpar[2]*180.0/3.14159,aqpar[3],
		   aqpar[4]*180.0/3.14159);
    printf(" %7.1f  %7.1f  %5.1f  %7.1f  %5.1f \n",1e3*dxpar[0],
		   1e3*dxpar[1],dxpar[2]*180.0/3.14159,1e3*dxpar[3],
		   dxpar[4]*180.0/3.14159);
    printf(" %7.1f  %7.1f  %5.1f  %7.1f  %5.1f \n",1e3*dypar[0],
		   1e3*dypar[1],dypar[2]*180.0/3.14159,1e3*dypar[3],
		   dypar[4]*180.0/3.14159);
//     TString outstr=linetxt;
//     cout << outstr.Data() <<< endl;
//     outstr=linetxt;
//     cout << outstr <<< endl;
//     outstr=linetxt;
//     cout << outstr <<< endl;


}
