using namespace ROOT;


/*
ihwp = 1 -> IN
ihwp = 2 -> OUT

wien = 1 -> Flip Right
wien = 2 -> Flip Left

arm = 1 -> right HRS is good
arm = 2 -> left  HRS is good
arm = 0 -> Both good
*/



/******* Channel class for storing information regarding a particular device *******/

// Class initialization
class Channel{ 
    public: 
    TString name;          // Name of channel. Ends with _mean, _rms, or _slope according to aggregator convention.
    Int_t signCorrect = 0; // Whether or not to perform sign correction
    // FIXME Correction is applied to all values, and that isn't realistic for non-diff/asym quantities
    double value;          // value for the experiment
    double value_err;      // error for the experiment
    double value_Chi2;     // Chi2 for the experiment
    int value_NDOF;     // NDOF for the experiment
    double value_R;        // value for the R wien for the experiment
    double value_R_err;    // error for the R wien for the experiment
    double value_R_Chi2;   // Chi2 for the R wien for the experiment
    int value_R_NDOF;   // NDOF for the R wien for the experiment
    double value_L;
    double value_L_err;
    double value_L_Chi2;
    int value_L_NDOF;
    double value_IN;       // value for in HWP for the experiment
    double value_IN_err;   // error for in HWP for the experiment
    double value_IN_Chi2;  // Chi2 for in HWP for the experiment
    int value_IN_NDOF;  // NDOF for in HWP for the experiment
    double value_OUT;
    double value_OUT_err;
    double value_OUT_Chi2;
    int value_OUT_NDOF;
    double value_R_IN;
    double value_R_IN_err;
    double value_R_IN_Chi2;
    int value_R_IN_NDOF;
    double value_L_IN;
    double value_L_IN_err;
    double value_L_IN_Chi2;
    int value_L_IN_NDOF;
    double value_R_OUT;
    double value_R_OUT_err;
    double value_R_OUT_Chi2;
    int value_R_OUT_NDOF;
    double value_L_OUT;
    double value_L_OUT_err;
    double value_L_OUT_Chi2;
    int value_L_OUT_NDOF;
  
    Channel (TString a): name (a)  {};
    void draw(TTree *, TString);
    void drawRange(TTree *, TString, Int_t, Int_t);
    TGraphErrors drawReg(Int_t, Double_t *, Double_t *, Double_t *);
    void printInfo() { std::cout << "Plotting "<< name<<" vs slug number" << endl;}
};  

//Draw, but only in a range of slug numbers
void Channel::drawRange(TTree* tree,TString output, Int_t min = 0, Int_t max = 100000){
  // Overload draw() by default initializing range to 0-10000
  // But enable user specified cutting if wanted
  auto chan_name=this->name;
  auto chan_t=tree;
  TTree* cut_tree = tree->CopyTree(Form("n_runs>=%d && n_runs<=%d",min,max));
  draw(cut_tree, output);
}

