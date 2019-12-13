int QuerySlugNumber(int run_number){

  // Experimenting Function to Get slug number based on run number 
  // Author : Tao Ye

  TSQLResult* res;
  TSQLRow *row;
  cout << " -- Getting Slug Number from RCDB -- " << endl;
  cout << " -- Connecting to RCDB -- " << endl;
  TSQLServer *rcdb = TSQLServer::Connect("mysql://hallcdb.jlab.org:3306/a-rcdb","rcdb","");
  cout << " -- ServerInfo: " << rcdb->ServerInfo() << endl;
  cout << " -- Host : " << rcdb->GetHost() << endl;
  cout << " -- Query DataBase " << endl;
  TString select_q ="SELECT run_number,name,int_value "; 
  TString from_q =  "FROM `a-rcdb`.conditions,`a-rcdb`.condition_types ";
  TString where_q = Form("WHERE conditions.condition_type_id=condition_types.id and name='slug' and run_number='%d'",
			 run_number);
  res = rcdb->Query(select_q + from_q + where_q);
  if(res==NULL){
    cout << " -- Failed to Query " << endl;
    cout << " -- Bye-bye! " << endl;
    delete row;
    delete res;
    cout << " -- Closing Connection to RCDB " << endl;
    rcdb->Close();
    delete rcdb;
    return -1;
  }

  int nFields =res->GetFieldCount();
  row = res->Next();
  if(row==NULL){
    cout << " -- Failed to load slug number " << endl;
    cout << " -- Bye-bye! " << endl;
    delete row;
    delete res;
    cout << " -- Closing Connection " << endl;
    rcdb->Close();
    delete rcdb;
    return -1;
  }
  cout << " ----------------------------- " << endl;
  for(int j=0; j< nFields; j++)
    cout << "\t" << row->GetField(j) ;
  cout << endl;
  const char* slug_char = row->GetField(2);
  int slug_id = TString(slug_char).Atoi();
  delete row;
  cout << " ----------------------------- " << endl;
  cout << " -- Slug Number found  " << slug_id << endl;
  cout << " -- Closing Connection " << endl;
  rcdb->Close();
  delete res;
  delete rcdb;

  return slug_id;
}
