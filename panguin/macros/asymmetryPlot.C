/**************************************************************
 *                                                            *
 * Author: Cameron Clarke                                     *
 * Date: 1-31-2019                                            *
 * Purpose: A script that loops through events in the         *
 *          event-based tree and generates auto-correlations  *
 *          and also estimates the helicity pattern-tree and  *
 *          the alternating-pair-wise ppm asymmetry noise     *
 *          floor for a ~7 volt signal or a mid-range scaler  *
 *                                                            *
 **************************************************************/

#include <sstream>
#include <string>
#include <vector>
#include "TTree.h"

void asymmetryPlot(std::string channel) {

    gStyle->SetOptStat(0);

    TTree *oldPatternTree = (TTree*)gDirectory->Get("Hel_Tree");
    TTree *newPatternTree = new TTree("noise_Hel_Tree","Tree of Pattern based noise");

    Double_t patternAsyms;
    oldPatternTree->SetBranchAddress( Form("asym_%s",channel.c_str()), &patternAsyms );
    Int_t numEntries = oldPatternTree->GetEntries();

    // Calculate noise floor from pattern tree directly, no steps for this
    //std::vector < Double_t > *patternNoises;// = new std::vector < Double_t >;
    Double_t patternNoises;
    TBranch *patternNoisesBranch;
    patternNoisesBranch = newPatternTree->Branch(Form("pattern_noise_%s",channel.c_str()), &patternNoises);
    newPatternTree->SetEntries(numEntries);
    for (int j = 0; j < numEntries; j++) {
        oldPatternTree->GetEntry(j);
        patternNoises=patternAsyms;
        patternNoisesBranch->Fill();
    }
    newPatternTree->Draw(Form("pattern_noise_%s",channel.c_str()));
}
