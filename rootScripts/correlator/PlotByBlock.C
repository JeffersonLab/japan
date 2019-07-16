// Usage (one-liner):
//   root -l -q -b 'rootScripts/correlator/PlotByBlock.C("slopes","blueR%d.000.block%d.new.slope.root",2948)'
// Usage (in ROOT):
//   .L rootScripts/correlator/PlotByBlock.C
//   PlotByBlock("slopes","blueR%d.000.block%d.new.slope.root",2948)
// Output:
// - PlotByBlock.%d.%s.png

void PlotByBlock(const char* matrix, const char* filename, Int_t run)
{
  gStyle->SetOptStat(0);

  TCanvas c("c","c",2048,1536);
  c.Divide(2,2);
  TH2D* h[4];
  for (size_t block = 0; block < 4; block++) {
    c.cd(block+1);
    TFile file(Form(filename, run, block));
    TMatrixD* m = (TMatrixD*) file.Get(matrix);
    if (m != 0) {
      m->Draw("text22,colz");
    } else {
      std::cout << "No such matrix in file." << std::endl;
      file.ls();
    }
  }
  c.SaveAs(Form("PlotByBlock.%d.%s.png", run, matrix));
}
