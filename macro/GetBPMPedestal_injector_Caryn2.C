// jaPAN Macro Script - BPM Pedestal Scan for injector

void GetBPMPedestal_injector_Caryn2(int run_num = 1, TString scan_data = "scandata1",TString user_cut ="1",Double_t MIN=-1e6, Double_t MAX=1e6,Int_t myii=0){
  gROOT->SetStyle("Plain");
  // define style here 
  // general parameters
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.3);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(1011);
  gStyle->SetStatX(0.7);
  gStyle->SetStatY(0.9);
  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  // pads parameters
  gStyle->SetPadColor(39); 
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadRightMargin(0.01);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetLabelSize(0.035,"x");
  gStyle->SetLabelSize(0.035,"y");
  gStyle->SetTitleSize(0.06,"hxyz");
  gROOT->ForceStyle();  
  
  //  TString rf_name =Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num);
  //TFile *rootfile = TFile::Open(rf_name);
  TFile *rootfile = TFile::Open(Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num));
  TTree *tree= (TTree*)rootfile->Get("evt");
  tree->SetAlias("beam_current", scan_data);
  user_cut += "&& cleandata";
  
	TString outputDir = "/adaqfs/home/apar/PREX/japan/plots/pedestalOutputs";

  // Extract scandata1
  tree->Draw("beam_current>>htemp",user_cut.Data(),"goff");
  TH1D* htemp = (TH1D*)gDirectory->FindObject("htemp");
  double max = htemp->GetXaxis()->GetXmax();
  int nbinx = (int)(10*max); // to one-tenth precision.

  vector<double> vec_scandata;
  TH1D *hsd = new TH1D("hsd","scan data",nbinx,-0.05,max-0.05); 
  tree->Draw("beam_current>>hsd",user_cut.Data(),"goff");
  int bin_content; 
  double bin_center;
  for(int ibin=0;ibin<nbinx;ibin++){
    bin_content = hsd->GetBinContent(ibin+1); // Histogram bin number starts from 1
    if(bin_content>0){
      bin_center = hsd->GetBinCenter(ibin+1);
      vec_scandata.push_back(bin_center);
    }
  }
  const int ndata = vec_scandata.size();
  int ndata_fit = 0;  // number of scandata points used for fitting
  double scandata[ndata];
  double scandata_error[ndata];
  for(int i=0; i<ndata;i++){
    scandata[i] = vec_scandata[i];
    scandata_error[i] = 0;
    //    if(scandata[i] > lowcurrent)
      ndata_fit++;
  }
  int startpt = ndata - ndata_fit;  //starting point in a data array for fitting
  
  TString device_name[]={"1i02","1i04", //, //ADC0
  			     "0i01","0i01a", //ADC1
  			     "1i06","0i02", // ADC2
  			     "0i02a","0i05", //ADC3
  			     "0i07","0l01", // ADC4
  			     "0l02","0l03", //ADC5
  			     "0l04","0l05", //ADC6
  			     "0l06","0l07", //ADC7
  			     "0l08","0l09", //ADC8
  			     "0l10","2i02", //ADC9
  			     "0r05","0r03",//ADC10
  			     "2i01", //ADC11
  			     "0r02","0r04" }; //scalers

  cout<<"hello"<<endl;

  const int nBPM= sizeof(device_name)/sizeof(*device_name);
  TString ch_name[4]={"XP","XM","YP","YM"};

  TCanvas *c1 = new TCanvas("c1","c1",1500,600);
  c1->Divide(1,2);
  TVirtualPad* c_fit = c1->cd(1);
  TVirtualPad* c_res = c1->cd(2);
  c_fit->Divide(4,1);
  c_res->Divide(4,1);

  TF1 *f_zero = new TF1("f_zero","0",0,100);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);
  
  TString branch_name;
  TString num_samples_name;
  
  TString my_cut; // cut for extracting each scan data point
  
  double adc_mean[5][ndata]; // 4 channels  + 1channel for wire sum
  double adc_error[5][ndata];
  double adc_res[5][ndata]; // residual

  TString gfit_title[5];
  TString gres_title[5];
  TH1D *h_stat;
  double ped[nBPM][5];
  double slope[nBPM][5];

