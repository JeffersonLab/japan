#include "TVirtualPad.h"
#include <fstream>

class ToolBox{
  public:
    ~ToolBox();
    TVirtualPad* master_multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, TString, Int_t);
    TVirtualPad* applied_master_multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, std::vector<std::string>, TString, Int_t);
    TVirtualPad* corrected_multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, TString, TString, Int_t);
    TVirtualPad* OLD_mini_tree_multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, TString, Int_t);
    TVirtualPad* combo_multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, TString, std::vector<TString>, Int_t);
    TVirtualPad* vs_multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, TString, std::vector<TString>, Int_t);
    TVirtualPad* vs_ranked_multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, TString, std::vector<TString>, Int_t);
    TVirtualPad* vs_ranked_weighted_slope_multiGraph(TPad*, TVirtualPad*, TChain*, int, TString, TString, TString, std::vector<TString>, TString, Int_t);
    TVirtualPad* highlightedGraph(TCanvas*, TVirtualPad*, TChain*, int, std::string, TString, TString, TString, int, TString, int, TString);
    TVirtualPad* combo_segment_multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, TString, TString, std::vector<TString>, Int_t);
    TVirtualPad* combo_segment_compare_multiGraph(TPad*, TVirtualPad*, TChain*, TChain* dit, int, std::string, std::string, TString, TString, TString, TString, Int_t);

    // Weighted averaging plots
    TVirtualPad* manyGraph(TPad*, TVirtualPad*, TString, TChain*, TString, TString, TString, std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, Int_t);
    // Simplified TGraphErrors drawing script
    TVirtualPad* multiGraph(TPad*, TVirtualPad*, TChain*, int, std::string, TString, Int_t);

    // Straight averaging for further analyses
    void combo_segment_getter(TString,TTree*, TTree*, TString, TString, std::vector<TString>, std::vector<TString>, std::vector<TString>);
    void combo_err_segment_getter(TString,TTree*, TTree*, TString, std::vector<TString>, std::vector<TString>, std::vector<TString>, Int_t);

    void eigenvector_averaging(TString);
    void dit_slopes_averaging(TString);

    // Weighted averaging into rootfiles
    void tg_err_averaging(TString,Int_t,TString,TString,TString,TString,TString,TString);
    void combo_tg_err_segment_getter(TString, TTree*, TTree*, TString, TString, TString, std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, Int_t);

    // Data management
    void aggregator_friend_to_mini(TTree*, TTree*, TTree*, TTree*);
    void manage_aggregator_friend_to_mini();
    void dit_copy_and_friend_to_mini(TTree*, TTree*, TTree*, TTree*);
    void manage_dit_copy_and_friend_to_mini();
    void dit_segmentwise_copy_and_friend_to_mini(TTree*, TTree*, TTree*);
    void expand_dit_averaged_slopes();
    void postpan_copy_and_friend_to_mini(TTree*, TTree*, TTree*);
    void manage_postpan_copy_and_friend_to_mini();
};

ToolBox::~ToolBox(){
};

