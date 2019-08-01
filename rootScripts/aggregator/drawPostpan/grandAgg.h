using namespace ROOT;




/******* Channel class for storing information regarding a particular device *******/

// Class initialization
class Channel{ 
    public: 
    TString name;                           // Name of channel. Ends with _mean, _rms, or _slope according to aggregator convention.
    double value;                           // value for the slug
    double value_err;                       // error for the slug 
    double value_cor;                       // sign corrected value for the slug
    double value_cor_err;                   // error on sign corrected value for the slug
  
    Channel (TString a): name (a)  {};
    void draw(TTree *, TString);
    TGraphErrors drawReg(Int_t, Double_t *, Double_t *, Double_t *);
    void printInfo() { std::cout << "Plotting "<< name<<" vs slug number" << endl;}
};  


void Channel::draw(TTree* tree,TString output){
auto chan_name=this->name;
auto chan_t=tree;

// global cuts
TString gc1= "&&"+ chan_name+"!=0 &&"+ chan_name+"!=-1e6 && "+ chan_name+"!=1e6";   
TString gc2= "&&"+ chan_name+"_error!=0 &&"+chan_name+"_error!=-1e6 && "+chan_name+"_error!=1e6";
//
// device cut
TString lc="";
if (chan_name.Contains("usl")|| chan_name.Contains("dsl") || chan_name.Contains("left_avg") || chan_name.Contains("left_dd")){
    lc+= "&& hrs!=1";
}
if (chan_name.Contains("usr")|| chan_name.Contains("dsr") || chan_name.Contains("right_avg") || chan_name.Contains("right_dd")){
    lc+= "&& hrs!=2";
}
if (chan_name.Contains("us_avg")|| chan_name.Contains("ds_avg") || chan_name.Contains("us_dd") || chan_name.Contains("ds_dd")){
    lc+= "&& hrs!=1 && hrs!=2";
}
//
TString cut1="ihwp==1 && wein==1"+ gc1+ gc2+ lc;
TString cut2="ihwp==2 && wein==1" + gc1+ gc2+ lc;

TTreeFormula f("name",chan_name, chan_t);

if(f.GetNdim()!=0){
TCanvas *c=new TCanvas(chan_name, chan_name, 1200,800);

Int_t nEntries=chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error",cut1 , "goff");
TGraphErrors g1 = drawReg(nEntries, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());
    
nEntries= chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error",cut2 ,  "goff");
TGraphErrors g2 = drawReg(nEntries, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());

if (  !chan_name.Contains("yield") &&  !chan_name.Contains("rms")){ 
for( auto i=0; i< nEntries; i ++){
  std::cout<< chan_name<<" : "<< *(chan_t->GetV1()+i) << std::endl;
 *(chan_t->GetV2()+i)=-1*(*(chan_t->GetV2()+i));  
}
}
TGraphErrors g2_cor = drawReg(nEntries, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3());
/*
nEntries.push_back(chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error","run_number==1 && split_n==2",  "goff");
nEntries.push_back(chan_t->Draw("n_runs:"+chan_name+":"+chan_name+"_error","run_number==2 && split_n==2",  "goff");  
*/

gStyle->SetOptFit(0);

TF1* fconst1=new TF1("fconst1", "pol0");
TF1* fconst2=new TF1("fconst2", "pol0");
TF1* fconst3=new TF1("fconst3", "pol0");

c->cd(1);
g1.SetMarkerColor(kBlue);
g1.SetMarkerStyle(20);
fconst1->SetLineColor(kBlue);
g1.Fit(fconst1,"Q");

g2.SetMarkerColor(kRed);
g2.SetMarkerStyle(21);
g2_cor.SetMarkerColor(kRed);
g2_cor.SetMarkerStyle(21);
fconst2->SetLineColor(kRed);
g2.Fit(fconst2,"Q");
fconst3->SetLineColor(kRed);
g2_cor.Fit(fconst3,"Q");

TMultiGraph mg;
mg.Add(&g1);
mg.Add(&g2);
mg.SetTitle(chan_name+" vs slug");

gPad->Modified();
mg.SetMaximum(g1.GetMean(2)+6*g1.GetRMS(2));
mg.SetMinimum(g2.GetMean(2)-6*g2.GetRMS(2));

mg.Draw("ap");
TLatex *pt1= new TLatex(0.25,0.85, Form("In:%3.3f+/-%3.3f\t #chi^{2}/NDF:%3.1f/%d",fconst1->GetParameter(0), fconst1->GetParError(0),fconst1->GetChisquare(), fconst1->GetNDF() ));
pt1->SetNDC();
pt1->SetTextFont(43);
pt1->SetTextSize(40);
pt1->SetTextColor(kBlue);
pt1->Draw();
TLatex *pt2= new TLatex(0.25,0.15, Form("Out:%3.3f+/-%3.3f\t #chi^{2}/NDF:%3.1f/%d",fconst2->GetParameter(0), fconst2->GetParError(0),fconst2->GetChisquare(), fconst2->GetNDF() ));
pt2->SetNDC();
pt2->SetTextFont(43);
pt2->SetTextSize(40);
pt2->SetTextColor(kRed);
pt2->Draw();
/*
c->cd(1);
TMultiGraph mg_cor;
mg_cor.Add(&g1);
mg_cor.Add(&g2_cor);
mg_cor.SetTitle(chan_name+" vs slug (sign corrected)");
mg_cor.Draw("ap");
*/
c->Print(output+".pdf");
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
       
  public:
    std::vector<Channel> list;
    TChain *T;   
    Source (TString a, TString b, TString c): file(a), tree(b), output(c) {T=new TChain(tree); T->Add(file);}
    void printInfo() { std::cout << "Reading from  " << tree  << " tree in file " << std::endl;} 
    void drawAll();
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
     channel.printInfo();
     channel.draw(this->T, filename);
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




