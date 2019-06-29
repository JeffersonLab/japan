/////////CalculateQuality.C
///// ---  Victoria Owen ----
/////---- Quinn Campgana ----
/////---- Cameron Clarke ---- 
//// ----- June 2019 --------
///Panguin gui compatible diagnostic plots for shift taking online replay 


void CalculateQuality(TString tree="evt",TString tree2="mul", Bool_t ldebug = kFALSE){
  gStyle->SetOptStat(0);
  TTree* tree_Evt = (TTree*)gDirectory->Get(tree);
  TTree* tree_Mul = (TTree*)gDirectory->Get(tree2);

  /*
   * Want to know: 
   * Total Number of events in this run
   * Total Number of multiplets in this run
   *
   * Total number of multiplets that pass eventcuts
          Just use the full ErrorFlag variable, != 0 => failed
   *
   * Kinds of event cut failures:
   * Total beam related failures:
   *
   * Hardware channel types of cuts:
   *
   * Total number failing BCM type cuts (aka beam trip)
   *  evt->Draw("bcm_an_ds3","(ErrorFlag&0x8000000)!=0") // Denotes a global EventRing level failure (Beam trip)
   *  evt->Draw("bcm_an_ds3","(ErrorFlag&0x1000000)!=0") // Denotes a global EventRing level failure (Stability Cut)
   *  evt->Draw("bcm_an_ds3","(ErrorFlag&0x100)!=0")     // Denotes a single BCM failed, not yet implemented
   *
   * Total number failing BPM saturation cuts
   *  evt->Draw("bpm12XP.hw_sum","(ErrorFlag&0x400)==0") // Counts BPM saturation cut fails
   *  evt->Draw("bpm12XP.hw_sum","bpm12XP.hw_sum>50000") // While BPM saturation cut has not been applied
   * Total number failing Detector saturation cuts
   *  evt->Draw("usr.hw_sum","(usr.Device_Error_Code&0x01)!=0")  // Counts VQWK saturations failures
   *  evt->Draw("usr.hw_sum","(usr.Device_Error_Code&0x800)!=0") // Counts PMT saturation failures
   *
   * Total number of missed helicity windows
      evt->Draw("ch_clock_4mhz/4e3","ch_clock_4mhz<1000") // Comes from looking at CH integration clock and counting N<~3000 counts (==0) = missed trigger - in mul tree would be a missed pattern

  */

  // Example code ->
  /*

  if (tree_Evt->GetEntries("ErrorFlag>0")>0){
    if (ldebug) std::cerr << "We have events :) "
      <<std::endl;
    return;
  }
  if (tree_Evt->GetEntries("ErrorFlag==0")<=0){
    if (ldebug) std::cerr << "No events :( "
      <<std::endl;
    return;
  }

  tree_Evt->Draw(">>elist","bmwcycnum>0","entrylist");  //picks out unique cycle numbers
  TEntryList *elist = (TEntryList*)gROOT->FindObject("elist");
  tree_Evt->SetEntryList(elist);
  TLeaf *l_bmwcycnum = tree_Evt->GetLeaf("bmwcycnum");
  int nonzero = tree_Evt->Draw("bmwcycnum","bmwcycnum>0","goff");
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
  TString evcut = "ErrorFlag && 0xffff5fff"; //basic cut, all events with beam on
  TString evcutxcorr = "ErrorFlag && 0xffff5fff && bpm4aX>2"; //cut for x sensitivities
  TString evcutycorr = "ErrorFlag && 0xffff5fff"; //cut for y sensitivities
  TString evcutbcm = "ErrorFlag && 0xffff5fff && bmwcycnum==1"; //cut to look at one supercycle
  TString evcutx = "ErrorFlag && 0xffff5fff && bmwobj==1 | bmwobj==3 | bmwobj==5";//cut for x modulations
  TString evcuty = "ErrorFlag && 0xffff5fff && bmwobj==2 | bmwobj==4 | bmwobj==6";// cut for y modulations
  TString evcute = "ErrorFlag && 0xffff5fff && bmwobj==8";//cut for energy modulations

  TString coil[7] = {"bmod_trim1","bmod_trim2","bmod_trim3","bmod_trim4","bmod_trim5","bmod_trim6","bmod_trim7"};

  TPad *cBMWPlot2 = new TPad("cBMWPlot2","cBMWPlot2",0,0,1,1);
  cBMWPlot2->Divide(1,3);
  cBMWPlot2->Draw();

  TGraph *tGraphBMWPlot3[8];  //plots trim cards for the whole run
  for (int i=0;i<8; i++){
    if(i<7){
      int npt = tree_Evt->Draw(Form("%s:CodaEventNumber>>hc%d",coil[i].Data(),i),evcut,"goff");
      tGraphBMWPlot3[i] = new TGraph(npt,tree_Evt->GetV2(),tree_Evt->GetV1());
      tGraphBMWPlot3[i]->SetLineStyle(1);
      tGraphBMWPlot3[i]->SetLineColor(i+1);
      tGraphBMWPlot3[i]->SetTitle("Trim Cards vs CodaEventNumber");
      tGraphBMWPlot3[i]->GetXaxis()->SetTitle("CodaEventNumber");
      tGraphBMWPlot3[i]->GetYaxis()->SetTitle("Trim Cards");
    }
    else{
      int npt = tree_Evt->Draw("bcm_an_ds3*150:CodaEventNumber","","goff");
      tGraphBMWPlot3[i] = new TGraph(npt,tree_Evt->GetV2(),tree_Evt->GetV1());
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
  tree_Mul->Draw("(diff_bpm4eX-diff_bpm4aX):(diff_bpm4aX+diff_bpm4eX)","yield_bmwobj<0");

  cBMWPlot2->cd(3);
  tree_Mul->Draw("(diff_bpm4eY-diff_bpm4aY):(diff_bpm4aY+diff_bpm4eY)","yield_bmwobj<0");
  // }
  */
  return 0;
  }
