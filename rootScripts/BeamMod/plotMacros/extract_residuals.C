void extract_residuals(Int_t ana = 0) {
  TChain * residuals_lagr_res   = new TChain("residuals_lagr_res");
  TChain * residuals_lagr_reg_diff_det_weighted = new TChain("residuals_lagr_reg_diff_det_weighted");
  TChain * slopes_part_avg_lagr = new TChain("slopes_part_avg_lagr");

  residuals_lagr_res   -> Add("processed_respin2_data/results_4th_Pass/CREX_All_IncludeBMOD_crex_part_Avg_Outputs_main_det_slopes_residuals.root");
  slopes_part_avg_lagr -> Add("processed_respin2_data/results_4th_Pass/CREX_All_IncludeBMOD_crex_part_Avg_Outputs_main_det_slopes_residuals.root");
  residuals_lagr_reg_diff_det_weighted -> Add("processed_respin2_data/results_4th_Pass/CREX_All_IncludeBMOD_crex_part_Avg_Outputs_main_det_slopes_residuals.root");

  residuals_lagr_res->AddFriend(residuals_lagr_reg_diff_det_weighted,"diffs");
  residuals_lagr_res->AddFriend(slopes_part_avg_lagr,"slopes");

  if (ana == 0) {
    Printf("residuals_lagr_res, usl_coili_mean:usl_coili_stddev:usr_coili_mean:usr_coili_stddev");
    residuals_lagr_res->Scan("usl_coil1_mean:usl_coil1_stddev:usr_coil1_mean:usr_coil1_stddev:usl_coil2_mean:usl_coil2_stddev:usr_coil2_mean:usr_coil2_stddev:usl_coil3_mean:usl_coil3_stddev:usr_coil3_mean:usr_coil3_stddev:usl_coil4_mean:usl_coil4_stddev:usr_coil4_mean:usr_coil4_stddev:usl_coil5_mean:usl_coil5_stddev:usr_coil5_mean:usr_coil5_stddev:usl_coil6_mean:usl_coil6_stddev:usr_coil6_mean:usr_coil6_stddev:usl_coil7_mean:usl_coil7_stddev:usr_coil7_mean:usr_coil7_stddev");

    Printf("residuals_lagr_res, usl_coili_mean");
    residuals_lagr_res->Scan("usl_coil1_mean:usl_coil2_mean:usl_coil3_mean:usl_coil4_mean:usl_coil5_mean:usl_coil6_mean:usl_coil7_mean");

    Printf("residuals_lagr_res, usl_coili_stddev");
    residuals_lagr_res->Scan("usl_coil1_stddev:usl_coil2_stddev:usl_coil3_stddev:usl_coil4_stddev:usl_coil5_stddev:usl_coil6_stddev:usl_coil7_stddev");

    Printf("residuals_lagr_res, usr_coili_mean");
    residuals_lagr_res->Scan("usr_coil1_mean:usr_coil2_mean:usr_coil3_mean:usr_coil4_mean:usr_coil5_mean:usr_coil6_mean:usr_coil7_mean");

    Printf("residuals_lagr_res, usr_coili_stddev");
    residuals_lagr_res->Scan("usr_coil1_stddev:usr_coil2_stddev:usr_coil3_stddev:usr_coil4_stddev:usr_coil5_stddev:usr_coil6_stddev:usr_coil7_stddev");

    Printf("residuals_lagr_res, us_avg_coili_mean");
    residuals_lagr_res->Scan("us_avg_coil1_mean:us_avg_coil2_mean:us_avg_coil3_mean:us_avg_coil4_mean:us_avg_coil5_mean:us_avg_coil6_mean:us_avg_coil7_mean");

    Printf("residuals_lagr_res, us_avg_coili_stddev");
    residuals_lagr_res->Scan("us_avg_coil1_stddev:us_avg_coil2_stddev:us_avg_coil3_stddev:us_avg_coil4_stddev:us_avg_coil5_stddev:us_avg_coil6_stddev:us_avg_coil7_stddev");



    Printf("residuals_lagr_res, usl_coili_mean_self:usl_coili_stddev_self:usr_coili_mean_self:usr_coili_stddev_self");
    residuals_lagr_res->Scan("usl_coil1_mean_self:usl_coil1_stddev_self:usr_coil1_mean_self:usr_coil1_stddev_self:usl_coil2_mean_self:usl_coil2_stddev_self:usr_coil2_mean_self:usr_coil2_stddev_self:usl_coil3_mean_self:usl_coil3_stddev_self:usr_coil3_mean_self:usr_coil3_stddev_self:usl_coil4_mean_self:usl_coil4_stddev_self:usr_coil4_mean_self:usr_coil4_stddev_self:usl_coil5_mean_self:usl_coil5_stddev_self:usr_coil5_mean_self:usr_coil5_stddev_self:usl_coil6_mean_self:usl_coil6_stddev_self:usr_coil6_mean_self:usr_coil6_stddev_self:usl_coil7_mean_self:usl_coil7_stddev_self:usr_coil7_mean_self:usr_coil7_stddev_self");

    Printf("residuals_lagr_res, usl_coili_mean_self");
    residuals_lagr_res->Scan("usl_coil1_mean_self:usl_coil2_mean_self:usl_coil3_mean_self:usl_coil4_mean_self:usl_coil5_mean_self:usl_coil6_mean_self:usl_coil7_mean_self");

    Printf("residuals_lagr_res, usl_coili_stddev_self");
    residuals_lagr_res->Scan("usl_coil1_stddev_self:usl_coil2_stddev_self:usl_coil3_stddev_self:usl_coil4_stddev_self:usl_coil5_stddev_self:usl_coil6_stddev_self:usl_coil7_stddev_self");

    Printf("residuals_lagr_res, usr_coili_mean_self");
    residuals_lagr_res->Scan("usr_coil1_mean_self:usr_coil2_mean_self:usr_coil3_mean_self:usr_coil4_mean_self:usr_coil5_mean_self:usr_coil6_mean_self:usr_coil7_mean_self");

    Printf("residuals_lagr_res, usr_coili_stddev_self");
    residuals_lagr_res->Scan("usr_coil1_stddev_self:usr_coil2_stddev_self:usr_coil3_stddev_self:usr_coil4_stddev_self:usr_coil5_stddev_self:usr_coil6_stddev_self:usr_coil7_stddev_self");

    Printf("residuals_lagr_res, us_avg_coili_mean_self");
    residuals_lagr_res->Scan("us_avg_coil1_mean_self:us_avg_coil2_mean_self:us_avg_coil3_mean_self:us_avg_coil4_mean_self:us_avg_coil5_mean_self:us_avg_coil6_mean_self:us_avg_coil7_mean_self");

    Printf("residuals_lagr_res, us_avg_coili_stddev_self");
    residuals_lagr_res->Scan("us_avg_coil1_stddev_self:us_avg_coil2_stddev_self:us_avg_coil3_stddev_self:us_avg_coil4_stddev_self:us_avg_coil5_stddev_self:us_avg_coil6_stddev_self:us_avg_coil7_stddev_self");
  }

  if (ana == 1) {

    Printf("US Avg slopes per monitor per CREX part, us_avg_evMonN_mean, slope diff us_avg_evMonN_mean, slope diff us_avg_evMonN_stddev");
    for (Int_t part = 1 ; part <= 3 ; part++) {
      Printf("Part %d",part);
      for (Int_t mon = 0 ; mon < 12 ; mon++) {
        residuals_lagr_res->Draw(Form("slopes.us_avg_evMon%d_mean/1e-3:diffs.us_avg_evMon%d_mean:diffs.us_avg_evMon%d_stddev",mon,mon,mon),Form("crex_part == %d",part),"goff");
        Printf("%d, %0.1f, %0.1f, %0.1f",mon,residuals_lagr_res->GetV1()[0],residuals_lagr_res->GetV2()[0],residuals_lagr_res->GetV3()[0]);
      }
    }

    /*
    Printf("US Avg slopes diffs percentages per monitor per CREX part, diff mean / us_avg_evMonN_mean, diff RMS / us_avg_evMonN_mean");
    for (Int_t part = 1 ; part <= 3 ; part++) {
      Printf("Part %d",part);
      for (Int_t mon = 0 ; mon < 12 ; mon++) {
        residuals_lagr_res->Draw(Form("100.0*(diffs.us_avg_evMon%d_mean)/(slopes.us_avg_evMon%d_mean/1e-3):100.0*(diffs.us_avg_evMon%d_stddev)/(slopes.us_avg_evMon%d_mean/1e-3)",mon,mon,mon,mon),Form("crex_part == %d",part),"goff");
        Printf("%d, %0.1f, %0.1f",mon,residuals_lagr_res->GetV1()[0],residuals_lagr_res->GetV2()[0]);
      }
    }*/

  }

}
