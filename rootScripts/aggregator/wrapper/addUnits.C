struct units_t
{ 
  double ppm;
  double ppb;
  double mm;
  double nm;
  double um;
  double V_uA;
  double mV_uA;
};


void addUnits(TString fnm, TString tree = "agg") {
  units_t unit;
  unit.ppm=1e-6;
  unit.ppb=1e-9;
  unit.mm=1;
  unit.um=1e-3;
  unit.nm=1e-6;
  unit.V_uA=1;
  unit.mV_uA=1e-3;  
/*
    static const TString kUnitsName;
    static Double_t kUnitsValue[];

    /// Construct the tree
    void ConstructNewTree() {
      QwMessage << "New tree: " << fName << ", " << fDesc << QwLog::endl;
      fTree = new TTree(fName.c_str(), fDesc.c_str());
    }

    void ConstructUnitsBranch() {
      std::string name = "units";
      fTree->Branch(name.c_str(), &kUnitsValue, kUnitsName);
    }
    */

  // Get the post pan file, open it, find minirun, find its final entry, get that value, return
  //Open(runNumber); // FIXME For JAPAN version of code
  TFile *fin = TFile::Open(fnm.Data(),"UPDATE");
  TTree *T=(TTree*)fin->Get(tree);  
    
  TBranch *branch= T->Branch("units", &unit, "ppm/D:ppb/D:mm/D:nm/D:um/D:V_uA/D:mV_uA/D");
  for (auto i=0;i <T->GetEntries();i++){
  	T->GetEntry(i);
        branch->Fill();
  }

  T->Write(tree, TObject::kOverwrite);
  fin->Close();
}