TVirtualPad* ToolBox::master_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.8,0.50,0.95);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  if (cut.Contains("rcdb_sign")) { 
    nen = mini->Draw(Form("rcdb_sign*((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.mean)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.mean)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.mean))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.Data(),"goff");
  }
  else {
    nen = mini->Draw(Form("((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.mean)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.mean)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.mean))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.Data(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.01);
    mg->Add(tmpG,"lp");
    legend->AddEntry(tmpG,((TString)draw)(((TString)draw).Last('_')+1,((TString)draw).Length())+" all eigen reg","lp");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s grand reg avg",draw.c_str()),Form("%s grand reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* ToolBox::applied_master_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", std::vector<std::string> monitors = {""}, TString cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.8,0.50,0.95);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  // FIXME ADD SLOPES UNITS
  std::string us_avg_slopes = draw + ".us_avg_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  std::string usl_slopes = draw + ".usl_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  std::string usr_slopes = draw + ".usr_" + monitors.at(0) + "*" + draw + ".diff_" + monitors.at(0) + ".mean";
  for (Int_t i = 1 ; i < monitors.size() ; i++) {
    us_avg_slopes = us_avg_slopes + "+" + draw + ".us_avg_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
    usl_slopes = usl_slopes + "+" + draw + ".usl_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
    usr_slopes = usr_slopes + "+" + draw + ".usr_" + monitors.at(i) + "*" + draw + ".diff_" + monitors.at(i) + ".mean";
  }

  Int_t nen=0;
  if (cut.Contains("rcdb_sign")) { 
    //nen = mini->Draw(Form("rcdb_sign*((rcdb_arm_flag==0)*(mini.asym_us_avg.mean+%s)+(rcdb_arm_flag==1)*(mini.asym_usr.mean+%s)+(rcdb_arm_flag==2)*(mini.asym_usl.mean+%s))/(ppb):(rcdb_arm_flag==0)+(rcdb_arm_flag==1)+(rcdb_arm_flag==2):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str()),cut.Data(),"goff");
    nen = mini->Draw(Form("rcdb_sign*((rcdb_arm_flag==0)*(asym_us_avg.mean-(%s))+(rcdb_arm_flag==1)*(asym_usr.mean-(%s))+(rcdb_arm_flag==2)*(asym_usl.mean-(%s)))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.Data(),"goff");

  }
  else {
    //nen = mini->Draw(Form("((rcdb_arm_flag==0)*(mini.asym_us_avg.mean+%s)+(rcdb_arm_flag==1)*(mini.asym_usr.mean+%s)+(rcdb_arm_flag==2)*(mini.asym_usl.mean+%s))/(ppb):(rcdb_arm_flag==0)+(rcdb_arm_flag==1)+(rcdb_arm_flag==2):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str()),cut.Data(),"goff");
    nen = mini->Draw(Form("((rcdb_arm_flag==0)*(mini.asym_us_avg.mean-(%s))+(rcdb_arm_flag==1)*(mini.asym_usr.mean-(%s))+(rcdb_arm_flag==2)*(mini.asym_usl.mean-(%s)))/(ppb):((rcdb_arm_flag==0)*(%s.reg_asym_us_avg.err)+(rcdb_arm_flag==1)*(%s.reg_asym_usr.err)+(rcdb_arm_flag==2)*(%s.reg_asym_usl.err))/(ppb):run+0.1*mini",us_avg_slopes.c_str(),usr_slopes.c_str(),usl_slopes.c_str(),draw.c_str(),draw.c_str(),draw.c_str()),cut.Data(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.01);
    mg->Add(tmpG,"lp");
    legend->AddEntry(tmpG,((TString)draw)(((TString)draw).Last('_')+1,((TString)draw).Length())+" all eigen reg","lp");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s grand reg avg",draw.c_str()),Form("%s grand reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* ToolBox::corrected_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString slope = "test", TString cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.75,0.50,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  if (cut.Contains("rcdb_sign")) { 
    nen = mini->Draw(Form("rcdb_sign*((%s.mean/nm)*(%s/(ppm/um))):((%s.err/nm)*(%s/(ppm/um))):run+0.1*mini",draw.c_str(),slope.Data(),draw.c_str(),slope.Data()),cut.Data(),"goff");
  }
  else {
    nen = mini->Draw(Form("((%s.mean/nm)*(%s/(ppm/um))):((%s.err/nm)*(%s/(ppm/um))):run+0.1*mini",draw.c_str(),slope.Data(),draw.c_str(),slope.Data()),cut.Data(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.01);
    mg->Add(tmpG,"lp");
    legend->AddEntry(tmpG,((TString)draw)(((TString)draw).First(".")+1,((TString)draw).Length())+" * "+(slope)(slope.First(".")+1,slope.Length()),"lp");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s reg avg",draw.c_str()),Form("%s reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

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
            tge->SetNameTitle(Form("tge %s",draw.Data()),Form("%s ;%s ;%s",draw.Data(),averaging.Data(),draw.Data()));
            (tge->GetHistogram())->SetNameTitle(Form("tge h %s",draw.Data()),Form("%s ;%s ;%s",draw.Data(),averaging.Data(),draw.Data()));
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

TVirtualPad* ToolBox::multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string c_draw = "cyclenum", TString cut = "", Int_t startcolor=30){
  TString draw = (TString)c_draw;
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  //if (cut.Contains("rcdb_sign")) { 
    //nen = mini->Draw(Form("rcdb_sign*(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.Data(),draw.Data()),cut.Data(),"goff");
  //}
  //else {
    //nen = mini->Draw(Form("(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.Data(),draw.Data()),cut.Data(),"goff");
  //}
  nen = mini->Draw(draw,cut.Data(),"goff"); 
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.Data(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.Data(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV1(),mini->GetV2(),0,mini->GetV3());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.Data(),subpad),Form("%s_%d",draw.Data(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.2);
    mg->Add(tmpG,"p");
    legend->AddEntry(tmpG,(TString)draw,"p");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s reg avg",draw.Data()),Form("%s reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.Data(),draw.Data()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.Data()));
  //}
  return p1;
}

TVirtualPad* ToolBox::OLD_mini_tree_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  if (cut.Contains("rcdb_sign")) { 
    nen = mini->Draw(Form("rcdb_sign*(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.c_str(),draw.c_str()),cut.Data(),"goff");
  }
  else {
    nen = mini->Draw(Form("(%s.mean)/(ppb):(%s.err)/(ppb):run+0.1*mini",draw.c_str(),draw.c_str()),cut.Data(),"goff");
  }
  //printf("Plotting %d good entries where 13746 is ideal\n",nen);
  if (nen > 0) {
    //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
    //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
    tmpG = new TGraphErrors(nen,mini->GetV3(),mini->GetV1(),0,mini->GetV2());
    mgs.push_back(tmpG);
    tmpG->SetNameTitle(Form("%s_%d",draw.c_str(),subpad),Form("%s_%d",draw.c_str(),subpad));
    tmpG->SetLineColor(startcolor);
    tmpG->SetMarkerColor(startcolor);
    tmpG->SetMarkerStyle(20);
    tmpG->SetMarkerSize(0.2);
    mg->Add(tmpG,"p");
    legend->AddEntry(tmpG,(TString)draw,"p");
  }

  mg->Fit("pol0");
  gStyle->SetOptFit(1);
  mg->SetNameTitle(Form("%s reg avg",draw.c_str()),Form("%s reg avg;Run+0.1*Minirun;%s slope (ppm/um)",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  legend->Draw();
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* ToolBox::combo_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", std::vector<TString> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;
  TString x_axis_name = "run+0.1*mini";
  if ((TString)mini->GetName() == "dit") {
    x_axis_name = "run";
  }

  Int_t nen=0;
  for (Int_t i = vs.size()-1 ; i >= 0 ; i--) {
    if (cut == "") {
      nen = mini->Draw(Form("(%s_%s):%s",draw.c_str(),vs.at(i).Data(),x_axis_name.Data()),Form("run!=7760"),"goff");
    }
    else {
      // No more abs cut here...
      nen = mini->Draw(Form("(%s_%s):%s",draw.c_str(),vs.at(i).Data(),x_axis_name.Data()),Form("run!=7760 && %s",cut.Data()),"goff");
      //nen = mini->Draw(Form("abs(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).Data()),Form("run!=7760 && %s",cut.Data()),"goff");
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d_combo",vs.at(i).Data(),((TString)draw).Data(),subpad),Form("%s_%s_%d",vs.at(i).Data(),((TString)draw).Data(),subpad));
      //if (cut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.4);
      //if (cut == "") {}
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,(TString)vs.at(i),"lp");
    }
  }

  if (((TString)draw).Contains("evMon")) {
    mg->SetNameTitle(Form("%s BPM combination composition_combo %d",draw.c_str(),subpad),Form("%s BPM combination composition;Run+0.1*Minirun;%s composition",draw.c_str(),draw.c_str()));
  }
  else {
    mg->SetNameTitle(Form("%s slopes vs evMons_combo %d",draw.c_str(),subpad),Form("%s slopes vs, evMons;Run+0.1*Minirun;%s slopes",draw.c_str(),draw.c_str()));
  }
  mg->Draw("alp");

  if (subpad%3==0 || cut=="rcdb_arm_flag==0") {
    legend->Draw();
  }
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* ToolBox::vs_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", std::vector<TString> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  Int_t nen=0;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    if (cut == "") {
      // FIXME has a hardcoded set of units here...
      nen = mini->Draw(Form("(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).Data()),Form("run!=7760"),"goff");
    }
    else {
      // No more abs cut here...
      // // FIXME has a hardcoded set of units here...
      nen = mini->Draw(Form("(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).Data()),Form("run!=7760 && %s",cut.Data()),"goff");
      //nen = mini->Draw(Form("abs(%s_%s)/(ppm/um):run+0.1*mini",draw.c_str(),vs.at(i).Data()),Form("run!=7760 && %s",cut.Data()),"goff");
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d",vs.at(i).Data(),draw.c_str(),subpad),Form("%s_%s_%d",vs.at(i).Data(),draw.c_str(),subpad));
      //if (cut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.2);
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,(TString)vs.at(i),"p");
    }
  }

  mg->SetNameTitle(Form("%s BPM combination composition",draw.c_str()),Form("%s BPM combination composition;Run+0.1*Minirun;%s composition",draw.c_str(),draw.c_str()));
  mg->Draw("alp");

  // FIXME has an outdated check here
  if (subpad%3==0 || cut=="rcdb_arm_flag==0") {
    legend->Draw();
  }
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* ToolBox::vs_ranked_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", std::vector<TString> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  TString localCut = cut;

  Int_t nen=0;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    localCut = cut;
    if (localCut == "") {
      localCut = Form("(run>5000 && run != 7760)");
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s%s_%s) > abs(%s%s_%s))",localCut.Data(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(k).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str());
        }
      }
      Printf("%s",localCut.Data());
      nen = mini->Draw(Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.Data()),"goff");
      Printf("%s",Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()));
    }
    else {
      localCut = Form("(%s)",localCut.Data());
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s%s_%s) > abs(%s%s_%s))",localCut.Data(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str(),draw.substr(0,draw.find(".")+1).c_str(),vs.at(k).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str());
        }
      }
      Printf("%s",localCut.Data());
      nen = mini->Draw(Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.Data()),"goff");
      // No more absolute values in here
      //nen = mini->Draw(Form("abs(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()),Form("%s",localCut.Data()),"goff");
      Printf("%s",Form("(%s%s_%s):run+0.1*mini",draw.substr(0,draw.find(".")+1).c_str(),vs.at(i).Data(),draw.substr(draw.find(".")+1,draw.length()).c_str()));
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d",vs.at(i).Data(),draw.c_str(),subpad),Form("%s_%s_%d",vs.at(i).Data(),draw.c_str(),subpad));
      //if (localCut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.2);
      //tmpG->SetMarkerSize(0.2);
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,(TString)vs.at(i),"p");
    }
  }

  mg->SetNameTitle(Form("%s BPM combo's ranked largest contribution",draw.c_str()),Form("%s BPM combo's ranked largest contribution;Run+0.1*Minirun;%s",draw.c_str(),draw.c_str()));
  mg->Draw("ap");

  //if (subpad%3==0 || localCut=="") {
  legend->Draw();
  //}
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* ToolBox::vs_ranked_weighted_slope_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, TString tree = "mini", TString detector = "us_avg", TString device = "bpm12X", std::vector<TString> vs = {""}, TString cut = "", Int_t startcolor=30){
  // Ana == 
  // slope - calculate the amount of BPM Q in detector P, find the evMon that has the most Q in P and plot it
  // err   - calculate the uncertainty on correction to detector from evMonitor
  p1=c1->cd(subpad);
  p1->SetGrid();

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  TString localCut = cut;

  Int_t nen=0;
  for (Int_t i = 0 ; i < vs.size() ; i++) {
    localCut = cut;
    if (localCut == "") {
      localCut = Form("(run>5000 && run != 7760)");
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s.%s_%s*%s.%s_%s) > abs(%s.%s_%s*%s.%s_%s))",localCut.Data(),tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data(),tree.Data(),detector.Data(),vs.at(k).Data(),tree.Data(),vs.at(k).Data(),device.Data());
        }
      }
      Printf("%s",localCut.Data());
      nen = mini->Draw(Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()),Form("%s",localCut.Data()),"goff");
      Printf("%s",Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()));
    }
    else {
      localCut = Form("(%s)",localCut.Data());
      for (Int_t k = 0 ; k < vs.size() ; k++) {
        if (k != i) {
          localCut = Form("%s && (abs(%s.%s_%s*%s.%s_%s) > abs(%s.%s_%s*%s.%s_%s))",localCut.Data(),tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data(),tree.Data(),detector.Data(),vs.at(k).Data(),tree.Data(),vs.at(k).Data(),device.Data());
        }
      }
      Printf("%s",localCut.Data());
      // FIXME has a hardcoded set of units here...
      nen = mini->Draw(Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()),Form("%s",localCut.Data()),"goff");
      // No more absolute values 
      // nen = mini->Draw(Form("abs(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()),Form("%s",localCut.Data()),"goff");
      // FIXME has a hardcoded set of units here...
      Printf("%s",Form("(%s.%s_%s*%s.%s_%s)/(ppm/um):run+0.1*mini",tree.Data(),detector.Data(),vs.at(i).Data(),tree.Data(),vs.at(i).Data(),device.Data()));
    }
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
      tmpG = new TGraph(nen,mini->GetV2(),mini->GetV1());
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s.%s_%s_%d",tree.Data(),vs.at(i).Data(),device.Data(),subpad),Form("%s.%s_%s_%d",tree.Data(),vs.at(i).Data(),device.Data(),subpad));
      //if (localCut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.2);
      //tmpG->SetMarkerSize(0.2);
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,Form("%s_%s",detector.Data(),vs.at(i).Data()),"p");
    }
  }

  mg->SetNameTitle(Form("%s's ranked EigVecMon with largest amount of %s BPM slope;Run+0.1*Minirun;%s",detector.Data(),device.Data(),device.Data()),Form("%s's ranked EigVecMon with largest amount of %s BPM slope;Run+0.1*Minirun;%s_monitor_%s",detector.Data(),device.Data(),detector.Data(),device.Data()));
  mg->Draw("ap");

  //if (subpad%3==0 || localCut=="") {
  legend->Draw();
  //}
  //p1->BuildLegend(); // Automated legend builder and placer

  //if (p1->FindObject("htemp")) {
  //  ((TH1*)p1->FindObject("htemp"))->SetTitle(Form("%s",draw.c_str()));
  //}
  return p1;
}

TVirtualPad* ToolBox::highlightedGraph(TCanvas* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "run:cyclenum", TString cut = "", TString cut2 = "0",TString drawopts = "*", int color = 4, TString cut3 = "0", int color3=2, TString cut4 = ""){
  if (cut4 != "") {
    cut = cut+"&&"+cut4;
    cut2 = cut2+"&&"+cut4;
    if (cut3!= "") {
      cut3 = cut3+"&&"+cut4;
    }
  }

  p1=c1->cd(subpad);
  p1->SetGrid();
  mini->Draw(draw.c_str(),cut.Data(),drawopts.Data());

  if (p1->FindObject("Graph")) {
    ((TGraph*)p1->FindObject("Graph"))->SetName("full");
    mini->Draw(draw.c_str(),Form("%s",cut2.Data()),Form("%ssame",drawopts.Data()));
    if (p1->FindObject("Graph")) {
      ((TGraph*)p1->FindObject("Graph"))->SetName("color");
      ((TGraph*)p1->FindObject("color"))->SetMarkerColor(color);
    }
    if (cut3 != "") {
      mini->Draw(draw.c_str(),Form("%s",cut3.Data()),Form("%ssame",drawopts.Data()));
      if (p1->FindObject("Graph")) {
        ((TGraph*)p1->FindObject("Graph"))->SetName("color3");
        ((TGraph*)p1->FindObject("color3"))->SetMarkerColor(color3);
      }
      mini->Draw(draw.c_str(),Form("(%s) && (%s)",cut2.Data(),cut3.Data()),Form("%ssame",drawopts.Data()));
      if (p1->FindObject("Graph")) {
        ((TGraph*)p1->FindObject("Graph"))->SetName("color2and3");
        ((TGraph*)p1->FindObject("color2and3"))->SetMarkerColor(color+color3);
      }
    }
  }
  if (p1->FindObject("htemp")) {
    ((TH1*)p1->FindObject("htemp"))->SetTitle(draw.c_str());
  }
  return p1;
}

