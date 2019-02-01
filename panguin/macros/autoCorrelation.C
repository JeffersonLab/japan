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

void autoCorrelation(TString channel, TString drawoption, Int_t n_steps = 1, Bool_t just_noise_floor=false, Bool_t just_auto_correlation=false, Bool_t ignore_helicity_pattern) {

    gStyle->SetOptStat(0);

    TTree *oldEventTree   = (TTree*)gDirectory->Get("Mps_Tree");
    TTree *oldPatternTree = (TTree*)gDirectory->Get("Hel_Tree");
    TTree *newEventTree   = new TTree("noise_Mps_Tree","Tree of Event based noise");
    TTree *newPatternTree = new TTree("noise_Hel_Tree","Tree of Pattern based noise");

    std::vector < Double_t > *dataValues    = 0;
    std::vector < Int_t >    *evNums        = 0;
    std::vector < Double_t > *patternAsyms  = 0;

    oldEventTree->SetBranchAddress(   Form("%s",channel)       , &dataValues    );
    oldEventTree->SetBranchAddress(   Form("event_number")     , &evNums        );
    oldPatternTree->SetBranchAddress( Form("asym_%s",channel)  , &patternAsyms  );

    const int nStepsToCheck = 10;
    // Calculate nStepsToCheck number of event separated (event based) differences of yields and asymmetry
    std::vector < Double_t > newEvNums[nStepsToCheck];
    Double_t                 stepValues[nStepsToCheck] = {0.0}; // Array of steps, where the index refers to how many events ago the data refers to
    std::vector < Double_t > stepNoises[nStepsToCheck];
    std::vector < Double_t > relNoises[nStepsToCheck];
    // Calculate noise floor from pattern tree directly, no steps for this
    std::vector < Double_t > *patternNoises;
    for (int steps = 0; steps < nStepsToCheck; steps++) {
        newEventTree->Branch(Form("event_number")                        , newEvNums[steps]     );
        newEventTree->Branch(Form("values_%dstep_%s",steps+1,channel)    , stepValues[steps]    );
        newEventTree->Branch(Form("step_%dstep_%s",steps+1,channel)      , stepNoises[steps]    );
        newEventTree->Branch(Form("rel_%dstep_%s",steps+1,channel)       , relNoises[steps]     );
    }
    newPatternTree->Branch(Form("pattern_noise_%s",channel)              , &patternNoises       );
    // Assume we start at event 0, and use this variable to check for skipped events (due to decoding errors or DAQ deadtime problems)
    Int_t prior_event_number   = 0;
    Int_t current_event_number = 1;
    Double_t noise_step        = 0.0;
    Double_t noise_rel         = 0.0;

    for (size_t i = 0; i < eventTree->GetEntries(); i++) {
        eventTree->GetEntry(i);
        // There should only ever be one value in each branch per entry, so checking for the length is overkill - just use the last value for safety
        ////  for (size_t j = 0; j < evnum->size(); j++)
        current_event_number = evNums->at(evnum->size()-1);
        if (current_event_number != (1 + prior_event_number)) {
            // If an event is skipped then update the current run number and loop back through, skipping calculations
            prior_event_number = current_event_number;
        }
        else {
            for (int steps1 = nStepsToCheck-1; steps1 > 0; steps1--) {      // Shift all data by 1 upwards in the array
                stepValues[steps1] = stepValues[steps-1];
            } 
            stepValues[0] = dataValues->at(dataValues->size()-1);           // Grab the new data value - Get the last value in the branch for this event, because I am paranoid
            if ((current_event_number-prior_event_number)>nStepsToCheck){
                for (int steps2 = 0; steps2 < nStepsToCheck; steps2++) {
                    noise_step = stepValues[steps2] - stepValues[0];    // Take the difference w.r.t. the step2-th previous value
                    noise_rel  = (stepValues[steps2] - stepValues[0])/(stepValues[step2] + stepValues[0]);    
                    // Take the relative difference (pair asymmetry) w.r.t. the step2-th previous value
                    stepNoises[steps2].push_back(noise_step);
                    relNoises[steps2].push_back(noise_rel);
                }
            }
        }
        prior_event_number = current_event_number;
    }

    for (int steps3 = 0; steps3 < nStepsToCheck; steps3++) {
        c1->cd(steps3);
        newEventTree->Draw(Form("rel_%dstep_%s",step3+1,channel));
    }

    if (!ignore_helicity_pattern){
        for (size_t i = 0; i < patternTree->GetEntries(); i++) {
            patternTree->GetEntry(i);
            patternNoises.push_back(patternAsyms->at(patternAsyms->size()-1));
        }
        newPatternTree->Draw();
    }
}
