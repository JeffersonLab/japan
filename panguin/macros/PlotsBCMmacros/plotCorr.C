void plotCorr(Int_t runnum, Double_t fit_RMS = 3.5, TString first = "asym_bcm_an_ds3", TString first_unit = "ppm", TString tree = "mul"){

  Int_t showSubBlocks = 0;
  //The variables we want to find correlations for
  const Int_t n_vars = 17;
  //const Int_t n_vars = 5;
  const Int_t n_blocks = 1;
  //Total # of pairs/ block = n_vars choose 2
  //const Int_t n_combo = (Int_t)TMath::Factorial(n_vars)/2/(Int_t)TMath::Factorial(n_vars-2);
  const Int_t n_combo = n_vars-1;
  cout << "There are " << n_combo << " variable combinations. \n";
  
  //TString vars[n_vars] = {"diff_bpm4aX", "diff_bpm4aY", "diff_bpm4eX", "diff_bpm4eY", "diff_bpm12X", "asym_bpm4aWS", "asym_bpm4eWS", "asym_bpm12WS", "asym_bcm_an_ds3", "asym_bcm_an_us", "asym_usl", "asym_usr", "asym_dsl", "asym_dsr"};
  
  
  TString vars[n_vars] = {first,"asym_bcm_an_ds3","asym_bcm_an_ds","asym_bcm_an_us","asym_bcm_dg_ds","asym_bcm_dg_us","asym_cav4bQ","asym_cav4cQ","asym_cav4dQ","asym_bpm12WS","asym_bpm4eWS","asym_bpm4aWS","asym_bpm8WS","asym_bpm14WS","asym_bpm1WS","asym_bpm4ecWS","asym_bpm4acWS"};
  TString units[n_vars] = {first_unit,"ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm","ppm"};
  //TString vars[n_vars] = {"asym_bcm_an_ds3","asym_bcm_an_ds3","asym_bcm_an_us","asym_bcm_dg_ds","asym_cav4bQ"};
  //TString units[n_vars] = {"ppm","ppm","ppm","ppm","ppm"};

  //TString vars[n_vars] = {"diff_bpm4aX", "diff_bpm4aY", "diff_bpm4eX", "diff_bpm4eY", "diff_bpm12X"};
  //TString vars[n_vars] = {"asym_bpm4aWS", "asym_bpm4eWS", "asym_bpm12WS", "asym_bcm_an_ds3", "asym_bcm_an_us"};

  //TString blocks[n_blocks] = {"block0", "block1", "block2","block3","hw_sum"};
  TString blocks[n_blocks] = {"hw_sum"};

  //Open the file and get the tree
  //TFile * file = new TFile(Form("$QW_ROOTFILES/prexALL_%i.000.root", runnum));
  TFile * file = new TFile(Form("$QW_ROOTFILES/prexPrompt_pass2_%i.000.root", runnum));
  TTree *ttree = (TTree*)file->Get(tree);
  TString cutsX = "ErrorFlag==0";
  TString cutsY = "ErrorFlag==0";
  TString cutsDD = "ErrorFlag==0";
  TString cuts = "ErrorFlag==0";

  gStyle->SetOptStat(1221);
  gStyle->SetOptFit(1);
  //Keep track of the subcanvas number
  Int_t n_canvas = 1;
  Int_t n_1D_canvas = 1;
  //Give a unique number so it doesn't get the histograms confused
  Int_t n_plot = 0;
  //Everything we need to plot things
  TCanvas *canvas[n_blocks];
  //TCanvas *canvas1DX[n_blocks];
  TCanvas *canvas1DY[n_blocks];
  TCanvas *canvas1DD[n_blocks];
  TString x_var, y_var;
  //Total number of plots is the number of plot combinations per block * number of blocks
  Double_t gaus_dd_mean[n_combo];
  Double_t gaus_dd_mean_error[n_combo];
  Double_t gaus_dd_width[n_combo];
  Double_t gaus_dd_width_error[n_combo];
  Double_t gaus_asym_mean[n_combo];
  Double_t gaus_asym_mean_error[n_combo];
  Double_t gaus_asym_width[n_combo];
  Double_t gaus_asym_width_error[n_combo];
  Double_t slope[n_combo*n_blocks];
  Double_t slope_error[n_combo*n_blocks];
  Double_t offset[n_combo*n_blocks];
  Double_t offset_error[n_combo*n_blocks];
  TString slope_n[n_combo*n_blocks];

  //Open a file to write to
  FILE *fp;
  fp = fopen(Form("Output_%s_%s_%d_BCMs.txt", tree.Data(), first.Data(), runnum), "w");
  FILE *fpg;
  fpg = fopen(Form("Output_Gaus_%s_%s_%d_BCMs.txt", tree.Data(), first.Data(), runnum), "w");
  FILE *fpgdd;
  fpgdd = fopen(Form("Output_Gaus_DD_%s_%s_%d_BCMs.txt", tree.Data(), first.Data(), runnum), "w");
  //fp = fopen(Form("Output_%d_ASYMs.txt", runnum), "w");
  if(fp!=NULL){
    fprintf(fp, "Correlation, slope, slope error, offset, offset error\n");
  }
  if(fpg!=NULL){
    fprintf(fpg, "Asym, mean, mean error, width, width error\n");
  }
  if(fpgdd!=NULL){
    fprintf(fpgdd, "Double Difference, mean, mean error, width, width error\n");
  }


  //For each block // Looks at diagonal terms in block vs. block, and hw_sum
  // To look at off diagonal terms swap these two for loops to be n_vars and then n_blocks instead.
  for(int b = 0 ; b < n_blocks ; b++){
    canvas[b] = new TCanvas(Form("c%s", blocks[b].Data()), Form("c%s", blocks[b].Data()));
    //canvas1DX[b] = new TCanvas(Form("c1DX%s", blocks[b].Data()), Form("c1DX%s", blocks[b].Data()));
    canvas1DY[b] = new TCanvas(Form("c1DY%s", blocks[b].Data()), Form("c1DY%s", blocks[b].Data()));
    canvas1DD[b] = new TCanvas(Form("c1DD%s", blocks[b].Data()), Form("c1DD%s", blocks[b].Data()));
    if (n_combo/4 >= 1 && n_combo/4<2){
      canvas[b] -> Divide(n_combo/1, 1);
      canvas1DY[b] -> Divide(n_combo/1, 1);
      canvas1DD[b] -> Divide(n_combo/1, 1);
    }
    else if (n_combo/4 >= 2 && n_combo/4<3){
      canvas[b] -> Divide(n_combo/2, 2);
      canvas1DY[b] -> Divide(n_combo/2, 2);
      canvas1DD[b] -> Divide(n_combo/2, 2);
    }
    else if (n_combo/4 >= 3 && n_combo/4<4){
      canvas[b] -> Divide(n_combo/3, 3);
      canvas1DY[b] -> Divide(n_combo/3, 3);
      canvas1DD[b] -> Divide(n_combo/3, 3);
    }
    else if (n_combo/4 >= 4 && n_combo/4<5){
      canvas[b] -> Divide(n_combo/3, 3);
      canvas1DY[b] -> Divide(n_combo/3, 3);
      canvas1DD[b] -> Divide(n_combo/3, 3);
    }
    else {
      canvas[b] -> Divide(n_combo/5, 5);
      canvas1DY[b] -> Divide(n_combo/5, 5);
      canvas1DD[b] -> Divide(n_combo/5, 5);
    }
    n_canvas = 1;
    n_1D_canvas = 1;

    //Plot each variable against the other // To look at off diagonal terms swap these two for loops to be n_vars and then n_blocks instead.
    for(int i = 0 ; i < (showSubBlocks!=0? n_vars : 1); i++){ // Set showSubBlocks=1 to do all subblocks vs all
      for(int j = i+1; j < n_vars; j++){
        //canvas[b]->cd(n_canvas);
        canvas1DD[b]->cd(n_canvas);

        x_var = Form("%s.%s/%s", vars[i].Data(), blocks[b].Data(), units[i].Data());
        y_var = Form("%s.%s/%s", vars[j].Data(), blocks[b].Data(), units[j].Data());
        cout << "x_var = " << x_var << " y_var = " << y_var << "\n";

        //Only fit a line to the data that goes from -2*RMS to 2*RMS if RMS = 1 is selected
        TString cut_RMS;
        if(fit_RMS == 0)
          cut_RMS = Form("%s && %s.Device_Error_Code==0", cuts.Data(),y_var.Data());

        else{
          n_1D_canvas = 0;
          cutsX = Form("ErrorFlag==0 && %s.Device_Error_Code==0",vars[i].Data()); 
          ttree->Draw(Form("%s>>hx_%d", x_var.Data(), n_plot), cutsX.Data()); 
          //canvas1DX[b]->cd();
          //ttree->Draw(Form("%s>>hx_%d", x_var.Data(), n_plot), cutsX.Data()); 
          cout<<"Draw: "<<Form("%s>>hx_%d", x_var.Data(), n_plot)<<"\nCuts: "<<cutsX.Data()<<endl; 
          TH1 *hx = (TH1*)gROOT -> FindObject(Form("hx_%d", n_plot));
          Double_t x_rms = hx -> GetRMS();
          //n_1D_canvas++;

          //Find the RMS values of the x and y variables
          cutsDD = Form("ErrorFlag==0 && %s.Device_Error_Code==0 && %s.Device_Error_Code==0",vars[j].Data(),vars[i].Data()); 
          //ttree->Draw(Form("(%s-%s)/sqrt(2)>>h_DoubleDifference_%d", y_var.Data(), x_var.Data(), n_plot), cutsDD.Data()); 
          ttree->Draw(Form("(%s-%s)/sqrt(2)>>h_DoubleDifference_%s_%s", y_var.Data(), x_var.Data(), y_var.Data(), x_var.Data()), cutsDD.Data()); 
          //TH1 *h_DoubleDifference = (TH1*)gROOT -> FindObject(Form("h_DoubleDifference_%d", n_plot));
          TH1 *h_DoubleDifference = (TH1*)gROOT -> FindObject(Form("h_DoubleDifference_%s_%s",y_var.Data(),x_var.Data()));
          if (h_DoubleDifference->GetEntries()!=0){
            h_DoubleDifference -> GetXaxis() -> SetTitle(x_var + " DD " + y_var);
            h_DoubleDifference -> SetTitle(Form("%s DD %s", y_var.Data(), x_var.Data()));
            h_DoubleDifference -> Fit("gaus");
            cout << h_DoubleDifference->GetFunction("gaus")->GetParameter(1)<<" +- "; 
            cout << h_DoubleDifference->GetFunction("gaus")->GetParError(1) << "\n";
            cout << h_DoubleDifference->GetFunction("gaus")->GetParameter(2)<<" +- "; 
            cout << h_DoubleDifference->GetFunction("gaus")->GetParError(2) << "\n";
            gaus_dd_mean[j-1] = h_DoubleDifference->GetFunction("gaus")->GetParameter(1); 
            gaus_dd_mean_error[j-1] = h_DoubleDifference->GetFunction("gaus")->GetParError(1);
            gaus_dd_width[j-1] = h_DoubleDifference->GetFunction("gaus")->GetParameter(2); 
            gaus_dd_width_error[j-1] = h_DoubleDifference->GetFunction("gaus")->GetParError(2);
          }
          else {
            gaus_asym_mean[j-1] = 0;
            gaus_asym_mean_error[j-1] = 0;
            gaus_asym_width[j-1] = 0;
            gaus_asym_width_error[j-1] = 0; 
          }
          h_DoubleDifference->Draw();
          canvas1DD[b]->cd(n_canvas);
          canvas1DD[b]->cd(n_canvas)->SetLogy(1);
          h_DoubleDifference->Draw();
          //ttree->Draw(Form("(%s-%s)/sqrt(2)>>h_DoubleDifference_%d", y_var.Data(), x_var.Data(), n_plot), cutsDD.Data()); 
          //TH1 *h_DoubleDifference = (TH1*)gROOT -> FindObject(Form("h_DoubleDifference_%d", n_plot));
          Double_t ddx_rms = h_DoubleDifference -> GetRMS();
          n_1D_canvas++;

          canvas1DY[b]->cd(n_canvas);
          cutsY = Form("ErrorFlag==0 && %s.Device_Error_Code==0",vars[j].Data());
          //ttree->Draw(Form("%s>>h_Asym_%d", y_var.Data(), n_plot), cutsY.Data()); 
          ttree->Draw(Form("%s>>h_Asym_%s", y_var.Data(), y_var.Data()), cutsY.Data()); 
          //TH1 *h_Asym = (TH1*)gROOT -> FindObject(Form("h_Asym_%d", n_plot));
          TH1 *h_Asym = (TH1*)gROOT -> FindObject(Form("h_Asym_%s",y_var.Data()));
          if (h_Asym->GetEntries()!=0){
            h_Asym -> GetXaxis() -> SetTitle(y_var);
            h_Asym -> SetTitle(Form("Asym_%s", y_var.Data()));
            h_Asym -> Fit("gaus");
            cout << h_Asym->GetFunction("gaus")->GetParameter(1)<<" +- "; 
            cout << h_Asym->GetFunction("gaus")->GetParError(1) << "\n";
            cout << h_Asym->GetFunction("gaus")->GetParameter(2)<<" +- "; 
            cout << h_Asym->GetFunction("gaus")->GetParError(2) << "\n";
            gaus_asym_mean[j-1] = h_Asym->GetFunction("gaus")->GetParameter(1); 
            gaus_asym_mean_error[j-1] = h_Asym->GetFunction("gaus")->GetParError(1);
            gaus_asym_width[j-1] = h_Asym->GetFunction("gaus")->GetParameter(2); 
            gaus_asym_width_error[j-1] = h_Asym->GetFunction("gaus")->GetParError(2);
          }
          else {
            gaus_asym_mean[j-1] = 0;
            gaus_asym_mean_error[j-1] = 0;
            gaus_asym_width[j-1] = 0;
            gaus_asym_width_error[j-1] = 0; 
          }
          h_Asym->Draw();
          canvas1DY[b]->cd(n_canvas);
          canvas1DY[b]->cd(n_canvas)->SetLogy(1);
          h_Asym->Draw();
          //ttree->Draw(Form("%s>>h_Asym_%d", y_var.Data(), n_plot), cutsY.Data()); 
          //cout<<"Draw: "<<Form("%s>>h_Asym_%d", y_var.Data(), n_plot)<<"\nCuts: "<<cutsY.Data()<<endl; 
          cout<<"Draw: "<<Form("%s>>h_Asym_%s", y_var.Data(), y_var.Data())<<"\nCuts: "<<cutsY.Data()<<endl; 
          //TH1 *h_Asym = (TH1*)gROOT -> FindObject(Form("h_Asym_%d", n_plot));
          Double_t y_rms = h_Asym -> GetRMS();
          //cut_RMS = Form("%s && %s>-%f*%f&&%s<%f*%f && %s>-%f*%f&&%s<%f*%f", cutsDD.Data(), y_var.Data(), x_var.Data(), fit_RMS, x_rms, x_var.Data(), fit_RMS, x_rms, y_var.Data(), fit_RMS, y_rms, y_var.Data(), fit_RMS, y_rms);
          cut_RMS = Form("%s && %s>-%f*%f&&%s<%f*%f && %s>-%f*%f&&%s<%f*%f", cutsDD.Data(), y_var.Data(), fit_RMS, y_rms, y_var.Data(), fit_RMS, y_rms, x_var.Data(), fit_RMS, x_rms, x_var.Data(), fit_RMS, x_rms);

        }

        //Now draw the plot and make the fit.
        canvas[b]->cd(n_canvas);
        canvas[b]->cd(n_canvas)->SetLogy(0);
        ttree->Draw(Form("%s:%s>>h_xy_%d", y_var.Data(), x_var.Data(), n_plot), cut_RMS.Data(),"box");
        cout<<Form("Draw: %s:%s>>h_xy_%d", y_var.Data(), x_var.Data(), n_plot)<<"\nCuts:"<<cut_RMS.Data();
        TH1 *h_xy = (TH1*)gROOT -> FindObject(Form("h_xy_%d", n_plot));
        if (h_xy->GetEntries()!=0){
          h_xy -> Fit("pol1");
          h_xy -> SetTitle(Form("%s vs %s", y_var.Data(), x_var.Data()));
          h_xy -> GetXaxis() -> SetTitle(x_var);
          h_xy -> GetYaxis() -> SetTitle(y_var);
//        h_xy -> SetStats(1);
          h_xy -> Draw("box");

          cout << h_xy->GetFunction("pol1")->GetParameter(1)<<" +- "; 
          cout << h_xy->GetFunction("pol1")->GetParError(1) << "\n";

          offset[n_plot] = h_xy->GetFunction("pol1")->GetParameter(0);
          offset_error[n_plot] = h_xy->GetFunction("pol1")->GetParError(0);
          slope[n_plot] = h_xy->GetFunction("pol1")->GetParameter(1);
          slope_error[n_plot] = h_xy->GetFunction("pol1")->GetParError(1);
        }
        else {
          offset[n_plot] = 0.0;
          offset_error[n_plot] = 0.0;
          slope[n_plot] = 0.0;
          slope_error[n_plot] = 0.0;
        }
        slope_n[n_plot] = vars[j].Data();
        slope_n[n_plot].Append(" vs. ");
        slope_n[n_plot].Append(vars[i].Data());

        if(fp!=NULL)  
          fprintf(fp, "%s:%s,%f,%f,%f,%f\n", x_var.Data(), y_var.Data(), slope[n_plot], slope_error[n_plot], offset[n_plot], offset_error[n_plot]);
        if(fpg!=NULL){
          fprintf(fpg, "%s,%f,%f,%f,%f\n",y_var.Data(), gaus_asym_mean[j-1] , gaus_asym_mean_error[j-1] , gaus_asym_width[j-1] , gaus_asym_width_error[j-1]);
        }
        if(fpgdd!=NULL){
          fprintf(fpgdd, "%s-%s,%f,%f,%f,%f\n",x_var.Data(), y_var.Data(), gaus_dd_mean[j-1] , gaus_dd_mean_error[j-1] , gaus_dd_width[j-1] , gaus_dd_width_error[j-1]);
        }
        n_canvas++;
        n_plot++;
      }
      if (b==0){
        canvas[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf(",runnum,tree.Data(),first.Data()));
        canvas1DY[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf",runnum,tree.Data(),first.Data()));
        if (n_blocks == 1){
          canvas1DD[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf)",runnum,tree.Data(),first.Data()));
        }
        else {
          canvas1DD[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf",runnum,tree.Data(),first.Data()));
        }
      }
      else if (b<(n_blocks-1)) {
        canvas[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf",runnum,tree.Data(),first.Data()));
        canvas1DY[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf",runnum,tree.Data(),first.Data()));
        canvas1DD[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf",runnum,tree.Data(),first.Data()));
      }
      else {
        canvas[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf",runnum,tree.Data(),first.Data()));
        canvas1DY[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf",runnum,tree.Data(),first.Data()));
        canvas1DD[b]->SaveAs(Form("run_%d_%s_%s_vs.pdf)",runnum,tree.Data(),first.Data()));
      }
    }
  }
  cout << "We have made " << n_plot << " total plots." << endl;
  fclose(fp);
  fclose(fpg);
  fclose(fpgdd);

  if (showSubBlocks == 1){ // FIXME For plotting sub-block changes

    //Plot the correlation slopes and their errors for each set of variables
    //4 data points per graph - one for each block
    TCanvas *c_corr[n_combo];
    TGraphErrors *g_corr[n_combo];
    Double_t g_slope[n_blocks];
    Double_t g_slope_ey[n_blocks];
    Double_t block_num[n_blocks];
    Double_t g_slope_ex[n_blocks];
    for (Int_t k = 0 ; k<n_blocks; k++) {
      block_num[k] = k; // 4 is the hw_sum
      g_slope[k] = 0;    
      g_slope_ex[k] = 0;    
      g_slope_ey[k] = 0;    
    }

    cout << "There are " << n_combo*n_blocks << " plots total with " << n_vars << " different variables and " << n_combo << "TGraphs to make \n"; 
    for(int i = 0; i < n_combo; i++){
      c_corr[i] = new TCanvas(Form("c%d", i), Form("c%d", i));
      //Each block has ncombo/nvar slopes in it. Get the slopes from the same two variables plotted against each other
      for(int j = 0; j < n_blocks; j++){
        g_slope[j]=slope[i+j*n_combo];
        g_slope_ey[j]=slope_error[i+j*n_combo];
        cout << "The slope is: " << g_slope[j] << " and the error is " << g_slope_ey[j] << "\n";
      }
      g_corr[i] = new TGraphErrors(n_blocks, block_num, g_slope, g_slope_ex, g_slope_ey);
      g_corr[i] -> SetTitle(slope_n[i]);
      g_corr[i] -> GetXaxis()->SetTitle("Block Number");
      g_corr[i] -> GetXaxis()->SetRangeUser(-0.5,4.5);
      g_corr[i] -> GetYaxis()->SetTitle("Slope");
      g_corr[i]->Draw("AP");
    }
  }
}
