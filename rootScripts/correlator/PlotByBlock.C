void PlotByBlock(const char* matrix, const char* filename, Int_t run)
{
  TCanvas c;
  c.Divide(2,2);
  for (size_t block = 0; block < 4; block++) {
    c.cd(block+1);
    TFile file(Form(filename, run, block));
    TMatrixD* m = (TMatrixD*) file.Get(matrix);
    if (m != 0) {
      m->Draw("colz");
    } else {
      std::cout << "No such matrix in file." << std::endl;
      file.ls();
    }
  }
  c.SaveAs(Form("PlotByBlock.%d.%s.png", run, matrix));
}
