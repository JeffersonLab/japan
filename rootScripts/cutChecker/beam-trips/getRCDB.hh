void getRCDB(TTree * tree, Int_t runNumber, std::vector<Double_t> &singleEntries, std::vector<Double_t> &singleEntriesValues){
  std::vector<TString> rcdbNames = {
    "slug",
    "run_type",       // = 'Production'
    "run_flag",       // = 'Good'
    "ihwp",           // = 'IN' or 'OUT'
    "flip_state",     // = 'FLIP-RIGHT' or 'FLIP-LEFT'
    "horizontal_wien",// = -29.6402
    "vertical_wien",  // = 88.0008
    "rhwp",           // = 1500.0
    "arm_flag",       // = 0, or 1, or 2
    "beam_current",   // = 109.044
    "total_charge",   // = 584807.0
    "target_type",    // = '48Ca', or '40Ca'
    "target_45encoder",//= 2.0
    "target_90encoder",//= 13163100.0
    "dpp",            // = 0.000142342
    "dpp_stdev",      // = 1.4e-6
    "energy_avg",     // = 2.156234
    "energy_sig",     // = 1.4e-6
  };
  Int_t success = 0;
  // Experimenting Function to Get slug number based on run number 
  // Author : Tao Ye

  TSQLResult* res;
  TSQLRow *row;
  cout << "Getting Info from RCDB" << endl;
  //cout << " -- Connecting to RCDB -- " << endl;
  TSQLServer *rcdb = TSQLServer::Connect("mysql://hallcdb.jlab.org:3306/a-rcdb","rcdb","");
  //cout << " -- ServerInfo: " << rcdb->ServerInfo() << endl;
  cout << " -- Host : " << rcdb->GetHost() << endl;
  //cout << " -- Query DataBase " << endl;
  TString select_q ="SELECT run_number,name,int_value ";  //int_value
  TString from_q =  "FROM `a-rcdb`.conditions,`a-rcdb`.condition_types ";
  TString where_q = Form("WHERE conditions.condition_type_id=condition_types.id and name='slug' and run_number='%d'",
			 runNumber);
  res = rcdb->Query(select_q + from_q + where_q);
  if(res==NULL){
    cout << " -- ERROR: Failed to Query " << endl;
    delete row;
    delete res;
    cout << " -- Closing Connection to RCDB " << endl;
    rcdb->Close();
    delete rcdb;
    success = -1;
  }

  int nFields =res->GetFieldCount();
  row = res->Next();
  if(row==NULL){
    cout << " -- ERROR: Failed to load slug number " << endl;
    delete row;
    delete res;
    cout << " -- Closing Connection to RCDB " << endl;
    rcdb->Close();
    delete rcdb;
    success = -1;
  }
  //cout << " ----------------------------- " << endl;
  //for(int j=0; j< nFields; j++)
    //cout << "\t" << row->GetField(j) ;
  //cout << endl;
  const char* tmp_char = row->GetField(2);
  int slug_id = TString(tmp_char).Atoi();
  int wien = 0;
  int ihwp = 0;
  int sign = 0;
  //if ( ((Int_t)slug_id) != ((Int_t)nRuns) ){
    //std::cout << " -- ERROR: Slug number from RCDB disagrees with slug number in aggregator command line arguments" << std::endl;
  //}
  // For each rcdb entry get the entry from RCDB, convert to double or integer, and fill a tmpChan into the vector channels into a singleEntry field.
  singleEntries.clear();
  singleEntriesValues.clear();
  for (Int_t i = 0 ; i<rcdbNames.size() ; i++) {
    Double_t tmpEntry = -1.0;
    Double_t tmpEntryValue = -1.0;
    singleEntries.push_back(-1.0);
    singleEntriesValues.push_back(-1.0);
  }
  for (Int_t i = 0 ; i<rcdbNames.size() ; i++) {
    tree->Branch("rcdb_"+rcdbNames.at(i),&(singleEntries.at(i)));
    where_q = Form("WHERE conditions.condition_type_id=condition_types.id and name='%s' and run_number='%d'",
        rcdbNames.at(i).Data(),runNumber);
    if (rcdbNames.at(i) == "slug" || rcdbNames.at(i) == "arm_flag") {
      select_q ="SELECT run_number,name,int_value ";
    }
    else if (rcdbNames.at(i) == "run_type" || rcdbNames.at(i) == "run_flag" || rcdbNames.at(i) == "ihwp" || rcdbNames.at(i) == "flip_state" || rcdbNames.at(i) == "target_type") {
      select_q ="SELECT run_number,name,text_value ";
    }
    else{
      select_q ="SELECT run_number,name,float_value ";
    }
    res = rcdb->Query(select_q + from_q + where_q);
    nFields =res->GetFieldCount();
    row = res->Next();
    if(row==NULL || nFields <= 1 || success==-1){
      std::cout << " -- RCDB failed to load " << rcdbNames.at(i) << std::endl;
      singleEntriesValues.at(i) = -1.0e6;
    }
    else {
      // Good connection, good stuff
      const char* tmp_char2 = row->GetField(2);
      //if (debug > 1) std::cout<<"For " << row->GetField(1) << " we have " << row->GetField(2)<<std::endl;
      if ( rcdbNames.at(i) == "run_type" ){
        if ((TString)tmp_char2 == "Production"){
          singleEntriesValues.at(i) = 1.0;
        }
        else if ((TString)tmp_char2 == "Pedestal"){
          singleEntriesValues.at(i) = 2.0;
        }
        else if ((TString)tmp_char2 == "Calibration"){
          singleEntriesValues.at(i) = 3.0;
        }
        else if ((TString)tmp_char2 == "ParityScan"){
          singleEntriesValues.at(i) = 4.0;
        }
        else if ((TString)tmp_char2 == "Test"){
          singleEntriesValues.at(i) = 5.0;
        }
        else {
          singleEntriesValues.at(i) = -1.0;
        }
      }
      else if ( rcdbNames.at(i) == "run_flag" ){
        if ((TString)tmp_char2 == "Good") {
          singleEntriesValues.at(i) = 1.0;
        }
        else if ((TString)tmp_char2 == "NeedCut" || (TString)tmp_char2 == "NeedCuts") {
          singleEntriesValues.at(i) = 2.0;
        }
        else if ((TString)tmp_char2 == "Suspicious") {
          singleEntriesValues.at(i) = 3.0;
        }
        else if ((TString)tmp_char2 == "Bad") {
          singleEntriesValues.at(i) = 4.0;
        }
        else {
          singleEntriesValues.at(i) = -1.0;
        }
      }
      else if ( rcdbNames.at(i) == "ihwp" ){
        if ((TString)tmp_char2 == "IN"){
          singleEntriesValues.at(i) = 1.0;
          ihwp = 1;
        }
        else if ((TString)tmp_char2 == "OUT"){
          singleEntriesValues.at(i) = 2.0;
          ihwp = -1;
        }
        else {
          singleEntriesValues.at(i) = -1.0;
          ihwp = 0;
        }
      }
      else if ( rcdbNames.at(i) == "flip_state" ){
        if ((TString)tmp_char2 == "Vertical(UP)"){
          // Transverse Asymmetry
          singleEntriesValues.at(i) = 3.0;
          wien = 1;
        }
        else if ((TString)tmp_char2 == "FLIP-LEFT"){
          singleEntriesValues.at(i) = 2.0;
          wien = -1;
        }
        else if ((TString)tmp_char2 == "FLIP-RIGHT"){
          singleEntriesValues.at(i) = 1.0;
          wien = 1;
        }
        else {
          singleEntriesValues.at(i) = -1.0;
          wien = 0;
        }
      }
      else if ( rcdbNames.at(i) == "target_type" ){
        if ((TString)tmp_char2 == "48Ca"){
          // Transverse Asymmetry
          singleEntriesValues.at(i) = 1.0;
        }
        else if ((TString)tmp_char2 == "40Ca"){
          singleEntriesValues.at(i) = 2.0;
        }
        else if (((TString)tmp_char2).Contains("Pb")){
          singleEntriesValues.at(i) = 4.0;
        }
        else if (((TString)tmp_char2).Contains("Carbon 1")){
          singleEntriesValues.at(i) = 3.0;
        }
        else if ((TString)tmp_char2 == "Home"){
          singleEntriesValues.at(i) = 5.0;
        }
        else {
          singleEntriesValues.at(i) = -1.0;
        }
      }
      else if ( rcdbNames.at(i) == "slug" ) {
          singleEntriesValues.at(i) = (Double_t)(TString(tmp_char2).Atof());
          Printf("slug = %s",tmp_char2);
          Printf("slug double = %f",(Double_t)(TString(tmp_char2).Atof()));
      }
      else {
      singleEntriesValues.at(i) = (Double_t)(TString(tmp_char2).Atof());
      }
    }
  }
  Double_t tmpEntry;
  Double_t tmpEntryValue;
  singleEntries.push_back(-1.0);
  singleEntriesValues.push_back(-1.0);
  tree->Branch("rcdb_sign",&(singleEntries.at(singleEntries.size()-1)));
  Printf("Done adding RCDB info");
  Printf("%zu RCDB values",rcdbNames.size());
  singleEntriesValues.at(singleEntriesValues.size()-1) = ((Double_t)(wien*ihwp));
  delete row;
  //cout << " ----------------------------- " << endl;
  //cout << " -- Slug Number found  " << slug_id << endl;
  //cout << " -- Closing Connection " << endl;
  rcdb->Close();
  delete res;
  delete rcdb;
}
