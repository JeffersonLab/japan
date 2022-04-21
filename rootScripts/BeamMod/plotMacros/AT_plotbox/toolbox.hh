#include "TVirtualPad.h"
#include <fstream>

class ToolBox{
  public:
    ~ToolBox();

    // Weighted averaging plots
    TVirtualPad* manyGraph(TPad*, TVirtualPad*, TString, TChain*, TString, TString, TString, std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, Int_t);

    void tg_err_averaging(TString,Int_t,TString,TString,TString,TString,TString,TString);

    void combo_tg_err_segment_getter(TString, TTree*, TTree*, TString, TString, TString, std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, Int_t);

};

ToolBox::~ToolBox(){
};

TVirtualPad* ToolBox::manyGraph(TPad* c1, TVirtualPad* p1, TString averaging, TChain* intree, TString proto_draw, TString proto_draw_weighting_error, TString proto_drawn_channels_error, std::vector<TString> draws_piece1, std::vector<TString> draws_piece2, std::vector<TString> draws_piece3 = {}, std::vector<TString> cuts = {}, std::vector<TString> cuts2 = {}, Int_t startcolor = 30) {
  Int_t nen=0;
  Int_t use_mon_cuts = 0;
  Int_t icut = 0;
  if (cuts.size() == 0) {
    for (Int_t i = 0 ; i < draws_piece1.size() ; i++) {
      TString tmp1 = "";
      cuts.push_back(tmp1);
    }
    TString tmp1 = "";
    cuts.push_back(tmp1);  // Bonus cut at the end for the "get data to select from" cut->map stuff
  }
  if (cuts2.size() != 0) {
    if (cuts2.size() < draws_piece2.size()) {
      Printf("Error, expected that cuts2 would have >= number of cuts as draws_piece2 has");
      return 0;
    }
    use_mon_cuts = 1;
    cuts.clear();
    for (Int_t i = 0 ; i < cuts2.size() ; i++) {
      Printf("Adding cut %s",cuts2.at(i).Data());
      cuts.push_back(cuts2.at(i));
    }
  }
  
  TString draw = "";
  TString drawn_channels_error = "";
  TString draw_weighting_error = "";
  TString draw_1 = "";
  TString draw_2 = "";
  TString draw_3 = "";
  TString draw_4 = "";
  TString draw_1_err = "";
  TString draw_2_err = "";
  TString draw_3_err = "";
  TString draw_4_err = "";
  TString draw_1_weight_err = "";
  TString draw_2_weight_err = "";
  TString draw_3_weight_err = "";
  TString draw_4_weight_err = "";

  TGraphErrors* tge;
  Int_t subpad = 1;
  std::ofstream outfile0;
  outfile0.open(Form("./processed_respin1_data/Results_%s.csv",averaging.Data()),std::ofstream::app);

  for (Int_t idet = 0 ; idet < draws_piece1.size() ; idet++) {
    //Printf("test 1 slopes");
    for (Int_t imon = 0 ; imon < draws_piece2.size() ; imon++) {
      if (draws_piece3.size() <= imon) { // draws 3 assumes to be using the same monitor as draws 2 (for applying corrections)
        TString tmp2 = "";
        draws_piece3.push_back(tmp2);
      }
      if (proto_draw.Contains("#")) {
        draw_1 = proto_draw(0,proto_draw.First('#'));
        draw_2 = proto_draw(proto_draw.First('#')+1,proto_draw.Length()-1);
        if (draw_2.Contains("#")) {
          draw_3 = draw_2(draw_2.First('#')+1,draw_2.Length()-1);
          draw_2 = draw_2(0,draw_2.First('#'));
          if (draw_3.Contains("#")) {
            draw_4 = draw_3(draw_3.First('#')+1,draw_3.Length()-1);
            //draw_4_err = draw_4;
            //draw_4_err = draw_4_err.ReplaceAll(".mean",".err");
            //draw_4_err = draw_4_err.ReplaceAll("_mean","_mean_error");
            draw_3 = draw_3(0,draw_3.First('#'));
          }
          else {
            draw_4 = "";
            //draw_4_err = "";
            //draw_3 = "";
            draws_piece3.at(imon) = "";
          }
        }
        else {
          draw_4 = "";
          //draw_4_err = "";
          draw_3 = "";
          draws_piece2.at(imon) = "";
        }
        draw = draw_1+draws_piece1.at(idet)+draw_2+draws_piece2.at(imon)+draw_3+draws_piece3.at(imon)+draw_4;
        if (draws_piece1.at(idet).Contains("manual_main_det")) {
          draw = 
            "((rcdb_arm_flag==0)*(" + draw_1+"us_avg"+draw_2+draws_piece2.at(imon)+draw_3+draws_piece3.at(imon)+draw_4 + ")"
            +"+(rcdb_arm_flag==1)*(" + draw_1+"usr"+draw_2+draws_piece2.at(imon)+draw_3+draws_piece3.at(imon)+draw_4 + ")"
            +"+(rcdb_arm_flag==2)*(" + draw_1+"usl"+draw_2+draws_piece2.at(imon)+draw_3+draws_piece3.at(imon)+draw_4 + "))";
        }
        if (draws_piece2.at(imon).Contains("manual_main_det")) {
          if (draws_piece3.at(imon).Contains("manual_main_det")) {
            draw = 
              "((rcdb_arm_flag==0)*(" + draw_1+draws_piece1.at(idet)+draw_2+"us_avg"+draw_3+"us_avg"+draw_4 + ")"
              +"+(rcdb_arm_flag==1)*(" + draw_1+draws_piece1.at(idet)+draw_2+"usr"+draw_3+"usr"+draw_4 + ")"
              +"+(rcdb_arm_flag==2)*(" + draw_1+draws_piece1.at(idet)+draw_2+"usl"+draw_3+"usl"+draw_4 + "))";
          }
          else {
            draw = 
              "((rcdb_arm_flag==0)*(" + draw_1+draws_piece1.at(idet)+draw_2+"us_avg"+draw_3+draw_4 + ")"
              +"+(rcdb_arm_flag==1)*(" + draw_1+draws_piece1.at(idet)+draw_2+"usr"+draw_3+draw_4 + ")"
              +"+(rcdb_arm_flag==2)*(" + draw_1+draws_piece1.at(idet)+draw_2+"usl"+draw_3+draw_4 + "))";
          }
        }
      }
      else {
        draw = proto_draw;
      }

 

      // Now do the error draw
      if (proto_drawn_channels_error.Contains("#")) {
        draw_1_err = proto_drawn_channels_error(0,proto_drawn_channels_error.First('#'));
        draw_2_err = proto_drawn_channels_error(proto_drawn_channels_error.First('#')+1,proto_drawn_channels_error.Length()-1);
        if (draw_2_err.Contains("#")) {
          draw_3_err = draw_2_err(draw_2_err.First('#')+1,draw_2_err.Length()-1);
          draw_2_err = draw_2_err(0,draw_2_err.First('#'));
          if (draw_3_err.Contains("#")) {
            draw_4_err = draw_3_err(draw_3_err.First('#')+1,draw_3_err.Length()-1);
            draw_3_err = draw_3_err(0,draw_3_err.First('#'));
          }
          else {
            draw_4_err = "";
            //draw_3_err = "";
            draws_piece3.at(imon) = "";
          }
        }
        else {
          draw_4_err = "";
          draw_3_err = "";
          draws_piece2.at(imon) = "";
        }
        drawn_channels_error = draw_1_err+draws_piece1.at(idet)+draw_2_err+draws_piece2.at(imon)+draw_3_err+draws_piece3.at(imon)+draw_4_err;
        if (draws_piece1.at(idet).Contains("manual_main_det")) {
          drawn_channels_error = 
             "((rcdb_arm_flag==0)*(" + draw_1_err+"us_avg"+draw_2_err+draws_piece2.at(imon)+draw_3_err+draws_piece3.at(imon)+draw_4_err + ")"
            +"+(rcdb_arm_flag==1)*(" + draw_1_err+"usr"+draw_2_err+draws_piece2.at(imon)+draw_3_err+draws_piece3.at(imon)+draw_4_err + ")"
            +"+(rcdb_arm_flag==2)*(" + draw_1_err+"usl"+draw_2_err+draws_piece2.at(imon)+draw_3_err+draws_piece3.at(imon)+draw_4_err + "))";
        }
        if (draws_piece2.at(imon).Contains("manual_main_det")) {
          if (draws_piece3.at(imon).Contains("manual_main_det")) {
            drawn_channels_error = 
               "((rcdb_arm_flag==0)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"us_avg"+draw_3_err+"us_avg"+draw_4_err + ")"
              +"+(rcdb_arm_flag==1)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"usr"+draw_3_err+"usr"+draw_4_err + ")"
              +"+(rcdb_arm_flag==2)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"usl"+draw_3_err+"usl"+draw_4_err + "))";
          }
          else {
            drawn_channels_error = 
               "((rcdb_arm_flag==0)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"us_avg"+draw_3_err+draw_4_err + ")"
              +"+(rcdb_arm_flag==1)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"usr"+draw_3_err+draw_4_err + ")"
              +"+(rcdb_arm_flag==2)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"usl"+draw_3_err+draw_4_err + "))";
          }
        }
      }
      else {
        drawn_channels_error = proto_drawn_channels_error;
      }

      // Now do the weighting source error draw
      // FIXME // Draw error here assumes we are using the part-avged eigen vector based regression errors as input, and that the intree tree has been friended into it.
      if (proto_draw_weighting_error == proto_drawn_channels_error) {
        draw_weighting_error = drawn_channels_error;
      }
      else {
        if (proto_draw_weighting_error.Contains("#")) {
          draw_1_weight_err = proto_draw_weighting_error(0,proto_draw_weighting_error.First('#'));
          draw_2_weight_err = proto_draw_weighting_error(proto_draw_weighting_error.First('#')+1,proto_draw_weighting_error.Length()-1);
          if (draw_2_weight_err.Contains("#")) {
            draw_3_weight_err = draw_2_weight_err(draw_2_weight_err.First('#')+1,draw_2_weight_err.Length()-1);
            draw_2_weight_err = draw_2_weight_err(0,draw_2_weight_err.First('#'));
            if (draw_3_weight_err.Contains("#")) {
              draw_4_weight_err = draw_3_weight_err(draw_3_weight_err.First('#')+1,draw_3_weight_err.Length()-1);
              draw_3_weight_err = draw_3_weight_err(0,draw_3_weight_err.First('#'));
            }
            else {
              draw_4_weight_err = "";
              //draw_3_weight_err = "";
              draws_piece3.at(imon) = "";
            }
          }
          else {
            draw_4_weight_err = "";
            draw_3_weight_err = "";
            draws_piece2.at(imon) = "";
          }
          draw_weighting_error = draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+draws_piece2.at(imon)+draw_3_weight_err+draws_piece3.at(imon)+draw_4_weight_err;
          if (draws_piece1.at(idet).Contains("manual_main_det")) {
            // Add in a part before and after "manual_main_det" to preserve other stuff (like reg_asym_ or _reg_cor_diff...) // FIXME FIXME 
            draw_weighting_error = 
              "((rcdb_arm_flag==0)*(" + draw_1_weight_err+"us_avg"+draw_2_weight_err+draws_piece2.at(imon)+draw_3_weight_err+draws_piece3.at(imon)+draw_4_weight_err + ")"
              +"+(rcdb_arm_flag==1)*(" + draw_1_weight_err+"usr"+draw_2_weight_err+draws_piece2.at(imon)+draw_3_weight_err+draws_piece3.at(imon)+draw_4_weight_err + ")"
              +"+(rcdb_arm_flag==2)*(" + draw_1_weight_err+"usl"+draw_2_weight_err+draws_piece2.at(imon)+draw_3_weight_err+draws_piece3.at(imon)+draw_4_weight_err + "))";
          }
          if (draws_piece2.at(imon).Contains("manual_main_det")) {
            if (draws_piece3.at(imon).Contains("manual_main_det")) {
              draw_weighting_error = 
                "((rcdb_arm_flag==0)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"us_avg"+draw_3_weight_err+"us_avg"+draw_4_weight_err + ")"
                +"+(rcdb_arm_flag==1)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"usr"+draw_3_weight_err+"usr"+draw_4_weight_err + ")"
                +"+(rcdb_arm_flag==2)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"usl"+draw_3_weight_err+"usl"+draw_4_weight_err + "))";
            }
            else {
              draw_weighting_error = 
                "((rcdb_arm_flag==0)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"us_avg"+draw_3_weight_err+draw_4_weight_err + ")"
                +"+(rcdb_arm_flag==1)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"usr"+draw_3_weight_err+draw_4_weight_err + ")"
                +"+(rcdb_arm_flag==2)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"usl"+draw_3_weight_err+draw_4_weight_err + "))";
            }
          }
        }
        else {
          draw_weighting_error = proto_draw_weighting_error;
        }
      }
      //Printf("%s + %s + %s = %s",proto_draw.Data(),draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),draw.Data());

      // Default tge p0 case
      //TString draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";

      // Similar to default tge p0 case - increase error in fixed scale factor = 4x
      //TString draw_weighting_error = "4*((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";

      // Similar to default tge p0 case - increase error in fixed scale factor = 1/(sigma total experiment)
      //TString draw_weighting_error = "(1/87.0)*((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";

      // Set error to 0 - unweighted TGraphErrors approach
      //TString draw_weighting_error = "((rcdb_arm_flag==0)*(0)+(rcdb_arm_flag==1)*(0)+(rcdb_arm_flag==2)*(0))/(ppb)";

      // Set error to 1 - fixed weight TGraphErrors approach
      //TString draw_weighting_error = "((rcdb_arm_flag==0)*(1)+(rcdb_arm_flag==1)*(1)+(rcdb_arm_flag==2)*(1))/(ppb)";

      // Special test case using self-BPM only weighting as the relative rescaled error bar (instead of maindet asym error).
      //draw_weighting_error = "agg_part_avgd_friendable.diff_evMon0_mean_error";

      TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
      //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
      std::vector<TGraph*> mgs;
      TMultiGraph *mg = new TMultiGraph();

      if (use_mon_cuts == 1) {
        icut = imon;
      }
      else {
        icut = idet;
      }

      TString localCut = "1==1";
      if (cuts.at(icut) != "") {
        localCut = cuts.at(icut);
      }
      /* // Replace explicit cut!="" check with localCut variable arithmetic
      if (cuts.at(icut) == "") {
        //Printf("Test e) iterLocalAvg_time_scale = %d",iterLocalAvg_time_scale);
        nen = intree->Draw(Form("%s:%s:%s:%s",draw.Data(),averaging.Data(),draw_weighting_error.Data(),drawn_channels_error.Data()),Form(""),"goff");
        //Printf("Test f) iterLocalAvg_time_scale = %d",iterLocalAvg_time_scale);
        if (imon == 0 && idet == 0){
          //Printf("C) Avg timescale %s = %d for entry %d",averaging.Data(),iterLocalAvg_time_scale,ient);
          //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d",nen,data_tree_name.Data(),draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),averaging.Data(),iterLocalAvg_time_scale);
        }
      }
      else {
        nen = intree->Draw(Form("%s:%s:%s:%s",draw.Data(),averaging.Data(),draw_weighting_error.Data(),drawn_channels_error.Data()),Form("%s",cuts.at(icut).Data()),"goff");
        //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d && %s",nen,data_tree_name.Data(),draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),averaging.Data(),iterLocalAvg_time_scale,cuts.at(icut).Data());
      }
      */
      nen = intree->Draw(Form("%s:%s:%s:%s",draw.Data(),averaging.Data(),draw_weighting_error.Data(),drawn_channels_error.Data()),Form("%s",localCut.Data()),"goff");
 


      if (nen != 0) {
        // Special case calculating sum(errs2)
        Double_t sum_sigs = 0.0;
        Double_t sum_self_sigs = 0.0;
        //Double_t ssigs = 0.0;
        for (Int_t k = 0 ; k < nen ; k++) {
          //ssigs += pow(intree->GetV3()[k],2);
          sum_sigs += 1.0/pow(intree->GetV3()[k],2); // Sum of sigmas here = SUM(1/sigma^2)
          sum_self_sigs += 1.0/pow(intree->GetV4()[k],2); // Sum of sigmas here = SUM(1/sigma^2)
        }
        Printf("");
        //Printf("Using sum of 1/sigmas2 = %e",sum_sigs);
        //Printf("Using sum of sigmas = %f",ssigs);

        // Special case calculating weighted mean and errors explicitly sum(Data/errs2)
        Double_t sum_signal_sig2 = 0.0;
        Double_t sum_signal_self_sig2 = 0.0;
        for (Int_t k = 0 ; k < nen ; k++) {
          //Printf("Error for run+0.1*mini %f = %e +- %e",intree->GetV2()[k],intree->GetV1()[k],intree->GetV3()[k]);
          sum_signal_sig2 += intree->GetV1()[k]/pow(intree->GetV3()[k],2);
          sum_signal_self_sig2 += intree->GetV1()[k]/pow(intree->GetV4()[k],2);
        }
        Double_t weighted_mean = sum_signal_sig2/sum_sigs;
        Double_t self_weighted_mean = sum_signal_self_sig2/sum_self_sigs;
        Double_t sum_deviation_signal_sig2 = 0.0;
        Double_t sum_det_sig2 = 0.0;
        Double_t sum_deviation_signal_self_sig2 = 0.0;
        for (Int_t k = 0 ; k < nen ; k++) {
          sum_deviation_signal_sig2 += pow(intree->GetV1()[k]-(weighted_mean),2)/pow(intree->GetV3()[k],2);
          sum_det_sig2 += pow(intree->GetV4()[k],2)/pow(intree->GetV3()[k],4);
          // Self weighted stddev
          sum_deviation_signal_self_sig2 += pow(intree->GetV1()[k]-(self_weighted_mean),2)/pow(intree->GetV4()[k],2);
        }
        Double_t weighted_mean_stddev = sqrt(sum_deviation_signal_sig2/sum_sigs);
        Double_t self_weighted_mean_stddev = sqrt(sum_deviation_signal_self_sig2/sum_self_sigs);
        Double_t mean_self_red_chi2_tmp1 = 0.0;
        Double_t weighted_mean_err_scale = sqrt(sum_det_sig2/sum_sigs);
        Double_t weighted_mean_err = sqrt(sum_det_sig2)/sum_sigs;
        Double_t self_weighted_mean_err = sqrt(1.0/sum_self_sigs);

        Printf("The weighted mean is = %e/%e = %e",sum_signal_sig2,sum_sigs,weighted_mean);
        Printf("The weighted sample variance (stdev) is = sqrt(Sum(weights*(deviation from mean)^2)/Sum(weights)) = %e",weighted_mean_stddev);
        Printf("The weighted error bar (sigma) is = sqrt(Sum(weights^2*(detector sigma)^2)/Sum(weights)) = %e",weighted_mean_err);
        Printf("The weighted mean error scaling = weighted mean of ratio of error bars (1/sqrt(weights)) is = sqrt(Sum(weights^2*(detector sigma)^2)/Sum(weights)) = %e",weighted_mean_err_scale);

        // Use 1/sum(errs^2) directly
        //nen = intree->Draw(Form("%s:run+0.1*mini:(%e)*%s",draw.Data(),sum_sigs,draw_weighting_error.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale,localCut.Data()),"goff");

        // Use 1/sqrt(sum(errs^2)) instead
        //nen = intree->Draw(Form("%s:run+0.1*mini:sqrt(%e)*%s",draw.Data(),sum_sigs,draw_weighting_error.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale,localCut.Data()),"goff");

        // Use sigma = sigma*main det slugwise rescale factor
        Printf("Using error bar from %s",draw_weighting_error.Data());
        nen = intree->Draw(Form("%s:%s:%e*%s",draw.Data(),averaging.Data(),weighted_mean_err_scale,draw_weighting_error.Data()),Form("%s",localCut.Data()),"goff");
        // Get mean
        tge = 0;

        // Use TGraphErrors
        tge = new TGraphErrors(nen,intree->GetV2(),intree->GetV1(),0,intree->GetV3());
  
        // MultiGraph area
        // idet goes from 0 to draws_piece1.size(), imon goes from 0 to draws_piece2.size();
        subpad = idet*draws_piece2.size() + imon + 1; 
        p1=c1->cd(subpad);
        p1->SetGrid();


        //if (cut.Contains("rcdb_sign")) { 
        //nen = intree->Draw(Form("rcdb_sign*(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.Data(),draw.Data()),cut.Data(),"goff");
        //}
        //else {
        //nen = intree->Draw(Form("(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.Data(),draw.Data()),cut.Data(),"goff");
        //}
        //nen = intree->Draw(draw,cut.Data(),"goff"); 
        //printf("Plotting %d good entries where 13746 is ideal\n",nen);
        tge = new TGraphErrors(nen,intree->GetV2(),intree->GetV1(),0,intree->GetV3());
        mgs.push_back(tge);
        tge->SetNameTitle(Form("%s_%d",draw.Data(),subpad),Form("%s_%d",draw.Data(),subpad));
        tge->SetLineColor(startcolor);
        tge->SetMarkerColor(startcolor);
        tge->SetMarkerStyle(20);
        tge->SetMarkerSize(0.2);
        if (tge != 0) {
          /* Remove multigraph
          mg->Add(tge,"p");

          if (mg!=0) {
            mg->Fit("pol0");
            gStyle->SetOptFit(1);
            tge->SetNameTitle(Form("tge %s",draw.Data()),Form("%s ;%s ;%s",draw.Data(),averaging.Data(),draw.Data()));
            mg->SetNameTitle(Form("mg %s",draw.Data()),Form("%s ;%s ;%s",draw.Data(),averaging.Data(),draw.Data()));
            (mg->GetHistogram())->SetNameTitle(Form("mg %s",draw.Data()),Form("%s ;%s ;%s",draw.Data(),averaging.Data(),draw.Data()));
            mg->Draw("alp");
            */


            tge->Fit("pol0");
            gStyle->SetOptFit(1);
           

             tge->SetNameTitle(Form("%sasym",draws_piece2.at(imon).Data()),Form("Sign Corrected %s Asymmetry (ppb);%s ;%s",draws_piece2.at(imon).Data(),averaging.Data(),draws_piece2.at(imon).Data())); 
           

          // (tge->GetHistogram())->SetNameTitle(Form("tge h %s",draw.Data()),Form("%s ;%s;%s",draw.Data(),averaging.Data(),draw.Data()));
     
             (tge->GetHistogram())->SetNameTitle(Form("%sasym",draws_piece2.at(imon).Data()),Form("Sign Corrected %s Asymmetry (ppb); %s; %s",draws_piece2.at(imon).Data(),averaging.Data(),draws_piece2.at(imon).Data()));
          tge->Draw("ap");

            if (mgs.size() > 1) { // FIXME
              legend->AddEntry(tge,localCut,"p"); // Figure this one out later... FIXME FIXME
              legend->Draw();
            }
            //p1->BuildLegend(); // Automated legend builder and placer

            //if (p1->FindObject("htemp")) {
            //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.Data()));
            //}

            // Use a regular TGraph
            //tge = new TGraph(nen,intree->GetV2(),intree->GetV1());//,0,intree->GetV3());
            //Printf("test 3 slopes means");
            //tge->Fit("pol0");

            /*  Remove multigraph
            TF1* tfe = mg->GetFunction("pol0");
            */
            TF1* tfe = tge->GetFunction("pol0");
            Double_t avg_tmp1 = 0.0;
            Double_t mean_err_tmp1 = 0.0;
            Double_t mean_red_chi2_err_tmp1 = 0.0;
            Double_t mean_red_chi2_tmp1 = 0.0;
            Double_t chi2_tmp1 = 0.0;
            avg_tmp1 = (Double_t)(tfe->GetParameter(0));
            mean_err_tmp1 = (Double_t)(tfe->GetParError(0));
            chi2_tmp1 = (Double_t)(tfe->GetChisquare());
            if (nen > 1) {
              mean_red_chi2_err_tmp1 = (Double_t)(tfe->GetParError(0)*sqrt(chi2_tmp1/(nen-1)));
              mean_red_chi2_tmp1 = (Double_t)(sqrt(chi2_tmp1/(nen-1)));
              mean_self_red_chi2_tmp1 = sqrt(sum_deviation_signal_self_sig2/(nen-1));
            }
            else {
              mean_red_chi2_err_tmp1 = (Double_t)(tfe->GetParError(0)*sqrt(chi2_tmp1));
              mean_red_chi2_tmp1 = (Double_t)(sqrt(chi2_tmp1));
              mean_self_red_chi2_tmp1 = sqrt(sum_deviation_signal_self_sig2);
            }
            if (imon == 0 && idet == 0){
              //Printf("New mean %s %s = %f, entries = %d",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),avg_tmp1,nen);
            }
            /*
            dataVec2_mean[idet][imon]              = avg_tmp1;
            dataVec2_mean_red_chi2_err[idet][imon] = mean_red_chi2_err_tmp1;
            dataVec2_mean_err[idet][imon]          = mean_err_tmp1;
            dataVec2_stddev[idet][imon]            = weighted_mean_stddev;
            dataVec2_mean_red_chi2[idet][imon]     = mean_red_chi2_tmp1;
            dataVec2_mean_chi2[idet][imon]         = chi2_tmp1;
            dataVec2_mean_self[idet][imon]         = self_weighted_mean;
            dataVec2_mean_self_err[idet][imon]     = self_weighted_mean_err;
            dataVec2_stddev_self[idet][imon]       = self_weighted_mean_stddev;
            dataVec2_mean_self_red_chi2[idet][imon]= mean_self_red_chi2_tmp1;
            dataVec2_mean_self_chi2[idet][imon]    = sum_deviation_signal_self_sig2;
            dataVec2_mean_nentries[idet][imon]     = nen;
            */
            outfile0
              << draw << ", "
              << averaging << ", "
              << draw_weighting_error << ", "
              << localCut << ", "
              << avg_tmp1 << ", "
              << mean_err_tmp1 << ", "
              << weighted_mean_stddev << ", "
              << mean_red_chi2_err_tmp1 << ", "
              << mean_red_chi2_tmp1 << ", "
              << chi2_tmp1 << ", "
              << self_weighted_mean << ", "
              << self_weighted_mean_err << ", "
              << self_weighted_mean_stddev << ", "
              << mean_self_red_chi2_tmp1 << ", "
              << sum_deviation_signal_self_sig2 << ", "
              << nen << std::endl;

          }
        /* Remove multigraph
        }
        */
      }
      else {
        //Printf("Using prior segment's value for now...");
        /*
           dataVec2_mean[idet][imon]              = 0.0; /// Use 0.0? Or -1e6?
           dataVec2_mean_red_chi2_err[idet][imon] = 0.0;
           dataVec2_mean_err[idet][imon]          = 0.0;
           dataVec2_stddev[idet][imon]            = 0.0;
           dataVec2_mean_red_chi2[idet][imon]     = 0.0;
           dataVec2_mean_chi2[idet][imon]         = 0.0;
           dataVec2_mean_self[idet][imon]         = 0.0;
           dataVec2_mean_self_err[idet][imon]     = 0.0;
           dataVec2_stddev_self[idet][imon]       = 0.0;
           dataVec2_mean_self_red_chi2[idet][imon]= 0.0;
           dataVec2_mean_self_chi2[idet][imon]    = 0.0;
           dataVec2_mean_nentries[idet][imon]     = 0.0;
           */
      }
    }
  }
  outfile0.close();
  return p1;
}

