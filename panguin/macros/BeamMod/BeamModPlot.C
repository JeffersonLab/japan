/////////BeamModPlot.C
///// ---  Victoria Owen ----
/////---- Quinn Campgana ----
//// ----- June 2019 --------
///Panguin gui compatible diagnostic plots for shift taking online replay 
/// Three Pads
//Pad one: trim card readback vs. time for full run (up to the point of replay)
//Pad two: X bpm correlations (strong correlations = bad)
//Pad three: Y bpm correlations (strong correlations = bad)



void BeamModPlot(TString type="evt",TString type2="mul", TString ref="CodaEventNumber", Bool_t ldebug = kFALSE){
  gStyle->SetOptStat(0);
  TTree* tree_R = (TTree*)gDirectory->Get(type);
  TTree* tree_M = (TTree*)gDirectory->Get(type2);

  if (tree_R->GetEntries("bmwcycnum>0")<=0){
    if (ldebug) std::cerr << "No entries with 'bmwcycnum>0' in this run."
			  <<std::endl;
    return;
  }
  if (tree_R->GetEntries("bmwobj>0")<=0){
    if (ldebug) std::cerr << "No entries with 'bmwobj>0' in this run."
			  <<std::endl;
    return;
  }

   tree_R->Draw(">>elist","bmwcycnum>0","entrylist");  //picks out unique cycle numbers
  TEntryList *elist = (TEntryList*)gROOT->FindObject("elist");
  tree_R->SetEntryList(elist);
  TLeaf *l_bmwcycnum = tree_R->GetLeaf("bmwcycnum");
  int nonzero = tree_R->Draw("bmwcycnum","bmwcycnum>0","goff");
  vector<Double_t> cycles;
  for(int i=0;i<nonzero;i++){
    l_bmwcycnum->GetBranch()->GetEntry(elist->GetEntry(i));
    Double_t cyclenum = l_bmwcycnum->GetValue();
    if(i==0){
      cycles.push_back(cyclenum);
    }
    else{
      int sizeVec = cycles.size();
      if(cyclenum != cycles[sizeVec-1]){
	cycles.push_back(cyclenum);
      }
    }
  }
  
  int Ncycles = cycles.size();

  TString bmwcut = "bmwcycnum>0";
  TString evcut = "(ErrorFlag & 0x7bfe6fff)==0"; //basic cut, all events with beam on
  TString evcutxcorr = "(ErrorFlag & 0x7bfe6fff)==0"; //cut for x sensitivities
  TString evcutycorr = "(ErrorFlag & 0x7bfe6fff)==0"; //cut for y sensitivities
  TString evcutbcm = "(ErrorFlag & 0x7bfe6fff)==0 && bmwcycnum==1"; //cut to look at one supercycle
  TString evcutx = "(ErrorFlag & 0x7bfe6fff)==0 && bmwobj==1 | bmwobj==3 | bmwobj==5";//cut for x modulations
  TString evcuty = "(ErrorFlag & 0x7bfe6fff)==0 && bmwobj==2 | bmwobj==4 | bmwobj==6";// cut for y modulations
  TString evcute = "(ErrorFlag & 0x7bfe6fff)==0 && bmwobj==8";//cut for energy modulations

  TString coil[7] = {"bmod_trim1","bmod_trim2","bmod_trim3","bmod_trim4","bmod_trim5","bmod_trim6","bmod_trim7"};

  /*if(Ncycles==0){
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
  else{*/
    TPad *cBMWPlot2 = new TPad("cBMWPlot2","cBMWPlot2",0,0,1,1);
    cBMWPlot2->Divide(1,3);
    cBMWPlot2->Draw();

    TGraph *tGraphBMWPlot3[8];  //plots trim cards for the whole run
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
	int npt = tree_R->Draw("bcm_an_ds3*150:CodaEventNumber","","goff");
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
    for(int i=1;i<8;i++){
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
    tree_M->Draw("(diff_bpm4eX-diff_bpm4aX):(diff_bpm4aX+diff_bpm4eX)","yield_bmwobj<0");
   
    cBMWPlot2->cd(3);
    tree_M->Draw("(diff_bpm4eY-diff_bpm4aY):(diff_bpm4aY+diff_bpm4eY)","yield_bmwobj<0");
    // }

  return 0;
}
