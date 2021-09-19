#include "toolbox.hh"
#include "tools.hh"
int plotbox(TString suffix = "", Int_t ana = 0){
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  
  // _no_err is only allbpms version available at the moment (should be fixed 8/6/2021)
  //tb->tg_err_averaging("crex_part",ana,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err",suffix);   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_slug",ana,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err",suffix);   // Slug avg all correction data
  
  //tb->tg_err_averaging("crex_part",0,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_part",2,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",0,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",2,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",0,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 1","ErrorFlag", "part","_no_err","_NULL_IN_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",0,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 2","ErrorFlag", "part","_no_err","_NULL_OUT_main_det_corrections");   // Part avg all correction data
//  tb->tg_err_averaging("crex_pitt",2,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 1","ErrorFlag", "part","_no_err","_NULL_IN_main_det_slopes_residuals");   // Part avg all correction data
//  tb->tg_err_averaging("crex_pitt",2,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 2","ErrorFlag", "part","_no_err","_NULL_OUT_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_ihwp",0,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_ihwp",2,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part<3","ErrorFlag", "part","_no_err","_main_det_corrections_wien1");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part<3","ErrorFlag", "part","_no_err","_main_det_slopes_residuals_wien1");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& rcdb_flip_state==2 && crex_part<3","ErrorFlag", "part","_no_err","_main_det_corrections_wien2");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==2 && crex_part<3","ErrorFlag", "part","_no_err","_main_det_slopes_residuals_wien2");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part==3","ErrorFlag", "part","_no_err","_main_det_corrections_wien3");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part==3","ErrorFlag", "part","_no_err","_main_det_slopes_residuals_wien3");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_sign",0,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_sign",2,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("crex_slow_control",0,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_slow_control",2,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("crex_slow_control_simple",0,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_slow_control_simple",2,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_slug",0,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Slug avg all correction data
  //tb->tg_err_averaging("rcdb_slug",2,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Slug avg all correction data



  //tb->tg_err_averaging("crex_part",0,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_part",2,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",0,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",2,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",0,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 1","IncludeBMOD", "part","_no_err","_NULL_IN_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_pitt",0,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 2","IncludeBMOD", "part","_no_err","_NULL_OUT_main_det_corrections");   // Part avg all correction data
//  tb->tg_err_averaging("crex_pitt",2,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 1","IncludeBMOD", "part","_no_err","_NULL_IN_main_det_slopes_residuals");   // Part avg all correction data
//  tb->tg_err_averaging("crex_pitt",2,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 2","IncludeBMOD", "part","_no_err","_NULL_OUT_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_ihwp",0,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_ihwp",2,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part<3","IncludeBMOD", "part","_no_err","_main_det_corrections_wien1");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part<3","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals_wien1");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& rcdb_flip_state==2 && crex_part<3","IncludeBMOD", "part","_no_err","_main_det_corrections_wien2");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==2 && crex_part<3","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals_wien2");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part==3","IncludeBMOD", "part","_no_err","_main_det_corrections_wien3");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part==3","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals_wien3");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_sign",0,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_sign",2,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("crex_slow_control",0,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_slow_control",2,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("crex_slow_control_simple",0,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  //tb->tg_err_averaging("crex_slow_control_simple",2,"rcdb_sign*","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  //tb->tg_err_averaging("rcdb_slug",0,"","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_corrections");   // Slug avg all correction data
  //tb->tg_err_averaging("rcdb_slug",2,"","&& rcdb_flip_state<=2","IncludeBMOD", "part","_no_err","_main_det_slopes_residuals");   // Slug avg all correction data



  /* Special case for Ryan's AT needs 
  ////tb->tg_err_averaging("rcdb_slug",0,"","&& run<7500","ErrorFlag", "part","",suffix+"_part12"); // Generate slug averaged data - NO sign correction applied here
  ////tb->tg_err_averaging("rcdb_slug",0,"","&& run>=7500 && run<=8443","ErrorFlag", "part","",suffix+"_part3"); // Generate slug averaged data - NO sign correction applied here
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_flip_state<=2 && rcdb_slug>=107","ErrorFlag", "part","",suffix+"_part12"); // Plot the data
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*","&& rcdb_flip_state<=2 && rcdb_slug>=107","ErrorFlag", "part","",suffix+"_part3"); // Plot the data
  End special case */

  // OLD
  //tb->tg_err_averaging("rcdb_slug",2,"","","ErrorFlag", "part","",suffix); // Generate slug averaged data - NO sign correction applied here
  //tb->tg_err_averaging("dit_segment",0,"rcdb_sign*","","ErrorFlag", "part",""); // Segmentwise avg
  //tb->tg_err_averaging("crex_pitt",0,"rcdb_sign*","","ErrorFlag", "part",""); // Pitt analysis
  ////tb->tg_err_averaging("crex_pitt",0,"","",ErrorFlag", "part",""); // Pitt analysis - NULL version
  //tb->tg_err_averaging("crex_slow_control_simple",0,"rcdb_sign*","(rcdb_flip_state<=2)","ErrorFlag", "part",""); 
  //tb->tg_err_averaging("crex_slow_control",0,"rcdb_sign*","(rcdb_flip_state<=2)","ErrorFlag", "part",""); 
  //tb->tg_err_averaging("rcdb_ihwp",0,"rcdb_sign*","(rcdb_flip_state<=2)","ErrorFlag", "part",""); 
  //tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","(rcdb_flip_state<=2)","ErrorFlag", "part",""); 
  //tb->tg_err_averaging("rcdb_sign",0,"rcdb_sign*","(rcdb_flip_state<=2)","ErrorFlag", "part",""); 
  //tb->tg_err_averaging("crex_part",0,"rcdb_sign*","(rcdb_flip_state<=2)","ErrorFlag", "part",""); 

  //////////////////////////////////////////////////////////////
  //// Plotting
  //// Generate grand plots from slug averaged data - with sign correction (I left the slug averaging outputs with NO sign correction, all larger time scales do have sign correction in the 1 pass outputs)
  //tb->tg_err_averaging("rcdb_slug",ana,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err",suffix); 
  //tb->tg_err_averaging("crex_part",ana,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err",suffix); 
  //tb->tg_err_averaging("crex_pitt",ana,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err",suffix); 
  //tb->tg_err_averaging("crex_pitt",ana,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_NULL"+suffix); 
  //tb->tg_err_averaging("crex_slow_control",ana,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err",suffix); 
  //tb->tg_err_averaging("crex_slow_control_simple",ana,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err",suffix); 
  /*
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*"," && crex_part == 1","ErrorFlag", "part","_no_err",suffix);
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*"," && crex_part == 2","ErrorFlag", "part","_no_err",suffix);
  tb->tg_err_averaging("rcdb_slug",1,"rcdb_sign*"," && crex_part == 3","ErrorFlag", "part","_no_err",suffix);
  */

  /*
  tb->tg_err_averaging("crex_part",1,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("crex_part",3,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("crex_pitt",1,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("crex_pitt",3,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("crex_pitt",1,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_NULL_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("crex_pitt",3,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_NULL_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_ihwp",1,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_ihwp",3,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",1,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_flip_state",3,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_sign",1,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_sign",3,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("crex_slow_control",1,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("crex_slow_control",3,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("crex_slow_control_simple",1,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Part avg all correction data
  tb->tg_err_averaging("crex_slow_control_simple",3,"rcdb_sign*","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
  tb->tg_err_averaging("rcdb_slug",1,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_corrections");   // Slug avg all correction data
  tb->tg_err_averaging("rcdb_slug",3,"","&& rcdb_flip_state<=2","ErrorFlag", "part","_no_err","_main_det_slopes_residuals");   // Slug avg all correction data
  */




  cout << "-- ToolBox Analysis done in "; tsw.Print(); cout << endl;
  delete tb;
  return 0;
}
