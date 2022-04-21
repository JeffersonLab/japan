/*
author: Cameron Clarke
last update: February 2021

*/

void Add_slowControls_eig(TString filename = "tmp.root" , TString tree = ""){ //(int start, int end)
  // List of pitts, from neighboring results folder, hardcoded here for ease of use
  std::map<Int_t,Double_t> pitt_map = {
    {100, 0},
    {101, 0},
    {102, 0},
    {103, 0},
    {104, 0},
    {105, 0},
    {106, 0},
    {107, 0},
    {108, 0},
    {109, 1},
    {110, 1},
    {111, 1},
    {112, 1},
    {113, 1},
    {114, 1},
    {115, 1},
    {116, 1},
    {117, 1},
    {118, 1},
    {119, 2},
    {120, 2},
    {121, 2},
    {122, 2},
    {123, 2},
    {124, 2},
    {125, 2},
    {126, 2},
    {127, 2},
    {128, 2},
    {129, 3},
    {130, 3},
    {131, 3},
    {132, 3},
    {133, 3},
    {134, 3},
    {135, 3},
    {136, 3},
    {137, 3},
    {138, 4},
    {139, 4},
    {140, 4},
    {141, 4},
    {142, 5},
    {143, 5},
    {144, 5},
    {145, 5},
    {146, 5},
    {147, 6},
    {148, 6},
    {149, 6},
    {150, 6},
    {151, 7},
    {152, 7},
    {153, 7},
    {154, 7},
    {155, 8},
    {156, 8},
    {157, 8},
    {158, 8},
    {159, 9},
    {160, 9},
    {161, 9},
    {162, 9},
    {163, 10},
    {164, 10},
    {165, 10},
    {166, 10},
    {167, 10},
    {168, 10},
    {169, 11},
    {170, 11},
    {171, 11},
    {172, 11},
    {173, 12},
    {174, 12},
    {175, 12},
    {176, 12},
    {177, 13},
    {178, 13},
    {179, 13},
    {180, 13},
    {181, 14},
    {182, 14},
    {183, 14},
    {184, 14},
    {185, 14},
    {186, 15},
    {187, 15},
    {188, 15},
    {189, 15},
    {190, 15},
    {191, 15},
    {192, 15},
    {193, 16},
    {194, 16},
    {195, 16},
    {197, 16},
    {196, 17},
    {198, 17},
    {199, 17},
    {201, 17},
    {200, 18},
    {202, 18},
    {203, 18},
    {205, 18},
    {204, 19},
    {206, 19},
    {207, 19},
    {208, 19},
    {209, 20},
    {210, 20},
    {211, 20},
    {212, 20},
    {213, 21},
    {214, 21},
    {215, 21},
    {216, 21},
    {217, 22},
    {218, 22},
    {219, 22},
    {220, 22},
    {221, 23},
    {222, 23},
    {223, 23},
    {4000, 4000},
    {4001, 4001},
    {4002, 4002},
    {4003, 4003},
    {4004, 4004},
    {4005, 4005},
    {4006, 4006},
    {4007, 4007},
    {4008, 4008},
    {4009, 4009},
    {4010, 4010},
    {4011, 4011},
    {4012, 4012},
    {4013, 4013},
    {4014, 4014},
    {4015, 4015},
    {4016, 4016},
    {4017, 4017},
    {4018, 4018},
    {4019, 4019},
  };

  TFile input_file(filename);
  TFile output("slowControls_added.root","recreate");
  //TFile output(Form("rootfiles/rcdb_AT_eigenvectors.root"),"recreate");

  TKey *key;
  TIter nextkey(input_file.GetListOfKeys(),kIterBackward);
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname); 
    if (!cl) continue;
    if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*)input_file.Get(key->GetName());
      // Avoid writing the data of a TTree more than once.
      // Note this assume that older cycles are (as expected) older
      // snapshots of the TTree meta data.
      if (!output.FindObject(key->GetName())) {
        output.cd();
        if (tree != "" && T->GetName() != tree) {
          // If we want to only update 1 tree and this tree in the loop is not it
          //   then just clone it real fast and move on
          TTree* newT = T->CloneTree(-1,"fast");
          newT->Write();
          continue;
        }
        // Otherwise do the slow clone and add in new branches as needed
        TTree* newT = T->CloneTree(0);

        //Int_t rcdb_slug = 100;
        Double_t rcdb_slug = 100;
        Double_t rcdb_ihwp, rcdb_flip_state, rcdb_sign, current_part;
        T->SetBranchAddress("crex_part",&current_part);
        T->SetBranchAddress("rcdb_sign",&rcdb_sign);
        T->SetBranchAddress("rcdb_slug",&rcdb_slug);
        T->SetBranchAddress("rcdb_ihwp",&rcdb_ihwp);
        T->SetBranchAddress("rcdb_flip_state",&rcdb_flip_state);

        Long64_t nEntries = T->GetEntries();
        Double_t crex_pitt = 0;
        Double_t crex_slow_control = 0;
        Double_t crex_slow_control_simple = 0;
        newT->Branch("crex_pitt",&crex_pitt);
        newT->Branch("crex_slow_control",&crex_slow_control);
        newT->Branch("crex_slow_control_simple",&crex_slow_control_simple);

        for(int ievt=0;ievt<nEntries;ievt++){
          T -> GetEntry(ievt);
          crex_pitt = pitt_map.at(rcdb_slug);
          if (rcdb_flip_state == 1.0) {
            crex_slow_control_simple = rcdb_ihwp;;
          }
          else {
            crex_slow_control_simple = 2 + rcdb_ihwp;
          }

          if (current_part >= 3.0) {
            crex_slow_control = crex_slow_control_simple + 4.0;
          }
          else {
            crex_slow_control = crex_slow_control_simple;
          }
          newT->Fill();
        }
        newT->Write();
      }
    }
  }
  input_file.Close();

  output.Close();
}