TVirtualPad* ToolBox::combo_segment_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, int subpad = 1, std::string draw = "cyclenum", TString cut = "", TString averaging = "dit_segment", std::vector<TString> vs = {""}, Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();
  Printf("test1");

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;

  TString x_axis_name = "run+0.1*mini";
  if ((TString)mini->GetName() == "dit") {
    x_axis_name = "run";
  }
  std::map<Int_t,Int_t> segments;
  Int_t segment;
  Int_t segment_run;
  Double_t double_segment;
  Printf("test2");
  if (((TString)averaging).Contains("segment")){
  Printf("test3");
    mini->SetBranchAddress(Form("%s",averaging.Data()),&segment);
    Printf("set branch address %s to an integer",averaging.Data());
    mini->Draw(Form(">>elist_%s_%d",draw.c_str(),subpad), Form("%s",cut.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(segment) == 0) {
        segments[segment] = 1;
      }
      else {
        segments[segment] = segments.at(segment) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  } 
  else if (((TString)averaging).Contains("run")){
  Printf("test3 - run");
    mini->SetBranchAddress(Form("%s",averaging.Data()),&segment_run);
    Printf("set branch address %s to an integer",averaging.Data());
    mini->Draw(Form(">>elist_%s_%d",draw.c_str(),subpad), Form("%s",cut.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(segment_run) == 0) {
        segments[segment_run] = 1;
      }
      else {
        segments[segment_run] = segments.at(segment_run) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  } 
  else {
  Printf("test4");
    mini->SetBranchAddress(Form("%s",averaging.Data()),&double_segment);
    mini->Draw(Form(">>elist_%s_%d",draw.c_str(),subpad), Form("%s",cut.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(double_segment) == 0) {
        segments[double_segment] = 1;
      }
      else {
        segments[double_segment] = segments.at(double_segment) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  }

  std::vector<std::map<Int_t,Double_t>> avg_combos;
  std::vector<std::vector<Double_t>> avg_combos_miniruns;
  std::vector<Double_t>  xaxis;
  Int_t nen=0;
  std::vector<Int_t> totalEntries;

  for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    std::ofstream outfile0;
    outfile0.open(Form("maps/out_%s_%d.map",averaging.Data(),iter->first),std::ofstream::app);
    outfile0 << std::endl;
    if ( vs.at(0).Contains("evMon") ){
      outfile0 << "[asym:@eigen_dit_asym_" << draw.c_str() << "]" << std::endl;
    }
    else {
      outfile0 << "[asym:@dit_asym_" << draw.c_str() << "]" << std::endl;
    }
    outfile0 << "asym:" << draw.c_str() << ",1.0" << std::endl;
    outfile0.close();
  }
  for (Int_t i = vs.size()-1 ; i >= 0 ; i--) {
    Int_t local_totalEntries = 0;
    std::vector<Double_t> tmp_avg;
    std::map<Int_t,Double_t> tmp_avg_combo;
    for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    //for (Int_t iSeg = 0; iSeg<segments.size() ; iSeg++) 
      //Printf("Adding %d events for segment %d",iter->second,iter->first);
  //Printf("test5");
      // generate a mapout textfile
      std::ofstream outfile0;
      outfile0.open(Form("maps/out_%s_%d.map",averaging.Data(),iter->first),std::ofstream::app);
      if (cut == "") {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw.c_str(),vs.at(i).Data(),i,iter->first),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw.c_str(),vs.at(i).Data()),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",averaging.Data(),iter->first,averaging.Data(),iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw.c_str(),vs.at(i).Data(),iter->first);
      }
      else {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw.c_str(),vs.at(i).Data(),i,iter->first),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut.Data(),iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw.c_str(),vs.at(i).Data()),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut.Data(),averaging.Data(),iter->first,averaging.Data(),iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut %s && dit_segment==%d",nen,draw.c_str(),vs.at(i).Data(),cut.Data(),iter->first);
      }
      if (nen != iter->second && nen!=0) {
        //Printf("Error, number of entries passing cut here = %d != above %d",nen,iter->second);
      }
      if (nen != 0) {
        //TGraph* g1 = (TGraph*)p1->FindObject("Graph");
        //g1->SetName(Form("g1_%d_%d",i,iter->first));
        //if (g1 != 0) {
          //Printf("g1 Works!");
        //}
        //TH2* ah1 = (TH2*)gROOT->Get("htemp");
  //Printf("test5.1");
        TH1* ah1;
        ah1 = (TH1*)(p1->FindObject("htemp"));
        //ah1 = (TH1*)(p1->FindObject(Form("h1_%d_%d",i,iter->first)));
        //if (ah1 != 0) {
          //ah1->SetName(Form("h1_%d_%d",i,iter->first));
        //}
        if (nen != 0 && ah1 != 0) {
  //Printf("test5.2");
          Double_t avg_tmp1 = 0.0;
          avg_tmp1 = (Double_t)(ah1->GetMean(1));
          //Printf("Mean = %f, entries = %d",avg_tmp1,nen);
          tmp_avg_combo[iter->first] = avg_tmp1;
          if (cut == "") {
            nen = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name.Data(),i,iter->first),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",averaging.Data(),iter->first,averaging.Data(),iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw.c_str(),vs.at(i).Data(),iter->first);
          }
          else {
            nen = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name.Data(),i,iter->first),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut.Data(),averaging.Data(),iter->first,averaging.Data(),iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut %s && dit_segment==%d",nen,draw.c_str(),vs.at(i).Data(),cut.Data(),iter->first);
          }
          std::vector<Double_t> tmp_xaxis(mini->GetV1(), mini->GetV1() + mini->GetSelectedRows() % mini->GetEstimate());
  //Printf("test5.3");
          // Print a mapfile for eigenvector based slopes (assume eigenvectors exist in mulc tree)
          outfile0 << "diff:";
          if (!vs.at(i).Contains("evMon")) {
            outfile0 << "bpm" << vs.at(i);
          }
          else {
            outfile0 << "diff_" << vs.at(i);
          }
          outfile0 << "," << 1e-3*-1.0*avg_tmp1 << std::endl;
          for (Int_t j = 0 ; j < nen ; j++){
            tmp_avg.push_back(avg_tmp1);
            if (i == 0) {
  //Printf("test5.4");
              Double_t local_xaxis = 0.0;
              local_xaxis = tmp_xaxis.at(j);
              //Printf("Adding run+0.1*minirun = %0.1f",tmp_xaxis.at(j));
              xaxis.push_back(local_xaxis);
            }
          }
          local_totalEntries += nen;
          //ah1->Delete();
        }
        //else {
          //Printf("outside cut %s",cut.Data());
        //}
      }
      else {
        Printf("outside cut %s",cut.Data());
      }
      outfile0.close();
    }
  Printf("test6");
    totalEntries.push_back(local_totalEntries);
    avg_combos_miniruns.push_back(tmp_avg);
    avg_combos.push_back(tmp_avg_combo);
  }
  //nen = mini->Draw(Form("run+0.1*mini"),cut.Data(),"goff");
  for (Int_t i = vs.size()-1 ; i >= 0 ; i--) {
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (nen > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw.c_str(),subpad));
  Printf("test7");
      tmpG = new TGraph(totalEntries.at(i),&xaxis[0],&(avg_combos_miniruns.at(i))[0]);
      //Printf("X axis has %d entries, Y has %d entries",xaxis.size(),avg_combos_miniruns.at(i).size());
      //tmpG = new TGraph(totalEntries,(const Double_t *)mini->GetV1(),(const Double_t *)(avg_combos_miniruns.at(i)));
      //tmpG = new TGraph(totalEntries.at(i),(const Double_t *)xaxis,(const Double_t *)(avg_combos_miniruns.at(i)));
      mgs.push_back(tmpG);
      tmpG->SetNameTitle(Form("%s_%s_%d_avg_%s_combo_seg",vs.at(i).Data(),draw.c_str(),subpad,cut.Data()),Form("%s_%s_%d_avg",vs.at(i).Data(),draw.c_str(),subpad));
      //if (cut == "") {
        tmpG->SetLineColor(startcolor+2*i);
        tmpG->SetMarkerColor(startcolor+2*i);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.4);
      mg->Add(tmpG,"p");
      legend->AddEntry(tmpG,(TString)vs.at(i),"lp");
    }
  }

  Printf("test8");
  // Assume evMon are compositions, else doing a straight slope
  if (((TString)draw).Contains("evMon")) {
  Printf("test9");
    mg->SetNameTitle(Form("%s BPM combination composition %s averaged %d %s_combo_seg",draw.c_str(),averaging.Data(),subpad,cut.Data()),Form("%s BPM combination composition %s averaged;Run+0.1*Minirun;%s composition %s averaged",draw.c_str(),averaging.Data(),draw.c_str(),averaging.Data()));
  }
  else {
  Printf("test10");
    mg->SetNameTitle(Form("%s slopes vs evMons, %s averaged %d %s_combo_seg",draw.c_str(),averaging.Data(),subpad,cut.Data()),Form("%s slopes vs, evMons, %s averaged;Run+0.1*Minirun;%s slopes averaged",draw.c_str(),averaging.Data(),draw.c_str()));
  }
  mg->Draw("alp");
  //if (subpad%3==0 || cut=="rcdb_arm_flag==0") {
    legend->Draw();
  //}
  p1->Update();
  p1->Modified();
  return p1;
}

