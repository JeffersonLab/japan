#include "toolbox.hh"
#include "tools.hh"
int plotbox(TString avg = "part", TString DataSetCut = "IncludeBMOD", TString suffix = "", Int_t ana = 0){
  TString additionalCleanCut = " && ( !(run==6564 && mini==4) && !(run==6567 && (mini==2 || mini==4)) && !(run==6571 && (mini==3 || mini==4)) && !(run==6593 && mini==2) && !(run==6983 && mini==8) && !(run==7149 && mini==6) && !(run==7211 && mini==4) && !(run==7889 && mini==0) && !(run==7942 && mini==5) && !(run==8036 && mini==2) && !(run==8240 && mini==1) && !(run==8549 && (mini==0 || mini==1 || mini==4)))";
  TString additionalATCut = " && rcdb_slug>=107 && run<8444";
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  // Generate data
  //tb->tg_err_averaging("crex_part",0,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalCleanCut ,DataSetCut,"part","_no_err",suffix); // Generate part averaged data
  //tb->tg_err_averaging("rcdb_slug",0,"",          "&& rcdb_flip_state<=2"+additionalCleanCut ,DataSetCut,"part","_no_err",suffix); // Generate part averaged data
  //tb->tg_err_averaging("crex_part",0,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalATCut+additionalCleanCut,DataSetCut,"part","_no_err",suffix+"_AT_det_cut");
  //tb->tg_err_averaging("rcdb_slug",0,"",          "&& rcdb_flip_state<=2"+additionalATCut+additionalCleanCut,DataSetCut,"part","_no_err",suffix+"_AT_det_cut"); 

  // Plot data
  //tb->tg_err_averaging("crex_part",1,"","",DataSetCut,"part","_no_err",suffix); // Plot the data
  //tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_flip_state<=2 && rcdb_slug<=185",DataSetCut,"part","_no_err",suffix+"_AT_det_cut"); // Plot the data independently from the other
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_flip_state<=2 && rcdb_slug>=186",DataSetCut,"part","_no_err",suffix+"_AT_det_cut"); // Plot the data independently from the other


  cout << "-- ToolBox Analysis done in "; tsw.Print(); cout << endl;
  delete tb;
  return 0;
}
