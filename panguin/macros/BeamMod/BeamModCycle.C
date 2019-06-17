/////////BeamModCycle.C
///// ---  Victoria Owen ----
/////---- Quinn Campgana ----
//// ----- June 2019 --------
///Panguin gui compatible diagnostic plots for shift taking online replay 
// 'zoomed in' on a single cyle
// examining each BPM response vs time. 
//color coded to quickly see X, Y , E modulation pattern
//if trim card readback for single cycle is good, bpm response should be good


  void BeamModCycle(TString type="evt", TString ref="CodaEventNumber"){
  gStyle->SetOptStat(0);
  TTree* tree = (TTree*)gDirectory->Get(type);


  TString bmwcut = "bmwcycnum>0";
  TString evcut = "ErrorFlag==0"; //basic cut, all events with beam on
  TString evcutxcorr = "ErrorFlag==0 && bpm4aX>2"; //cut for x sensitivities
  TString evcutycorr = "ErrorFlag==0"; //cut for y sensitivities
  TString evcutbcm = "ErrorFlag==0 && bmwcycnum==23"; //cut to look at one supercycle
  TString evcutx = "ErrorFlag==0 && bmwobj==1 | bmwobj==3 | bmwobj==6";//cut for x modulations
  TString evcuty = "ErrorFlag==0 && bmwobj==2 | bmwobj==4 | bmwobj==7";// cut for y modulations
  TString evcute = "ErrorFlag==0 && bmwobj==8";//cut for energy modulations

  TPad *cBMWPlot = new TPad("cBMWPlot","cBMWPlot",1000,1000);
  cBMWPlot->Divide(2,3);
  cBMWPlot->Draw();
  TString coil[7] = {"bmod_trim1","bmod_trim2","bmod_trim3","bmod_trim4","bmod_trim6","bmod_trim7","bmod_trim8"};

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

  TString bpms[5]={"bpm4aX","bpm4aY","bpm4eX","bpm4eY", "bpm12X"};
  
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

  // TPaveText *message1 = new TPaveText(.05,.1,.95,.8);
  //message1->AddText("Run Number: 1474");
  //message1->AddText("Cycle Number: 23");
  //message1->Print();


  return 0;
}
