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
#include "TFile.h"
#include "TCanvas.h"
#include <fstream>
#include <ostream>
#include <iostream>
//#include "japantypes.hh"

int main(int argc, char **argv)
{
    std::string rootfile = "run_1033.root";
    std::string channel = "usr";
    //std::string number = "1111";
    if (argc <= 1 || argc > 3)
    {
        std::cerr << "Usage: ./compTest char*:rootfileName char*:variable" << std::endl;
        exit(0);
    }
    if (argc >= 2) 
    {
        std::string fileName(argv[1]); 
        rootfile = fileName;
    }
    if (argc >= 3)
    {
        std::string channelName(argv[2]);
        channel = channelName;
    }
	// number = parsed filename, so just the number part.
	//int first = 0;
	//int last = 99;
	//first  = rootfile.find("_");
	//last   = rootfile.find(".");
	//number = rootfile.substr(first+1,last-4).c_str();
	
	// To grab the root tree output we can assume that PANGUIN has successfully opened a root file
    TFile *oldFile = new TFile(rootfile.c_str(),"READ");
    TTree *oldPatternTree = (TTree*)oldFile->Get("Hel_Tree");
    TTree *oldEventTree = (TTree*)oldFile->Get("Mps_Tree");
    std::string dir = (std::string)gDirectory->CurrentDirectory()->GetPath();
    // Find out how many entries are in the tree so we can loop over the pattern tree later
    Int_t numEntries = oldPatternTree->GetEntries();
    // Make a new ROOT tree to store the new data in (this is just an academic example)
    TTree *newPatternTree;
    TFile *resultsFile;
    //std::string outputFile = Form("second_pass_%s_%s.root",number.c_str(),channel.c_str());
    std::string outputFile = Form("second_pass_%s.root",channel.c_str());
    // Check to see if analysis has been run before
    std::ifstream file(outputFile.c_str());
    if(!file){
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
    delete oldFile;
    return 0;
}
