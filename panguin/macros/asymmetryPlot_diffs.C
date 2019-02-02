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
    Int_t numEntries = oldPatternTree->GetEntries();
    std::cout<<"N entries = "<<numEntries<<std::endl;
    TTree *newPatternTree = new TTree("noise_Hel_Tree","Tree of Pattern based noise");
    
    typedef struct {Double_t hw_sum, block[4],numsamp,errorcode;} PATTERNASYMS;
    
    PATTERNASYMS patternAsyms;
    oldPatternTree->SetBranchAddress( Form("asym_%s",channel.c_str()), &patternAsyms );

    Double_t patternNoises;
    TBranch *patternNoisesBranch;
    patternNoisesBranch = newPatternTree->Branch(Form("pattern_noise_%s",channel.c_str()), &patternNoises);
    for (int j = 0; j < numEntries-1; j++) {
        oldPatternTree->GetEntry(j);
        patternNoises=patternAsyms.hw_sum;
        oldPatternTree->GetEntry(j+1);
        patternNoises-=patternAsyms.hw_sum;
        newPatternTree->Fill();
    }
    oldPatternTree->Draw(Form("asym_%s",channel.c_str()));
    newPatternTree->SetLineColor(2);
    newPatternTree->Draw(Form("pattern_noise_%s",channel.c_str()),"","same");
    newPatternTree->Draw(Form("pattern_noise_%s",channel.c_str()));
}
