#include "toolbox.hh"
#include "tools.hh"
int plotbox(TString suffix = ""){
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  // Generate data
  tb->tg_err_averaging("rcdb_slug",0,"","&& run<7500","part","",suffix+"_part12"); // Generate slug averaged data - NO sign correction applied here
  tb->tg_err_averaging("rcdb_slug",0,"","&& run>=7500&&run<8444","part","",suffix+"_part3"); // Generate slug averaged data - NO sign correction applied here

  // Plot data
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_flip_state<=2 && rcdb_slug>=107","part","",suffix+"_part12"); // Plot the data
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_flip_state<=2 && rcdb_slug>=107","part","",suffix+"_part3"); // Plot the data


  cout << "-- ToolBox Analysis done in "; tsw.Print(); cout << endl;
  delete tb;
  return 0;
}
