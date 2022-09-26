using namespace ROOT;
#include "../camIO.hh"




/******* Channel class for storing information regarding a particular device *******/

// Class initialization
class Channel{ 
    public: 
    TString name;                           // Name of channel. Ends with _mean, _rms, or _slope according to aggregator convention.
    double value;                           // value for the slug
    double value_err;                       // error for the slug 
    double pull;                            // pull mean for the value
    double pull_err;                        // pull error for the value
  
    Channel (TString a): name (a)  {};
    void draw(TTree *, TString);
    TGraphErrors drawReg(Int_t, Double_t *, Double_t *, Double_t *, Double_t *, Double_t *);
    std::vector<TH1F> drawPull(Int_t, Double_t *, Double_t *, Double_t *, Double_t *, Double_t *);
    void printInfo() { std::cout << "Plotting "<< name<<" graph" << endl;}
};  


void Channel::draw(TTree* tree,TString output){
auto chan_name=this->name;
auto chan_t=tree;
TString unit="";
if (chan_name.Contains("slope")){
  unit="*nm/ppb";
} else { 
if (chan_name.Contains("epics_")||chan_name.Contains("slow_")){
  if (chan_name.Contains("_mean")) {unit="/1";} else{unit="/1";}
}
if (chan_name.Contains("asym_")||chan_name.Contains("cor_")){
  if (chan_name.Contains("_mean")) {unit="/ppb";} else{unit="/ppm";}
}
if (chan_name.Contains("bcm_") && (chan_name.Contains("_avg")||chan_name.Contains("_da") || chan_name.Contains("_dd"))){
  if (chan_name.Contains("_mean")) {unit="/ppb";} else{unit="/ppm";}
}
if (chan_name.Contains("diff_")){
  if (chan_name.Contains("_mean")) {unit="/nm";} else{unit="/nm";}
}  
if (chan_name.Contains("bpm_") && (chan_name.Contains("_avg")||chan_name.Contains("_da") || chan_name.Contains("_dd"))){
  if (chan_name.Contains("_mean")) {unit="/nm";} else {unit="/nm";}
}  

if (chan_name.Contains("yield_")){
  if (chan_name.Contains("bpm")) {unit="/mm";}
  if (chan_name.Contains("usl")||chan_name.Contains("dsl") || chan_name.Contains("usr") || chan_name.Contains("dsr")|| chan_name.Contains("sam")) {unit="/mV_uA";}
}
}  

TTreeFormula f("name",chan_name, chan_t);

if(f.GetNdim()!=0){
TBranch* br = (TBranch*)chan_t->GetListOfBranches()->FindObject(chan_name+"_error");
Int_t nEntries = 0;
if (br) {
  //nEntries=chan_t->Draw("run_number:minirun_n:"+chan_name+unit+":"+chan_name+"_error"+unit,"abs("+chan_name+unit+")<0.999e6 && abs("+chan_name+"_error"+unit+")<0.999e6", "goff");
  nEntries=chan_t->Draw("run_number:minirun_n:"+chan_name+unit+":"+chan_name+"_error"+unit,"abs("+chan_name+unit+")<0.999e6 && abs("+chan_name+"_error"+unit+")<0.999e6 && abs("+chan_name+"_error"+unit+")>1.0e-15 ", "goff");
}
else {
  nEntries=chan_t->Draw("run_number:minirun_n:"+chan_name+unit+":1.0e-15","abs("+chan_name+unit+")<0.999e6", "goff");
}
Double_t index[nEntries];
for(int i=0; i<nEntries;i++){
   index[i]=i;
}

TCanvas *c=new TCanvas(chan_name,chan_name, 800,600);
double ySep=0.3, xSep=0.65;
TPad *p1 = new TPad("p1","",0,ySep,xSep,1,21);
TPad *p2 = new TPad("p2","",0,0,xSep,ySep,21);
TPad *p3 = new TPad("p3","",xSep,0,1,1,21);

TGraphErrors g;
std::vector<TH1F> p;

if(chan_name.Contains("_mean")||chan_name.Contains("slope")){
p1->Draw();
p1->UseCurrentStyle();
p2->Draw();
p2->UseCurrentStyle();
p2->SetGrid(1,1);
p3->Draw();
p3->UseCurrentStyle();
p1->cd();
g=this->drawReg(nEntries, index, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3(), chan_t->GetV4());
g.Draw("ap"); 
p1->SetGrid();
p2->cd();
p=this->drawPull(nEntries, index, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3(), chan_t->GetV4());
p[0].Draw("b");
p3->cd();
p[1].Draw("HIST");
}else{
g=this->drawReg(nEntries, index, chan_t->GetV1(), chan_t->GetV2(), chan_t->GetV3(), chan_t->GetV4());
g.Draw("ap");
}

c->Print(output+".pdf");
}
}


TGraphErrors Channel::drawReg(Int_t nEntries, Double_t *index,  Double_t *v1, Double_t *v2, Double_t *v3, Double_t *v4){

auto chan_name =  (this->name).Data(); 
gStyle->SetOptFit(1);
TGraphErrors g(nEntries,index, v3, 0, v4);
TAxis* a = g.GetXaxis();
a->SetLimits(-1,nEntries);
a->SetNdivisions(-(nEntries+1));
double* run=v1;
double* minirun=v2;

Int_t binindex= 0;
for(int i=0; i<nEntries;i++){
   binindex=a->FindBin(i);
   if (minirun[i]<0){
      a->SetBinLabel(binindex, Form("%3.0f",run[i]));
   } else {
      a->SetBinLabel(binindex, Form("%3.0f.%3.0f",run[i],minirun[i]));
   }
}

TF1 *fconst=new TF1("fconst", "pol0");
g.Fit(fconst->GetName(), "Q", 0, nEntries);
this->value= fconst->GetParameter(0);
this->value_err = fconst->GetParError(0);
g.SetTitle(Form("%s vs run", chan_name));
g.SetMarkerStyle(20);
return g;
}