char outfilename[255];
	sprintf(outfilename,"%s/run%d_injector_bpm_pedestal_fit.txt",
			outputDir.Data(),run_num);
	printf("Writing output to %s\n",outfilename);
	//FILE *outfile = fopen(outfilename, "w"); 
	ofstream outfile;
	outfile.open(outfilename);


  TGraphErrors *g_res[5];
  TGraphErrors *g_fit[5];  
  TGraphErrors *g_res_ref[5];
  TGraphErrors *g_fit_ref[5];
  TMultiGraph *mg_res[5];
  TMultiGraph *mg_fit[5];

  TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");
  double init_par[2] = {1e3,1e4};

  for(int ibpm=0;ibpm<nBPM;ibpm++){
    for(int ich=0;ich<4;ich++){

      //      if(ich==4)  // WireSum
      //	branch_name = Form("bpm%sWS.hw_sum", device_name[ibpm].Data());
      //else{
	branch_name = Form("bpm%s%s.hw_sum_raw/bpm%s%s.num_samples",
			   device_name[ibpm].Data(),ch_name[ich].Data(),
			   device_name[ibpm].Data(),ch_name[ich].Data());
	// }

      for(int i=0;i<ndata;i++){
	my_cut = Form("beam_current <%f  && beam_current>%f && ",scandata[i]+0.05,scandata[i]-0.05);
	my_cut += user_cut;
	tree->Draw(Form("%s>>h_bpm%d_%d_%d%d",branch_name.Data(),ibpm,ich,i,myii),
		  my_cut.Data(),"goff");
	h_stat =(TH1D*)gDirectory->FindObject(Form("h_bpm%d_%d_%d%d",ibpm,ich,i,myii));
	adc_mean[ich][i] = h_stat->GetMean();
	adc_error[ich][i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_fit->cd(ich+1);
      
      mg_fit[ich] = new TMultiGraph();
      g_fit[ich] = new TGraphErrors(ndata-startpt,scandata+startpt,adc_mean[ich]+startpt,scandata_error+startpt,adc_error[ich]+startpt);
      g_fit_ref[ich] = new TGraphErrors(startpt,scandata,adc_mean[ich],scandata_error,adc_error[ich]);
      g_fit[ich]->SetMarkerStyle(20);
      g_fit_ref[ich]->SetMarkerStyle(24);

      mg_fit[ich]->Add(g_fit[ich]);
      if(startpt>0)
	mg_fit[ich]->Add(g_fit_ref[ich]);
      mg_fit[ich]->Draw("AP");
      gfit_title[ich] = Form("%s;beam_current; Raw ADC",branch_name.Data());
      mg_fit[ich]->SetTitle(gfit_title[ich].Data());

      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_fit[ich]->Fit("f_fit","QR","",MIN,MAX);

      ped[ibpm][ich] = f_fit->GetParameter(0);
      slope[ibpm][ich] = f_fit->GetParameter(1);
      
      for(int i=0;i<ndata;i++)
	adc_res[ich][i] = adc_mean[ich][i] - f_fit->Eval(scandata[i]);
      
      c_res->cd(ich+1);
      mg_res[ich] = new TMultiGraph();
      g_res[ich] = new TGraphErrors(ndata-startpt,scandata+startpt,adc_res[ich]+startpt,scandata_error+startpt,adc_error[ich]+startpt);
      g_res_ref[ich] = new TGraphErrors(startpt,scandata,adc_res[ich],scandata_error,adc_error[ich]);
      g_res[ich]->SetMarkerStyle(20);
      g_res_ref[ich]->SetMarkerStyle(24);
      mg_res[ich]->Add(g_res[ich]);
      if(startpt>0)
	mg_res[ich]->Add(g_res_ref[ich]); 
      mg_res[ich]->Draw("AP");
      gres_title[ich] = Form("%s;beam_current; Residual ",branch_name.Data());
      mg_res[ich]->SetTitle(gres_title[ich].Data());
      f_zero->SetRange(scandata[0]*0.9,scandata[ndata-1]*1.1);
      f_zero->Draw("same");
    } // End ofW Wires loop
    c1->SaveAs(Form("%s/run%d_bpm%s_ped_fit.png",outputDir.Data(),run_num,device_name[ibpm].Data()));
  } // End of BPMs loop
  gSystem->Exec(Form("convert %s/*bpm*fit.png %s/run%d_injector_bpm_pedestal_fit.pdf",outputDir.Data(),outputDir.Data(),run_num));
  gSystem->Exec(Form("rm %s/run%d_bpm*fit.png",outputDir.Data(),run_num));

  for(int ibpm=0;ibpm<nBPM;ibpm++){
    for(int ich=0;ich<4;ich++){
      printf("bpm%s%s, %.2f, %.1f \n",
	      device_name[ibpm].Data(),
	      ch_name[ich].Data(),
	      ped[ibpm][ich],
	      1.0);
      outfile<<Form("bpm%s%s, %.2f, %.1f ",
	      device_name[ibpm].Data(),
	      ch_name[ich].Data(),
	      ped[ibpm][ich],
	      1.0)<<endl;

    }
  }
  rootfile->Close();

  
  // delete c1, c_fit, c_res;



}
