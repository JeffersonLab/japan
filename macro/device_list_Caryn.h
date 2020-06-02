#ifndef _DEVICE_LIST_H_
#define _DEVICE_LIST_H_

// File IO control variable
TString path;
TString prefix;
TString file_name;
TString output_path;
TString plot_title;
Int_t run_number;

//  Beamline Monitors

vector<const char*> vBCM={"bcm_an_us", "bcm_an_ds", 
			  "bcm_an_ds3","unser", 
			  "bcm_dg_us","bcm_dg_ds"};

vector<const char*> vBPM={"bpm4a","bpm4e","bpm4ac","bpm4ec","bpm1","bpm12","bpm14","bpm8"};

vector<const char*> vBPMXY={"bpm4aX","bpm4aY",
			    "bpm4eX","bpm4eY",
			    "bpm14X","bpm14Y",
			    "bpm18X","bpm18Y"};

vector<const char*> vDitherBPM={"bpm4aX","bpm4aY",
				"bpm4eX","bpm4eY",
				"bpm14X"};

vector<const char*> vCavBPM={"cav4b","cav4c","cav4d"};

vector<const char*> vSAM={"sam2","sam4","sam6","sam8"};

//vector<TString> stp_root={"sam2","sam4","sam6","sam8"};  // This won't work

vector<const char*> vBlkM = {"asym_bcm_an_ds3","asym_sam2","asym_sam4"};  // Block Monitors


// Postpan Related Variable

vector<const char*> IV = {"diff_bpm4aX","diff_bpm4aY",
			  "diff_bpm4eX","diff_bpm4eY", 
			  "diff_bpm14X"};

vector<const char*> DV_reg={"reg_asym_sam2","reg_asym_sam4",
			    "reg_asym_sam6","reg_asym_sam8"};

vector<const char*> DV_raw={"asym_sam2","asym_sam4",
			    "asym_sam6","asym_sam8"};
//Injector Plots
//make matrix to hold data points, injector daq can only hold 22 bpms max. 
vector<const char*> vInjBPM = {"bpm2i01","bpm2i02","bpm1i02","bpm1i04",
			       "bpm1i06", "bpm0i01","bpm0i01a","bpm0i02",
			       "bpm0i02a","bpm0i05","bpm0i07","bpm0l01",
			       "bpm0l02","bpm0l03","bpm0l04","bpm0l05",
			       "bpm0l06","bpm0l07","bpm0l08","bpm0l09",
			       "bpm0l10", "bpm0r03", "bpm0r05"};
  
/* vector<const char*> detnams = {"bpm2i01","bpm2i02","bpm1i02","bpm1i04", */
/* 			 "bpm1i06","bpm0i01","bpm0i01a","bpm0i02", */
/* 			 "bpm0i02a","bpm0i05", "bpm0i07","bpm0l01", */
/* 			 "bpm0l02","bpm0l03","bpm0l04","bpm0l05", */
/* 			 "bpm0l06","bpm0l07","bpm0l08","bpm0l09", */
/* 			 "bpm0l10","bpm0r03","bpm0r05"}; */


#endif

