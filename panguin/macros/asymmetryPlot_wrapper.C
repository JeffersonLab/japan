/**************************************************************
 *                                                            *
 * Author:  Cameron Clarke                                    *
 * Date:    1-31-2019                                         *
 * Title:   asymmetryPlot_example(string channel)             *
 * Purpose: A script that loops through patterns and generates*
 *          a new branch with the asymmetries in it. This is  *
 *          intended simply to serve as an example of how to  *
 *          properly access a JAPAN output tree, define a     *
 *          struct to store the data itself, and then make a  *
 *          new tree (or any other manipulation) to look at   *
 *          your results. This is a fully functioning macro   *
 *          in PANGUIN and in ROOT in general as well, and it *
 *          only represents one possible way of accessing and *
 *          manipulating JAPAN ROOT output.                   *
 *                                                            *
 **************************************************************/

#include <sstream>
#include <string>
#include <vector>
#include "TTree.h"
//#include "japantypes.hh"

void asymmetryPlot_wrapper(std::string channel) {

    gStyle->SetOptStat(0);

    // To grab the root tree output we can assume that PANGUIN has successfully opened a root file
    TFile *oldFile = gFile;
    TTree *oldPatternTree = (TTree*)oldFile->Get("Hel_Tree");
	//std::string dir = (std::string)gDirectory->CurrentDirectory()->GetPath();
    // Find out how many entries are in the tree so we can loop over the pattern tree later
    Int_t numEntries = oldPatternTree->GetEntries();
    // Make a new ROOT tree to store the new data in (this is just an academic example)
    TTree *newPatternTree;
    TFile *resultsFile;

	//std::string number = "1111";

	//number = dir.substr(dir.find("_")+1,4).c_str();
    //std::string outputFile = Form("second_pass_%s_%s.root",number.c_str(),channel.c_str());
    std::string outputFile = Form("second_pass_%s.root",channel.c_str());
	//gSystem->Exec(Form("CURRENT_RUN=%s"),dir.substr(dir.find("_")+1,4).c_str());

    // Check to see if analysis has been run before
	//gSystem->Exec(Form("ln -sf ${QW_ROOTFILES}/prex*%s.root run_%s.root",number.c_str(),number.c_str()));
    //gSystem->Exec(Form("./compTest run_%s.root %s",number.c_str(),channel.c_str()));
    gSystem->Exec(Form("./compTest run.root %s",channel.c_str()));
    if(gSystem->AccessPathName(outputFile.c_str())){
        // Write a new root file for output 
        resultsFile = new TFile(outputFile.c_str(),"RECREATE");
        // Create a new tree to store results in
        newPatternTree = new TTree("noise_Hel_Tree","Tree of Pattern based noise");

        // Make an instance of the struct we need to read the old Tree's data
        typedef struct {
            Double_t hw_sum;
            Double_t block0;
            Double_t block1;
            Double_t block2;
            Double_t block3;
            Double_t numsamp;
            Double_t errorcode;
        } PATTERNASYMS;
        PATTERNASYMS patternAsyms;
        // Assign that instance to the new ROOT tree's branch location of interest
        oldPatternTree->SetBranchAddress( Form("asym_%s",channel.c_str()), &patternAsyms );

        // Make a double to hold the data that is the result of our manipulation
        Double_t patternNoises; // call the asymmetry "noise", as an unblinded asym is also the noise floor
        // Make a new branch to hold the data, but this object is ideally not manipulated much
        TBranch *patternNoisesBranch;
        // Assign that branch to hold the double we made earlier as the contents of each entry
        patternNoisesBranch = newPatternTree->Branch(Form("pattern_noise_%s",channel.c_str()), &patternNoises);
        for (int j = 0; j < numEntries; j++) {
            // Loop over the input file's entries and fill the new tree with the results of one of the leaves
            oldPatternTree->GetEntry(j);
            patternNoises=patternAsyms.hw_sum;
            newPatternTree->Fill();
        }
        // Save your root tree for later use
        resultsFile->Write();
        newPatternTree->Draw(Form("pattern_noise_%s",channel.c_str()));
    }
    // Draw your results
    else{
        resultsFile = new TFile(outputFile.c_str(),"READ");
        newPatternTree = (TTree*)resultsFile->Get("noise_Hel_Tree");
        newPatternTree->Draw(Form("pattern_noise_%s",channel.c_str()));
    }

    delete newPatternTree;
    resultsFile->Close();
    delete resultsFile;
	//delete dir;
    gFile=oldFile;
}
