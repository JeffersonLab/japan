// Usage (one-liner):
//   root -l -q -b 'rootScripts/correlator/CompareBlockToHwSum.C("blueR%d.000new.slope.root","blueR%d.000.block%d.new.slope.root",2948)'
// Usage (in ROOT):
//   .L rootScripts/correlator/CompareBlockToHwSum.C
//   CompareBlockToHwSum("blueR%d.000new.slope.root","blueR%d.000.block%d.new.slope.root",2948)
// Output:
// - BlockAndHwSum.%d.png
// - CompareBlockToHwSum.%d.png

void CompareBlockToHwSum(const char* hwsumfilename, const char* blockfilename, Int_t run)
{
  TCanvas c("c","c",3072,3840);
  c.Divide(3,5);
  TFile hwsumfile(Form(hwsumfilename, run));
  TMatrixD* slopes_hwsum    = (TMatrixD*) hwsumfile.Get("slopes");
  TMatrixD* sigSlopes_hwsum = (TMatrixD*) hwsumfile.Get("sigSlopes");
  c.cd(1);
  slopes_hwsum->Print();
  slopes_hwsum->Draw("text22,colz");
  c.cd(2);
  sigSlopes_hwsum->Draw("text22,colz");
  sigSlopes_hwsum->Print();
  if (slopes_hwsum != 0 && sigSlopes_hwsum != 0) {
    for (size_t block = 0; block < 4; block++) {
      TFile blockfile(Form(blockfilename, run, block));
      TMatrixD* slopes_block    = (TMatrixD*) blockfile.Get("slopes");
      TMatrixD* sigSlopes_block = (TMatrixD*) blockfile.Get("sigSlopes");
      c.cd(4+3*block);
      slopes_block->Print();
      slopes_block->Draw("text22,colz");
      c.cd(5+3*block);
      sigSlopes_block->Print();
      sigSlopes_block->Draw("text22,colz");
      if (slopes_block != 0 && sigSlopes_block != 0) {
        TMatrixD sig(*slopes_block);
        sig -= (*slopes_hwsum);
        for (Int_t i = 0; i < sig.GetNrows(); i++)
          for (Int_t j = 0; j < sig.GetNcols(); j++)
            sig(i,j) /= TMath::Sqrt((*sigSlopes_block)(i,j)*(*sigSlopes_block)(i,j) + (*sigSlopes_hwsum)(i,j)*(*sigSlopes_hwsum)(i,j));
        c.cd(6+3*block);
        sig.Print();
        sig.Draw("text22,colz");
      } else {
        std::cout << "No slopes matrix in block file." << std::endl;
        blockfile.ls();
      }
    }
  } else {
    std::cout << "No slopes matrix in hwsum file." << std::endl;
    hwsumfile.ls();
  }
  c.SaveAs(Form("BlockAndHwSum.%d.png", run));
}
