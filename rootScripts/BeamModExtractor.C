/**********************************************************\
* File: BeamModExtractor.C                                *
*                                                         *
* Author: Ezekiel Wertz                                   *
* Time-stamp: 06/10/19                                    *
\**********************************************************/

//Function that creates a new tree of only modulated beam data. This corresponds to when bmwobj is greater than 0. The parameter oldfilename corresponds to the name of the file where the old tree is coming from. The parameter newfilename corresponds to the name of the file where you want the new tree of only beam modulated data written to.
void BeamModExtractor(TString oldfilename,TString newfilename){
  //Get the file for the old tree
   TFile oldfile(oldfilename);
   //Create a reference for that old tree
   TTree *oldtree;
   //Get the data from the old tree. That tree is named evt.
   oldfile.GetObject("evt", oldtree);
   // Deactivate all branches
   oldtree->SetBranchStatus("*", 1);
  
   // Create a new file + a copy of old tree in new file, the new copy should only have modulated beam.
   TFile newfile(newfilename,"recreate");
   auto newtree = oldtree->CopyTree("bmwobj>0");
   //Write the tree full of only modulated data to the new tree.
   newtree->Print();
   newfile.Write();
}
