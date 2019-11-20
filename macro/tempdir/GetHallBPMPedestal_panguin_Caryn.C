// jaPAN Macro Script - BPM Pedestal Scan
// based on UNSER readout
void GetHallBPMPedestal_panguin_Caryn(TString devnam="4a",TString scan_data = "unser", TString user_cut ="1", Double_t MIN=-1e6, Double_t MAX=1e6,Int_t myii=0){
  gROOT->SetStyle("Plain");
  gStyle->SetStatH(0.2);
  gStyle->SetStatW(0.3);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(1011);
  gStyle->SetStatX(0.7);
  gStyle->SetStatY(0.9);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
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
  
   TTree *tree= (TTree*)gROOT->FindObject("evt");
   //---------------------------

 tree->SetAlias("beam_current", scan_data);

  tree->Draw(Form("beam_current>>htemp%d",myii),Form("%s&&scandata1!=0",user_cut.Data()),"goff");
  TH1D* htemp = (TH1D*)gDirectory->FindObject(Form("htemp%d",myii));
  double max = htemp->GetXaxis()->GetXmax();
  int nbinx = (int)(10*max); // to one-tenth precision.

  vector<double> vec_scandata;
  TH1D *hsd = new TH1D(Form("hsd%d",myii),"scan data",nbinx,-0.05,max-0.05); 
  tree->Draw(Form("beam_current>>hsd%d",myii),user_cut.Data(),"goff");
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
  //   int ndata_fit = 0;  // number of scandata points used for fitting
  //  double scandata[ndatatemp];
  // double scandata_error[ndatatemp];
  //   for(int i=0; i<ndatatemp;i++){
  //   scandata[i] = vec_scandata[i];
  //   scandata_error[i] = 0;
     //      ndata_fit++;
      //  }
    //  int startpt = ndatatemp - ndata_fit;  //starting point in a data array for fitting
    //  const int ndata = ndatatemp-startpt;

//-----------
   TString device_name[] = {devnam.Data()};
     //{"4a","4e","12","14","8","4ac","4ec","1p02b","1p03a"};

  const int nBPM= sizeof(device_name)/sizeof(*device_name);
  TString ch_name[4]={"XP","XM","YP","YM"};

  const int nbpm= sizeof(device_name)/sizeof(*device_name);

  TPad *c3 = new TPad("c3","c3",0,0,1,1);
  c3->Divide(1,2);
  c3->Draw();
  TVirtualPad* c_fit = c3->cd(1);
  TVirtualPad* c_res = c3->cd(2);
  c_fit->Divide(4,1);
  c_res->Divide(4,1);
  
  TF1 *f_zero = new TF1("f_zero","0",0,100);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);
  
  TString branch_name;
  TString num_samples_name;
 
  // for(Int_t iev=0; iev<
  // tree->Draw(Form("scandata1>>h1_%d",myii),"cleandata==1");
  // tree->Draw(Form("scandata2>>h2_%d",myii),"cleandata==1&&scandata2<99");
  // tree->Draw(Form("scandata2>>h200_%d",myii),"cleandata==1&&scandata2>99");
  // TH1* h1 = (TH1D*)gDirectory->FindObject(Form("h1_%d",myii));
  // TH1* h2 = (TH1D*)gDirectory->FindObject(Form("h2_%d",myii));
  //TH1* h200 = (TH1D*)gDirectory->FindObject(Form("h200_%d",myii));


  //const int ndata = sizeof(beam_evtcut)/sizeof(*beam_evtcut);

  double adc_mean[5][ndata]; // 4 channels  + 1channel for wire sum
  double adc_error[5][ndata];
  double adc_res[5][ndata]; // residual

  double unser_mean[ndata]; 
  double unser_error[ndata];

  TString gfit_title[5];
  TString gres_title[5];
  TH1D *h_stat;
  double ped[nBPM][5];
  double slope[nBPM][5];

  TGraphErrors *g_hbpmres[5];
  // g_hbpmres->SetNameTitle(Form("g_hbpmres%d",myii));
  TGraphErrors *g_hbpmfit[5];  
  TGraphErrors *g_hbpmres_ref[5];
  TGraphErrors *g_hbpmfit_ref[5];
  TMultiGraph *mg_hbpmres[5];
  TMultiGraph *mg_hbpmfit[5];

  TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");
  double init_par[2] = { 1e3,1e4};


  TString my_cut;
  for(int i=0;i<ndata;i++){
    // my_cut = beam_evtcut[i];
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0",i+1);
    tree->Draw(Form("%s>>h_unser_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_unser_%d_%d",i,myii));
    unser_mean[i] = h_stat->GetMean();
    unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
    //cout << unser_mean[i] << endl;
    // my_cut = pedestal_unser_evtcut[i];
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1==0",i+1);
    tree->Draw(Form("%s>>h_ped_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_ped_%d_%d",i,myii));
     unser_mean[i]  = unser_mean[i] - h_stat->GetMean(); // pedestal subtraction
    //cout << unser_mean[i] << endl;
  }

  for(int ibpm=0;ibpm<nBPM;ibpm++){
    for(int ich=0;ich<4;ich++){

     
	branch_name = Form("bpm%s%s.hw_sum_raw/bpm%s%s.num_samples",
			   device_name[ibpm].Data(),ch_name[ich].Data(),
			   device_name[ibpm].Data(),ch_name[ich].Data());
	

      for(int i=0;i<ndata;i++){
	//	my_cut = beam_evtcut[i];
	my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0",i+1);
	tree->Draw(Form("%s>>h_bpm%d_%d_%d_%d",branch_name.Data(),ibpm,ich,i,myii),
		  my_cut.Data(),"goff");
	h_stat =(TH1D*)gDirectory->FindObject(Form("h_bpm%d_%d_%d_%d",ibpm,ich,i,myii));
	adc_mean[ich][i] = h_stat->GetMean();
	adc_error[ich][i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_fit->cd(ich+1);
      
      g_hbpmfit[ich] = new TGraphErrors(ndata,unser_mean,adc_mean[ich],unser_error,adc_error[ich]);
      g_hbpmfit[ich]->SetMarkerStyle(20);
      g_hbpmfit[ich]->Draw("AP");
      gfit_title[ich] = Form("%s%s;%s; Raw ADC",device_name[ibpm].Data(),ch_name[ich].Data(),scan_data.Data());
      g_hbpmfit[ich]->SetTitle(gfit_title[ich].Data());

      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_hbpmfit[ich]->Fit("f_fit","Q","",MIN,MAX);
      // f_fit->Draw("same");

      ped[ibpm][ich] = f_fit->GetParameter(0);
      slope[ibpm][ich] = f_fit->GetParameter(1);
      
      for(int i=0;i<ndata;i++)
	adc_res[ich][i] = adc_mean[ich][i] - f_fit->Eval(unser_mean[i]);
      
      c_res->cd(ich+1);
      g_hbpmres[ich] = new TGraphErrors(ndata,unser_mean,adc_res[ich],unser_error,adc_error[ich]);
      g_hbpmres[ich]->SetMarkerStyle(20);
      g_hbpmres[ich]->Draw("AP");

      gres_title[ich] = Form("%s%s;%s; Residual ",device_name[ibpm].Data(),ch_name[ich].Data(),scan_data.Data());
      g_hbpmres[ich]->SetTitle(gres_title[ich].Data());
      f_zero->Draw("same");
    } // End ofW Wires loop
   
  } // End of BPMs loop
 

  delete  tree, htemp, max, nbinx, vec_scandata, hsd, bin_content, bin_center, device_name, nBPM, ch_name, nbpm,f_zero, branch_name, num_samples_name, ndata, adc_mean, adc_error, adc_res, unser_mean, unser_error, gfit_title, gres_title, ped, slope, f_fit, init_par,my_cut;

}
