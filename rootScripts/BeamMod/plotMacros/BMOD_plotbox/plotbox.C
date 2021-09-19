#include "toolbox.hh"
#include "tools.hh"
int plotbox(TString suffix = ""){
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  // Generate data
  ////tb->tg_err_averaging("rcdb_slug",0,"","&& rcdb_flip_state<=2","part","_no_err","_main_det_corrections"); // Generate slug averaged data - NO sign correction applied here

  // Plot data
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_slug!=103 && rcdb_slug!=186","part","_no_err","_main_det_corrections"); // Plot the data


  cout << "-- ToolBox Analysis done in "; tsw.Print(); cout << endl;
  delete tb;
  return 0;
}
