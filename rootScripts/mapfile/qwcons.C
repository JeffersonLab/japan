#include <iostream>
#include <TROOT.h>
#include <TSystem.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TMapFile.h>

int main(int argc, char** argv)
{
   gROOT->Reset();

   // Create a new canvas and 3 pads
   TCanvas *c1;
   TPad *pad1, *pad2;
   if (!gROOT->IsBatch()) {
      c1 = new TCanvas("c1","Shared Memory Consumer Example",200,10,700,780);
      pad1 = new TPad("pad1","This is pad1",0.02,0.52,0.98,0.98,21);
      pad2 = new TPad("pad2","This is pad2",0.02,0.02,0.48,0.48,21);
      pad1->Draw();
      pad2->Draw();
   }

   // Open the memory mapped file "hsimple.map" in "READ" (default) mode.
   TMapFile* mfile = 0;
   while (mfile == 0) {
      mfile = TMapFile::Create("/dev/shm//QwMemMapFile.map");
      if (mfile->IsZombie()) {
         delete mfile;
         mfile = 0;
      }
      gSystem->Sleep(1000);   // sleep for 1 seconds
   }

   // Print status of mapped file and list its contents
   mfile->Print();
   mfile->ls();

   // Create pointers to the objects in shared memory.
   TTree *mps = 0;
   TTree *hel = 0;

   // Loop displaying the histograms. Once the producer stops this
   // script will break out of the loop.
   while (1) {
      mps = (TTree *) mfile->Get("Mps_Tree", mps);
      hel = (TTree *) mfile->Get("Hel_Tree", hel);
      if (!gROOT->IsBatch()) {
         pad1->cd();
         mps->Draw("CodaEventNumber");
         pad2->cd();
         hel->Draw("CodaEventNumber");
         c1->Modified();
         c1->Update();
      } else {
         printf("Entries, mps=%lld, hel=%lld\n",mps->GetEntries(),hel->GetEntries());
      }
      gSystem->Sleep(1000);   // sleep for 1 seconds
      if (gSystem->ProcessEvents())
         break;
   }
}
