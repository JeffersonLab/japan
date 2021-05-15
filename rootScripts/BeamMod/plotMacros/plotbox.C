#include "toolbox.hh"
#include "tools.hh"
int plotbox(){
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  //tb->tg_err_averaging("crex_part");   // Test case
  //tb->tg_err_averaging("rcdb_slug",0); // Generate slug averaged data
  //tb->tg_err_averaging("crex_pitt",0,"rcdb_sign*"); // Pitt analysis
  //tb->tg_err_averaging("crex_slow_control_simple",0,"rcdb_sign*"); // Pitt analysis
  //tb->tg_err_averaging("crex_slow_control",0,"rcdb_sign*"); // Pitt analysis
  //tb->tg_err_averaging("rcdb_ihwp",0,"rcdb_sign*"); // Pitt analysis
  //tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*"); // Pitt analysis
  //tb->tg_err_averaging("rcdb_sign",0,"rcdb_sign*"); // Pitt analysis
  //tb->tg_err_averaging("crex_part",0,"rcdb_sign*"); // Pitt analysis

  //// Plotting
  //tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*"); // Generate grand plots from slug averaged data - with sign correction (I left the slug averaging outputs with NO sign correction, all larger time scales do have sign correction in the 1 pass outputs)
  //tb->tg_err_averaging("rcdb_slug",1,""); // Generate NULL grand plots from slug averaged data - no sign correction
  //tb->tg_err_averaging("crex_pitt",1); // Pitt analysis - sign corrected (NULL would require passing NO "rcdb_sign*" at the 1st pass)

  // Slow controls
  tb->tg_err_averaging("crex_slow_control_simple",1); // Pitt analysis
  tb->tg_err_averaging("crex_slow_control",1); // Pitt analysis
  tb->tg_err_averaging("rcdb_ihwp",1); // Pitt analysis
  tb->tg_err_averaging("rcdb_flip_state",1); // Pitt analysis
  tb->tg_err_averaging("rcdb_sign",1); // Pitt analysis
  tb->tg_err_averaging("crex_part",1); // Pitt analysis


  cout << "-- ToolBox Analysis done in "; tsw.Print(); cout << endl;
  delete tb;
  return 0;
}
