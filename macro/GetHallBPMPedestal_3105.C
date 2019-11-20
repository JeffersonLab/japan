// jaPAN Macro Script - BPM Pedestal Scan
// based on UNSER readout
void GetHallBPMPedestal_3105(int run_num=3105,TString scan_data = "bcm_an_us",TString user_cut ="1", Double_t MIN=44, Double_t MAX=70,Int_t myii=0){

  user_cut = Form("%s&&bcm_an_us>25",user_cut.Data());


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
  
  // TString rf_name =Form("$QW_ROOTFILES/prexPrompt_pass2_%d.000.root",run_num);
  TFile *rootfile = TFile::Open(Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num));
  TTree *tree= (TTree*)rootfile->Get("evt");
  
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

	TString outputDir = "/adaqfs/home/apar/PREX/japan/plots/pedestalOutputs";

	TString device_name[] ={"4a","4e","12","14","8","4ac","4ec"};

  const int nBPM= sizeof(device_name)/sizeof(*device_name);
  TString ch_name[4]={"XP","XM","YP","YM"};

  const int nbpm= sizeof(device_name)/sizeof(*device_name);

  TCanvas *c3 = new TCanvas("c3","c3",1500,600);
  c3->Divide(1,2);
  TVirtualPad* c_fit = c3->cd(1);
  TVirtualPad* c_res = c3->cd(2);
  c_fit->Divide(4,1);
  c_res->Divide(4,1);
  
  TF1 *f_zero = new TF1("f_zero","0",0,10000);
  f_zero->SetLineWidth(2);
  f_zero->SetLineColor(kRed);
  f_zero->SetLineStyle(9);
  
  TString branch_name;
  TString num_samples_name;

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

	char outfilename[255];
	sprintf(outfilename,"%s/run%d_bpm_pedestal_fit.txt",
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
  double init_par[2] = { 1e3,1e4};

  TString my_cut;
  for(int i=0;i<ndata;i++){
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0&&%s",i+1,user_cut.Data());
     tree->Draw(Form("%s>>h_unser_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_unser_%d_%d",i,myii));
    unser_mean[i] = h_stat->GetMean();
    unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
    //cout << unser_mean[i] << endl;
    my_cut = Form("cleandata==1&&scandata2==%d&&scandata1==0&&%s",i+1,user_cut.Data());
    tree->Draw(Form("%s>>h_ped_%d_%d",scan_data.Data(),i,myii),
	       my_cut.Data(),"goff");
    h_stat =(TH1D*)gDirectory->FindObject(Form("h_ped_%d_%d",i,myii));
    if(scan_data=="unser"){
    unser_mean[i]  = unser_mean[i] - h_stat->GetMean(); // pedestal subtraction
    //cout << unser_mean[i] << endl;
    }
  }

  for(int ibpm=0;ibpm<nBPM;ibpm++){
    for(int ich=0;ich<4;ich++){

      //      if(ich==4) { // WireSum
	// TString wiresum_text = "0.0+";
	// for(int ich=0;ich<4;ich++){
	//    wiresum_text += Form("+bpm%s%s.hw_sum_raw/bpm%s%s.num_samples",
	// 			device_name[ibpm].Data(),ch_name[ich].Data(),
	// 			device_name[ibpm].Data(),ch_name[ich].Data());
	// }
	// tree->SetAlias("WireSum", wiresum_text);
	// branch_name = "WireSum";
      //	branch_name = Form("bpm%sWS.hw_sum", device_name[ibpm].Data());
      // }
      // else{
	branch_name = Form("bpm%s%s.hw_sum_raw/bpm%s%s.num_samples",
			   device_name[ibpm].Data(),ch_name[ich].Data(),
			   device_name[ibpm].Data(),ch_name[ich].Data());
	//}

      for(int i=0;i<ndata;i++){
	my_cut = Form("cleandata==1&&scandata2==%d&&scandata1!=0&&%s",i+1,user_cut.Data());
	tree->Draw(Form("%s>>h_bpm%d_%d_%d_%d",branch_name.Data(),ibpm,ich,i,myii),
		  my_cut.Data(),"goff");
	h_stat =(TH1D*)gDirectory->FindObject(Form("h_bpm%d_%d_%d_%d",ibpm,ich,i,myii));
	adc_mean[ich][i] = h_stat->GetMean();
	adc_error[ich][i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
      c_fit->cd(ich+1);
      
      g_fit[ich] = new TGraphErrors(ndata,unser_mean,adc_mean[ich],unser_error,adc_error[ich]);
      g_fit[ich]->SetMarkerStyle(20);
      g_fit[ich]->Draw("AP");
      gfit_title[ich] = Form("%s;%s; Raw ADC",branch_name.Data(),scan_data.Data());
      g_fit[ich]->SetTitle(gfit_title[ich].Data());

      f_fit->SetParameters(init_par);
      f_fit->SetLineColor(kRed);
      g_fit[ich]->Fit("f_fit","Q","",MIN,MAX);
      // f_fit->Draw("same");

      ped[ibpm][ich] = f_fit->GetParameter(0);
      slope[ibpm][ich] = f_fit->GetParameter(1);
      
      for(int i=0;i<ndata;i++)
	adc_res[ich][i] = adc_mean[ich][i] - f_fit->Eval(unser_mean[i]);
      
      c_res->cd(ich+1);
      g_res[ich] = new TGraphErrors(ndata,unser_mean,adc_res[ich],unser_error,adc_error[ich]);
      g_res[ich]->SetMarkerStyle(20);
      g_res[ich]->Draw("AP");

      gres_title[ich] = Form("%s;%s; Residual ",branch_name.Data(),scan_data.Data());
      g_res[ich]->SetTitle(gres_title[ich].Data());
      f_zero->Draw("same");
    } // End ofW Wires loop
    c3->SaveAs(Form("%s/run%d_bpm%s_ped_fit.png",outputDir.Data(),run_num,device_name[ibpm].Data()));
  } // End of BPMs loop
  gSystem->Exec(Form("convert %s/*bpm*fit.png %s/run%d_bpm_pedestal_fit.pdf",outputDir.Data(),outputDir.Data(),run_num));
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

  delete rootfile, tree, outputDir, device_name, nBPM, ch_name, nbpm,f_zero, branch_name, num_samples_name, ndata, adc_mean, adc_error, adc_res, unser_mean, unser_error, gfit_title, gres_title, ped, slope, outfilename, outfile, f_fit, init_par,my_cut, max, htemp,  nbinx, vec_scandata, hsd, bin_content, bin_center;
  //max, htemp
}