std::vector<TH1F> Channel::drawPull(Int_t nEntries, Double_t *index, Double_t *run, Double_t *minirun, Double_t *value, Double_t *value_error){
  
std::vector<TH1F> pulls;
TH1F pull("pull", "",nEntries+1,0,nEntries+1);
TH1F pull1D("pull1D", "", 40, -8, 8);
auto a=pull.GetXaxis();
a->SetNdivisions(-(nEntries+1));
Int_t binindex=0;
for(auto i=0; i<nEntries; i++){
  auto val=(*(value+i)- this->value)/(abs(*(value_error+i)));
  auto binindex=a->FindBin(i);
  pull.SetBinContent(binindex,val);
  a->SetBinLabel(binindex, "");
  pull1D.Fill(val);
}
pull.SetFillColor(kGreen-3);
pull.SetBarOffset(0.5);


pulls.push_back(pull);
pulls.push_back(pull1D);
return pulls;
}

/***********************************End Class Channel*************************************/




class Source {
    TString file, tree, output;
    Int_t slug, ihwp, wein, hrs;
  public:
    std::vector<Channel> list;
    TChain *T;   
    Source (TString a, TString b, TString c, Int_t i, Int_t j, Int_t k, Int_t l): file(a), tree(b), output(c), slug(i), ihwp(j), wein(k), hrs(l) {T=new TChain(tree); T->Add(file+slug+".root");}
    void printInfo() { std::cout << "Reading from  " << tree  << " tree in file " << std::endl;} 
    void drawAll();
    Channel GetChannelByName(TString name);    

};

void Source::drawAll(){
  Double_t tmpRunN     = -1.0;
  Double_t tmpNRuns    = -1.0;
  Double_t tmpSplitN   = -1.0;
  Double_t tmpMinirunN = -1.0;

  auto filename= this->output+this->slug;

  TString empty = "CAM_OUTPUTDIR: Undefined variable.";
  TString outputDir = "";
  TString out_dir = gSystem->Getenv("CAM_OUTPUTDIR"); 
  if (out_dir == "" || out_dir == "NULL"){
    Printf("Error: Output dir (%s) invalid, must be a string\n",out_dir.Data     ());
    out_dir = "./";
  }
  outputDir = out_dir; 

  TString aggregatorFileName = Form("%s/aggregator.root",outputDir.Data()); // FIXME, this is very specific, and doesn't allow for aggregating over slugs, for instance
  // Store all trees
  aggregatorFileName = Form("%s/grand_aggregator.root",outputDir.Data());
  TFile *aggregatorFile = new TFile(aggregatorFileName,"UPDATE");
  aggregatorFile->cd();
  TTree * outputTree = new TTree("agg","Aggregator Tree");
  // Intentionally not using a struct here to match prior aggregator output definition 
  // and to simplify life for other people, and to allow for non-mean kinds of variables to be used
  tmpNRuns = (Double_t)this->slug;
  outputTree->Branch("run_number", &tmpRunN);
  outputTree->Branch("n_runs", &tmpNRuns);
  outputTree->Branch("split_n", &tmpSplitN);
  outputTree->Branch("minirun_n", &tmpMinirunN);

  //if (gSystem->Exec("echo $CAM_OUTPUTDIR")==empty) {
  //  Printf("Error: Must define an output directory, setting to default = ./");
  //  gSystem->Setenv("CAM_OUTPUTDIR","./");
  //}
  // Make a root file - add branches to it - fill run_number, n_runs, minirun_n, split_n with -1 for safety
  Double_t tmpIHWP = this->ihwp;
  Double_t tmpWIEN = this->wein; // Misspelled wien
  Double_t tmpHRS  = this->hrs;
  outputTree->Branch("ihwp", &tmpIHWP);
  outputTree->Branch("wein", &tmpWIEN);
  outputTree->Branch("hrs", &tmpHRS);

  TObjArray *var_list=(this->T)->GetListOfBranches();
  TIter var_iter(var_list);

  Int_t nChans = 0;
  while (auto *var= var_iter.Next()){
    TString name(var->GetName());
    bool createPlotObj = ((name.Contains("_mean") || name.Contains("_slope") || name.Contains("_pslope")|| name.Contains("_rms")) && !(name.Contains("_error"))); 
    if (createPlotObj) {
      Channel *channel = new Channel(var->GetName());     
      channel->printInfo();
      channel->draw(this->T, filename);
      (this->list).push_back(*channel);
      outputTree->Branch(name.Data(),&(channel->value));
      outputTree->Branch(Form("%s_error",name.Data()),&(channel->value_err));
      //outputTree->Branch(name.Data(),channel->value);
      //outputTree->Branch(Form("%s_error",name.Data()),channel->value_err);
      //outputTree->Branch(name.Data(),this->list.at(nChans).value);
      //outputTree->Branch(Form("%s_error",name.Data()),this->list.at(nChans).value_err);
      nChans++;
    }
  } 
  outputTree->Fill();
  outputTree->Write("agg",TObject::kOverwrite);
  aggregatorFile->Close();

}

Channel Source::GetChannelByName(TString name){

  Channel chan("");
  for(auto iter=(this->list).begin();iter!=(this->list).end();iter++){
    if (iter->name==name){
      chan=*iter;
      break;
    }
  } 
  //printf("Found %s", (chan.name).Data());
  return chan;
}
