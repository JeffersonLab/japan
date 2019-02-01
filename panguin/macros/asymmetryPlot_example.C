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

void asymmetryPlot_example(std::string channel) {

    gStyle->SetOptStat(0);

    // To grab the root tree output we can assume that PANGUIN has successfully opened a root file
    TTree *oldPatternTree = (TTree*)gDirectory->Get("Hel_Tree");
    // Find out how many entries are in the tree so we can loop over the pattern tree later
    Int_t numEntries = oldPatternTree->GetEntries();
    // Make a new ROOT tree to store the new data in (this is just an academic example)
    TTree *newPatternTree = new TTree("noise_Hel_Tree","Tree of Pattern based noise");
    
    // Define a new struct that will hold the individual leaves from the branch of interest
    typedef struct {Double_t hw_sum, block[4],numsamp,errorcode;} PATTERNASYMS;
    
    // Make an instance of the struct
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
    // Draw your results
    newPatternTree->Draw(Form("pattern_noise_%s",channel.c_str()));
}
