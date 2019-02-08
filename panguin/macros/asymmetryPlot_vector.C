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
#include "TTree.h"

void asymmetryPlot(std::string channel) {

    gStyle->SetOptStat(0);

    TTree *oldPatternTree = (TTree*)gDirectory->Get("Hel_Tree");
    TTree *newPatternTree = new TTree("noise_Hel_Tree","Tree of Pattern based noise");

    std::vector < Double_t > *patternAsyms  = 0;
    oldPatternTree->SetBranchAddress( Form("asym_%s",channel.c_str())  , &patternAsyms  );

    // Calculate noise floor from pattern tree directly, no steps for this
    std::vector < Double_t > *patternNoises;
    newPatternTree->Branch(Form("pattern_noise_%s",channel.c_str()), &patternNoises);
    for (size_t i = 0; i < oldPatternTree->GetEntries(); i++) {
        oldPatternTree->GetEntry(i);
        patternNoises->push_back(patternAsyms->at(patternAsyms->size()-1));
    }
    newPatternTree->Draw(Form("pattern_noise_%s",channel.c_str()));
}
