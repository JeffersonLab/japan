////////BeamModCycle.C
///// ---  Victoria Owen ----
/////---- Quinn Campgana ----
/////----Carrington Metts-----
//// ----- June-July 2019 --------
///Panguin gui compatible diagnostic plots for shift taking online replay 
// 'zoomed in' on a single cyle
// examining each BPM response vs time. 
//color coded to quickly see X, Y , E modulation pattern
//if trim card readback for single cycle is good, bpm response should be good

void CycleTest(TString type="evt", TString ref="CodaEventNumber", Bool_t ldebug = kFALSE) {
  gStyle->SetOptStat(0);
  TTree* tree_R = (TTree*)gDirectory->Get(type);

  //test if beam mod is on, print message and abort if it is off 
  //this must happen first, otherwise it´ll break
  TString bmon = "(ErrorFlag & 0x00009000) != 0"; //flags true if BM is on
  int bmnum = tree_R->Draw("CodaEventNumber", bmon, "goff"); //number of events with BM on
  if(bmnum<10) {
    TPad *cBMWPlot = new TPad("cBMWPlot","cBMWPlot",0,0,1,1);
    cBMWPlot->Divide(1,3);
    cBMWPlot->Draw();
    TLatex line1;
    TLatex line2;
    TLatex line3;
    line1.SetTextSize(.2);
    line1.SetTextAlign(12);
    line2.SetTextSize(.2);
    line2.SetTextAlign(12);
    line3.SetTextSize(.2);
    line3.SetTextAlign(12);

    cBMWPlot->cd(1);
    line1.DrawLatex(.2,.5,"No Beam Mod");

    cBMWPlot->cd(2);
    line2.DrawLatex(.2,.5,"this run");

    cBMWPlot->cd(3);
    line3.DrawLatex(.2,.5,":(");
    return 0;
  }
  
  tree_R->Draw(">>elist", "bmwcycnum>0","entrylist"); //picks out unique cycle numbers 
  TEntryList *elist = (TEntryList*)gDirectory->Get("elist");
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
  TString cyclechoice = Form("%f",cycles[1]);
  int Ncycles = cycles.size();
  

  TString bmwcut = "bmwcycnum>0";
  TString evcut = "(ErrorFlag & 0x7bfe6fff)==0"; //basic cut, all events with beam on
  TString evcutxcorr = "(ErrorFlag & 0x7bfe6fff)==0"; //cut for x sensitivities
  TString evcutycorr = "(ErrorFlag & 0x7bfe6fff)==0"; //cut for y sensitivities
  TString evcutbcm = "(ErrorFlag & 0x7bfe6fff)==0 && bmwcycnum==" + cyclechoice; //cut to look at one supercycle
  TString evcutx = "(ErrorFlag & 0x7bfe6fff)==0 && bmwobj==1 | bmwobj==3 | bmwobj==5";//cut for x modulations
  TString evcuty = "(ErrorFlag & 0x7bfe6fff)==0 && bmwobj==2 | bmwobj==4 | bmwobj==6";// cut for y modulations
  TString evcute = "(ErrorFlag & 0x7bfe6fff)==0 && bmwobj==7";//cut for energy modulations

  TString coil[7] = {"bmod_trim1","bmod_trim2","bmod_trim3","bmod_trim4","bmod_trim5","bmod_trim6","bmod_trim7"};
  TString bpms[5]={"bpm4aX","bpm4aY","bpm4eX","bpm4eY", "bpm12X"};

  
  TPad *cBMWPlot = new TPad("cBMWPlot","cBMWPlot",0,0,1,1);
  cBMWPlot->Divide(2,3);
  cBMWPlot->Draw();

  TGraph *tGraphBMWPlot[7];  //plots one super cycle
  for (int i=0;i<7; i++){
    int npt = tree_R->Draw(Form("%s:CodaEventNumber>>hc%d",coil[i].Data(),i),evcutbcm,"goff");
    tGraphBMWPlot[i]= new TGraph(npt,tree_R->GetV2(),tree_R->GetV1());
    tGraphBMWPlot[i]->SetLineStyle(1);
    if(i==6){
      tGraphBMWPlot[i]->SetLineColor(8);
    }
    else{
      if(i==0||i==2||i==4){
	tGraphBMWPlot[i]->SetLineColor(4);
      }
      else{
	if(i==1||i==3||i==5){
	  tGraphBMWPlot[i]->SetLineColor(2);
	}
      }
    }
    tGraphBMWPlot[i]->SetTitle("Trim Cards vs CodaEventNumber");
    tGraphBMWPlot[i]->GetXaxis()->SetTitle("CodaEventNumber");
    tGraphBMWPlot[i]->GetYaxis()->SetTitle("Trim Cards");
  }
  TGraph *tGraphBMWPlot2[5]; //plots bpms vs CodaEventNumber
  for(int i=0;i<5;i++){
    cBMWPlot->cd(i+1);
    int npt = tree_R->Draw(Form("%s:CodaEventNumber>>h%d",bpms[i].Data(),i),
			evcutbcm,"goff");
    tGraphBMWPlot2[i] = new TGraph(npt,tree_R->GetV2(),tree_R->GetV1());
    tGraphBMWPlot2[i]->SetLineStyle(3);
    tGraphBMWPlot2[i]->SetLineColor(1);
    tGraphBMWPlot2[i]->Draw("Ap");
    tGraphBMWPlot2[i]->SetTitle(Form(" %s",bpms[i].Data() ));
    tGraphBMWPlot2[i]->GetYaxis()->SetTitle(Form(" %s",bpms[i].Data()));
    tGraphBMWPlot2[i]->GetXaxis()->SetTitle("CodaEventNumber");
  }
  for(int i=0;i<5;i++){ //color codes y modulations
    cBMWPlot->cd(i+1);
    int npt = tree_R->Draw(Form("%s:CodaEventNumber>>h%d",bpms[i].Data(),i),
			     evcuty,"goff");
    tGraphBMWPlot2[i] = new TGraph(npt,tree_R->GetV2(),tree_R->GetV1());
    tGraphBMWPlot2[i]->SetLineStyle(3);
    tGraphBMWPlot2[i]->SetLineColor(2);
    tGraphBMWPlot2[i]->SetLineWidth(1);
    tGraphBMWPlot2[i]->Draw("SAME");
  }

  for(int i=0;i<5;i++){ //color codes x modulations
    cBMWPlot->cd(i+1);
    int npt = tree_R->Draw(Form("%s:CodaEventNumber>>h%d",bpms[i].Data(),i),
			     evcutx,"goff");
    tGraphBMWPlot2[i] = new TGraph(npt,tree_R->GetV2(),tree_R->GetV1());
    tGraphBMWPlot2[i]->SetLineStyle(3);
    tGraphBMWPlot2[i]->SetLineColor(4);
    tGraphBMWPlot2[i]->SetLineWidth(1);
    tGraphBMWPlot2[i]->Draw("SAME");
  }
  for(int i=0;i<5;i++){ //color codes energy modulations
    cBMWPlot->cd(i+1);
    int npt = tree_R->Draw(Form("%s:CodaEventNumber>>h%d",bpms[i].Data(),i),
			     evcute,"goff");
    tGraphBMWPlot2[i] = new TGraph(npt,tree_R->GetV2(),tree_R->GetV1());
    tGraphBMWPlot2[i]->SetLineStyle(3);
    tGraphBMWPlot2[i]->SetLineColor(8);
    tGraphBMWPlot2[i]->SetLineWidth(1);
    tGraphBMWPlot2[i]->Draw("SAME");
  }

  cBMWPlot->cd(6);
  tGraphBMWPlot[0]->Draw("AL");
  
  for(int i=1;i<7;i++){
    tGraphBMWPlot[i]->Draw("same");
  }
  
  auto legend = new TLegend(0.1,0.7,0.4,0.9);
  legend->SetHeader("Trim Cards","C");
  legend->SetNColumns(2);
  legend->AddEntry(tGraphBMWPlot[0],"X mod","l");
  legend->AddEntry(tGraphBMWPlot[1],"Y mod","l");
  legend->AddEntry(tGraphBMWPlot[6],"E mod","l");
  legend->Draw();
   
}
