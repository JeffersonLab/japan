// jaPAN Macro Script - BPM Pedestal Scan for injector
void GetPedestal_injector(int run_num = 1, TString user_cut ="1",Double_t lowcurrent=0){
  gROOT->SetStyle("Plain");
  // define style here 
  // general parameters
  gStyle->SetOptDate(0);
  gStyle->SetStatColor(10);
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
  
  TString rf_name =Form("../rootfiles/prexinj_%d.root",run_num);
  TFile *rootfile = TFile::Open(rf_name);
  TTree *mps = rootfile->Get("Mps_Tree");

  user_cut += "&& cleandata";
  
  // Extract scandata1
  mps->Draw("scandata1>>htemp",user_cut.Data(),"goff");
  int nbinx = (int)htemp->GetXaxis()->GetXmax();

  vector<int> vec_scandata;
  TH1D *hsd = new TH1D("hsd","scan data",nbinx,-0.5,nbinx-0.5); 
  mps->Draw("scandata1>>hsd",user_cut.Data(),"goff");
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
    if(scandata[i] > lowcurrent)
      ndata_fit++;
  }
  int startpt = ndata - ndata_fit;  //starting point in a data array for fitting
  
  // BPM device array for injector data, August 2018
  const int nBPM=21;
  TString device_name[nBPM]={"0i01a","1i04", //ADC0
			     "1i02","0i01", //ADC1
			     "1i06","0i02", // ADC2
			     "0i02a","0i05", //ADC3
			     "0i07","0l01", // ADC4
			     "0l02","0l03", //ADC5
			     "0l04","0l05", //ADC6
			     "0l06","0l07", //ADC7
			     "0l08","0l09", //ADC8
			     "0l10","2i02", //ADC9
			     "2i01"}; //ADC10
  
  TString ch_name[4]={"XP","XM","YP","YM"};

  TCanvas *c_fit = new TCanvas("c_fit","c_fit",800,800);
  c_fit->Divide(2,2);

  TCanvas *c_res = new TCanvas("c_res","c_res",800,800);
  c_res->Divide(2,2);
  
  TF1 *f_zero = new TF1("f_zero","0",0,100);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);
  
  TString branch_name;
  TString num_samples_name;
  
  TString my_cut; // cut for extracting each scan data point
  
  double adc_mean[4][ndata];
  double adc_error[4][ndata];
  double adc_res[4][ndata]; // residual

  TString gfit_title[4];
  TString gres_title[4];
  TF1 *f_fit;
  TH1D *h_stat;
  double ped[nBPM][4];
  double slope[nBPM][4];

  TGraphErrors *g_res[4];
  TGraphErrors *g_fit[4];  
  TGraphErrors *g_res_ref[4];
  TGraphErrors *g_fit_ref[4];
  TMultiGraph *mg_res[4];
  TMultiGraph *mg_fit[4];
  
  for(int ibpm=0;ibpm<nBPM;ibpm++){
    for(int ich=0;ich<4;ich++){
      
      branch_name = Form("bpm%s%s.hw_sum_raw/bpm%s%s.num_samples",
			 device_name[ibpm].Data(),ch_name[ich].Data(),
      			 device_name[ibpm].Data(),ch_name[ich].Data());
      for(int i=0;i<ndata;i++){
	my_cut = Form("scandata1 <%f  && scandata1>%f && ",scandata[i]+0.5,scandata[i]-0.5);
	my_cut += user_cut;
	mps->Draw(Form("%s>>h_bpm%d_%d_%d",branch_name.Data(),ibpm,ich,i),
		  my_cut.Data(),"goff");
	h_stat =(TH1D*)gDirectory->FindObject(Form("h_bpm%d_%d_%d",ibpm,ich,i));
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
      mg_fit[ich]->Add(g_fit_ref[ich]);
      mg_fit[ich]->Draw("AP");
      gfit_title[ich] = Form("%s;scandata1; Raw ADC",branch_name.Data());
      mg_fit[ich]->SetTitle(gfit_title[ich].Data());
      
      g_fit[ich]->Fit("pol1","Q");
      f_fit = g_fit[ich]->GetFunction("pol1");
      f_fit->SetLineColor(kRed);
      f_fit->SetRange(scandata[0]*0.9,scandata[ndata-1]*1.1);
      f_fit->Draw("same");
      ped[ibpm][ich] = f_fit->GetParameter(0);
      slope[ibpm][ich] = f_fit->GetParameter(1);
      
      for(int i=0;i<ndata;i++){
	adc_res[ich][i] = adc_mean[ich][i] - f_fit(scandata[i]);
      }
      c_res->cd(ich+1);
      mg_res[ich] = new TMultiGraph();
      g_res[ich] = new TGraphErrors(ndata-startpt,scandata+startpt,adc_res[ich]+startpt,scandata_error+startpt,adc_error[ich]+startpt);
      g_res_ref[ich] = new TGraphErrors(startpt,scandata,adc_res[ich],scandata_error,adc_error[ich]);
      g_res[ich]->SetMarkerStyle(20);
      g_res_ref[ich]->SetMarkerStyle(24);
      mg_res[ich]->Add(g_res[ich]);
      mg_res[ich]->Add(g_res_ref[ich]);
      mg_res[ich]->Draw("AP");
      gres_title[ich] = Form("%s;scandata1; Residual ",branch_name.Data());
      mg_res[ich]->SetTitle(gres_title[ich].Data());
      f_zero->SetRange(scandata[0]*0.9,scandata[ndata-1]*1.1);
      f_zero->Draw("same");
    }
    c_fit->SaveAs(Form("run%d_bpm%s_ped_fit.pdf",run_num,device_name[ibpm].Data()));
    c_res->SaveAs(Form("run%d_bpm%s_ped_res.pdf",run_num,device_name[ibpm].Data()));
  }
  gSystem->Exec(Form("pdfunite *bpm*fit.pdf run%d_fit.pdf",run_num));
  gSystem->Exec(Form("pdfunite *bpm*res.pdf run%d_res.pdf",run_num));
  gSystem->Exec("rm *bpm*.pdf");

  FILE *mapfile;
  mapfile = fopen(Form("prexinj_beamline_pedestal.%d.map",run_num),"w");

  for(int ibpm=0;ibpm<nBPM;ibpm++){
    for(int ich=0;ich<4;ich++){
      fprintf(mapfile,"bpm%s%s, %f, %f \n",
	      device_name[ibpm].Data(),
	      ch_name[ich].Data(),
	      ped[ibpm][ich],
	      1.0);
      cout << ped[ibpm][ich] << endl;
    }
  }
  fclose(mapfile);
  rootfile->Close();
}
