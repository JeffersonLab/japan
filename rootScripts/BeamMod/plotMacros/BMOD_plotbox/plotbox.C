#include "toolbox.hh"
#include "tools.hh"
int plotbox(TString avg = "part", TString DataSetCut = "IncludeBMOD", TString suffix = "", Int_t ana = 0){
  Int_t do_residuals = 0;
  // TString additionalCut = "";
  // Aggressive cut: TString additionalCut = " && ( !(run==6563 && mini==0) && !(run==6564 && mini==4) && !(run==6567 && (mini==2 || mini==4)) && !(run==6571 && (mini==3 || mini==4)) && !(run==6593 && mini==2) && !(run==6629 && (mini==1 || mini==5)) && !(run==6976 && mini==0) && !(run==6983 && mini==8) && !(run==7013 && (mini==2 || mini==4)) && !(run==7014 && (mini==1 || mini==2 || mini==3)) && !(run==7149 && mini==6) && !(run==7211 && mini==4) && !(run==7257 && (mini==1 || mini==3 || mini==7)) && !(run==7657 && (mini==0 || mini==1)) && !(run==7865 && (mini==2 || mini==4 || mini==7 || mini==9)) && !(run==7866 && (mini==2 || mini==4 || mini==7)) && !(run==7867 && (mini==0 || mini==4)) && !(run==7889 && mini==0) && !(run==7942 && mini==5) && !(run==8036 && mini==2) && !(run==8046 && mini==2) && !(run==8091 && (mini==3 || mini==4)) && !(run==8129 && mini==2) && !(run==8133 && mini==1) && !(run==8187 && mini==3) && !(run==8240 && mini==1) && !(run==8549 && (mini==0 || mini==1 || mini==4)))";
  // Only fatal miniruns Cut: 
  TString additionalCut = " && ( !(run==6564 && mini==4) && !(run==6567 && (mini==2 || mini==4)) && !(run==6571 && (mini==3 || mini==4)) && !(run==6593 && mini==2) && !(run==6983 && mini==8) && !(run==7149 && mini==6) && !(run==7211 && mini==4) && !(run==7889 && mini==0) && !(run==7942 && mini==5) && !(run==8036 && mini==2) && !(run==8240 && mini==1) && !(run==8549 && (mini==0 || mini==1 || mini==4)))";
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  // _no_err is only allbpms version available at the moment (should be fixed 8/6/2021)

  //agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && 
  if (avg == "part") {
    tb->tg_err_averaging("crex_part",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections");   // Part avg all correction data
    if (DataSetCut == "ErrorFlag" && do_residuals == 1) {
      tb->tg_err_averaging("crex_part",2,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
    }
  }
  if (avg == "pitt") {
    tb->tg_err_averaging("crex_pitt",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections");   // Part avg all correction data
    tb->tg_err_averaging("crex_pitt",0,"","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2 && rcdb_ihwp == 1"+additionalCut,DataSetCut, "part","_no_err","_NULL_IN_main_det_corrections");   // Part avg all correction data
    tb->tg_err_averaging("crex_pitt",0,"","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2 && rcdb_ihwp == 2"+additionalCut,DataSetCut, "part","_no_err","_NULL_OUT_main_det_corrections");   // Part avg all correction data
    if (DataSetCut == "ErrorFlag" && do_residuals == 1) {
      tb->tg_err_averaging("crex_pitt",2,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
      tb->tg_err_averaging("crex_pitt",2,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 1"+additionalCut,DataSetCut, "part","_no_err","_NULL_IN_main_det_slopes_residuals");   // Part avg all correction data
      tb->tg_err_averaging("crex_pitt",2,"","&& rcdb_flip_state<=2 && rcdb_ihwp == 2"+additionalCut,DataSetCut, "part","_no_err","_NULL_OUT_main_det_slopes_residuals");   // Part avg all correction data
    }
  }
  if (avg == "slow") {
    tb->tg_err_averaging("rcdb_ihwp",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections");   // Part avg all correction data
    tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state==1 && crex_part<3"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections_wien1");   // Part avg all correction data
    tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state==2 && crex_part<3"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections_wien2");   // Part avg all correction data
    tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state==1 && crex_part==3"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections_wien3");   // Part avg all correction data
    tb->tg_err_averaging("rcdb_flip_state",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections");   // Part avg all correction data
    tb->tg_err_averaging("rcdb_sign",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections");   // Part avg all correction data
    tb->tg_err_averaging("crex_slow_control",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections");   // Part avg all correction data
    tb->tg_err_averaging("crex_slow_control_simple",0,"rcdb_sign*","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections");   // Part avg all correction data
    if (DataSetCut == "ErrorFlag" && do_residuals == 1) {
      tb->tg_err_averaging("rcdb_ihwp",2,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
      tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part<3"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals_wien1");   // Part avg all correction data
      tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==2 && crex_part<3"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals_wien2");   // Part avg all correction data
      tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state==1 && crex_part==3"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals_wien3");   // Part avg all correction data
      tb->tg_err_averaging("rcdb_flip_state",2,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
      tb->tg_err_averaging("rcdb_sign",2,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
      tb->tg_err_averaging("crex_slow_control",2,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
      tb->tg_err_averaging("crex_slow_control_simple",2,"rcdb_sign*","&& rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals");   // Part avg all correction data
    }
  }
  if (avg == "slug") {
    tb->tg_err_averaging("rcdb_slug",0,"","&& agg_part_avgd_allbpms_friendable.eigen_lagr_asym_main_det_nentries>=5 && rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_corrections");   // Slug avg all correction data
    if (DataSetCut == "ErrorFlag" && do_residuals == 1) {
      tb->tg_err_averaging("rcdb_slug",2,"","&& rcdb_flip_state<=2"+additionalCut,DataSetCut, "part","_no_err","_main_det_slopes_residuals");   // Slug avg all correction data
    }
  }




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
