void chain()
{
  // initial parameters
  TString TreeName = "mul_prune"; //This is the tree name in the root file you want to process.
  TChain *chain = new TChain(TreeName);

  std::cout << "Creating the chain" << std::endl;
  chain->SetCacheSize(20*1024*1024);
  TString dir=gSystem->DirName(__FILE__); //Get the current file path
  dir.ReplaceAll("/./","/");
  // std::cout<<dir<<std::endl;
  gSystem->Setenv("Dir",dir);//current file path, fill in absolute or relative path
  chain->Add("$Dir/prune_*.root");//link the root file to be processed; note that the root file has the same tree name

  chain->LoadTree(-1);
  // Call Process data analysis
  TFile *file0 = TFile::Open("All_Lagr_Production.root","RECREATE");
  chain->CloneTree(-1);
  file0->Write();
  delete file0;
}