void ToolBox::combo_tg_err_segment_getter(TString averaging,TTree* intree, TTree* outtree/*_orig*/, TString proto_draw, TString proto_draw_weighting_error, TString proto_drawn_channels_error, std::vector<TString> draws_piece1, std::vector<TString> draws_piece2, std::vector <TString> draws_piece3 = {}, std::vector<TString> cuts = {}, std::vector<TString> cuts2 = {}, Int_t print_per_entry = 0){
  // FIXME FIXME FIXME Add a 3rd tree here to be the tree over which the entries are calculated and the print_per_entry is looped over... 
  // ASSUMES the averaging time scale name works equally well in both trees... if not then add a bunch of logic and another TString here. 
  // (if tree's name == dit then dit_segment->segment should be the only caveat) 
  // -> The goal here is to print the segment-wise averaged slopes in one analysis and the run-wise averaged BMOD sensitivity outputs at the burst-wise level... no averaging, just reprinting the same number over and over again - for missing entries in dit tree use either neighbor or segment averaged values...
  // FIXME FIXME FIXME Also add in a draws_piece3 here to allow for correction calculations, etc.

  // This program finds segmentwise averages (using goff TH1 auto-draws)
  // of Eigen Vector BPM combo compositions (and slopes too) and prints to tree
  //

  // Define a new method which takes as argument the variable name against which to perform averaging, TTree of slopes to average, the 3 deep vector to assign as new Branches, and the DV, Monitor, and IV name vectors. Execute this for all trees of slopes that need averaging.
  // For each DV, loop over draws_piece2, for each Monitor loop over component BPMs.
  // For each thing create a branch to hold it - these need to be 3 deep vectors, pre-filled with 0.0's (each 0.0 needs to be a double, each layer needs to be a std::vector, etc. be careful)
  // Loop over every event to fill a map with segment numbers (and number of miniruns within them)
  // Loop over the map of segment numbers to generate averages per segment for the 3 deep vector created before (consider writing this code so it can be augmented in the future to do run-wise or slug-wise averaging as well)
  //   After finishing the 3 deep vector, for each segment value Fill the tree with those numbers. Be sure to pass the htemp->GetMean() into a locally defined Double_t and then assign the vector's value = this new Double_t before doing Fill (otherwise the pointer to GetMean() will be obtained and you will fail)

  Int_t use_mon_cuts = 0;
  Int_t icut = 0;
  if (cuts.size() == 0 && cuts2.size() == 0) {
    for (Int_t i = 0 ; i < draws_piece1.size() ; i++) {
      TString tmp1 = "";
      cuts.push_back(tmp1);
    }
    TString tmp1 = "";
    cuts.push_back(tmp1);  // Bonus cut at the end for the "get data to select from" cut->map stuff
  }
  if (cuts2.size() != 0) {
    if (cuts2.size() < draws_piece2.size()) {
      Printf("Error, expected that cuts2 would have >= number of cuts as draws_piece2 has");
      return;
    }
    use_mon_cuts = 1;
    cuts.clear();
    for (Int_t i = 0 ; i < cuts2.size() ; i++) {
      Printf("Adding cut %s",cuts2.at(i).Data());
      cuts.push_back(cuts2.at(i));
    }
  }
  
  TString draw = "";
  TString drawn_channels_error = "";
  TString draw_weighting_error = "";
  TString draw_1 = "";
  TString draw_2 = "";
  TString draw_3 = "";
  TString draw_4 = "";
  TString draw_1_err = "";
  TString draw_2_err = "";
  TString draw_3_err = "";
  TString draw_4_err = "";
  TString draw_1_weight_err = "";
  TString draw_2_weight_err = "";
  TString draw_3_weight_err = "";
  TString draw_4_weight_err = "";
  //TTree * outtree = (TTree*) outtree_orig->Clone(0);
  //Int_t nentries_orig = outtree_orig->GetEntries();
  Int_t nentries_orig = outtree->GetEntries();

  std::vector<std::vector<Double_t>> dataVec2_mean;
  std::vector<std::vector<Double_t>> dataVec2_mean_red_chi2_err;
  std::vector<std::vector<Double_t>> dataVec2_mean_err;
  std::vector<std::vector<Double_t>> dataVec2_mean_err_global;
  std::vector<std::vector<Double_t>> dataVec2_mean_err_global_rescale;   // Renormalize between minirun-wise error bar size to main-det weighting
  std::vector<std::vector<Double_t>> dataVec2_mean_err_local_rescale;    // Renormalize " " but only over the local segment time period
  std::vector<std::vector<Double_t>> dataVec2_stddev;
  std::vector<std::vector<Double_t>> dataVec2_mean_red_chi2;
  std::vector<std::vector<Double_t>> dataVec2_mean_chi2;
  std::vector<std::vector<Double_t>> dataVec2_mean_self;
  std::vector<std::vector<Double_t>> dataVec2_mean_self_err;
  std::vector<std::vector<Double_t>> dataVec2_stddev_self;
  std::vector<std::vector<Double_t>> dataVec2_mean_self_red_chi2;
  std::vector<std::vector<Double_t>> dataVec2_mean_self_chi2;
  std::vector<std::vector<Double_t>> dataVec2_mean_nentries;
  std::vector<Double_t> dataVec_mean;
  std::vector<Double_t> dataVec_mean_red_chi2_err;
  std::vector<Double_t> dataVec_mean_err;
  std::vector<Double_t> dataVec_mean_err_global;
  std::vector<Double_t> dataVec_mean_err_global_rescale;
  std::vector<Double_t> dataVec_mean_err_local_rescale;
  std::vector<Double_t> dataVec_stddev;
  std::vector<Double_t> dataVec_mean_red_chi2;
  std::vector<Double_t> dataVec_mean_chi2;
  std::vector<Double_t> dataVec_mean_self;
  std::vector<Double_t> dataVec_mean_self_err;
  std::vector<Double_t> dataVec_stddev_self;
  std::vector<Double_t> dataVec_mean_self_red_chi2;
  std::vector<Double_t> dataVec_mean_self_chi2;
  std::vector<Double_t> dataVec_mean_nentries;
  // FIXME Technically nDet and nBPM are 0 unless you've executed "calculateSensitivities" at least once...
  for(int idet=0;idet<draws_piece1.size();idet++){
    dataVec_mean.clear();
    dataVec_mean_red_chi2_err.clear();
    dataVec_mean_err.clear();
    dataVec_mean_err_global.clear();
    dataVec_mean_err_global_rescale.clear();
    dataVec_mean_err_local_rescale.clear();
    dataVec_stddev.clear();
    dataVec_mean_red_chi2.clear();
    dataVec_mean_chi2.clear();
    dataVec_mean_self.clear();
    dataVec_mean_self_err.clear();
    dataVec_stddev_self.clear();
    dataVec_mean_self_red_chi2.clear();
    dataVec_mean_self_chi2.clear();
    dataVec_mean_nentries.clear();
    for(int imon=0;imon<draws_piece2.size();imon++){
      Double_t dataVec1_mean = 0.0;
      Double_t dataVec1_mean_red_chi2_err = 0.0;
      Double_t dataVec1_mean_err = 0.0;
      Double_t dataVec1_mean_err_global = 0.0;
      Double_t dataVec1_mean_err_global_rescale = 0.0;
      Double_t dataVec1_mean_err_local_rescale = 0.0;
      Double_t dataVec1_stddev = 0.0;
      Double_t dataVec1_mean_red_chi2 = 0.0;
      Double_t dataVec1_mean_chi2 = 0.0;
      Double_t dataVec1_mean_self = 0.0;
      Double_t dataVec1_mean_self_err = 0.0;
      Double_t dataVec1_stddev_self = 0.0;
      Double_t dataVec1_mean_self_red_chi2 = 0.0;
      Double_t dataVec1_mean_self_chi2 = 0.0;
      Double_t dataVec1_mean_nentries = 0.0;
      dataVec_mean.push_back(dataVec1_mean);
      dataVec_mean_red_chi2_err.push_back(dataVec1_mean_red_chi2_err);
      dataVec_mean_err.push_back(dataVec1_mean_err);
      dataVec_mean_err_global.push_back(dataVec1_mean_err_global);
      dataVec_mean_err_global_rescale.push_back(dataVec1_mean_err_global_rescale);
      dataVec_mean_err_local_rescale.push_back(dataVec1_mean_err_local_rescale);
      dataVec_stddev.push_back(dataVec1_stddev);
      dataVec_mean_chi2.push_back(dataVec1_mean_chi2);
      dataVec_mean_red_chi2.push_back(dataVec1_mean_red_chi2);
      dataVec_mean_self.push_back(dataVec1_mean_self);
      dataVec_mean_self_err.push_back(dataVec1_mean_self_err);
      dataVec_stddev_self.push_back(dataVec1_stddev_self);
      dataVec_mean_self_red_chi2.push_back(dataVec1_mean_self_red_chi2);
      dataVec_mean_self_chi2.push_back(dataVec1_mean_self_chi2);
      dataVec_mean_nentries.push_back(dataVec1_mean_nentries);
    }
    dataVec2_mean.push_back(dataVec_mean);
    dataVec2_mean_red_chi2_err.push_back(dataVec_mean_red_chi2_err);
    dataVec2_mean_err.push_back(dataVec_mean_err);
    dataVec2_mean_err_global.push_back(dataVec_mean_err_global);
    dataVec2_mean_err_global_rescale.push_back(dataVec_mean_err_global_rescale);
    dataVec2_mean_err_local_rescale.push_back(dataVec_mean_err_local_rescale);
    dataVec2_stddev.push_back(dataVec_stddev);
    dataVec2_mean_red_chi2.push_back(dataVec_mean_red_chi2);
    dataVec2_mean_chi2.push_back(dataVec_mean_chi2);
    dataVec2_mean_self.push_back(dataVec_mean_self);
    dataVec2_mean_self_err.push_back(dataVec_mean_self_err);
    dataVec2_stddev_self.push_back(dataVec_stddev_self);
    dataVec2_mean_self_red_chi2.push_back(dataVec_mean_self_red_chi2);
    dataVec2_mean_self_chi2.push_back(dataVec_mean_self_chi2);
    dataVec2_mean_nentries.push_back(dataVec_mean_nentries);
  }
  // Set branch address after the vectors are filled to avoid C++ dynamic memory allocation shenanigans
  for(int idet=0;idet<draws_piece1.size();idet++){
    for(int imon=0;imon<draws_piece2.size();imon++){
      outtree->Branch(Form("%s_%s_mean",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_red_chi2_err",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_red_chi2_err[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_err",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_err[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_err_global",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_err_global[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_err_global_rescale",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_err_global_rescale[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_err_local_rescale",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_err_local_rescale[idet][imon]);
      outtree->Branch(Form("%s_%s_stddev",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_stddev[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_red_chi2",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_red_chi2[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_chi2",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_chi2[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_self",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_self[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_self_err",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_self_err[idet][imon]);
      outtree->Branch(Form("%s_%s_stddev_self",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_stddev_self[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_self_red_chi2",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_self_red_chi2[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_self_chi2",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_self_chi2[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_nentries",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_nentries[idet][imon]);
    }
  }

  std::map<Int_t,Int_t> segments;
  Int_t segment;
  Int_t segment_run;
  Double_t double_segment;
  Printf("test2");
  if (((TString)averaging).Contains("segment")){
  Printf("test3");
    intree->SetBranchAddress(Form("%s",averaging.Data()),&segment);
    Printf("set branch address %s to an integer",averaging.Data());
    intree->Draw(Form(">>elist_%s",proto_draw.Data()), Form("%s",cuts.at(cuts.size()-1).Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s",proto_draw.Data()));
    intree->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<intree->GetEntries()*/ ; nent++) {
      intree->GetEntry(intree->GetEntryNumber(nent));
      if (segments.count(segment) == 0) {
        segments[segment] = 1;
      }
      else {
        segments[segment] = segments.at(segment) + 1;
      }
    }
    intree->SetEntryList(0);
    delete elist;
  } 
  else if (((TString)averaging).Contains("run")){
  Printf("test3 - run");
    intree->SetBranchAddress(Form("%s",averaging.Data()),&segment_run);
    Printf("set branch address %s to an integer",averaging.Data());
    intree->Draw(Form(">>elist_%s",proto_draw.Data()), Form("%s",cuts.at(cuts.size()-1).Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s",proto_draw.Data()));
    intree->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<intree->GetEntries()*/ ; nent++) {
      intree->GetEntry(intree->GetEntryNumber(nent));
      if (segments.count(segment_run) == 0) {
        segments[segment_run] = 1;
      }
      else {
        segments[segment_run] = segments.at(segment_run) + 1;
      }
    }
    intree->SetEntryList(0);
    delete elist;
  } 
  else {
  Printf("test4");
    intree->SetBranchAddress(Form("%s",averaging.Data()),&double_segment);
    intree->Draw(Form(">>elist_%s",proto_draw.Data()), Form("%s",cuts.at(cuts.size()-1).Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s",proto_draw.Data()));
    intree->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<intree->GetEntries()*/ ; nent++) {
      intree->GetEntry(intree->GetEntryNumber(nent));
      if (segments.count(double_segment) == 0) {
        segments[double_segment] = 1;
      }
      else {
        segments[double_segment] = segments.at(double_segment) + 1;
      }
    }
    intree->SetEntryList(0);
    delete elist;
  }

  // Add rcdb info if available
  TObjArray *var_list=intree->GetListOfBranches();
  TIter var_iter(var_list);
  std::vector<Double_t> rcdb_vals;
  std::vector<TString> rcdb_names;
  Int_t nRCDB = 0;
  
  TString rcdb_draws = "";
  while (auto *var= var_iter.Next()){
    TString name(var->GetName());
    if (name.Contains("rcdb_")) {
      if ((name == "rcdb_ihwp" && averaging == "rcdb_ihwp") || (name == "rcdb_slug" && averaging == "rcdb_slug") || (name == "rcdb_flip_state" && averaging == "rcdb_flip_state")) {
        Printf("Skipping %s",name.Data());
      }
      else {
        rcdb_names.push_back(name);
        Double_t tmpval = 0.0;
        rcdb_vals.push_back(tmpval);
        nRCDB++;
        rcdb_draws = rcdb_draws + name + "+";
      }
    }   
  }
  for (int m = 0 ; m < nRCDB ; m++) {
    // Add new branch to out_dit_tree
    intree->SetBranchAddress(rcdb_names[m],&rcdb_vals[m]);
    outtree->Branch(rcdb_names[m],&rcdb_vals[m]);
  }

  // Get the number of segments from the tree directly
  Int_t run = 0;
  Double_t run_number = 0;
  Double_t rcdb_ihwp = 0;
  Double_t crex_part_save = 0;
  Double_t crex_part_b_save = 0;
  Double_t crex_pitt = 0;
  Double_t crex_slow_control = 0;
  Double_t crex_slow_control_simple = 0;
  Int_t flag = 0;
  Double_t cyclenum = 0.0;
  Int_t totalEntries = intree->GetEntries();
  Int_t int_avg_time_scale = 0;
  Double_t double_avg_time_scale = 0;
  if (averaging == "rcdb_slug" || averaging == "rcdb_ihwp" || averaging == "rcdb_flip_state" || averaging == "crex_part" || averaging == "crex_part_b" || averaging == "crex_pitt" || averaging == "crex_slow_control" || averaging == "crex_slow_control_simple" || averaging == "run_number") {
    intree->SetBranchAddress(averaging,&double_avg_time_scale);
    rcdb_draws = rcdb_draws + averaging + "+";
  }
  if (averaging == "dit_segment" || averaging == "segment" || averaging == "run") {
    intree->SetBranchAddress(averaging,&int_avg_time_scale);
    rcdb_draws = rcdb_draws + averaging + "+";
  }
  if (averaging != "run" && intree->GetBranch("run")) {
    intree->SetBranchAddress("run",&run);
    outtree->Branch("run",&run);
    rcdb_draws = rcdb_draws + "run" + "+";
  }
  if (intree->GetBranch("cyclenum")) {
    intree->SetBranchAddress("cyclenum",&cyclenum);
    outtree->Branch("cyclenum",&cyclenum);
    rcdb_draws = rcdb_draws + "cyclenum" + "+";
  }
  if (intree->GetBranch("flag")) {
    intree->SetBranchAddress("flag",&flag);
    outtree->Branch("dit_flag",&flag);
    rcdb_draws = rcdb_draws + "flag" + "+";
  }
  if (intree->GetBranch("dit_flag")) {
    intree->SetBranchAddress("dit_flag",&flag);
    outtree->Branch("dit_flag",&flag);
    rcdb_draws = rcdb_draws + "dit_flag" + "+";
  }
  if (averaging != "crex_part" && intree->GetBranch("crex_part")) {
    intree->SetBranchAddress("crex_part",&crex_part_save);
    outtree->Branch("crex_part",&crex_part_save);
    rcdb_draws = rcdb_draws + "crex_part" + "+";
  }
  if (averaging != "crex_part_b" && intree->GetBranch("crex_part_b")) {
    intree->SetBranchAddress("crex_part_b",&crex_part_b_save);
    outtree->Branch("crex_part_b",&crex_part_b_save);
    rcdb_draws = rcdb_draws + "crex_part_b" + "+";
  }
  if (averaging != "crex_pitt" && intree->GetBranch("crex_pitt")) {
    intree->SetBranchAddress("crex_pitt",&crex_pitt);
    outtree->Branch("crex_pitt",&crex_pitt);
    rcdb_draws = rcdb_draws + "crex_pitt" + "+";
  }
  if (averaging != "crex_slow_control" && intree->GetBranch("crex_slow_control")) {
    intree->SetBranchAddress("crex_slow_control",&crex_slow_control);
    outtree->Branch("crex_slow_control",&crex_slow_control);
    rcdb_draws = rcdb_draws + "crex_slow_control" + "+";
  }
  if (averaging != "crex_slow_control_simple" && intree->GetBranch("crex_slow_control_simple")) {
    intree->SetBranchAddress("crex_slow_control_simple",&crex_slow_control_simple);
    outtree->Branch("crex_slow_control_simple",&crex_slow_control_simple);
    rcdb_draws = rcdb_draws + "crex_slow_control_simple" + "+";
  }
  if (averaging != "run_number" && intree->GetBranch("run_number")) {
    intree->SetBranchAddress("run_number",&run_number);
    outtree->Branch("run_number",&run_number);
    rcdb_draws = rcdb_draws + "run_number" + "+";
  }
  rcdb_draws = rcdb_draws + "1.0";


  // Use TGraphErrors
  TGraphErrors * tge;

  // Use a regular TGraph
  //TGraph * tge;

  // FIXME Should this calculation be done once per each minirun entry in intree->outtree? Or store a giant map and print per entry later (seems inefficient). I am storing the number of entries that mattch the localAvg_time_scale == X cut, but there are possible entries with NO localAvg_time_scale value, or something like that, which would be edge cases... also I want to preserve the order of points from intree->outtree for friendability later, so I need to loop over friend tree for sure with some kind of lookup reference.
  // FIXME Idea: check if the localAvg_time_scale branch value has changed since the last event, if it has then calculate a new set of means and Fill, but doing a Fill && Continue if not
  Int_t localAvg_time_scale = -1000; // Default initialize to rediculous number
  Int_t iterLocalAvg_time_scale = -1000; // Default initialize to rediculous number
  if (nentries_orig == 0){
    outtree->Branch(averaging,&localAvg_time_scale);
  }
  Int_t previousAvg_time_scale = -1000; // Default initialize to rediculous number
  Int_t nen=0;
  Int_t outN=0;
  Int_t nSegments=0;
  for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    nSegments++;
    localAvg_time_scale = iter->first;
    //// Original spot for burstwise printing... not sure if it made sense really
    //else {}
      previousAvg_time_scale  = iter->first;
      iterLocalAvg_time_scale = iter->first;
      // else grab all of the slopes and combos for this iterLocalAvg_time_scale value
      // Slopes
      for (Int_t idet = 0 ; idet < draws_piece1.size() ; idet++) {
        //Printf("test 1 slopes");
        for (Int_t imon = 0 ; imon < draws_piece2.size() ; imon++) {
          if (draws_piece3.size() <= imon) { // draws 3 assumes to be using the same monitor as draws 2 (for applying corrections)
            TString tmp2 = "";
            draws_piece3.push_back(tmp2);
          }
          if (proto_draw.Contains("#")) {
            draw_1 = proto_draw(0,proto_draw.First('#'));
            draw_2 = proto_draw(proto_draw.First('#')+1,proto_draw.Length()-1);
            if (draw_2.Contains("#")) {
              draw_3 = draw_2(draw_2.First('#')+1,draw_2.Length()-1);
              draw_2 = draw_2(0,draw_2.First('#'));
              if (draw_3.Contains("#")) {
                draw_4 = draw_3(draw_3.First('#')+1,draw_3.Length()-1);
                //draw_4_err = draw_4;
                //draw_4_err = draw_4_err.ReplaceAll(".mean",".err");
                //draw_4_err = draw_4_err.ReplaceAll("_mean","_mean_error");
                draw_3 = draw_3(0,draw_3.First('#'));
              }
              else {
                draw_4 = "";
                //draw_4_err = "";
                //draw_3 = "";
                draws_piece3.at(imon) = "";
              }
            }
            else {
              draw_4 = "";
              //draw_4_err = "";
              draw_3 = "";
              draws_piece2.at(imon) = "";
            }
            draw = draw_1+draws_piece1.at(idet)+draw_2+draws_piece2.at(imon)+draw_3+draws_piece3.at(imon)+draw_4;
            if (draws_piece1.at(idet).Contains("manual_main_det")) {
              draw = 
                 "((rcdb_arm_flag==0)*(" + draw_1+"us_avg"+draw_2+draws_piece2.at(imon)+draw_3+draws_piece3.at(imon)+draw_4 + ")"
                +"+(rcdb_arm_flag==1)*(" + draw_1+"usr"+draw_2+draws_piece2.at(imon)+draw_3+draws_piece3.at(imon)+draw_4 + ")"
                +"+(rcdb_arm_flag==2)*(" + draw_1+"usl"+draw_2+draws_piece2.at(imon)+draw_3+draws_piece3.at(imon)+draw_4 + "))";
            }
            if (draws_piece2.at(imon).Contains("manual_main_det")) {
              if (draws_piece3.at(imon).Contains("manual_main_det")) {
                draw = 
                   "((rcdb_arm_flag==0)*(" + draw_1+draws_piece1.at(idet)+draw_2+"us_avg"+draw_3+"us_avg"+draw_4 + ")"
                  +"+(rcdb_arm_flag==1)*(" + draw_1+draws_piece1.at(idet)+draw_2+"usr"+draw_3+"usr"+draw_4 + ")"
                  +"+(rcdb_arm_flag==2)*(" + draw_1+draws_piece1.at(idet)+draw_2+"usl"+draw_3+"usl"+draw_4 + "))";
              }
              else {
                draw = 
                   "((rcdb_arm_flag==0)*(" + draw_1+draws_piece1.at(idet)+draw_2+"us_avg"+draw_3+draw_4 + ")"
                  +"+(rcdb_arm_flag==1)*(" + draw_1+draws_piece1.at(idet)+draw_2+"usr"+draw_3+draw_4 + ")"
                  +"+(rcdb_arm_flag==2)*(" + draw_1+draws_piece1.at(idet)+draw_2+"usl"+draw_3+draw_4 + "))";
              }
            }
          }
          else {
            draw = proto_draw;
          }

          // Now do the error draw
          if (proto_drawn_channels_error.Contains("#")) {
            draw_1_err = proto_drawn_channels_error(0,proto_drawn_channels_error.First('#'));
            draw_2_err = proto_drawn_channels_error(proto_drawn_channels_error.First('#')+1,proto_drawn_channels_error.Length()-1);
            if (draw_2_err.Contains("#")) {
              draw_3_err = draw_2_err(draw_2_err.First('#')+1,draw_2_err.Length()-1);
              draw_2_err = draw_2_err(0,draw_2_err.First('#'));
              if (draw_3_err.Contains("#")) {
                draw_4_err = draw_3_err(draw_3_err.First('#')+1,draw_3_err.Length()-1);
                draw_3_err = draw_3_err(0,draw_3_err.First('#'));
              }
              else {
                draw_4_err = "";
                //draw_3_err = "";
                draws_piece3.at(imon) = "";
              }
            }
            else {
              draw_4_err = "";
              draw_3_err = "";
              draws_piece2.at(imon) = "";
            }
            drawn_channels_error = draw_1_err+draws_piece1.at(idet)+draw_2_err+draws_piece2.at(imon)+draw_3_err+draws_piece3.at(imon)+draw_4_err;
            if (draws_piece1.at(idet).Contains("manual_main_det")) {
              drawn_channels_error = 
                 "((rcdb_arm_flag==0)*(" + draw_1_err+"us_avg"+draw_2_err+draws_piece2.at(imon)+draw_3_err+draws_piece3.at(imon)+draw_4_err + ")"
                +"+(rcdb_arm_flag==1)*(" + draw_1_err+"usr"+draw_2_err+draws_piece2.at(imon)+draw_3_err+draws_piece3.at(imon)+draw_4_err + ")"
                +"+(rcdb_arm_flag==2)*(" + draw_1_err+"usl"+draw_2_err+draws_piece2.at(imon)+draw_3_err+draws_piece3.at(imon)+draw_4_err + "))";
            }
            if (draws_piece2.at(imon).Contains("manual_main_det")) {
              if (draws_piece3.at(imon).Contains("manual_main_det")) {
                drawn_channels_error = 
                  "((rcdb_arm_flag==0)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"us_avg"+draw_3_err+"us_avg"+draw_4_err + ")"
                  +"+(rcdb_arm_flag==1)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"usr"+draw_3_err+"usr"+draw_4_err + ")"
                  +"+(rcdb_arm_flag==2)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"usl"+draw_3_err+"usl"+draw_4_err + "))";
              }
              else {
                drawn_channels_error = 
                  "((rcdb_arm_flag==0)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"us_avg"+draw_3_err+draw_4_err + ")"
                  +"+(rcdb_arm_flag==1)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"usr"+draw_3_err+draw_4_err + ")"
                  +"+(rcdb_arm_flag==2)*(" + draw_1_err+draws_piece1.at(idet)+draw_2_err+"usl"+draw_3_err+draw_4_err + "))";
              }
            }
          }
          else {
            drawn_channels_error = proto_drawn_channels_error;
          }

          // Now do the weighting source error draw
          // FIXME // Draw error here assumes we are using the part-avged eigen vector based regression errors as input, and that the mini tree has been friended into it.
          if (proto_draw_weighting_error == proto_drawn_channels_error) {
            draw_weighting_error = drawn_channels_error;
          }
          else {
            if (proto_draw_weighting_error.Contains("#")) {
              draw_1_weight_err = proto_draw_weighting_error(0,proto_draw_weighting_error.First('#'));
              draw_2_weight_err = proto_draw_weighting_error(proto_draw_weighting_error.First('#')+1,proto_draw_weighting_error.Length()-1);
              if (draw_2_weight_err.Contains("#")) {
                draw_3_weight_err = draw_2_weight_err(draw_2_weight_err.First('#')+1,draw_2_weight_err.Length()-1);
                draw_2_weight_err = draw_2_weight_err(0,draw_2_weight_err.First('#'));
                if (draw_3_weight_err.Contains("#")) {
                  draw_4_weight_err = draw_3_weight_err(draw_3_weight_err.First('#')+1,draw_3_weight_err.Length()-1);
                  draw_3_weight_err = draw_3_weight_err(0,draw_3_weight_err.First('#'));
                }
                else {
                  draw_4_weight_err = "";
                  //draw_3_weight_err = "";
                  draws_piece3.at(imon) = "";
                }
              }
              else {
                draw_4_weight_err = "";
                draw_3_weight_err = "";
                draws_piece2.at(imon) = "";
              }
              draw_weighting_error = draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+draws_piece2.at(imon)+draw_3_weight_err+draws_piece3.at(imon)+draw_4_weight_err;
              if (draws_piece1.at(idet).Contains("manual_main_det")) {
                draw_weighting_error = 
                   "((rcdb_arm_flag==0)*(" + draw_1_weight_err+"us_avg"+draw_2_weight_err+draws_piece2.at(imon)+draw_3_weight_err+draws_piece3.at(imon)+draw_4_weight_err + ")"
                  +"+(rcdb_arm_flag==1)*(" + draw_1_weight_err+"usr"+draw_2_weight_err+draws_piece2.at(imon)+draw_3_weight_err+draws_piece3.at(imon)+draw_4_weight_err + ")"
                  +"+(rcdb_arm_flag==2)*(" + draw_1_weight_err+"usl"+draw_2_weight_err+draws_piece2.at(imon)+draw_3_weight_err+draws_piece3.at(imon)+draw_4_weight_err + "))";
              }
              if (draws_piece2.at(imon).Contains("manual_main_det")) {
                if (draws_piece3.at(imon).Contains("manual_main_det")) {
                  draw_weighting_error = 
                    "((rcdb_arm_flag==0)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"us_avg"+draw_3_weight_err+"us_avg"+draw_4_weight_err + ")"
                    +"+(rcdb_arm_flag==1)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"usr"+draw_3_weight_err+"usr"+draw_4_weight_err + ")"
                    +"+(rcdb_arm_flag==2)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"usl"+draw_3_weight_err+"usl"+draw_4_weight_err + "))";
                }
                else {
                  draw_weighting_error = 
                    "((rcdb_arm_flag==0)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"us_avg"+draw_3_weight_err+draw_4_weight_err + ")"
                    +"+(rcdb_arm_flag==1)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"usr"+draw_3_weight_err+draw_4_weight_err + ")"
                    +"+(rcdb_arm_flag==2)*(" + draw_1_weight_err+draws_piece1.at(idet)+draw_2_weight_err+"usl"+draw_3_weight_err+draw_4_weight_err + "))";
                }
              }
            }
            else {
              draw_weighting_error = proto_draw_weighting_error;
            }
          }
          //Printf("%s + %s + %s = %s",proto_draw.Data(),draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),draw.Data());
          
          // Default tge p0 case
          //TString draw_weighting_error = "((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";

          // Similar to default tge p0 case - increase error in fixed scale factor = 4x
          //TString draw_weighting_error = "4*((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";

          // Similar to default tge p0 case - increase error in fixed scale factor = 1/(sigma total experiment)
          //TString draw_weighting_error = "(1/87.0)*((rcdb_arm_flag==0)*(mini_eigen_reg_5bpms_part_avg.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usr.err)+(rcdb_arm_flag==2)*(mini_eigen_reg_5bpms_part_avg.reg_asym_usl.err))/(ppb)";

          // Set error to 0 - unweighted TGraphErrors approach
          //TString draw_weighting_error = "((rcdb_arm_flag==0)*(0)+(rcdb_arm_flag==1)*(0)+(rcdb_arm_flag==2)*(0))/(ppb)";

          // Set error to 1 - fixed weight TGraphErrors approach
          //TString draw_weighting_error = "((rcdb_arm_flag==0)*(1)+(rcdb_arm_flag==1)*(1)+(rcdb_arm_flag==2)*(1))/(ppb)";

          // Special test case using self-BPM only weighting as the relative rescaled error bar (instead of maindet asym error).
          //draw_weighting_error = "agg_part_avgd_friendable.diff_evMon0_mean_error";

          if (use_mon_cuts == 1) {
            icut = imon;
          }
          else {
            icut = idet;
          }
          TString localCut = "";
          if (cuts.at(icut) != "") { 
            localCut = Form(" && %s",cuts.at(icut).Data());
          }
          Double_t sum_global_sigs = 0.0;
          Double_t sum_det_global_sig2 = 0.0;
          Double_t weighted_mean_err_scale_global = 1.0;
          if (nSegments == 1) {
            // Do global renormalization calculation - on the first segment calculation pass through
            nen = intree->Draw(Form("%s:run+0.1*mini:%s:%s",draw.Data(),draw_weighting_error.Data(),drawn_channels_error.Data()),Form("%s",cuts.at(icut).Data()),"goff");
            if (nen != 0) {
              sum_global_sigs = 0.0;
              sum_det_global_sig2 = 0.0;
              for (Int_t k = 0 ; k < nen ; k++) {
                sum_global_sigs += 1.0/pow(intree->GetV3()[k],2); // Sum of sigmas here = SUM(1/sigma^2)
                sum_det_global_sig2 += pow(intree->GetV4()[k],2)/pow(intree->GetV3()[k],4);
              }
              weighted_mean_err_scale_global = sqrt(sum_det_global_sig2/sum_global_sigs);
            }
          }

          // Do local renormalization calculation
          nen = intree->Draw(Form("%s:run+0.1*mini:%s:%s",draw.Data(),draw_weighting_error.Data(),drawn_channels_error.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 %s",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale,localCut.Data()),"goff");
          //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d && %s",nen,data_tree_name.Data(),draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),averaging.Data(),iterLocalAvg_time_scale,cuts.at(idet).Data());

          if (nen != 0) {
            // Special case calculating sum(errs2)
            Double_t sum_sigs = 0.0;
            Double_t sum_self_sigs = 0.0;
            //Double_t ssigs = 0.0;
            for (Int_t k = 0 ; k < nen ; k++) {
              //ssigs += pow(intree->GetV3()[k],2);
              sum_sigs += 1.0/pow(intree->GetV3()[k],2); // Sum of sigmas here = SUM(1/sigma^2)
              sum_self_sigs += 1.0/pow(intree->GetV4()[k],2); // Sum of sigmas here = SUM(1/sigma^2)
            }
            Printf("");
            //Printf("Using sum of 1/sigmas2 = %e",sum_sigs);
            //Printf("Using sum of sigmas = %f",ssigs);

            // Special case calculating weighted mean and errors explicitly sum(Data/errs2)
            Double_t sum_signal_sig2 = 0.0;
            Double_t sum_signal_self_sig2 = 0.0;
            for (Int_t k = 0 ; k < nen ; k++) {
              //Printf("Error for run+0.1*mini = %f = %e",intree->GetV2()[k],intree->GetV3()[k]);
              sum_signal_sig2 += intree->GetV1()[k]/pow(intree->GetV3()[k],2);
              sum_signal_self_sig2 += intree->GetV1()[k]/pow(intree->GetV4()[k],2);
            }
            Double_t weighted_mean = sum_signal_sig2/sum_sigs;
            Double_t self_weighted_mean = sum_signal_self_sig2/sum_self_sigs;
            Double_t sum_deviation_signal_sig2 = 0.0;
            Double_t sum_det_sig2 = 0.0;
            Double_t sum_deviation_signal_self_sig2 = 0.0;
            for (Int_t k = 0 ; k < nen ; k++) {
              sum_deviation_signal_sig2 += pow(intree->GetV1()[k]-(weighted_mean),2)/pow(intree->GetV3()[k],2);
              sum_det_sig2 += pow(intree->GetV4()[k],2)/pow(intree->GetV3()[k],4);
              // Self weighted stddev
              sum_deviation_signal_self_sig2 += pow(intree->GetV1()[k]-(self_weighted_mean),2)/pow(intree->GetV4()[k],2);
            }
            Double_t weighted_mean_stddev = sqrt(sum_deviation_signal_sig2/sum_sigs);
            Double_t self_weighted_mean_stddev = sqrt(sum_deviation_signal_self_sig2/sum_self_sigs);
            Double_t mean_self_red_chi2_tmp1 = 0.0;
            Double_t weighted_mean_err_scale = sqrt(sum_det_sig2/sum_sigs);
            Double_t weighted_mean_err = weighted_mean_err_scale/sqrt(sum_sigs);
            // Do this in safe scope: Double_t weighted_mean_err_global = weighted_mean_err_scale_global/sqrt(sum_sigs); // FIXME FIXME FIXME FIXME replace sum_sigs with global_sum_sigs???? Need to use the global err scale comparable to the local one????
            Double_t self_weighted_mean_err = sqrt(1.0/sum_self_sigs);

            Printf("The weighted mean is = %e/%e = %e",sum_signal_sig2,sum_sigs,weighted_mean);
            Printf("The weighted sample variance (stdev) is = sqrt(Sum(weights*(deviation from mean)^2)/Sum(weights)) = %e",weighted_mean_stddev);
            Printf("The weighted error bar (sigma) is = sqrt(Sum(weights^2*(detector sigma)^2)/Sum(weights)) = %e",weighted_mean_err);
            Printf("The weighted mean error scaling = weighted mean of ratio of error bars (1/sqrt(weights)) is = sqrt(Sum(weights^2*(detector sigma)^2)/Sum(weights)) = %e",weighted_mean_err_scale);

            // Use 1/sum(errs^2) directly
            //nen = intree->Draw(Form("%s:run+0.1*mini:(%e)*%s",draw.Data(),sum_sigs,draw_weighting_error.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale,cuts.at(icut).Data()),"goff");

            // Use 1/sqrt(sum(errs^2)) instead
            //nen = intree->Draw(Form("%s:run+0.1*mini:sqrt(%e)*%s",draw.Data(),sum_sigs,draw_weighting_error.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale,cuts.at(icut).Data()),"goff");

            // Use sigma = sigma*main det slugwise rescale factor
            Printf("Using error bar from %s",draw_weighting_error.Data());
            nen = intree->Draw(Form("%s:run+0.1*mini:%e*%s",draw.Data(),weighted_mean_err_scale,draw_weighting_error.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale,cuts.at(icut).Data()),"goff");
        // Get mean
            tge = 0;

            // Use TGraphErrors
            tge = new TGraphErrors(nen,intree->GetV2(),intree->GetV1(),0,intree->GetV3());

            // Use a regular TGraph
            //tge = new TGraph(nen,intree->GetV2(),intree->GetV1());//,0,intree->GetV3());
            if (tge != 0) {
              //Printf("test 3 slopes means");
              tge->Fit("pol0");
              TF1* tfe = tge->GetFunction("pol0");
              Double_t avg_tmp1 = 0.0;
              Double_t mean_err_tmp1 = 0.0;
              Double_t mean_red_chi2_err_tmp1 = 0.0;
              Double_t mean_red_chi2_tmp1 = 0.0;
              Double_t chi2_tmp1 = 0.0;
              avg_tmp1 = (Double_t)(tfe->GetParameter(0));
              mean_err_tmp1 = (Double_t)(tfe->GetParError(0));
              chi2_tmp1 = (Double_t)(tfe->GetChisquare());
              if (nen > 1) {
                mean_red_chi2_err_tmp1 = (Double_t)(tfe->GetParError(0)*sqrt(chi2_tmp1/(nen-1)));
                mean_red_chi2_tmp1 = (Double_t)(sqrt(chi2_tmp1/(nen-1)));
                mean_self_red_chi2_tmp1 = sqrt(sum_deviation_signal_self_sig2/(nen-1));
              }
              else {
                mean_red_chi2_err_tmp1 = (Double_t)(tfe->GetParError(0)*sqrt(chi2_tmp1));
                mean_red_chi2_tmp1 = (Double_t)(sqrt(chi2_tmp1));
                mean_self_red_chi2_tmp1 = sqrt(sum_deviation_signal_self_sig2);
              }
              if (imon == 0 && idet == 0){
                //Printf("New mean %s %s = %f, entries = %d",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),avg_tmp1,nen);
              }
              dataVec2_mean[idet][imon]              = avg_tmp1;
              dataVec2_mean_red_chi2_err[idet][imon] = mean_red_chi2_err_tmp1;
              dataVec2_mean_err[idet][imon]          = mean_err_tmp1;
              if (nSegments == 1) {
                dataVec2_mean_err_global_rescale[idet][imon] = weighted_mean_err_scale_global;
              }
              dataVec2_mean_err_global[idet][imon]           = dataVec2_mean_err_global_rescale[idet][imon]/sqrt(sum_sigs);
              dataVec2_mean_err_local_rescale[idet][imon]    = weighted_mean_err_scale;
              dataVec2_stddev[idet][imon]            = weighted_mean_stddev;
              dataVec2_mean_red_chi2[idet][imon]     = mean_red_chi2_tmp1;
              dataVec2_mean_chi2[idet][imon]         = chi2_tmp1;
              dataVec2_mean_self[idet][imon]         = self_weighted_mean;
              dataVec2_mean_self_err[idet][imon]     = self_weighted_mean_err;
              dataVec2_stddev_self[idet][imon]       = self_weighted_mean_stddev;
              dataVec2_mean_self_red_chi2[idet][imon]= mean_self_red_chi2_tmp1;
              dataVec2_mean_self_chi2[idet][imon]    = sum_deviation_signal_self_sig2;
              dataVec2_mean_nentries[idet][imon]     = nen;
            }
            /*
            TH1* ah1;
            ah1 = (TH1*)(gROOT->FindObject("htemp"));
            if (nen != 0 && ah1 != 0) {
              //Printf("test 3 slopes means");
              Double_t avg_tmp1 = 0.0;
              Double_t mean_red_chi2_err_tmp1 = 0.0;
              Double_t rms_tmp1 = 0.0;
              avg_tmp1 = (Double_t)(ah1->GetMean(1));
              mean_red_chi2_err_tmp1 = (Double_t)(ah1->GetMeanError(1));
              rms_tmp1 = (Double_t)(ah1->GetRMS(1));
              if (imon == 0 && idet == 0){
                //Printf("New mean %s %s = %f, entries = %d",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),avg_tmp1,nen);
              }
              dataVec2_mean[idet][imon]     = avg_tmp1;
              dataVec2_mean_red_chi2_err[idet][imon] = mean_red_chi2_err_tmp1;
            }
            */
          }
          else {
            //Printf("Using prior segment's value for now...");
            dataVec2_mean[idet][imon]              = 0.0; /// Use 0.0? Or -1e6?
            dataVec2_mean_red_chi2_err[idet][imon] = 0.0;
            dataVec2_mean_err[idet][imon]          = 0.0;
            dataVec2_stddev[idet][imon]            = 0.0;
            dataVec2_mean_red_chi2[idet][imon]     = 0.0;
            dataVec2_mean_chi2[idet][imon]         = 0.0;
            dataVec2_mean_self[idet][imon]         = 0.0;
            dataVec2_mean_self_err[idet][imon]     = 0.0;
            dataVec2_stddev_self[idet][imon]       = 0.0;
            dataVec2_mean_self_red_chi2[idet][imon]= 0.0;
            dataVec2_mean_self_chi2[idet][imon]    = 0.0;
            dataVec2_mean_nentries[idet][imon]     = 0.0;
          }
          intree->Draw(Form("run+0.1*mini+%s",rcdb_draws.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale,cuts.at(cuts.size()-1).Data()),"goff"); // Grab the entry list with the "global" cut (not sing rcdb_arm_flag cuts) to fill the RCDB branches properly
        }
      }
      //Printf("test fill");
      //outtree_orig->GetEntry(outN);
      if (print_per_entry == 0) {
        outtree->Fill();
      }
      outN++;
    //{}
    //// New spot
    if (print_per_entry == 1) {
      for (Int_t ient = 0 ; ient<totalEntries ; ient++) {
        intree->GetEntry(ient);
        if (averaging == "rcdb_slug" || averaging == "rcdb_flip_state" || averaging == "crex_part" || averaging == "crex_part_b" || averaging == "crex_pitt" || averaging == "crex_slow_control" || averaging == "crex_slow_control_simple") {
          localAvg_time_scale = (Int_t)double_avg_time_scale;
        }
        if (averaging == "segment" || averaging == "dit_segment" || averaging == "run") {
          localAvg_time_scale = int_avg_time_scale;
          if (ient % 100 == 0) { Printf("Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient); }
        }
        //Printf("A) Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient);
        if (localAvg_time_scale == previousAvg_time_scale) {
          //Printf("Same localAvg_time_scale, filling another instance of the previously calculated means, etc. for slopes and BPM eigen vector monitor combos");
          Printf("Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient);
          // FIXME FIXME FIXME FIXME FIXME Uncommenting this line will print 1 output for ever entry in input tree. Commenting it out means there is only one output per averaged-time-period (as desired, but requires adding in that variable manually) 
          outtree->Fill();
          continue;
        }
      }
    }
  }
  //outtree_orig = outtree;
}

