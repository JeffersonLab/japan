#include "toolbox.hh"
#include "tools.hh"
int plotbox(){
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  //tb->tg_err_averaging("crex_part");   // Test case
  //tb->tg_err_averaging("rcdb_slug",0); // Generate slug averaged data
  tb->tg_err_averaging("rcdb_slug",1); // Generate grand plots from slug averaged data



  cout << "-- ToolBox Analysis done in "; tsw.Print(); cout << endl;
  delete tb;
  return 0;
}