TVirtualPad* ToolBox::combo_segment_compare_multiGraph(TPad* c1, TVirtualPad* p1, TChain* mini, TChain* dit, int subpad = 1, std::string draw_reg = "cyclenum", std::string draw_dit = "cyclenum", TString cut_reg = "", TString cut_dit = "", TString averaging = "dit_segment", TString vs = "evMon0", Int_t startcolor=30){
  p1=c1->cd(subpad);
  p1->SetGrid();
  Printf("test1");

  TLegend* legend = new TLegend(0.1,0.7,0.30,0.9);
  //legend->SetHeader("Header");//,"C");// option "C" allows to center the header
  std::vector<TGraph*> mgs;
  TMultiGraph *mg = new TMultiGraph();
  TGraph *tmpG;
  TGraph *tmpG_dit;

  TString x_axis_name = "run+0.1*mini";
  TString x_axis_name_dit = "run";
  if ((TString)mini->GetName() == "dit") {
    x_axis_name = "run";
  }
  std::map<Int_t,Int_t> segments;
  Int_t segment;
  Int_t run_segment;
  Double_t double_segment;
  TString dit_averaging = "segment";
  if (averaging == "dit_segment") {
    dit_averaging = "segment";
  }
  else {
    dit_averaging = averaging;
  }
  if (((TString)averaging).Contains("segment")){
    mini->SetBranchAddress(Form("%s",averaging.Data()),&segment);
    mini->Draw(Form(">>elist_%s_%d",draw_reg.c_str(),subpad), Form("%s",cut_reg.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw_reg.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(segment) == 0) {
        segments[segment] = 1;
      }
      else {
        segments[segment] = segments.at(segment) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  } 
  else if (((TString)averaging).Contains("run")){
    mini->SetBranchAddress(Form("%s",averaging.Data()),&run_segment);
    mini->Draw(Form(">>elist_%s_%d",draw_reg.c_str(),subpad), Form("%s",cut_reg.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw_reg.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(run_segment) == 0) {
        segments[run_segment] = 1;
      }
      else {
        segments[run_segment] = segments.at(run_segment) + 1;
      }
    }
    mini->SetEntryList(0);
    delete elist;
  } 
  else {
    mini->SetBranchAddress(Form("%s",averaging.Data()),&double_segment);
    mini->Draw(Form(">>elist_%s_%d",draw_reg.c_str(),subpad), Form("%s",cut_reg.Data()), "entrylist");
    TEntryList *elist = (TEntryList*)gDirectory->Get(Form("elist_%s_%d",draw_reg.c_str(),subpad));
    mini->SetEntryList(elist);
    for (Int_t nent = 0 ; nent<elist->GetN()/*nent<mini->GetEntries()*/ ; nent++) {
      mini->GetEntry(mini->GetEntryNumber(nent));
      if (segments.count(double_segment) == 0) {
        segments[double_segment] = 1;
      }
      else {
        segments[double_segment] = segments.at(double_segment) + 1;
      }
    }
    mini->SetEntryList(0);
  }

  std::vector<std::map<Int_t,Double_t>> avg_combos;
  std::vector<std::vector<Double_t>> avg_combos_miniruns;
  std::vector<std::vector<Double_t>> avg_combos_miniruns_dit;
  std::vector<Double_t>  xaxis;
  std::vector<Double_t>  xaxis_dit;
  Int_t nen=0;
  Int_t nen_dit=0;
  Int_t totalEntries;
  Int_t totalEntries_dit;

  //for (Int_t i = 0 ; i < vs.size() ; i++) {
    Int_t local_totalEntries = 0;
    Int_t local_totalEntries_dit = 0;
    std::vector<Double_t> tmp_avg;
    std::vector<Double_t> tmp_avg_dit;
    std::map<Int_t,Double_t> tmp_avg_combo;
    for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    //for (Int_t iSeg = 0; iSeg<segments.size() ; iSeg++) 
      //Printf("Adding %d events for segment %d",iter->second,iter->first);
      if (cut_reg == "") {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw_reg.c_str(),vs.at(i).Data(),i,iter->first),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw_reg.c_str(),vs.Data()),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",averaging.Data(),iter->first,averaging.Data(),iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw_reg.c_str(),vs.Data(),iter->first);
      }
      else {
        //nen = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw_reg.c_str(),vs.Data(),i,iter->first),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut_reg.Data(),iter->first,iter->first));
        nen = mini->Draw(Form("(%s_%s)",draw_reg.c_str(),vs.Data()),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut_reg.Data(),averaging.Data(),iter->first,averaging.Data(),iter->first));
        //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut %s && dit_segment==%d",nen,draw_reg.c_str(),vs.Data(),cut_reg.Data(),iter->first);
      }
      if (nen != iter->second && nen!=0) {
        //Printf("Error, number of entries passing cut here = %d != above %d",nen,iter->second);
      }
      if (nen != 0) {
        //TGraph* g1 = (TGraph*)p1->FindObject("Graph");
        //g1->SetName(Form("g1_%d_%d",i,iter->first));
        //if (g1 != 0) {
          //Printf("g1 Works!");
        //}
        //TH2* ah1 = (TH2*)gROOT->Get("htemp");
        TH1* ah1;
        ah1 = (TH1*)(p1->FindObject("htemp"));
        //ah1 = (TH1*)(p1->FindObject(Form("h1_%d_%d",i,iter->first)));
        //if (ah1 != 0) {
          //ah1->SetName(Form("h1_%d_%d",i,iter->first));
        //}
        if (nen != 0 && ah1 != 0) {
          Double_t avg_tmp1 = 0.0;
          avg_tmp1 = (Double_t)(ah1->GetMean(1));
          //Printf("Mean = %f, entries = %d",avg_tmp1,nen);
          tmp_avg_combo[iter->first] = avg_tmp1;

          if (cut_dit == "") {
            //nen_dit = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw_dit.c_str(),vs.at(i).Data(),i,iter->first),Form("dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",iter->first,iter->first));
            nen_dit = dit->Draw(Form("(%s_%s)",draw_dit.c_str(),vs.Data()),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first));
            //Printf("nen_dit = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen_dit,draw_dit.c_str(),vs.Data(),iter->first);
          }
          else {
            //nen_dit = mini->Draw(Form("(%s_%s)>>h1_%d_%d",draw_dit.c_str(),vs.Data(),i,iter->first),Form("%s && dit_segment>=(%d-0.1) && dit_segment<=(%d+0.1)",cut_dit.Data(),iter->first,iter->first));
            //Printf("nen_dit = dit->Draw(Form(\"(%s_%s)\", cut = ,Form(\"%s && %s>=(%d-0.1) && %s<=(%d+0.1)\"",draw_dit.c_str(),vs.Data(),cut_dit.Data(),dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first);
            nen_dit = dit->Draw(Form("(%s_%s)",draw_dit.c_str(),vs.Data()),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut_dit.Data(),dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first));
            //Printf("nen_dit = %d, Draw run+0.1*mini:(%s_%s), cut_dit %s && dit_segment==%d",nen_dit,draw_dit.c_str(),vs.Data(),cut_dit.Data(),iter->first);
          }
          //local_totalEntries_dit += nen_dit;
          TH1* ah2;
          ah2 = (TH1*)(p1->FindObject("htemp"));
          Double_t avg_tmp1_dit = 0.0;
          if (!ah2 || nen_dit == 0) {
            avg_tmp1_dit = 0.0;
          }
          else {
            avg_tmp1_dit = (Double_t)(ah2->GetMean(1));
          }
          for (Int_t j = 0 ; j < nen_dit ; j++) {
            tmp_avg_dit.push_back(avg_tmp1_dit);
          }
          // Do X axis
          if (cut_reg == "") {
            nen = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name.Data(),0,iter->first),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",averaging.Data(),iter->first,averaging.Data(),iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen,draw_reg.c_str(),vs.Data(),iter->first);
          }
          else {
            nen = mini->Draw(Form("%s>>h2_%d_%d",x_axis_name.Data(),0,iter->first),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut_reg.Data(),averaging.Data(),iter->first,averaging.Data(),iter->first));
            //Printf("nen = %d, Draw run+0.1*mini:(%s_%s), cut_reg %s && dit_segment==%d",nen,draw_reg.c_str(),vs.Data(),cut_reg.Data(),iter->first);
          }
          std::vector<Double_t> tmp_xaxis(mini->GetV1(), mini->GetV1() + mini->GetSelectedRows() % mini->GetEstimate());
          for (Int_t j = 0 ; j < nen ; j++){
            tmp_avg.push_back(avg_tmp1);
          //  if (i == 0) {
              Double_t local_xaxis = 0.0;
              local_xaxis = tmp_xaxis.at(j);
              //Printf("Adding run+0.1*minirun = %0.1f",tmp_xaxis.at(j));
              xaxis.push_back(local_xaxis);
            //}
          }
          local_totalEntries += nen;
          // X axis for dit
          if (cut_dit == "") {
            nen_dit = dit->Draw(Form("%s>>h2_%d_%d",x_axis_name_dit.Data(),0,iter->first),Form("%s>=(%d-0.1) && %s<=(%d+0.1)",dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first));
            //Printf("nen_dit = %d, Draw run+0.1*mini:(%s_%s), dit_segment==%d",nen_dit,draw_dit.c_str(),vs.Data(),iter->first);
          }
          else {
            nen_dit = dit->Draw(Form("%s>>h2_%d_%d",x_axis_name_dit.Data(),0,iter->first),Form("%s && %s>=(%d-0.1) && %s<=(%d+0.1)",cut_dit.Data(),dit_averaging.Data(),iter->first,dit_averaging.Data(),iter->first));
            //Printf("nen_dit = %d, Draw run+0.1*mini:(%s_%s), cut_dit %s && dit_segment==%d",nen_dit,draw_dit.c_str(),vs.Data(),cut_dit.Data(),iter->first);
          }
          std::vector<Double_t> tmp_xaxis_dit(dit->GetV1(), dit->GetV1() + dit->GetSelectedRows() % dit->GetEstimate());
          for (Int_t j = 0 ; j < nen_dit ; j++){
              Double_t local_xaxis = 0.0;
              local_xaxis = tmp_xaxis_dit.at(j);
              //Printf("Adding run+0.1*minirun = %0.1f",tmp_xaxis.at(j));
              xaxis_dit.push_back(local_xaxis);
            //}
          }
          local_totalEntries_dit += nen_dit;
          //ah1->Delete();
        }
        //else {
          //Printf("outside cut %s",cut_reg.Data());
        //}
      }
      else {
        //Printf("outside cut %s",cut_reg.Data());
      }
    }
  Printf("test6");
    totalEntries = local_totalEntries;
    totalEntries_dit = local_totalEntries_dit;
    avg_combos_miniruns.push_back(tmp_avg);
    avg_combos_miniruns_dit.push_back(tmp_avg_dit);
    avg_combos.push_back(tmp_avg_combo);
  //
  //nen = mini->Draw(Form("run+0.1*mini"),cut_reg.Data(),"goff");
  //for (Int_t i = 0 ; i < vs.size() ; i++) {
    //printf("Plotting %d good entries where 13746 is ideal\n",nen);
    if (totalEntries > 0) {
      //((TGraph*)p1->FindObject("Graph"))->SetName(Form("eq3542_%s_%d",draw_reg.c_str(),subpad));
      //eq3542 = (TGraph*)p1->FindObject(Form("eq3542_%s_%d",draw_reg.c_str(),subpad));
  Printf("test7");
      tmpG = new TGraph(totalEntries,&xaxis[0],&(avg_combos_miniruns.at(0))[0]);
      tmpG_dit = new TGraph(totalEntries_dit,&xaxis_dit[0],&(avg_combos_miniruns_dit.at(0))[0]);
      //Printf("X axis has %d entries, Y has %d entries",xaxis.size(),avg_combos_miniruns.at(i).size());
      //tmpG = new TGraph(totalEntries,(const Double_t *)mini->GetV1(),(const Double_t *)(avg_combos_miniruns.at(i)));
      //tmpG = new TGraph(totalEntries.at(i),(const Double_t *)xaxis,(const Double_t *)(avg_combos_miniruns.at(i)));
      mgs.push_back(tmpG);
      mgs.push_back(tmpG_dit);
      tmpG->SetNameTitle(Form("%s_%s_%d_avg_combo_seg",vs.Data(),draw_reg.c_str(),subpad),Form("%s_%s_%d_avg",vs.Data(),draw_reg.c_str(),subpad));
      //if (cut_reg == "") {}
        tmpG->SetLineColor(startcolor);
        tmpG->SetMarkerColor(startcolor);
      tmpG_dit->SetNameTitle(Form("%s_%s_%d_avg_combo_seg_dit",vs.Data(),draw_dit.c_str(),subpad),Form("%s_%s_%d_avg",vs.Data(),draw_dit.c_str(),subpad));
      //if (cut_dit == "") {
        tmpG_dit->SetLineColor(startcolor+10);
        tmpG_dit->SetMarkerColor(startcolor+10);
      //}
      //else {
      //  tmpG->SetLineColor(startcolor+i);
      //  tmpG->SetMarkerColor(startcolor+i);
      //}
      tmpG->SetMarkerStyle(20);
      tmpG->SetMarkerSize(0.4);
      mg->Add(tmpG,"p");
      tmpG_dit->SetMarkerStyle(20);
      tmpG_dit->SetMarkerSize(0.4);
      mg->Add(tmpG_dit,"p");
      legend->AddEntry(tmpG,(TString)vs+" reg","lp");
      legend->AddEntry(tmpG_dit,(TString)vs+" dit","lp");
    }
  //}

  Printf("test8");
  // Assume evMon are compositions, else doing a straight slope
  if (((TString)draw_reg).Contains("evMon")) {
  Printf("test9");
    mg->SetNameTitle(Form("%s BPM combination composition %s averaged %d %s_combo_seg",draw_reg.c_str(),averaging.Data(),subpad,cut_reg.Data()),Form("%s BPM combination composition %s averaged;Run+0.1*Minirun;%s composition %s averaged",draw_reg.c_str(),averaging.Data(),draw_reg.c_str(),averaging.Data()));
  }
  else {
  Printf("test10");
    mg->SetNameTitle(Form("%s slopes vs evMons, %s averaged %d %s_combo_seg",draw_reg.c_str(),averaging.Data(),subpad,cut_reg.Data()),Form("%s slopes vs, evMons, %s averaged;Run+0.1*Minirun;%s slopes averaged",draw_reg.c_str(),averaging.Data(),draw_reg.c_str()));
  }
  mg->Draw("alp");
  //if (subpad%3==0 || cut_reg=="rcdb_arm_flag==0") {
    legend->Draw();
  //}
  return p1;
}

