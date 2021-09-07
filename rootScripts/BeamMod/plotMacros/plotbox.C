#include "toolbox.hh"
#include "tools.hh"
int plotbox(TString suffix = "", Int_t ana = 0){
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  // _no_err is only allbpms version available at the moment (should be fixed 8/6/2021)
  //tb->tg_err_averaging("crex_part",ana,"rcdb_sign*","&& rcdb_flip_state<=2","part","_no_err",suffix);   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",0,"rcdb_sign*","&& rcdb_flip_state<=2","part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",2,"rcdb_sign*","&& rcdb_flip_state<=2","part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("crex_slow_control",0,"rcdb_sign*","&& rcdb_flip_state<=2","part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("crex_slow_control",2,"rcdb_sign*","&& rcdb_flip_state<=2","part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("crex_slow_control_simple",0,"rcdb_sign*","&& rcdb_flip_state<=2","part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("crex_slow_control_simple",2,"rcdb_sign*","&& rcdb_flip_state<=2","part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_slug",ana,"","&& rcdb_flip_state<=2","part","_no_err",suffix);   // Slug avg all correction data


  /* Special case for Ryan's AT needs 
  ////tb->tg_err_averaging("rcdb_slug",0,"","&& run<7500","part","",suffix+"_part12"); // Generate slug averaged data - NO sign correction applied here
  ////tb->tg_err_averaging("rcdb_slug",0,"","&& run>=7500 && run<=8443","part","",suffix+"_part3"); // Generate slug averaged data - NO sign correction applied here
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_flip_state<=2 && rcdb_slug>=107","part","",suffix+"_part12"); // Plot the data
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_flip_state<=2 && rcdb_slug>=107","part","",suffix+"_part3"); // Plot the data
  End special case */

  // OLD
  //tb->tg_err_averaging("rcdb_slug",2,"","","part","",suffix); // Generate slug averaged data - NO sign correction applied here
  //tb->tg_err_averaging("dit_segment",0,"rcdb_sign*","","part",""); // Segmentwise avg
  //tb->tg_err_averaging("crex_pitt",0,"rcdb_sign*","","part",""); // Pitt analysis
  ////tb->tg_err_averaging("crex_pitt",0,"","",part",""); // Pitt analysis - NULL version
  //tb->tg_err_averaging("crex_slow_control_simple",0,"rcdb_sign*","(rcdb_flip_state<=2)","part",""); 
  //tb->tg_err_averaging("crex_slow_control",0,"rcdb_sign*","(rcdb_flip_state<=2)","part",""); 
  //tb->tg_err_averaging("rcdb_ihwp",0,"rcdb_sign*","(rcdb_flip_state<=2)","part",""); 
  //tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","(rcdb_flip_state<=2)","part",""); 
  //tb->tg_err_averaging("rcdb_sign",0,"rcdb_sign*","(rcdb_flip_state<=2)","part",""); 
  //tb->tg_err_averaging("crex_part",0,"rcdb_sign*","(rcdb_flip_state<=2)","part",""); 

  //////////////////////////////////////////////////////////////
  //// Plotting
  //tb->tg_err_averaging("rcdb_slug",ana,"rcdb_sign*","&& rcdb_flip_state<=2","part","_no_err",suffix); // Generate grand plots from slug averaged data - with sign correction (I left the slug averaging outputs with NO sign correction, all larger time scales do have sign correction in the 1 pass outputs)
  //tb->tg_err_averaging("crex_part",ana,"","&& rcdb_flip_state<=2","part","_no_err",suffix); // Generate grand plots from slug averaged data - with sign correction (I left the slug averaging outputs with NO sign correction, all larger time scales do have sign correction in the 1 pass outputs)
 //tb->tg_err_averaging("crex_pitt",ana,"","&& rcdb_flip_state<=2","part","_no_err",suffix); // Generate grand plots from slug averaged data - with sign correction (I left the slug averaging outputs with NO sign correction, all larger time scales do have sign correction in the 1 pass outputs)
  /*
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*"," && crex_part == 1");
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*"," && crex_part == 2");
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*"," && crex_part == 3");
  */

  //tb->tg_err_averaging("rcdb_slug",1,""); // Generate NULL grand plots from slug averaged data - no sign correction
  //tb->tg_err_averaging("crex_pitt",1); // Pitt analysis either way - sign corrected (NULL would require passing NO "rcdb_sign*" at the 1st pass)

  // Slow controls
  //tb->tg_err_averaging("crex_slow_control_simple",1); // Pitt analysis
  //tb->tg_err_averaging("crex_slow_control",1); // Pitt analysis
  //tb->tg_err_averaging("rcdb_ihwp",1); // Pitt analysis
  //tb->tg_err_averaging("rcdb_flip_state",1); // Pitt analysis
  //tb->tg_err_averaging("rcdb_sign",1); // Pitt analysis
  //tb->tg_err_averaging("crex_part",1); // Pitt analysis


  cout << "-- ToolBox Analysis done in "; tsw.Print(); cout << endl;
  delete tb;
  return 0;
}
