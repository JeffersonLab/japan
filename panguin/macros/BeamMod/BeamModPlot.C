/////////BeamModPlot.C
///// ---  Victoria Owen ----
/////---- Quinn Campgana ----
////-----Carrington Metts------
//// ----- June-July 2019 --------E
///Panguin gui compatible diagnostic plots for shift taking online replay 
/// Three Pads
//Pad one: trim card readback vs. time for full run (up to the point of replay)
//Pad two: X bpm correlations (strong correlations = bad)
//Pad three: Y bpm correlations (strong correlations = bad)


void BeamModPlot(TString type="evt",TString type2="mul", TString ref="CodaEventNumber", Bool_t ldebug = kFALSE){
  
  gStyle->SetOptStat(0);
  TTree* tree_R = (TTree*)gDirectory->Get(type);
  TTree* tree_M = (TTree*)gDirectory->Get(type2);
  
  /*
  if (1==1){
    //if(ldebug) std::cerr << "No entries with 'bmwcycnum>0' in this run." << std::endl;
    //TODO: print error message directly to GUI screen, then shut down
    return 0;
  }
  */
  
  
  tree_R->Draw(">>elist","bmwcycnum>0","entrylist"); //should pick out unique cycle numbers
  TEntryList *elist = (TEntryList*)gDirectory->Get("elist");
  tree_R->SetEntryList(elist);
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  
  

  TString bmwcut = "bmwcycnum>0";
  TString evcut = "(ErrorFlag & 0xda7e6bff)==0";
  TString evcutxcorr = "(ErrorFlag & 0xda7e6bff)==0"; //cut for x sensitivities
  TString evcutycorr = "(ErrorFlag & 0xda7e6bff)==0"; //cut for y sensitivities
  TString evcutbcm = "(ErrorFlag & 0xda7e6bff)==0 && bmwcycnum==1"; //cut to look at one supercycle
  TString evcutx = "(ErrorFlag & 0xda7e6bff)==0 && bmwobj==1 | bmwobj==3 | bmwobj==5";//cut for x modulations
  TString evcuty = "(ErrorFlag & 0xda7e6bff)==0 && bmwobj==2 | bmwobj==4 | bmwobj==6";// cut for y modulations
  TString evcute = "(ErrorFlag & 0xda7e6bff)==0 && bmwobj==8";//cut for energy modulations
  TString bmon = "(ErrorFlag & 0x00009000) !=0"; //flags true if BM is on
  //TString bmon = "bcm_an_us>5 && (ErrorFlag & 0x00001000) !=0"; //flags true if BM is on
  //TString bmon = "(ErrorFlag & 0x00001000) !=0"; //flags true if BM is on
  TString coil[7] = {"bmod_trim1","bmod_trim2","bmod_trim3","bmod_trim4","bmod_trim5","bmod_trim6","bmod_trim7"};

  //get the number of events with beam mod on, no other cuts
  int bmnum = tree_R->Draw("CodaEventNumber", bmon, "goff");
  //if no beam mod, draw canvas with error message
  if(bmnum<10) {
    TPad *cBMWPlot2 = new TPad("cBMWPlot2","cBMWPlot2",0,0,1,1);
    cBMWPlot2->Divide(1,3);
    cBMWPlot2->Draw();

    TLatex line1;
    TLatex line2;
    TLatex line3;
    line1.SetTextSize(.2);
    line1.SetTextAlign(12);
    line2.SetTextSize(.2);
    line2.SetTextAlign(12);
    line3.SetTextSize(.2);
    line3.SetTextAlign(12);

    cBMWPlot2->cd(1);
    line1.DrawLatex(.2,.5,"No Beam Mod");

    cBMWPlot2->cd(2);
    line2.DrawLatex(.2,.5,"this run");

    cBMWPlot2->cd(3);
    line3.DrawLatex(.2,.5,":(");
    
    
  }
  else{

    TPad *cBMWPlot2 = new TPad("cBMWPlot2", "cBMWPlot2",0,0,1,1);
    cBMWPlot2->Divide(1,1);
//    cBMWPlot2->Divide(1,3);
    cBMWPlot2->Draw();

    TGraph *tGraphBMWPlot3[8]; //plots trim cards for the whole run
    for (int i=0;i<8; i++){
      if(i<7){
        int npt = tree_R->Draw(Form("%s:CodaEventNumber>>hc%d",coil[i].Data(),i),evcut,"goff");
        tGraphBMWPlot3[i] = new TGraph(npt,tree_R->GetV2(),tree_R->GetV1());
        tGraphBMWPlot3[i]->SetLineStyle(1);
        tGraphBMWPlot3[i]->SetLineColor(i+1);
        tGraphBMWPlot3[i]->SetTitle("Trim Cards vs CodaEventNumber");
        tGraphBMWPlot3[i]->GetXaxis()->SetTitle("CodaEventNumber");
        tGraphBMWPlot3[i]->GetYaxis()->SetTitle("Trim Cards");
      }
      else{
        int npt = tree_R->Draw("bcm_an_us*150:CodaEventNumber","","goff");
        tGraphBMWPlot3[i] = new TGraph(npt,tree_R->GetV2(),tree_R->GetV1());
        tGraphBMWPlot3[i]->SetLineStyle(1);
        tGraphBMWPlot3[i]->SetLineColor(i+1);
        tGraphBMWPlot3[i]->SetTitle("Trim Cards vs CodaEventNumber");
        tGraphBMWPlot3[i]->GetXaxis()->SetTitle("CodaEventNumber");
        tGraphBMWPlot3[i]->GetYaxis()->SetTitle("Trim Cards");
      }
    }
  
    cBMWPlot2->cd(1);
    tGraphBMWPlot3[0]->Draw("AL");
  
    for(int i=1;i<7;i++){
      tGraphBMWPlot3[i]->Draw("same");
    }

    auto legend2 = new TLegend(0.1,0.6,0.2,0.9);
    legend2->SetHeader("Trim Cards","C");
    legend2->SetNColumns(2);
    legend2->AddEntry(tGraphBMWPlot3[0],"Trim1","l");
    legend2->AddEntry(tGraphBMWPlot3[1],"Trim2","l");
    legend2->AddEntry(tGraphBMWPlot3[2],"Trim3","l");
    legend2->AddEntry(tGraphBMWPlot3[3],"Trim4","l");
    legend2->AddEntry(tGraphBMWPlot3[4],"Trim5","l");
    legend2->AddEntry(tGraphBMWPlot3[5],"Trim6","l");
    legend2->AddEntry(tGraphBMWPlot3[6],"Trim7","l");
    legend2->AddEntry(tGraphBMWPlot3[7],"Beam Current","l");
    legend2->Draw();

    cBMWPlot2->cd(2);
    //    tree_M->Draw("(diff_bpm4eX-diff_bpm4aX):(diff_bpm4aX+diff_bpm4eX)","yield_bmwobj<0");
    tree_M->Draw("0.18*diff_bpm4eX-diff_bpm4aX:diff_bpm4aX+0.18*diff_bpm4eX","ErrorFlag==0 && diff_bpm4aX>-5 && diff_bpm4aX<5 && diff_bpm4eX>-5 && diff_bpm4eX<5","goff");

    cBMWPlot2->cd(3);
    //    tree_M->Draw("(diff_bpm4eY-diff_bpm4aY):(diff_bpm4aY+diff_bpm4eY)","yield_bmwobj<0");
    tree_M->Draw("diff_bpm4eY-0.65*diff_bpm4aY:0.65*diff_bpm4aY+diff_bpm4eY","ErrorFlag==0 && diff_bpm4eY>-5 && diff_bpm4eY<5 && diff_bpm4aY>-5 && diff_bpm4aY<5","goff");
    
  }

}