void Channel::draw(TTree* tree,TString output){
  auto chan_name=this->name;
  auto chan_t=tree;

  // global cuts
  TString gc1= "&&"+ chan_name+"!=0 && abs("+ chan_name+")<0.99e6";
  TBranch* br1 = (TBranch*)chan_t->GetListOfBranches()->FindObject(chan_name+"_error");
  TString gc2 = "";
  if (br1) {
    gc2= "";
  }
  else {
    gc2= "&&"+ chan_name+"_error!=0 && abs("+chan_name+"_error)<0.99e6";
  }
  //
  // device cut
  TString lc="";
  if (chan_name.Contains("usl")   || chan_name.Contains("dsl")    || chan_name.Contains("left_avg")  || chan_name.Contains("left_dd")){
    lc+= "&& hrs!=1";
  }
  if (chan_name.Contains("usr")   || chan_name.Contains("dsr")    || chan_name.Contains("right_avg") || chan_name.Contains("right_dd")){
    lc+= "&& hrs!=2";
  }
  if (chan_name.Contains("us_avg")|| chan_name.Contains("ds_avg") || chan_name.Contains("us_dd")     || chan_name.Contains("ds_dd")){
    lc+= "&& hrs!=1 && hrs!=2";
  }
  //
  TString cut1="ihwp==1 && wein==1"+gc1+gc2+lc;
  TString cut2="ihwp==2 && wein==1"+gc1+gc2+lc;
  TString cut3="ihwp==1 && wein==2"+gc1+gc2+lc;
  TString cut4="ihwp==2 && wein==2"+gc1+gc2+lc;

  TTreeFormula f("name",chan_name, chan_t);

  if(f.GetNdim()!=0){
    TCanvas c(chan_name, chan_name, 1200,1000);

    std::map<TString,Int_t> sign={{cut1,1}, {cut2,-1}, {cut3,-1}, {cut4,1}};

    TBranch* br2 = (TBranch*)chan_t->GetListOfBranches()->FindObject(chan_name+"_error");
    Int_t nEntries_R_IN = 0;
    Int_t nEntries_R_OUT = 0;
    Int_t nEntries_L_IN = 0;
    Int_t nEntries_L_OUT = 0;
    if (br2) {
      nEntries_R_IN=chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error",cut1 , "goff");
    }
    else {
      nEntries_R_IN=chan_t->Draw("n_runs:"+chan_name+":0",cut1 , "goff");
    }
    if ( signCorrect && !chan_name.Contains("yield") && !chan_name.Contains("rms") && !chan_name.Contains("epics") && !chan_name.Contains("slope") )
    { 
      for( auto i=0; i< nEntries_R_IN; i ++)
      {
        *(chan_t->GetV2()+i)=sign[cut1]*(*(chan_t->GetV2()+i));  
      }
    }
    else {
    }
    TGraphErrors g_R_IN = drawReg(nEntries_R_IN, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());

    if (br2) {
      nEntries_R_OUT= chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error",cut2 ,  "goff");
    }
    else {
      nEntries_R_OUT= chan_t->Draw("n_runs:"+chan_name+":0",cut2 ,  "goff");
    }
    if ( signCorrect && !chan_name.Contains("yield") && !chan_name.Contains("rms") && !chan_name.Contains("epics") && !chan_name.Contains("slope") )
    { 
      for( auto i=0; i< nEntries_R_OUT; i ++)
      {
        *(chan_t->GetV2()+i)=sign[cut2]*(*(chan_t->GetV2()+i));  
      }
    }
    TGraphErrors g_R_OUT = drawReg(nEntries_R_OUT, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());

    if (br2) {
      nEntries_L_IN= chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error", cut3, "goff");
    }
    else {
      nEntries_L_IN= chan_t->Draw("n_runs:"+chan_name+":0", cut3, "goff");
    }
    if ( signCorrect && !chan_name.Contains("yield") && !chan_name.Contains("rms") && !chan_name.Contains("epics") && !chan_name.Contains("slope") )
    { 
      for( auto i=0; i< nEntries_L_IN; i ++)
      {
        *(chan_t->GetV2()+i)=sign[cut3]*(*(chan_t->GetV2()+i));  
      }
    }
    TGraphErrors g_L_IN= drawReg(nEntries_L_IN, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());

    if (br2) {
      nEntries_L_OUT =chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error", cut4, "goff");
    }
    else {
      nEntries_L_OUT =chan_t->Draw("n_runs:"+chan_name+":0", cut4, "goff");
    }
    if ( signCorrect && !chan_name.Contains("yield") && !chan_name.Contains("rms") && !chan_name.Contains("epics") && !chan_name.Contains("slope") )
    { 
      for( auto i=0; i< nEntries_L_OUT; i ++)
      {
        *(chan_t->GetV2()+i)=sign[cut4]*(*(chan_t->GetV2()+i));  
      }
    }
    TGraphErrors g_L_OUT= drawReg(nEntries_L_OUT, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());

    gStyle->SetOptFit(0);

    TF1* fConst_R_IN=new TF1("fConst_R_IN", "pol0");
    TF1* fConst_R_OUT=new TF1("fConst_R_OUT", "pol0");
    TF1* fConst_L_IN=new TF1("fConst_L_IN", "pol0");
    TF1* fConst_L_OUT=new TF1("fConst_L_OUT", "pol0");
    TF1* fConst_ALL=new TF1("fConst_ALL", "pol0");


    g_R_IN.SetMarkerColor(kBlue);
    g_R_OUT.SetMarkerColor(kRed);
    g_L_IN.SetMarkerColor(kMagenta+1);
    g_L_OUT.SetMarkerColor(kOrange+1);
    g_R_IN.SetMarkerStyle(20);
    g_R_OUT.SetMarkerStyle(21);
    g_L_IN.SetMarkerStyle(22);
    g_L_OUT.SetMarkerStyle(23);
    fConst_R_IN->SetLineColor(kBlue);
    if (nEntries_R_IN) g_R_IN.Fit(fConst_R_IN,"Q");
    fConst_R_OUT->SetLineColor(kRed);
    if (nEntries_R_OUT) g_R_OUT.Fit(fConst_R_OUT,"Q");
    fConst_L_IN->SetLineColor(kMagenta+1);
    if (nEntries_L_IN) g_L_IN.Fit(fConst_L_IN, "Q");
    fConst_L_OUT->SetLineColor(kOrange+1);
    if (nEntries_L_OUT) g_L_OUT.Fit(fConst_L_OUT, "Q");
    fConst_ALL->SetLineColor(kGreen+2);

    TMultiGraph mg_ALL_SHOW;
    TMultiGraph mg_ALL_Fit;

    mg_ALL_SHOW.Add(&g_R_IN);
    mg_ALL_SHOW.Add(&g_R_OUT);
    mg_ALL_SHOW.Add(&g_L_IN);
    mg_ALL_SHOW.Add(&g_L_OUT);

    mg_ALL_Fit.Add(&g_R_IN);
    mg_ALL_Fit.Add(&g_R_OUT);
    mg_ALL_Fit.Add(&g_L_IN);
    mg_ALL_Fit.Add(&g_L_OUT);


    TF1* fConst_L   = new TF1("fConst_L", "pol0");
    TF1* fConst_R   = new TF1("fConst_R", "pol0");
    TF1* fConst_IN  = new TF1("fConst_IN", "pol0");
    TF1* fConst_OUT = new TF1("fConst_OUT", "pol0");
    TF1* fConst_Positive = new TF1("fConst_Positive", "pol0");
    TF1* fConst_Negative = new TF1("fConst_Negative", "pol0");
    TMultiGraph mg_L;
    TMultiGraph mg_R;
    TMultiGraph mg_IN;
    TMultiGraph mg_OUT;
    mg_L.Add(&g_L_IN);
    mg_L.Add(&g_L_OUT);
    mg_R.Add(&g_R_IN);
    mg_R.Add(&g_R_OUT);
    mg_IN.Add(&g_L_IN);
    mg_IN.Add(&g_R_IN);
    mg_OUT.Add(&g_L_OUT);
    mg_OUT.Add(&g_R_OUT);
    if (nEntries_L_IN || nEntries_L_OUT)  mg_L.Fit(fConst_L,"Q");
    if (nEntries_R_IN || nEntries_R_OUT)  mg_R.Fit(fConst_R,"Q");
    if (nEntries_L_IN || nEntries_R_IN)   mg_IN.Fit(fConst_IN,"Q");
    if (nEntries_L_OUT || nEntries_L_OUT) mg_OUT.Fit(fConst_OUT,"Q");
    if (signCorrect) {
      mg_ALL_SHOW.SetTitle(chan_name+" vs Slug, Sign Corrected");
    }
    else {
      TMultiGraph mg_Positive;
      TMultiGraph mg_Negative;
      mg_Positive.Add(&g_R_IN);
      mg_Positive.Add(&g_L_OUT);
      mg_Negative.Add(&g_R_OUT);
      mg_Negative.Add(&g_L_IN);
      if (nEntries_R_IN && nEntries_L_OUT)  mg_Positive.Fit(fConst_Positive,"Q");
      if (nEntries_L_IN && nEntries_R_OUT)  mg_Negative.Fit(fConst_Negative,"Q");
      mg_ALL_SHOW.SetTitle(chan_name+" vs Slug");
    }
    mg_ALL_Fit.Fit(fConst_ALL,"Q");

    value            = fConst_ALL->GetParameter(0); // value for the experiment
    value_err        = fConst_ALL->GetParError(0);  // error for the experiment
    value_Chi2       = fConst_ALL->GetChisquare();  // Chi2 for the experiment
    value_NDOF       = fConst_ALL->GetNDF();        // NDOF for the experiment
    value_R          = fConst_R->GetParameter(0); 
    value_R_err      = fConst_R->GetParError(0);  
    value_R_Chi2     = fConst_R->GetChisquare();  
    value_R_NDOF     = fConst_R->GetNDF();        
    value_L          = fConst_L->GetParameter(0); 
    value_L_err      = fConst_L->GetParError(0);  
    value_L_Chi2     = fConst_L->GetChisquare();  
    value_L_NDOF     = fConst_L->GetNDF();        
    value_IN         = fConst_IN->GetParameter(0);
    value_IN_err     = fConst_IN->GetParError(0); 
    value_IN_Chi2    = fConst_IN->GetChisquare(); 
    value_IN_NDOF    = fConst_IN->GetNDF();       
    value_OUT        = fConst_OUT->GetParameter(0); 
    value_OUT_err    = fConst_OUT->GetParError(0);  
    value_OUT_Chi2   = fConst_OUT->GetChisquare();  
    value_OUT_NDOF   = fConst_OUT->GetNDF();        
    value_R_IN       = fConst_R_IN->GetParameter(0);
    value_R_IN_err   = fConst_R_IN->GetParError(0); 
    value_R_IN_Chi2  = fConst_R_IN->GetChisquare(); 
    value_R_IN_NDOF  = fConst_R_IN->GetNDF();       
    value_L_IN       = fConst_L_IN->GetParameter(0);
    value_L_IN_err   = fConst_L_IN->GetParError(0); 
    value_L_IN_Chi2  = fConst_L_IN->GetChisquare(); 
    value_L_IN_NDOF  = fConst_L_IN->GetNDF();       
    value_R_OUT      = fConst_R_OUT->GetParameter(0);
    value_R_OUT_err  = fConst_R_OUT->GetParError(0); 
    value_R_OUT_Chi2 = fConst_R_OUT->GetChisquare(); 
    value_R_OUT_NDOF = fConst_R_OUT->GetNDF();       
    value_L_OUT      = fConst_L_OUT->GetParameter(0);
    value_L_OUT_err  = fConst_L_OUT->GetParError(0); 
    value_L_OUT_Chi2 = fConst_L_OUT->GetChisquare(); 
    value_L_OUT_NDOF = fConst_L_OUT->GetNDF();       

    /*
       if (!chan_name.Contains("_rms")&& !chan_name.Contains("_mean")){
       gPad->Modified();
       mg_ALL_SHOW.SetMaximum(TMath::Max(g_R_IN.GetMean(2),g_L_OUT.GetMean(2))+6*TMath::Max(g_R_IN.GetRMS(2),g_L_OUT.GetRMS(2)));
       mg_ALL_SHOW.SetMinimum(TMath::Min(g_R_OUT.GetMean(2),g_L_IN.GetMean(2))-6*TMath::Max(g_R_OUT.GetRMS(2),g_L_IN.GetRMS(2)));
       }
       */
    c.Clear();
    TPad *p1 = new TPad("p1","",0,0.2,1,1);
    p1->SetBottomMargin(0.08);
    p1->Draw();
    TPad *p2 = new TPad("p2","",0,0,1,0.2);
    p2->SetTopMargin(0);
    p2->SetLeftMargin(0);
    p2->SetRightMargin(0);
    p2->Draw("B");
    p1->cd();

    mg_ALL_SHOW.Draw("ap");
    p2->cd();
    TLatex *pt_R_IN= new TLatex(0.0,0.8, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","Right In:", fConst_R_IN->GetParameter(0), fConst_R_IN->GetParError(0), "#chi^{2}/NDF", fConst_R_IN->GetChisquare(), fConst_R_IN->GetNDF() ));
    pt_R_IN->SetNDC();
    pt_R_IN->SetTextFont(43);
    pt_R_IN->SetTextSize(26);
    pt_R_IN->SetTextColor(kBlue);
    TLatex *pt_R_OUT= new TLatex(0.0,0.4, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","Right Out:", fConst_R_OUT->GetParameter(0), fConst_R_OUT->GetParError(0), "#chi^{2}/NDF", fConst_R_OUT->GetChisquare(), fConst_R_OUT->GetNDF() ));
    pt_R_OUT->SetNDC();
    pt_R_OUT->SetTextFont(43);
    pt_R_OUT->SetTextSize(26);
    pt_R_OUT->SetTextColor(kRed);
    TLatex *pt_L_IN= new TLatex(0.0,0.2, Form("%20s %3.3f+/-%3.3f %15s:%3.1f/%d","Left In:", fConst_L_IN->GetParameter(0), fConst_L_IN->GetParError(0), "#chi^{2}/NDF", fConst_L_IN->GetChisquare(), fConst_L_IN->GetNDF() ));
    pt_L_IN->SetNDC();
    pt_L_IN->SetTextFont(43);
    pt_L_IN->SetTextSize(26);
    pt_L_IN->SetTextColor(kMagenta+1);
    TLatex *pt_L_OUT= new TLatex(0.0,0.6, Form("%20s %3.3f+/-%3.3f %15s:%3.1f/%d","Left Out:", fConst_L_OUT->GetParameter(0), fConst_L_OUT->GetParError(0), "#chi^{2}/NDF", fConst_L_OUT->GetChisquare(), fConst_L_OUT->GetNDF() ));
    pt_L_OUT->SetNDC();
    pt_L_OUT->SetTextFont(43);
    pt_L_OUT->SetTextSize(26);
    pt_L_OUT->SetTextColor(kOrange+1);

    pt_R_IN->Draw();
    pt_R_OUT->Draw();
    pt_L_IN->Draw();
    pt_L_OUT->Draw();

    //if (((((TString)chan_name).Contains("slope") && !signCorrect) || (!(((TString)chan_name).Contains("slope")) && signCorrect))
    if (!signCorrect){
      TLatex *pt_Positive= new TLatex(0.45,0.7, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","+ sign", fConst_Positive->GetParameter(0), fConst_Positive->GetParError(0), "#chi^{2}/NDF", fConst_Positive->GetChisquare(), fConst_Positive->GetNDF() ));
      pt_Positive->SetNDC();
      pt_Positive->SetTextFont(43);
      pt_Positive->SetTextSize(26);
      pt_Positive->SetTextColor(kCyan+4);
      pt_Positive->Draw();
      TLatex *pt_Negative= new TLatex(0.45,0.3, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","- sign", fConst_Negative->GetParameter(0), fConst_Negative->GetParError(0), "#chi^{2}/NDF", fConst_Negative->GetChisquare(), fConst_Negative->GetNDF() ));
      pt_Negative->SetNDC();
      pt_Negative->SetTextFont(43);
      pt_Negative->SetTextSize(26);
      pt_Negative->SetTextColor(kRed+3);
      pt_Negative->Draw();
      TLatex *pt_ALL_normal= new TLatex(0.5,0.5, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","All:", fConst_ALL->GetParameter(0), fConst_ALL->GetParError(0), "#chi^{2}/NDF", fConst_ALL->GetChisquare(), fConst_ALL->GetNDF() ));
      pt_ALL_normal->SetNDC();
      pt_ALL_normal->SetTextFont(43);
      pt_ALL_normal->SetTextSize(26);
      pt_ALL_normal->SetTextColor(kGreen+2);
    
      pt_ALL_normal->Draw();
    }
    else {
      TLatex *pt_L= new TLatex(0.45,0.9, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","Left:", fConst_L->GetParameter(0), fConst_L->GetParError(0), "#chi^{2}/NDF", fConst_L->GetChisquare(), fConst_L->GetNDF() ));
      pt_L->SetNDC();
      pt_L->SetTextFont(43);
      pt_L->SetTextSize(26);
      pt_L->SetTextColor(kCyan+2);
      pt_L->Draw();
      TLatex *pt_R= new TLatex(0.45,0.7, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","Right:", fConst_R->GetParameter(0), fConst_R->GetParError(0), "#chi^{2}/NDF", fConst_R->GetChisquare(), fConst_R->GetNDF() ));
      pt_R->SetNDC();
      pt_R->SetTextFont(43);
      pt_R->SetTextSize(26);
      pt_R->SetTextColor(kCyan+4);
      pt_R->Draw();
      TLatex *pt_IN= new TLatex(0.45,0.5, Form("%20s %3.3f+/-%3.3f %15s:%3.1f/%d","In:", fConst_IN->GetParameter(0), fConst_IN->GetParError(0), "#chi^{2}/NDF", fConst_IN->GetChisquare(), fConst_IN->GetNDF() ));
      pt_IN->SetNDC();
      pt_IN->SetTextFont(43);
      pt_IN->SetTextSize(26);
      pt_IN->SetTextColor(kRed+2);
      pt_IN->Draw();
      TLatex *pt_OUT= new TLatex(0.45,0.3, Form("%20s %3.3f+/-%3.3f %15s:%3.1f/%d","Out:", fConst_OUT->GetParameter(0), fConst_OUT->GetParError(0), "#chi^{2}/NDF", fConst_OUT->GetChisquare(), fConst_OUT->GetNDF() ));
      pt_OUT->SetNDC();
      pt_OUT->SetTextFont(43);
      pt_OUT->SetTextSize(26);
      pt_OUT->SetTextColor(kRed+4);
      pt_OUT->Draw();
      TLatex *pt_ALL= new TLatex(0.5,0.1, Form("%20s %3.3f+/-%3.3f %15s: %3.1f/%d","All:", fConst_ALL->GetParameter(0), fConst_ALL->GetParError(0), "#chi^{2}/NDF", fConst_ALL->GetChisquare(), fConst_ALL->GetNDF() ));
      pt_ALL->SetNDC();
      pt_ALL->SetTextFont(43);
      pt_ALL->SetTextSize(26);
      pt_ALL->SetTextColor(kGreen+2);
      pt_ALL->Draw();

    }

    /*
       c->cd(1);
       TMultiGraph mg_cor;
       mg_cor.Add(&g_R_IN);
       mg_cor.Add(&g2_cor);
       mg_cor.SetTitle(chan_name+" vs slug (sign corrected)");
       mg_cor.Draw("ap");
       */
    p1->cd();
    mg_ALL_SHOW.Draw("ap");
    c.Print(output+".pdf");
  }
}



TGraphErrors Channel::drawReg(Int_t nEntries, Double_t *v1, Double_t *v2, Double_t *v3){
  auto chan_name =  (this->name).Data(); 
  TGraphErrors g(nEntries,v1, v2, 0, v3);
  return g;
}



/***********************************End Class Channel*************************************/




class Source {
    TString file, tree, output;
    Int_t signCorrect = 0;
       
  public:
    std::vector<Channel> list;
    TChain *T;   
    Source (TString a, TString b, TString c, Int_t d): file(a), tree(b), output(c), signCorrect(d) {T=new TChain(tree); T->Add(file);}
    void printInfo() { std::cout << "Reading from  " << tree  << " tree in file " << std::endl;} 
    void drawAll();
    void drawAllRange(Int_t min, Int_t max);
    Channel GetChannelByName(TString name);    

};

void Source::drawAll(){

  auto filename= this->output;

  TObjArray *var_list=(this->T)->GetListOfBranches();
  TIter var_iter(var_list);

  while (auto *var= var_iter.Next()){
    TString name(var->GetName());
    bool createPlotObj = ((name.Contains("_mean") || name.Contains("_slope") || name.Contains("_pslope")|| name.Contains("_rms")) && !(name.Contains("_error"))); 
    if (createPlotObj) {
      Channel channel(var->GetName());     
      channel.signCorrect = this->signCorrect;
      channel.printInfo();
      channel.draw(this->T, filename);
      (this->list).push_back(channel);
    }
  }   
}

void Source::drawAllRange(Int_t min = 0, Int_t max = 10000){

  auto filename= this->output;

  TObjArray *var_list=(this->T)->GetListOfBranches();
  TIter var_iter(var_list);

  while (auto *var= var_iter.Next()){
    TString name(var->GetName());
    bool createPlotObj = ((name.Contains("_mean") || name.Contains("_slope") || name.Contains("_pslope")|| name.Contains("_rms")) && !(name.Contains("_error"))); 
    if (createPlotObj) {
      Channel channel(var->GetName());     
      channel.signCorrect = this->signCorrect;
      channel.printInfo();
      channel.drawRange(this->T, filename, min, max);
      (this->list).push_back(channel);
    }
  }   
}

Channel Source::GetChannelByName(TString name){
  Channel chan("");
  for(auto iter=(this->list).begin();iter!=(this->list).end();iter++){
    if (iter->name==name){
      chan=*iter;
      break;
    }
  } 
  return chan;
}




