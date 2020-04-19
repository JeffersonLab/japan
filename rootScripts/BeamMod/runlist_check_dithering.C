void runlist_check_dithering(std::string input = "NULL",std::string runlist = "NULL"){
  std::map<Int_t,Int_t> textFile_runnums;
  std::map<Int_t,Int_t> rootFile_runnums;
  std::map<Int_t,Int_t> all_runnums;
  std::string runnum = ""; // Can be filled later if wanted
  TChain* dit = new TChain("dit");
  std::string cut = "";
  //if (runnum != "") {
  //  cut = Form("run == %s",runnum.c_str());
  //}
  if (input == "NULL" || runlist == "NULL") {
    Printf("ERROR: Must include root file and run list text file as inputs");
    return 0;
  }
  dit->AddFile(input.c_str());
  // For slug file
  ifstream ifinfile(runlist.c_str());
  if (ifinfile.is_open()){
    while(getline(ifinfile,runnum)){
      textFile_runnums[(Int_t)atoi(runnum.c_str())] = (Int_t)atoi(runnum.c_str());
      all_runnums[(Int_t)atoi(runnum.c_str())] = (Int_t)atoi(runnum.c_str());
    }
  }
  // run_num = entries
  std::map<Int_t,Int_t> mostRecentRun, checkRun, RunExists_TextFile, RunExists_RootFile;
  Int_t finalRun_TextFile = 0;
  Int_t finalRun_RootFile = 0;
  Int_t oldRun = 0;
  Int_t run = 0;
  Int_t checkrun = 0;

  //For runs
  std::map<Int_t,Int_t>::iterator it = textFile_runnums.begin();

  FILE *pFile;
  pFile = fopen(Form("checks/run-checks.txt"),"a");//_%d.txt",runnums.at(i)),"a");// Append //w+
//  fprintf(pFile, "Run %d, Unser scaled pedestals\n",runnums.at(i));
  //fprintf(pFile, "Run num, Entry Exists in run_list, Entry Exists in Slopes file, Nearest Run Number below in ROOT file, Final Run Number in ROOT file, Run List, Slopes ROOT file\n",runnums.at(i));

  while (it != textFile_runnums.end())
  {
    run = it->first;
    checkrun = it->second;
    if (run != checkrun) {
      Printf("ERROR, run didn't work\n");
      return 0;
    }
    if (oldRun == 0) oldRun = run;
    RunExists_RootFile[run] = dit->Draw("run",Form("run==%d",textFile_runnums[run]),"goff");
    RunExists_TextFile[run] = textFile_runnums[run];
    if (RunExists_RootFile[run] == 1) {
      Printf("Run %d exists in ROOT files",run);
      mostRecentRun[run] = oldRun;
      finalRun_RootFile = run;
      finalRun_TextFile = run;
    }
    oldRun = run;
    it++;
  }
  oldRun = 0;
  Int_t nTreeEntries = dit->GetEntries();
  for (Int_t i = 0 ; i<nTreeEntries; i++) {
    dit->SetBranchAddress("run",&run);
    dit->GetEntry(i);
    rootFile_runnums[run] = run;
    all_runnums[run] = run;
    RunExists_RootFile[run] = dit->Draw("run",Form("run==%d",rootFile_runnums[run]),"goff");
    rootFile_runnums[run] = run;
    finalRun_RootFile = run;
    if (oldRun == 0) oldRun = run;
    mostRecentRun[run] = oldRun;
    oldRun = run;
  }

  it = all_runnums.begin();
  while (it != all_runnums.end()) {
    run = it->first;
    fprintf(pFile, "%d, %s, %s, %d, %d, %s, %s\n",run,RunExists_TextFile[run]?"true":"false",RunExists_RootFile[run]?"true":"false",mostRecentRun[run],finalRun_RootFile,runlist.c_str(),input.c_str());

    it++;
  }
  fclose(pFile);
}
