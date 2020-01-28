#include "realtimeBmodAna.C"
//https://logbooks.jlab.org/entry/3763550
using namespace std;
Int_t QuerySlugNumber(Int_t run_number){
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
  return 0;
}

void plotAD(Int_t runnum = 999999, Long_t coils = 1133557664422){
  realtimeBmodAna();
  gStyle->SetOptStat(0);
  TPad *c2 = new TPad("cBMWPlotSens","cBMWPlotSens",0,0,1,1);
  Int_t slug_number = 0;
  if(runnum == 999999){
    TString s = gSystem->GetFromPipe("~/scripts/getRunNumber");
    TString s2 = gSystem->GetFromPipe("~/scripts/getSlugNumber");
    slug_number = s2.Atoi();
  }
  else {
    slug_number = QuerySlugNumber(runnum);
  }
  TChain* dit = new TChain("dit");
  Printf("Getting alphas and deltas from /adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/slopes/dithering_slopes_%ld_slug%d.root",coils,slug_number);
  dit->AddFile(Form("/adaqfs/home/apar/PREX/japan/panguin/macros/BeamMod/slopes/dithering_slopes_%ld_slug%d.root",coils,slug_number));
  c2->Divide(4,3);
  c2->Draw();
  c2->cd(1);
  dit->Draw("alpha35_4eX4aX:cyclenum","","*");
  c2->cd(2);
  dit->Draw("alpha15_4eX4aX:cyclenum","","*");
  c2->cd(3);
  dit->Draw("delta35_11X12X4aX:cyclenum","","*");
  c2->cd(4);
  dit->Draw("delta15_11X12X4aX:cyclenum","","*");
  c2->cd(5);
  dit->Draw("alpha13_4eX4aX:cyclenum","","*");
  c2->cd(6);
  dit->Draw("alpha46_4eY4aY:cyclenum","","*");
  c2->cd(7);
  dit->Draw("delta13_11X12X4aX:cyclenum","","*");
  c2->cd(8);
  dit->Draw("delta46_11X12X4aY:cyclenum","","*");
  c2->cd(9);
  dit->Draw("alpha26_4eY4aY:cyclenum","","*");
  c2->cd(10);
  dit->Draw("alpha24_4eY4aY:cyclenum","","*");
  c2->cd(11);
  dit->Draw("delta26_11X12X4aY:cyclenum","","*");
  c2->cd(12);
  dit->Draw("delta24_11X12X4aY:cyclenum","","*");
  //c2->SaveAs(Form("%s_alphas-deltas.pdf",input.substr(0,input.find(".root")).c_str()));
}
