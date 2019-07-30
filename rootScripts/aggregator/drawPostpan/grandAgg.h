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
TString cut1="run_number==1 && split_n==1"+ gc1+ gc2;
TString cut2="run_number==2 && split_n==1" + gc1+ gc2;

TTreeFormula f("name",chan_name, chan_t);

if(f.GetNdim()!=0){
TCanvas *c=new TCanvas(chan_name, chan_name, 1200,800);
c->Divide(2,2);




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

gStyle->SetOptFit(1);

c->cd(1);
g1.SetMarkerColor(kBlue);
g1.SetMarkerStyle(20);
auto g1_clone=*(TGraphErrors*) g1.Clone();
g1_clone.Fit("pol0");
g1_clone.SetTitle(chan_name+" vs slug (IHWP: In)");
g1_clone.Draw("ap");


c->cd(2);
g2.SetMarkerColor(kRed);
g2.SetMarkerStyle(21);
g2_cor.SetMarkerColor(kRed);
g2_cor.SetMarkerStyle(21);
auto g2_clone=*(TGraphErrors*) g2.Clone();
g2_clone.Fit("pol0");
g2_clone.SetTitle(chan_name+" vs slug (IHWP: Out)");
g2_clone.Draw("ap");



c->cd(3);
TMultiGraph mg;
mg.Add(&g1);
mg.Add(&g2);
mg.SetTitle(chan_name+" vs slug");
mg.Draw("ap"); 
mg.Fit("pol0");
c->cd(4);
TMultiGraph mg_cor;
mg_cor.Add(&g1);
mg_cor.Add(&g2_cor);
mg_cor.SetTitle(chan_name+" vs slug (sign corrected)");
mg_cor.Draw("ap");
mg_cor.Fit("pol0");
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