void ToolBox::combo_segment_getter(TString averaging,TTree* intree, TTree* outtree, TString data_tree_name, TString cut, std::vector<TString> DVs, std::vector<TString> Monitors, std::vector<TString> IVs){
  // This program finds segmentwise averages (using goff TH1 auto-draws)
  // of Eigen Vector BPM combo compositions (and slopes too) and prints to tree
  //

  // Define a new method which takes as argument the variable name against which to perform averaging, TTree of slopes to average, the 3 deep vector to assign as new Branches, and the DV, Monitor, and IV name vectors. Execute this for all trees of slopes that need averaging.
  // For each DV, loop over Monitors, for each Monitor loop over component BPMs.
  // For each thing create a branch to hold it - these need to be 3 deep vectors, pre-filled with 0.0's (each 0.0 needs to be a double, each layer needs to be a std::vector, etc. be careful)
  // Loop over every event to fill a map with segment numbers (and number of miniruns within them)
  // Loop over the map of segment numbers to generate averages per segment for the 3 deep vector created before (consider writing this code so it can be augmented in the future to do run-wise or slug-wise averaging as well)
  //   After finishing the 3 deep vector, for each segment value Fill the tree with those numbers. Be sure to pass the htemp->GetMean() into a locally defined Double_t and then assign the vector's value = this new Double_t before doing Fill (otherwise the pointer to GetMean() will be obtained and you will fail)

  std::vector<std::vector<Double_t>> comboVec2_mean;
  std::vector<std::vector<Double_t>> comboVec2_mean_err;
  std::vector<std::vector<Double_t>> comboVec2_rms;
  std::vector<std::vector<Double_t>> slopeVec2_mean;
  std::vector<std::vector<Double_t>> slopeVec2_mean_err;
  std::vector<std::vector<Double_t>> slopeVec2_rms;
  std::vector<Double_t> slopeVec_mean;
  std::vector<Double_t> slopeVec_mean_err;
  std::vector<Double_t> slopeVec_rms;
  std::vector<Double_t> comboVec_mean;
  std::vector<Double_t> comboVec2_norm2;
  std::vector<Double_t> comboVec_mean_err;
  std::vector<Double_t> comboVec_rms;
  // FIXME Technically nDet and nBPM are 0 unless you've executed "calculateSensitivities" at least once...
  for(int idet=0;idet<DVs.size();idet++){
    slopeVec_mean.clear();
    slopeVec_mean_err.clear();
    slopeVec_rms.clear();
    for(int imon=0;imon<Monitors.size();imon++){
      Double_t slopeVec1_mean = 0.0;
      Double_t slopeVec1_mean_err = 0.0;
      Double_t slopeVec1_rms = 0.0;
      slopeVec_mean.push_back(slopeVec1_mean);
      slopeVec_mean_err.push_back(slopeVec1_mean_err);
      slopeVec_rms.push_back(slopeVec1_rms);
    }
    slopeVec2_mean.push_back(slopeVec_mean);
    slopeVec2_mean_err.push_back(slopeVec_mean_err);
    slopeVec2_rms.push_back(slopeVec_rms);
  }
  for(int imon=0;imon<Monitors.size();imon++){
    comboVec_mean.clear();
    comboVec_mean_err.clear();
    comboVec_rms.clear();
    for (Int_t ibpm = 0; ibpm<IVs.size() ; ibpm++){
      Double_t comboVec1_mean = 0.0;
      Double_t comboVec1_mean_err = 0.0;
      Double_t comboVec1_rms = 0.0;
      comboVec_mean.push_back(comboVec1_mean);
      comboVec_mean_err.push_back(comboVec1_mean_err);
      comboVec_rms.push_back(comboVec1_rms);
    }
    comboVec2_mean.push_back(comboVec_mean);
    comboVec2_mean_err.push_back(comboVec_mean_err);
    comboVec2_rms.push_back(comboVec_rms);

    Double_t comboVec1_norm2 = 0.0;
    comboVec2_norm2.push_back(comboVec1_norm2);
  }
  // Set branch address after the vectors are filled to avoid C++ dynamic memory allocation shenanigans
  for(int idet=0;idet<DVs.size();idet++){
    for(int imon=0;imon<Monitors.size();imon++){
      outtree->Branch(Form("%s_%s_mean",DVs.at(idet).Data(),Monitors.at(imon).Data()),&slopeVec2_mean[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_err",DVs.at(idet).Data(),Monitors.at(imon).Data()),&slopeVec2_mean_err[idet][imon]);
      outtree->Branch(Form("%s_%s_rms",DVs.at(idet).Data(),Monitors.at(imon).Data()),&slopeVec2_rms[idet][imon]);
    }
  }
  for(int imon=0;imon<Monitors.size();imon++){
    for (Int_t ibpm = 0; ibpm<IVs.size() ; ibpm++){
      outtree->Branch(Form("%s_%s_mean",Monitors.at(imon).Data(),IVs.at(ibpm).Data()),&comboVec2_mean[imon][ibpm]);
      outtree->Branch(Form("%s_%s_mean_err",Monitors.at(imon).Data(),IVs.at(ibpm).Data()),&comboVec2_mean_err[imon][ibpm]);
      outtree->Branch(Form("%s_%s_rms",Monitors.at(imon).Data(),IVs.at(ibpm).Data()),&comboVec2_rms[imon][ibpm]);
    }
    outtree->Branch(Form("%s_norm",Monitors.at(imon).Data()),&comboVec2_norm2[imon]);
  }

  // Get the number of segments from the tree directly
  Int_t totalEntries = intree->GetEntries();
  Int_t int_avg_time_scale = 0;
  Double_t double_avg_time_scale = 0;
  if (averaging == "rcdb_slug" || averaging == "rcdb_flip_state" || averaging == "crex_part" || averaging == "crex_part_b") {
    intree->SetBranchAddress(averaging,&double_avg_time_scale);
  }
  if (averaging == "dit_segment" || averaging == "run") {
    intree->SetBranchAddress(averaging,&int_avg_time_scale);
  }
  /*std::map<Int_t,Int_t> avg_time_scales;
  for (Int_t ient = 0 ; ient<totalEntries ; ient++) {
    intree->GetEntry(ient);
    if (avg_time_scales.count(avg_time_scale) == 0) {
      avg_time_scales[avg_time_scale] = 1;
    }
    else {
      avg_time_scales[avg_time_scale] = avg_time_scales.at(avg_time_scale) + 1;
    }
  }
  */

  // FIXME Should this calculation be done once per each minirun entry in intree->outtree? Or store a giant map and print per entry later (seems inefficient). I am storing the number of entries that mattch the localAvg_time_scale == X cut, but there are possible entries with NO localAvg_time_scale value, or something like that, which would be edge cases... also I want to preserve the order of points from intree->outtree for friendability later, so I need to loop over friend tree for sure with some kind of lookup reference.
  // FIXME Idea: check if the localAvg_time_scale branch value has changed since the last event, if it has then calculate a new set of means and Fill, but doing a Fill && Continue if not
  Int_t localAvg_time_scale = -1000; // Default initialize to rediculous number
  Int_t previousAvg_time_scale = -1000; // Default initialize to rediculous number
  Int_t nen=0;
  //for (std::map<Int_t,Int_t>::iterator iter = avg_time_scales.begin() ; iter != avg_time_scales.end(); ++iter){}
  for (Int_t ient = 0 ; ient<totalEntries ; ient++) {
    intree->GetEntry(ient);
    if (averaging == "rcdb_slug" || averaging == "rcdb_flip_state" || averaging == "crex_part" || averaging == "crex_part_b") {
      localAvg_time_scale = (Int_t)double_avg_time_scale;
    }
    if (averaging == "dit_segment" || averaging == "run") {
      localAvg_time_scale = int_avg_time_scale;
    }
    //Printf("A) Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient);
    if (localAvg_time_scale == previousAvg_time_scale) {
      //Printf("Same localAvg_time_scale, filling another instance of the previously calculated means, etc. for slopes and BPM eigen vector monitor combos");
      //Printf("B) Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient);
      outtree->Fill();
      continue;
    }
    else {
      previousAvg_time_scale = localAvg_time_scale;
      // else grab all of the slopes and combos for this localAvg_time_scale value
      // Slopes
      for (Int_t idet = 0 ; idet < DVs.size() ; idet++) {
        //Printf("test 1 slopes");
        for (Int_t imon = 0 ; imon < Monitors.size() ; imon++) {
          if (cut == "") {
      //Printf("Test e) localAvg_time_scale = %d",localAvg_time_scale);
            nen = intree->Draw(Form("(%s.%s_%s)",data_tree_name.Data(),DVs.at(idet).Data(),Monitors.at(imon).Data()),Form("%s>=%d-0.1 && %s<=%d+0.1",averaging.Data(),localAvg_time_scale,averaging.Data(),localAvg_time_scale),"goff");
      //Printf("Test f) localAvg_time_scale = %d",localAvg_time_scale);
            if (imon == 0 && idet == 0){
              //Printf("C) Avg timescale %s = %d for entry %d",averaging.Data(),localAvg_time_scale,ient);
              //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d",nen,data_tree_name.Data(),DVs.at(idet).Data(),Monitors.at(imon).Data(),averaging.Data(),localAvg_time_scale);
            }
          }
          else {
            nen = intree->Draw(Form("(%s.%s_%s)",data_tree_name.Data(),DVs.at(idet).Data(),Monitors.at(imon).Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),localAvg_time_scale,averaging.Data(),localAvg_time_scale,cut.Data()),"goff");
            //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d && %s",nen,data_tree_name.Data(),DVs.at(idet).Data(),Monitors.at(imon).Data(),averaging.Data(),localAvg_time_scale,cut.Data());
          }
          // Get mean
          if (nen != 0) {
            TH1* ah1;
            ah1 = (TH1*)(gROOT->FindObject("htemp"));
            if (nen != 0 && ah1 != 0) {
              //Printf("test 3 slopes means");
              Double_t avg_tmp1 = 0.0;
              Double_t err_tmp1 = 0.0;
              Double_t rms_tmp1 = 0.0;
              avg_tmp1 = (Double_t)(ah1->GetMean(1));
              err_tmp1 = (Double_t)(ah1->GetMeanError(1));
              rms_tmp1 = (Double_t)(ah1->GetRMS(1));
              if (imon == 0 && idet == 0){
                //Printf("New mean %s %s = %f, entries = %d",DVs.at(idet).Data(),Monitors.at(imon).Data(),avg_tmp1,nen);
              }
              slopeVec2_mean[idet][imon]     = avg_tmp1;
              slopeVec2_mean_err[idet][imon] = err_tmp1;
              slopeVec2_rms[idet][imon]      = rms_tmp1;
            }
          }
          //else {
            //Printf("Using prior segment's value for now...");
          //}
        }
      }
      // Combos
      for (Int_t imon = 0 ; imon < Monitors.size() ; imon++) {
        //Printf("test 1 combos");
        comboVec2_norm2[imon] = 0.0;
        for (Int_t ibpm = 0 ; ibpm < IVs.size() ; ibpm++) {
          //Printf("test 2 combos");
          if (cut == "") {
            nen = intree->Draw(Form("(%s.%s_%s)",data_tree_name.Data(),Monitors.at(imon).Data(),IVs.at(ibpm).Data()),Form("%s==%d",averaging.Data(),localAvg_time_scale),"goff");
            //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d",nen,data_tree_name.Data(),Monitors.at(imon).Data(),IVs.at(ibpm).Data(),averaging.Data(),localAvg_time_scale);
          }
          else {
            nen = intree->Draw(Form("(%s.%s_%s)",data_tree_name.Data(),Monitors.at(imon).Data(),IVs.at(ibpm).Data()),Form("%s==%d && %s",averaging.Data(),localAvg_time_scale,cut.Data()),"goff"); // FIXME "goff"???
            //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d && %s",nen,data_tree_name.Data(),Monitors.at(imon).Data(),IVs.at(ibpm).Data(),averaging.Data(),localAvg_time_scale,cut.Data());
          }
          // Get mean
          if (nen != 0) {
            TH1* ah1;
            ah1 = (TH1*)(gROOT->FindObject("htemp"));
            if (nen != 0 && ah1 != 0) {
              //Printf("test 3 combos means");
              Double_t avg_tmp1 = 0.0;
              Double_t err_tmp1 = 0.0;
              Double_t rms_tmp1 = 0.0;
              avg_tmp1 = (Double_t)(ah1->GetMean(1));
              err_tmp1 = (Double_t)(ah1->GetMeanError(1));
              rms_tmp1 = (Double_t)(ah1->GetRMS(1));
              //Printf("New mean %s %s = %f, entries = %d",Monitors.at(imon).Data(),IVs.at(ibpm).Data(),avg_tmp1,nen);
              comboVec2_mean[imon][ibpm]     = avg_tmp1;
              comboVec2_norm2[imon]         += (avg_tmp1*avg_tmp1);
              comboVec2_mean_err[imon][ibpm] = err_tmp1;
              comboVec2_rms[imon][ibpm]      = rms_tmp1;
            }
          }
          //else {
            //Printf("Using prior segment's value for now... for %d in %d",ient,localAvg_time_scale);
          //}
        }
      }
      // Renormalize the averaged eigenvector definition.
      for (Int_t imon = 0 ; imon < Monitors.size() ; imon++) {
        for (Int_t ibpm = 0 ; ibpm < IVs.size() ; ibpm++) {
          comboVec2_mean[imon][ibpm]     = comboVec2_mean[imon][ibpm]/sqrt(comboVec2_norm2[imon]);
          comboVec2_mean_err[imon][ibpm] = comboVec2_mean_err[imon][ibpm]/sqrt(comboVec2_norm2[imon]);
          comboVec2_rms[imon][ibpm]      = comboVec2_rms[imon][ibpm]/sqrt(comboVec2_norm2[imon]);
        }
        comboVec2_norm2[imon] = sqrt(comboVec2_norm2[imon]);
      }
      // FIXME FIXME HACK -> Entry 0 is a special case where the dit_flag == 0 and it fills the tree with nothing, so just workaround by not filling here and then filling 2x on the next good entry. Serious HACK for CREX
      // FIXME this hack needs to be not just for entry 0, but for all dit_segment == 0
      if (cut == "dit_flag==1") {
        if (ient == 0) {
          continue;
        }
        if (ient == 1) {
          outtree->Fill(); 
        } // Note that this hack is only needed for this initial value, as in subsequent failed cases it will just adopt the previous successful segment's old values (and this never happens again).
      }
      //Printf("test fill");
      outtree->Fill();
    }
  }
}

void ToolBox::combo_err_segment_getter(TString averaging,TTree* intree, TTree* outtree/*_orig*/, TString proto_draw, std::vector<TString> draws_piece1, std::vector<TString> draws_piece2, std::vector<TString> cuts, Int_t print_per_entry = 0){ 
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
  
  TString draw = "";
  TString draw_1 = "";
  TString draw_2 = "";
  TString draw_3 = "";
  //TTree * outtree = (TTree*) outtree_orig->Clone(0);
  //Int_t nentries_orig = outtree_orig->GetEntries();
  Int_t nentries_orig = outtree->GetEntries();

  std::vector<std::vector<Double_t>> dataVec2_mean;
  std::vector<std::vector<Double_t>> dataVec2_mean_err;
  std::vector<std::vector<Double_t>> dataVec2_rms;
  std::vector<Double_t> dataVec_mean;
  std::vector<Double_t> dataVec_mean_err;
  std::vector<Double_t> dataVec_rms;
  // FIXME Technically nDet and nBPM are 0 unless you've executed "calculateSensitivities" at least once...
  for(int idet=0;idet<draws_piece1.size();idet++){
    dataVec_mean.clear();
    dataVec_mean_err.clear();
    dataVec_rms.clear();
    for(int imon=0;imon<draws_piece2.size();imon++){
      Double_t dataVec1_mean = 0.0;
      Double_t dataVec1_mean_err = 0.0;
      Double_t dataVec1_rms = 0.0;
      dataVec_mean.push_back(dataVec1_mean);
      dataVec_mean_err.push_back(dataVec1_mean_err);
      dataVec_rms.push_back(dataVec1_rms);
    }
    dataVec2_mean.push_back(dataVec_mean);
    dataVec2_mean_err.push_back(dataVec_mean_err);
    dataVec2_rms.push_back(dataVec_rms);
  }
  // Set branch address after the vectors are filled to avoid C++ dynamic memory allocation shenanigans
  for(int idet=0;idet<draws_piece1.size();idet++){
    for(int imon=0;imon<draws_piece2.size();imon++){
      outtree->Branch(Form("%s_%s_mean",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean[idet][imon]);
      outtree->Branch(Form("%s_%s_mean_err",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_mean_err[idet][imon]);
      outtree->Branch(Form("%s_%s_rms",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data()),&dataVec2_rms[idet][imon]);
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

  // Get the number of segments from the tree directly
  Int_t run = 0;
  Int_t flag = 0;
  Double_t cyclenum = 0.0;
  Int_t totalEntries = intree->GetEntries();
  Int_t int_avg_time_scale = 0;
  Double_t double_avg_time_scale = 0;
  if (averaging == "rcdb_slug" || averaging == "rcdb_flip_state" || averaging == "crex_part" || averaging == "crex_part_b") {
    intree->SetBranchAddress(averaging,&double_avg_time_scale);
  }
  if (averaging == "dit_segment" || averaging == "segment" || averaging == "run") {
    intree->SetBranchAddress(averaging,&int_avg_time_scale);
  }
  if (intree->GetBranch("run")) {
    intree->SetBranchAddress("run",&run);
    outtree->Branch("run",&run);
  }
  if (intree->GetBranch("cyclenum")) {
    intree->SetBranchAddress("cyclenum",&cyclenum);
    outtree->Branch("cyclenum",&cyclenum);
  }
  if (intree->GetBranch("flag")) {
    intree->SetBranchAddress("flag",&flag);
    outtree->Branch("flag",&flag);
  }

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
  for (std::map<Int_t,Int_t>::iterator iter = segments.begin() ; iter != segments.end(); ++iter){
    localAvg_time_scale = iter->first;
    if (print_per_entry == 1) {
      for (Int_t ient = 0 ; ient<totalEntries ; ient++) {
        intree->GetEntry(ient);
        if (averaging == "rcdb_slug" || averaging == "rcdb_flip_state" || averaging == "crex_part" || averaging == "crex_part_b") {
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
    //else {}
      previousAvg_time_scale  = iter->first;
      iterLocalAvg_time_scale = iter->first;
      // else grab all of the slopes and combos for this iterLocalAvg_time_scale value
      // Slopes
      for (Int_t idet = 0 ; idet < draws_piece1.size() ; idet++) {
        //Printf("test 1 slopes");
        for (Int_t imon = 0 ; imon < draws_piece2.size() ; imon++) {
          draw_1 = proto_draw(0,proto_draw.First('#'));
          draw_2 = proto_draw(proto_draw.First('#')+1,proto_draw.Length()-1);
          if (draw_2.Contains("#")) {
            draw_3 = draw_2(draw_2.First('#')+1,draw_2.Length()-1);
            draw_2 = draw_2(0,draw_2.First('#'));
          }
          else {
            draw_3 = "";
            draws_piece2.at(imon) = "";
          }
          draw = draw_1+draws_piece1.at(idet)+draw_2+draws_piece2.at(imon)+draw_3;
          //Printf("%s + %s + %s = %s",proto_draw.Data(),draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),draw.Data());
          if (cuts.at(idet) == "") {
      //Printf("Test e) iterLocalAvg_time_scale = %d",iterLocalAvg_time_scale);
            nen = intree->Draw(Form("%s",draw.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale),"goff");
      //Printf("Test f) iterLocalAvg_time_scale = %d",iterLocalAvg_time_scale);
            if (imon == 0 && idet == 0){
              //Printf("C) Avg timescale %s = %d for entry %d",averaging.Data(),iterLocalAvg_time_scale,ient);
              //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d",nen,data_tree_name.Data(),draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),averaging.Data(),iterLocalAvg_time_scale);
            }
          }
          else {
            nen = intree->Draw(Form("%s",draw.Data()),Form("%s>=%d-0.1 && %s<=%d+0.1 && %s",averaging.Data(),iterLocalAvg_time_scale,averaging.Data(),iterLocalAvg_time_scale,cuts.at(idet).Data()),"goff");
            //Printf("nen = %d, Draw run+0.1*mini:(%s.%s_%s), %s==%d && %s",nen,data_tree_name.Data(),draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),averaging.Data(),iterLocalAvg_time_scale,cuts.at(idet).Data());
          }
          // Get mean
          if (nen != 0) {
            TH1* ah1;
            ah1 = (TH1*)(gROOT->FindObject("htemp"));
            if (nen != 0 && ah1 != 0) {
              //Printf("test 3 slopes means");
              Double_t avg_tmp1 = 0.0;
              Double_t err_tmp1 = 0.0;
              Double_t rms_tmp1 = 0.0;
              avg_tmp1 = (Double_t)(ah1->GetMean(1));
              err_tmp1 = (Double_t)(ah1->GetMeanError(1));
              rms_tmp1 = (Double_t)(ah1->GetRMS(1));
              if (imon == 0 && idet == 0){
                //Printf("New mean %s %s = %f, entries = %d",draws_piece1.at(idet).Data(),draws_piece2.at(imon).Data(),avg_tmp1,nen);
              }
              dataVec2_mean[idet][imon]     = avg_tmp1;
              dataVec2_mean_err[idet][imon] = err_tmp1;
              dataVec2_rms[idet][imon]      = rms_tmp1;
            }
          }
          //else {
            //Printf("Using prior segment's value for now...");
          //}
        }
      }
      //Printf("test fill");
      //outtree_orig->GetEntry(outN);
      if (print_per_entry == 0) {
        outtree->Fill();
      }
      outN++;
    //{}
  }
  //outtree_orig = outtree;
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

void ToolBox::dit_copy_and_friend_to_mini(TTree* mini, TTree* dit_in, TTree* dit_out, TTree* dit_unfriended) {
  mini->BuildIndex("dit_segment", "run");
  dit_in->BuildIndex("segment","run");

  Int_t dit_run  = 0.0;
  Int_t dit_out_run  = 0.0;
  Int_t dit_out_mini  = 0.0;
  Double_t dit_cyclenum = 0.0;
  Int_t dit_flag = 0.0;
  Int_t dit_out_flag = 0.0;
  Int_t dit_segment = 0.0;
  Double_t mini_run_type = 0.0;
  Double_t mini_run_flag = 0.0;
  Int_t mini_segment = 0.0;
  Int_t mini_run  = 0.0;
  Int_t mini_mini = 0.0;
  dit_in->SetBranchAddress("run",&dit_run);
  dit_in->SetBranchAddress("cyclenum",&dit_cyclenum);
  dit_in->SetBranchAddress("flag",&dit_flag);
  dit_in->SetBranchAddress("segment",&dit_segment);
  mini  ->SetBranchAddress("rcdb_run_type",&mini_run_type);
  mini  ->SetBranchAddress("rcdb_run_flag",&mini_run_flag);
  mini  ->SetBranchAddress("run",&mini_run);
  mini  ->SetBranchAddress("mini",&mini_mini);
  mini  ->SetBranchAddress("dit_segment",&mini_segment);

  //dit_out->SetName("dit_friendable");
  dit_out->SetBranchStatus("*",1);
  dit_out->SetBranchAddress("run",&dit_out_run);
  dit_out->SetBranchAddress("flag",&dit_out_flag);
  dit_out->Branch("mini",&dit_out_mini);
  //dit_unfriended->SetName("dit_unfriended");
  dit_unfriended->SetBranchStatus("*",1);

  Int_t bytes = 0;
  Int_t entryN = 0;
  Int_t loopSegs = 0;

  // Loop over mini tree and add corresponding dit entries to outut dit file 1 at a time
  for (Int_t i = 0 ; i<mini->GetEntries() ; i++) {
    mini->GetEntry(i);
    Printf("Trying entry with run %d, minirun %d",mini_run,mini_mini);
    bytes = dit_in->GetEntryWithIndex(mini_segment,(Int_t)mini_run);
    //dit_in->GetEntry(dit_in->GetEntryNumberWithIndex(mini_run,mini_mini));
    if (bytes!=0 && (dit_run == (Int_t)mini_run)) {
      dit_out_run = (Int_t)mini_run;
      dit_out_flag = (Int_t)dit_flag;
      dit_out_mini = mini_mini;
      Printf("Bytes = %d",bytes);
      dit_out->Fill();
    }
    else {
      entryN = dit_in->GetEntryNumberWithIndex(mini_segment,(Int_t)mini_run);
      bytes = dit_in->GetEntry(entryN);
      while (entryN == -1) {
        loopSegs++;
        entryN = dit_in->GetEntryNumberWithIndex(mini_segment,(Int_t)(mini_run+loopSegs));
        bytes = dit_in->GetEntry(entryN);
        dit_out_run = (Int_t)mini_run;
        dit_out_flag = 0;
        dit_out_mini = mini_mini;
      }
      dit_out->Fill();
      Printf("Notice: Entry with run %d, minirun %d (run_flag %f, run_type %f) exists in mini-tree but not in dit-tree",mini_run,mini_mini,mini_run_flag,mini_run_type);
      loopSegs=0;
    }
  }
  for (Int_t i = 0 ; i<dit_in->GetEntries() ; i++) {
    dit_in->GetEntry(i);
    bytes = mini->GetEntryWithIndex(dit_segment,(Int_t)dit_run);
    if (bytes == 0) {
      Printf("Entry with run %d, minirun %d (run_flag %f, run_type %f) exists in dit-tree but not in mini-tree",mini_run,mini_mini,mini_run_flag,mini_run_type);
      dit_unfriended->Fill();
    }
  }
}

void ToolBox::dit_segmentwise_copy_and_friend_to_mini(TTree* mini, TTree* dit_in, TTree* dit_out) {
  mini->BuildIndex("dit_segment", "run");
  dit_in->BuildIndex("segment");

  Int_t dit_out_run  = 0.0;
  Int_t dit_out_mini  = 0.0;
  Int_t dit_out_flag = 0.0;
  Int_t dit_segment = 0.0;
  Double_t mini_run_type = 0.0;
  Double_t mini_run_flag = 0.0;
  Int_t mini_segment = 0.0;
  Int_t mini_run  = 0.0;
  Int_t mini_mini = 0.0;
  Int_t mini_flag = 0.0;
  dit_in->SetBranchAddress("segment",&dit_segment);
  mini  ->SetBranchAddress("rcdb_run_type",&mini_run_type);
  mini  ->SetBranchAddress("rcdb_run_flag",&mini_run_flag);
  mini  ->SetBranchAddress("run",&mini_run);
  mini  ->SetBranchAddress("mini",&mini_mini);
  mini  ->SetBranchAddress("dit_flag",&mini_flag);
  mini  ->SetBranchAddress("dit_segment",&mini_segment);

  //dit_out->SetName("dit_friendable");
  dit_out->SetBranchStatus("*",1);
  dit_out->SetBranchAddress("run",&dit_out_run);
  dit_out->SetBranchAddress("flag",&dit_out_flag);
  dit_out->Branch("mini",&dit_out_mini);

  Int_t bytes = 0;
  Int_t entryN = 0;
  Int_t loopSegs = 0;

  // Loop over mini tree and add corresponding dit entries to outut dit file 1 at a time
  for (Int_t i = 0 ; i<mini->GetEntries() ; i++) {
    mini->GetEntry(i);
    Printf("Trying copying entry with segment %d, run %d, minirun %d",mini_segment,mini_run,mini_mini);
    bytes = dit_in->GetEntryWithIndex(mini_segment);
    //dit_in->GetEntry(dit_in->GetEntryNumberWithIndex(mini_run,mini_mini));
    if (bytes!=0 && (dit_segment == (Int_t)mini_segment)) {
      dit_out_run = (Int_t)mini_run;
      dit_out_flag = (Int_t)mini_flag;
      dit_out_mini = mini_mini;
      Printf("Bytes = %d",bytes);
      dit_out->Fill();
    }
    else {
      entryN = dit_in->GetEntryNumberWithIndex(mini_segment,(Int_t)mini_run);
      bytes = dit_in->GetEntry(entryN);
      while (entryN == -1) {
        loopSegs++;
        entryN = dit_in->GetEntryNumberWithIndex(mini_segment,(Int_t)(mini_run+loopSegs));
        bytes = dit_in->GetEntry(entryN);
        dit_out_run = (Int_t)mini_run;
        dit_out_flag = 0;
        dit_out_mini = mini_mini;
      }
      dit_out->Fill();
      Printf("Notice: Entry with run %d, minirun %d (run_flag %f, run_type %f) exists in mini-tree but not in dit-tree",mini_run,mini_mini,mini_run_flag,mini_run_type);
      loopSegs=0;
    }
  }
}

void ToolBox::aggregator_friend_to_mini(TTree* mini, TTree* agg_in, TTree* agg_out, TTree* agg_unfriended) {
  mini->BuildIndex("run","mini");
  agg_in->BuildIndex("run_number","minirun_n");

  Double_t agg_run  = 0.0;
  Double_t agg_mini = 0.0;
  Double_t agg_run_type = 0.0;
  Double_t agg_run_flag = 0.0;
  Int_t mini_run  = 0.0;
  Int_t mini_mini = 0.0;
  agg_in->SetBranchAddress("run_number",&agg_run);
  agg_in->SetBranchAddress("minirun_n",&agg_mini);
  agg_in->SetBranchAddress("rcdb_run_type",&agg_run_type);
  agg_in->SetBranchAddress("rcdb_run_flag",&agg_run_flag);
  mini  ->SetBranchAddress("run",&mini_run);
  mini  ->SetBranchAddress("mini",&mini_mini);

  agg_out->SetBranchStatus("*",1);
  agg_unfriended->SetBranchStatus("*",1);

  Int_t bytes = 0;
  Int_t loopSegs = 0;

  // Loop over mini tree and add corresponding agg entries to outut agg file 1 at a time
  for (Int_t i = 0 ; i<mini->GetEntries() ; i++) {
    mini->GetEntry(i);
    Printf("Trying entry with run %d, minirun %d",mini_run,mini_mini);
    bytes = agg_in->GetEntryWithIndex((Double_t)mini_run,(Double_t)mini_mini);
    //agg_in->GetEntry(agg_in->GetEntryNumberWithIndex(mini_run,mini_mini));
    if (bytes!=0 && (agg_run == mini_run && agg_mini == mini_mini)) {
      Printf("Bytes = %d",bytes);
      agg_out->Fill();
    }
    else {
      Printf("Error: Entry with run %d, minirun %d exists in mini-tree but not in agg-tree",mini_run,mini_mini);
      while (agg_in->GetEntryNumberWithIndex((Double_t)(mini_run+loopSegs),(Double_t)mini_mini) == -1) {
        loopSegs++;
        agg_in->GetEntry(agg_in->GetEntryNumberWithIndex((Double_t)(mini_run+loopSegs),(Double_t)mini_mini));
        agg_run = mini_run;
        agg_mini = mini_mini;
      }
      Printf("Replacing with entry with run %f, minirun %f (run_flag %f, run_type %f) exists in agg-tree but not in mini-tree",agg_run,agg_mini,agg_run_flag,agg_run_type);
      agg_out->Fill();
      loopSegs=0;
      //return;
    }
  }

  // Just take the entries that exist in agg but not in mini tree, loop over agg to get these entries
  for (Int_t i = 0 ; i<agg_in->GetEntries() ; i++) {
    agg_in->GetEntry(i);
    bytes = mini->GetEntryWithIndex((Int_t)agg_run,(Int_t)agg_mini);
    if (bytes!=0 && (agg_run == mini_run && agg_mini == mini_mini)) {
      Printf("Bytes = %d",bytes);
      continue;
    }
    else {
      Printf("Entry with run %f, minirun %f (run_flag %f, run_type %f) exists in agg-tree but not in mini-tree",agg_run,agg_mini,agg_run_flag,agg_run_type);
      agg_unfriended->Fill();
    }
  }
}

void ToolBox::postpan_copy_and_friend_to_mini(TTree* mini, TTree* mini_in, TTree* mini_out) {
  mini->BuildIndex("run", "mini");
  mini_in->BuildIndex("run_number","minirun");

  Int_t mini_in_run  = 0.0;
  Int_t mini_in_mini  = 0.0;
  Int_t mini_run  = 0.0;
  Int_t mini_mini = 0.0;
  mini  ->SetBranchAddress("run",&mini_run);
  mini  ->SetBranchAddress("mini",&mini_mini);

  mini_in->SetBranchAddress("run_number",&mini_in_run);
  mini_in->SetBranchAddress("minirun",&mini_in_mini);
  mini_out->SetBranchStatus("*",1);
  mini_out->SetBranchAddress("run_number",&mini_in_run);
  mini_out->SetBranchAddress("minirun",&mini_in_mini);

  Int_t bytes = 0;
  Int_t entryN = 0;
  Int_t loopSegs = 0;

  // Loop over mini tree and add corresponding mini entries to outut mini file 1 at a time
  for (Int_t i = 0 ; i<mini->GetEntries() ; i++) {
    mini->GetEntry(i);
    bytes = mini_in->GetEntryWithIndex(mini_run,mini_mini);
    Printf("Trying copying entry with run %d, mini %d matching run_number %d, minirun %d",mini_run,mini_mini,mini_in_run,mini_in_mini);
    //mini_in->GetEntry(mini_in->GetEntryNumberWithIndex(mini_run,mini_mini));
    if (bytes!=0 && (mini_run == mini_in_run) && (mini_mini == mini_in_mini)) {
      Printf("Bytes = %d",bytes);
      mini_out->Fill();
    }
    else {
      entryN = mini_in->GetEntryNumberWithIndex(mini_run,mini_mini);
      bytes = mini_in->GetEntry(entryN);
      while (entryN == -1) {
        loopSegs++;
        entryN = mini_in->GetEntryNumberWithIndex(mini_run,mini_mini+loopSegs);
        bytes = mini_in->GetEntry(entryN);
        mini_in_run = mini_run;
        mini_in_mini = mini_mini;
      }
      mini_out->Fill();
      Printf("Notice: Entry with run %d, minirun %d exists in reference mini-tree but not in input mini-tree",mini_run,mini_mini);
      loopSegs=0;
    }
  }
}
