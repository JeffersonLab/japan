/*
Updated based on Tao and Catherine's DitAlias 

-- Example  from shell terminal:
$  root -b 'CorrectTree(3433)'

--with specific segment file 
$  root CorrectTree(3433,1)

Output Directory: 
                 ./DitOutputs/prexPrompt_dither_3454_000.root
 */

#include "QuerySlugNumber.C"

void CorrectTree(Int_t run_number=0, Double_t mix1 = 1.0, Double_t mix2 = 0.4, Int_t coils = 13764, Int_t seg_number=0 ){
  // Getting Slug Number from RCDB
  Int_t slug_id = QuerySlugNumber(run_number);
  if(slug_id==-1){
    cout << " -- Slug Number not Found " << endl;
    cout << " -- Correction Aborted " << endl;
    return;
  }

  //  Get Slopes From Dit slope Files
  vector<TString> vDet={"asym_usl",
			"asym_usr",
			"asym_dsl",
			"asym_dsr",
			"asym_atl1",
			"asym_atl2",
			"asym_atr1",
			"asym_atr2"};

  vector<TString> vMon={"diff_bpm4aX",
			"diff_bpm4aY",
			"diff_bpm4eX",
			"diff_bpm4eY",
			"diff_bpm11X",
			"diff_bpm12X"};
  TString rootfile_name = Form("~/PREX/prompt/japanOutput/prexPrompt_pass1_%d.%03d.root",
  			       run_number,seg_number);
  TFile *japanOutput = TFile::Open(rootfile_name);
  /*TTree *evt_tree = (TTree*)japanOutput->Get("evt");
  Int_t n_cyc_entries = evt->Draw("bmwcycnum","bmwcycnum>0","goff");
  TH1 *bmwcycnum_hist = (TH1*)gROOT->FindObject("htemp");
  std::vector<Double_t> * cycnumbers = {};
  for (Int_t nBin = 0; nBin < bmwcycnum_hist->GetNBins() ; nBin++){
    if (bmwcycnum_hist->GetBinContent(nBin)>0){
      cycnumbers->push_back(bmwcycnum_hist->GetBinCenter(nBin));
    }
  }
  Int_t starting_cycnum = 0;
  if (cycnumbers.size()>0){
    starting_cycnum = cycnumbers.at(0);
  }
  else { 
    Printf("No good cycles this run, ERROR (for now)");
    return; 
  }*/
  // Want to loop over the cycle numbers and do correction of a run using these slopes
  // This entails collecting the cyclenum based slope parameters and then applying them to any event that comes before/after that cycle number
  //    Start with the first cycle number of the run and change to the following cycle number once the cyclenumber in Japan file increases to the next in the vector of cycle numbers
  //    For now, if no available slopes just error out and leave, later try to get the one before and one after that cycle number and use the average (if available in slope root file)


  TTree *mul_tree = (TTree*)japanOutput->Get("mul");

  if(mul_tree==NULL){
    cout << "Error: " 
   << "mul tree doesn't exist!" << endl;
    return;
  }

  TFile* ditherOutput = TFile::Open(Form("./slopes/dithering_slopes_%d_slug%d.root",
  					 coils,slug_id));
  TTree *slope_tree = (TTree*)ditherOutput->Get("dit");

  if(slope_tree==NULL){
    cout << "Error: " 
      << "dit slope  tree doesn't exsit!" << endl;
    return;
  }

  // Adapting following part from Catherines' DitAlias.C
    //Create a cut for the blacklisted "bad" runs
  /*FILE *blacklist = fopen("./black.list","r");
  TCut run_cut;
  while(!feof(blacklist)){
    Int_t buff;
    fscanf(blacklist,"%d/n",&buff);
    run_cut += Form("run!=%d",buff);
  }*/

  //Build variable names
  //dit tree variable names go like "dv_iv", for example: "usr_4eX"
  TString maindet_array[8]={"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2"};
  TString bpm_array[6]={"4aX","4aY","4eX","4eY","11X12X","11X12X"};
  const Int_t ndet = sizeof(maindet_array)/sizeof(*maindet_array);
  const Int_t nmon= sizeof(bpm_array)/sizeof(*bpm_array);

  //Array for holding slopes. DV (maindets) are the rows, and IV (bpms) are the columns
  double slopes[ndet][nmon]={0};
  // for(int idet=0;idet<ndet;idet++)
  //   for(int imon=0;imon<nmon;imon++)
  //     slopes[idet][imon]=0.0;

  //Get the slopes from the tree
  // In order 4aX, 4aY, 4eX, 4eY, 12X
  // slope unit: fraction / mm
  // ** which is  1e-3(ppm/um)

  Int_t nCyc = slope_tree->Draw(">>elist1","cyclenum>0");//,run_cut);
  TEventList *elist = (TEventList*)gDirectory->Get("elist1");
  TString varname;

  //For each run number
  for(int ievt = 0; ievt<nCyc; ievt++){
    Int_t index = elist->GetEntry(ievt);
    slope_tree->GetEntry(index);
    //For each detector
    for(int idet = 0; idet<ndet; idet++){
      // cout << maindet_array[idet] << ": ";
      //For each bpm
      for(int imon = 0; imon <nmon; imon++){
        varname = Form("%s_%s",maindet_array[idet].Data(),bpm_array[imon].Data());
        //Get the slope
        slopes[idet][imon] += (slope_tree->GetLeaf(varname)->GetValue()*1.0e-3)/nCyc;
        // cout << slopes[idet][imon] << "\t";
      }
      // cout << "\n";
    }
  }
  cout << " --- Get Slug Averaged Slopes -- " << endl;
  for(int idet = 0; idet<ndet; idet++){
    cout << maindet_array[idet] << " Correction : ";
    for(int imon = 0; imon <nmon; imon++){
      if(imon!=0 && slopes[idet][imon]>0 )
	cout << " + ";
      else
	cout << " ";
      cout << setprecision(5)
	   << 1000*slopes[idet][imon]  // FIXME
	   << "*" 
	   << bpm_array[imon];
    }
    cout << endl;
  }
  ditherOutput->Close();
  //================== Done with slope extraction 

  // Doing Correction
  typedef struct {Double_t hw_sum,
      block0,block1,block2,block3,
      num_samples,
      Device_Error_Code;} JCONT;  // JAPAN TREE CONTAINER
  TString leaf_list="hw_sum/D:block0:block1:block2:block3:num_samples:Device_Error_Code";
  const int nDet = vDet.size();
  const int nMon = vMon.size();
  JCONT DetArray[nDet];
  JCONT MonArray[nMon];
  JCONT DetArray_dit[nDet];

  Double_t fErrorFlag;
  Double_t fCodaEventNumber;

  for(int iDet=0;iDet<nDet;iDet++)
    mul_tree->SetBranchAddress(vDet[iDet],&(DetArray[iDet]));
  for(int iMon=0;iMon<nMon;iMon++)
    mul_tree->SetBranchAddress(vMon[iMon],&(MonArray[iMon]));
  mul_tree->SetBranchAddress("ErrorFlag",&fErrorFlag);
  mul_tree->SetBranchAddress("CodaEventNumber",&fCodaEventNumber);
  TString outputName ;
  Ssiz_t first_t = rootfile_name.Last('_')+1;
  Ssiz_t last_t = rootfile_name.Last('.');
  Ssiz_t length_t = last_t -first_t;
  outputName = rootfile_name(first_t,length_t);
  outputName.ReplaceAll('.','_');
  outputName = "prexPrompt_dither_" +outputName;
  outputName +=".root";
  TString output_path="./DitOutputs/";
  cout << " -- Writing " << output_path+outputName << endl;
  
  TFile *output = TFile::Open(output_path+outputName,"RECREATE");
  TTree *dit_tree = new TTree("dit","BeamMod Correction Tree");
  // A corrected asym 
  for(int iDet=0;iDet<nDet;iDet++)
    dit_tree->Branch("dit_"+vDet[iDet],
  		     &(DetArray_dit[iDet]),
  		     leaf_list);
  // A copy of raw asym 
  for(int iDet=0;iDet<nDet;iDet++)
    dit_tree->Branch(vDet[iDet],
  		     &(DetArray[iDet]),
  		     leaf_list);
  // A copy of diff bpm 
  for(int iMon=0;iMon<nMon;iMon++)
    dit_tree->Branch(vMon[iMon],
  		     &(MonArray[iMon]),
  		     leaf_list);

  dit_tree->Branch("ErrorFlag",&fErrorFlag,"ErrorFlag/D");
  dit_tree->Branch("CodaEventNumber",&fCodaEventNumber,"CodaEventNumber/D");
  
  Int_t nEntries = mul_tree->GetEntries();
  for(int ievt=0;ievt<nEntries;ievt++){
    mul_tree->GetEntry(ievt);

    for(int iDet=0;iDet<nDet;iDet++){
      DetArray_dit[iDet].hw_sum = DetArray[iDet].hw_sum;
      DetArray_dit[iDet].block0 = DetArray[iDet].block0;
      DetArray_dit[iDet].block1 = DetArray[iDet].block1;
      DetArray_dit[iDet].block2 = DetArray[iDet].block2;
      DetArray_dit[iDet].block3 = DetArray[iDet].block3;
      DetArray_dit[iDet].num_samples = DetArray[iDet].num_samples;
      DetArray_dit[iDet].Device_Error_Code = DetArray[iDet].Device_Error_Code;

      Double_t correction = 0.0;
      Double_t scaleFactor = 1.0;
      for(int iMon=0;iMon<nMon;iMon++){
        if (iMon < nMon-2) {
          scaleFactor = 1.0;
        }
        else if (iMon < nMon-1) {
          scaleFactor = mix1;
        }
        else {
          scaleFactor = mix2;
        }
  	DetArray_dit[iDet].hw_sum -= scaleFactor*slopes[iDet][iMon]*MonArray[iMon].hw_sum;
  	DetArray_dit[iDet].block0 -= scaleFactor*slopes[iDet][iMon]*MonArray[iMon].block0;
  	DetArray_dit[iDet].block1 -= scaleFactor*slopes[iDet][iMon]*MonArray[iMon].block1;
  	DetArray_dit[iDet].block2 -= scaleFactor*slopes[iDet][iMon]*MonArray[iMon].block2;
  	DetArray_dit[iDet].block3 -= scaleFactor*slopes[iDet][iMon]*MonArray[iMon].block3;
      }
    }
    dit_tree->Fill();
  }
  
  japanOutput->Close();
  output->cd();
  cout << " -- Writing Dithering Corrected Tree "<< endl;
  dit_tree->Write();
  cout << " -- Closing " << output_path+outputName << endl;
  output->Close();
}

