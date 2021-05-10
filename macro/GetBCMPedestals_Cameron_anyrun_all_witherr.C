void LoadStyle();
void PrintRootFile(Double_t run_num, Double_t n_data, TString ana, TString version, TString type, std::vector<TString> device_names, std::vector<Int_t> validity, Double_t* pedestals, Double_t* pedestal_errors, Double_t* gains);
void GetBCMPedestals_Cameron_anyrun_all_witherr(
    Int_t run_num=6560,
    TString ana="unser",
    TString version="new",
    TString type="orig",
    Double_t user_ped = -1000000,
    Double_t user_norm = -1000000,
    TString user_bcm ="",
    Double_t user_lowlimit=-100.0,//15.0,// for PREX //25.0 for CREX, // FIXME temporarily use 25 uA cutoff// -100.0,
    Double_t user_uplimit=-100.0,
    TCut user_cut=""){
    //TString mybcm = "(bcm_an_us.hw_sum_raw/bcm_an_us.num_samples--636.04)*0.008921",
  LoadStyle();

  char* qwrootfile_path = getenv("QW_ROOTFILES");
  TString rf_name =Form("%s/prexPrompt_pass2_%d.000.root",
      qwrootfile_path,run_num);
  TFile *rootfile = TFile::Open(rf_name);  

  if(rootfile == NULL){
    cout << rf_name 
      << " doesn't exist !!" << endl;
    TString rf_name =Form("%s/prexALL_%d.000.root",
        qwrootfile_path,run_num);
    rootfile = TFile::Open(rf_name);  
    if (rootfile == NULL) {
      cout << rf_name 
        << " doesn't exist !!" << endl;
      TString rf_name =Form("%s/prexPrompt_pass1_%d.000.root",
          qwrootfile_path,run_num);
      rootfile = TFile::Open(rf_name);  
      if (rootfile == NULL) {
        cout << rf_name 
          << " doesn't exist !!" << endl;
        return;
      }
    }
  }

  TString outputDir = "/adaqfs/home/apar/cameronc/analyses/pedestal-calibrations/all-plots";
  TString label;
  TString plot_title;

  TTree *tree= (TTree*)rootfile->Get("evt");
  std::vector<TString> device_name;
  if (ana == "unser"){
    device_name = 
    {"bcm_an_us", "bcm_an_ds",
      "bcm_an_ds3","bcm_an_ds10",
      "bcm_dg_us","bcm_dg_ds",
      "bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y",
      "cav4bQ","cav4cQ","cav4dQ","bcm_dg_usc","bcm_dg_dsc",
      "cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
    if (run_num < 5000.0) { //PREX 
      device_name =
      {"bcm_an_us", "bcm_an_ds",
        "bcm_an_ds3","bcm_an_ds10",
        "bcm_dg_us","bcm_dg_ds",
        "bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y",
        "cav4bQ","cav4cQ","cav4dQ",
        "cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
    }
  }
  if (ana == "all"){
    device_name = 
    {"bcm_an_us", "bcm_an_ds",
      "bcm_an_ds3","bcm_an_ds10",
      "bcm_dg_us","bcm_dg_ds",
      "bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y",
      "cav4bQ","cav4cQ","cav4dQ","bcm_dg_usc","bcm_dg_dsc",
      "cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI",
      "usl","usr","dsl","dsr",
      "atl1","atr1","atl2","atr2",
      "sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8"};
    if (run_num < 5000.0) { //PREX 
      device_name =
      {"bcm_an_us", "bcm_an_ds",
        "bcm_an_ds3","bcm_an_ds10",
        "bcm_dg_us","bcm_dg_ds",
        "bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y",
        "cav4bQ","cav4cQ","cav4dQ",
        "cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI",
        "usl","usr","dsl","dsr",
        "atl1","atr1","atl2","atr2",
        "sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8"};
    }
  }
  std::map<Int_t,std::vector<TString>> device_blacklist;
  device_blacklist[2769] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","cav4bQ","cav4cQ","cav4dQ","cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[2963] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  // Insufficient current ramp, all blacklist
  device_blacklist[2965] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","bcm_an_us","bcm_an_ds","bcm_an_ds3","bcm_an_ds10","bcm_dg_us","bcm_dg_ds","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y","cav4bQ","cav4cQ","cav4dQ","cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  device_blacklist[2967] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y","cav4bQ","cav4cQ","cav4dQ","cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  // On Carbon, so blacklist detectors for sure
  device_blacklist[3098] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[3101] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  // Insufficient current ramp, all blacklist
  device_blacklist[3105] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","bcm_an_us","bcm_an_ds","bcm_an_ds3","bcm_an_ds10","bcm_dg_us","bcm_dg_ds","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y","cav4bQ","cav4cQ","cav4dQ","cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  device_blacklist[3106] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[3135] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  // First Beam on with Maindets in place
  // Run 3138 has a bad BCM scan and sam8, but it is fine for the main detectors 
  device_blacklist[3138] = {"sam8","atl1","atl2","atr1","atr2","bcm_an_us","bcm_an_ds","bcm_an_ds3","bcm_an_ds10","bcm_dg_us","bcm_dg_ds","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y","cav4bQ","cav4cQ","cav4dQ","cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  // Run 3125 has HV changes invalidating prior calibrations for SAMs and Main Det HVs
  // Run 3395 has no target in it
  device_blacklist[3395] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  // Insufficient current ramp, all blacklist
  device_blacklist[3424] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","bcm_an_us","bcm_an_ds","bcm_an_ds3","bcm_an_ds10","bcm_dg_us","bcm_dg_ds","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y","cav4bQ","cav4cQ","cav4dQ","cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  // Insufficient current ramp, all blacklist
  device_blacklist[3429] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","bcm_an_us","bcm_an_ds","bcm_an_ds3","bcm_an_ds10","bcm_dg_us","bcm_dg_ds","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y","cav4bQ","cav4cQ","cav4dQ","cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  device_blacklist[3430] = {"atl1","atl2","atr1","atr2","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[3597] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  // BCM analogs unplugged
  device_blacklist[3645] = {"bcm_an_us","bcm_an_ds","bcm_an_ds3","bcm_an_ds10","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  // BCM analog DS is unfortunately broken during this run, but an_us is fine
  device_blacklist[3735] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","bcm_an_ds","bcm_an_ds3","bcm_an_ds10","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[3749] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[3751] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[4229] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  // Potentially insufficient current ramp
  device_blacklist[4309] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y","cav4bQ","cav4cQ","cav4dQ","cav4bXI","cav4bYI","cav4cXI","cav4cYI","cav4dXI","cav4dYI"};
  device_blacklist[4610] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[4650] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[4652] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[4806] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[4872] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  // Stubby Quartz
  device_blacklist[4901] = {"usl","usr","dsl","dsr","atl1","atl2","atr1","atr2","sam1","sam2","sam3","sam4","sam5","sam6","sam7","sam8","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5360] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5396] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5417] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5434] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5436] = {"cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5474] = {"cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5566] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5576] = {"cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5653] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[5778] = {"cav4bQ","cav4dQ","cav4bXI","cav4bYI","cav4dXI","cav4dYI","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[6049] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[6176] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[6196] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[6233] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
	device_blacklist[6242] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[6442] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[6560] = {"usr","dsr","atr1","atr2","bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[6563] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[6739] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[7121] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[7122] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[7279] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[7282] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[7406] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};
  device_blacklist[7407] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm1X","bpm11X","bpm12X","bpm16X","bpm11Y"};

  if (device_blacklist.count(run_num) != 1 ) {
    Printf("Error, no device blacklist included for pedestal calculations for run %d",run_num);
    return 0;
  }

  const int nbcm = device_name.size();

  // Black List
  std::map<Int_t,std::vector<Int_t>> device_blacknumbers;
  for (Int_t k = 0 ; k<nbcm ; k++ ) {
    Int_t valid = 1;
    if (device_name[k].Contains("bpm")) {
      valid = 2;
    }
    else {
      for (Int_t l = 0 ; l<device_blacklist[run_num].size() ; l++ ) {
        if (device_name[k] == device_blacklist[run_num][l]) {
          valid = 0;
        }
      }
    }
    device_blacknumbers[run_num].push_back(valid);
  }

  //const int nbcm= sizeof(device_name)/sizeof(*device_name);

  TCanvas *c_bcm = new TCanvas("c_bcm","c_bcm",2000,1000);
  c_bcm->Divide(2,1);
  TCanvas *c_all = new TCanvas(Form("c_%s",ana.Data()),Form("c_%s",ana.Data()),2000,1000);
  c_all->Divide(2,1);

  //  TF1 *f_zero = new TF1("f_zero","0",0,170);
  TF1 *f_zero = new TF1("f_zero","[0]",-10,10e3); // x : current
  f_zero->SetLineWidth(1);
  f_zero->SetLineColor(kRed);
  //  f_zero->SetLineStyle(9);

  TString branch_name;
  TString num_samples_name;

  std::map <Int_t,std::vector<TCut>> beam_evtcut;// 
  beam_evtcut[2769] = { 
    "cleandata==1 && scandata2==1 && scandata1>50",
    "cleandata==1 && scandata2==2 && scandata1>50",
    "cleandata==1 && scandata2==3 && scandata1>50",
    "cleandata==1 && scandata2==4 && scandata1>50",
    "cleandata==1 && scandata2==5 && scandata1>50",
    "cleandata==1 && scandata2==6 && scandata1>50",
    "cleandata==1 && scandata2==7 && scandata1>50",
    "cleandata==1 && scandata2==8 && scandata1>50"
  };
  beam_evtcut[2963] = {
    "CodaEventNumber<17e3",//50
    "CodaEventNumber>40e3&&CodaEventNumber<55e3",//50
    "CodaEventNumber>84e3&&CodaEventNumber<90e3",//40
    "CodaEventNumber>95e3&&CodaEventNumber<97e3",//40
    "CodaEventNumber>104e3&&CodaEventNumber<114e3",//40
    "CodaEventNumber>132e3&&CodaEventNumber<134e3",//30
    "CodaEventNumber>140e3&&CodaEventNumber<141e3",//30
    "CodaEventNumber>147e3&&CodaEventNumber<149e3",//30
    "CodaEventNumber>170e3&&CodaEventNumber<176e3"//20
  };
  beam_evtcut[2965] = {
    "cleandata==1&&scandata1==524&&scandata2!=0",
    "cleandata==1&&scandata1==451&&scandata2!=0"
  };
  beam_evtcut[2967] = {
    "Entry$>22.93e3 && Entry$<23.23e3",
    "Entry$>27.1e3 && Entry$<28.4e3",
    "Entry$>36.2e3 && Entry$<41e3",
    "Entry$>53e3 && Entry$<53.33e3",
    "Entry$>33.22e3 && Entry$<33.55e3",
    "Entry$>45.77e3 && Entry$<46.11e3"
  };
  beam_evtcut[3098] = {
     "Entry$>60e3 && Entry$<68e3", //65uA
     "scandata2==3 && cleandata", //60
     "scandata2==4 && cleandata && Entry$<292e3", //55
     "scandata2==5 && cleandata", //50
     "scandata2==6 && cleandata && Entry$<336.1e3",//40uA
     "scandata2==7 && cleandata", //30uA
     "scandata2==8 && cleandata && Entry$<402e3", //20
     "Entry$>232e3 && Entry$<240e3"
  };
  // 17 uA point has very large error bar
  beam_evtcut[3101] = {
    "Entry$>16.3e3 && Entry$<17.3e3",
    "Entry$>32.4e3 && Entry$<37.2e3",
    "Entry$>45.17e3 && Entry$<45.56e3",
    "Entry$>55e3 && Entry$<79e3",
    "Entry$>81e3 && Entry$<100.5e3",
    "Entry$>102e3 && Entry$<123.5e3",
    "Entry$>126.6e3 && Entry$<140e3"
  };
  beam_evtcut[3105] = {
    "CodaEventNumber>10e3&&CodaEventNumber<33e3",  
    "CodaEventNumber>45e3&&CodaEventNumber<52e3",
    "CodaEventNumber>58e3&&CodaEventNumber<72e3",
    "CodaEventNumber>91e3&&CodaEventNumber<96e3"
  };
  beam_evtcut[3106] = {
    "scandata2!=0&&cleandata&&scandata1==495",
    "scandata2!=0&&cleandata&&scandata1==453",
    "scandata2!=0&&cleandata&&scandata1==399",
    "scandata2!=0&&cleandata&&scandata1==302",
    "scandata2!=0&&cleandata&&scandata1==203"
  };
  beam_evtcut[3138] = {
    "CodaEventNumber<9.4e3",
    "CodaEventNumber>16.69e3&&CodaEventNumber<16.75e3",
    "CodaEventNumber>21.87e3&&CodaEventNumber<21.98e3",
    "CodaEventNumber>60e3&&CodaEventNumber<69.5e3",
    "CodaEventNumber>79e3&&CodaEventNumber<95e3"
  };
  if (ana == "all") {
    beam_evtcut[3138] = {
      "scandata2!=0&&cleandata&&scandata1==501",
      "scandata2!=0&&cleandata&&scandata1==453",
      "scandata2!=0&&cleandata&&scandata1==400",
      "scandata2!=0&&cleandata&&scandata1==351",
      "scandata2!=0&&cleandata&&scandata1==304",
      "scandata2!=0&&cleandata&&scandata1==248",
      "scandata2!=0&&cleandata&&scandata1==197" // No 20 uA point for the HALOG entry for some reason
    };
  }
  beam_evtcut[3135] = {
    "CodaEventNumber>46e3&&CodaEventNumber<47e3",
    "CodaEventNumber>73.5e3&&CodaEventNumber<75e3",
    "CodaEventNumber>98.5e3&&CodaEventNumber<100e3",
    "CodaEventNumber>103.62e3&&CodaEventNumber<103.68e3",
    "CodaEventNumber>109.15e3&&CodaEventNumber<109.44e3"
  };
  beam_evtcut[3395] = { 
    "cleandata==1 && scandata2==1 && scandata1>50",
    "cleandata==1 && scandata2==2 && scandata1>50",
    "cleandata==1 && scandata2==3 && scandata1>50",
    "cleandata==1 && scandata2==4 && scandata1>50",
    "cleandata==1 && scandata2==5 && scandata1>50",
    "cleandata==1 && scandata2==6 && scandata1>50",
    "cleandata==1 && scandata2==7 && scandata1>50",
    "cleandata==1 && scandata2==8 && scandata1>50",
    "cleandata==1 && scandata2==9 && scandata1>50",
    "cleandata==1 && scandata2==10 && scandata1>50",
    "cleandata==1 && scandata2==11 && scandata1>50"
  };
  beam_evtcut[3424] = { 
    "cleandata==1 && scandata2==1 && scandata1>0 && CodaEventNumber>37000",
    "cleandata==1 && scandata2==2 && scandata1>0",
    "cleandata==1 && scandata2==3 && scandata1>0",
    "cleandata==1 && scandata2==4 && scandata1>0",
    "cleandata==1 && scandata2==5 && scandata1>0",
  };
  beam_evtcut[3429] = { 
    "cleandata==1 && scandata2==1 && scandata1>0",
    "cleandata==1 && scandata2==2 && scandata1>0",
    "cleandata==1 && scandata2==3 && scandata1>0",
  };
  beam_evtcut[3430] = { 
    "cleandata==1 && scandata2==1 && scandata1>0",
    "cleandata==1 && scandata2==2 && scandata1>0",
    "cleandata==1 && scandata2==3 && scandata1>0",
    "cleandata==1 && scandata2==4 && scandata1>0",
    "cleandata==1 && scandata2==5 && scandata1>0",
    "cleandata==1 && scandata2==6 && scandata1>0",
    "cleandata==1 && scandata2==7 && scandata1>0",
  };
  beam_evtcut[3597] = { 
    "cleandata==1 && scandata2==1 && scandata1>0",
    "cleandata==1 && scandata2==2 && scandata1>0",
    "cleandata==1 && scandata2==3 && scandata1>0",
    "cleandata==1 && scandata2==4 && scandata1>0",
    "cleandata==1 && scandata2==5 && scandata1>0",
    "cleandata==1 && scandata2==6 && scandata1>0",
    "cleandata==1 && scandata2==7 && scandata1>0",
    "cleandata==1 && scandata2==8 && scandata1>0",
    "cleandata==1 && scandata2==9 && scandata1>0",
  };
  beam_evtcut[3645] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8"
  };
  beam_evtcut[3735] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5 && Entry$>115e3",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7"
  };
  beam_evtcut[3749] = { 
    "Entry$>21.5e3 && Entry$<25.8e3",
    "Entry$>34.7e3 && Entry$<39.4e3",
    "Entry$>47.2e3 && Entry$<50.2e3",
    "Entry$>60.6e3 && Entry$<64.7e3"
  };
  beam_evtcut[3751] = { 
    "cleandata && scandata1>0 && scandata2==1&&!(scandata1==700&&CodaEventNumber>85800&&CodaEventNumber<89300)",
    "cleandata && scandata1>0 && scandata2==4&&!(scandata1==550&&CodaEventNumber>121e3&&CodaEventNumber<126e3)",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==7&&!(scandata1==300&&CodaEventNumber>162e3)",
    "cleandata && scandata1>0 && scandata2==8",
    "cleandata && scandata1>0 && scandata2==9"
  };
  if (ana == "all") {
    beam_evtcut[3751] = { 
      "cleandata && scandata1>0 && scandata2==1",
      "cleandata && scandata1>0 && scandata2==2",
      "cleandata && scandata1>0 && scandata2==3",
      "cleandata && scandata1>0 && scandata2==4",
      "cleandata && scandata1>0 && scandata2==5",
      "cleandata && scandata1>0 && scandata2==6",
      "cleandata && scandata1>0 && scandata2==8",
      "cleandata && scandata1>0 && scandata2==9"
    };
  }
  beam_evtcut[4229] = { 
    "cleandata && scandata1>0 && (scandata2==0)&&CodaEventNumber>26e3",
    "cleandata && scandata1>0 && scandata2==2&&unser>10",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7&&CodaEventNumber>285e3",
    "cleandata && scandata1>0 && scandata2==8",
    "cleandata && scandata1>0 && scandata2==9",
    "cleandata && scandata1>0 && scandata2==10",
    "cleandata && scandata1>0 && (scandata2==1)"
  };
  beam_evtcut[4309] = { 
    "cleandata && scandata1>0 && scandata2==0",
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3"
  };
  beam_evtcut[4610] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8",
    "cleandata && scandata1>0 && scandata2==9"
  };
  beam_evtcut[4650] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7"
  };
  beam_evtcut[4652] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7"
  };
  beam_evtcut[4806] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8",
    "cleandata && scandata1>0 && scandata2==9"
  };
  beam_evtcut[4872] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8",
    "cleandata && scandata1>0 && scandata2==9",
    "cleandata && scandata1>0 && scandata2==10"
  };
  beam_evtcut[4901] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8"
  };
  // CREX
  beam_evtcut[5360] = { 
    "CodaEventNumber>48.5e3&&CodaEventNumber<51e3",
    "CodaEventNumber>116e3&&CodaEventNumber<120e3",
    "CodaEventNumber>210e3&&CodaEventNumber<214e3",
    "CodaEventNumber>250e3&&CodaEventNumber<262e3"
  };
  beam_evtcut[5396] = { 
    "CodaEventNumber>120*60*2.1&&CodaEventNumber<120*60*2.6",
    "CodaEventNumber>120*60*13.3&&CodaEventNumber<120*60*13.8",
    "CodaEventNumber>120*60*18.3&&CodaEventNumber<120*60*18.8",
    "CodaEventNumber>120*60*25.5&&CodaEventNumber<120*60*26"
  };
  beam_evtcut[5417] = { 
    "scandata2==1",
    "scandata2==2",
    "scandata2==3",
    "scandata2==4",
    "scandata2==5",
    "scandata2==6",
    "scandata2==7"
  };
  beam_evtcut[5434] = { 
    "cleandata && scandata1>0 && scandata2==1",
    "cleandata && scandata1>0 && scandata2==2",
    "cleandata && scandata1>0 && scandata2==3",
    "cleandata && scandata1>0 && scandata2==4",
    "cleandata && scandata1>0 && scandata2==5",
    "cleandata && scandata1>0 && scandata2==6",
    "cleandata && scandata1>0 && scandata2==7",
    "cleandata && scandata1>0 && scandata2==8 && Entry$>181e3"
  };
  beam_evtcut[5436] = { 
    "scandata2==1",
    "scandata2==2",
    //"scandata2==2 && bcm_an_us>130",
    "scandata2==3",
    "scandata2==4",
    "scandata2==5",
    "scandata2==6",
    "scandata2==7",
    "scandata2==8",
    //"scandata2==8 && bcm_an_us>40",
    "scandata2==9"
  };
  beam_evtcut[5474] = { 
    "scandata2==1",
    "scandata2==2",
    "scandata2==3",
    "scandata2==4",
    "scandata2==5",
    "scandata2==6",
    "scandata2==7",
    "scandata2==8"
  };
  beam_evtcut[5566] = { 
    "CodaEventNumber>10e3&&CodaEventNumber<32e3", //50uA
    "CodaEventNumber>40e3&&CodaEventNumber<43.2e3", //50uA
    "CodaEventNumber>50.2e3&&CodaEventNumber<51.3e3", //75uA
    // "CodaEventNumber>63.7e3&&CodaEventNumber<64e3", //10uA
    "CodaEventNumber>76.07e3&&CodaEventNumber<76.13e3",  //40uA
    "CodaEventNumber>110e3&&CodaEventNumber<113e3",   //100uA
    "CodaEventNumber>153e3&&CodaEventNumber<169e3",//70uA
    "CodaEventNumber>250e3&&CodaEventNumber<271e3",   //65uA
    "CodaEventNumber>290e3&&CodaEventNumber<295.5e3",//70uA
    "CodaEventNumber>390e3&&CodaEventNumber<405e3" //80uA
  };
  beam_evtcut[5576] = { 
    "CodaEventNumber>4e3&&CodaEventNumber<8e3", //20uA
    "CodaEventNumber>11e3&&CodaEventNumber<14.57e3",//40uA
    "CodaEventNumber>40.8e3&&CodaEventNumber<41.9e3",//22uA 
    "CodaEventNumber>43.2e3&&CodaEventNumber<46e3",//20uA
    "CodaEventNumber>85.1e3&&CodaEventNumber<85.4e3",//11uA
    "CodaEventNumber>113.4e3&&CodaEventNumber<115.5e3",//60uA
    "CodaEventNumber>120.8e3&&CodaEventNumber<121.14e3",//10uA
    "CodaEventNumber>136.7e3&&CodaEventNumber<137.2e3",//90uA
    "CodaEventNumber>152.2e3&&CodaEventNumber<152.7e3",//120uA
    "CodaEventNumber>169e3&&CodaEventNumber<169.7e3",//145uA 
    "CodaEventNumber>192.4e3&&CodaEventNumber<193e3",//146uA
    "CodaEventNumber>201e3&&CodaEventNumber<203.6e3",//140uA
    "CodaEventNumber>272e3&&CodaEventNumber<275e3"//155uA
  };
  // MORE Cuts
  if (ana == "all") {
    beam_evtcut[5576] = {
    // "CodaEventNumber>120*60*2.1&&CodaEventNumber<120*60*2.6",
    //   " CodaEventNumber>4e3&&CodaEventNumber<8e3", //20uA
    "CodaEventNumber>11e3&&CodaEventNumber<14.57e3",//40uA
    // "CodaEventNumber>40.8e3&&CodaEventNumber<41.9e3",//22uA 
    // "CodaEventNumber>43.2e3&&CodaEventNumber<46e3",//20uA
    // "CodaEventNumber>85.1e3&&CodaEventNumber<85.4e3",//11uA
    //"CodaEventNumber>113.4e3&&CodaEventNumber<115.5e3",//60uA
    // "CodaEventNumber>120.8e3&&CodaEventNumber<121.14e3",//10uA
    "CodaEventNumber>136.7e3&&CodaEventNumber<137.2e3",//90uA
    "CodaEventNumber>152.2e3&&CodaEventNumber<152.7e3",//120uA
    "CodaEventNumber>169e3&&CodaEventNumber<169.7e3",//145uA 
    "CodaEventNumber>192.4e3&&CodaEventNumber<193e3",//146uA
    "CodaEventNumber>201e3&&CodaEventNumber<203.6e3",//140uA
    "CodaEventNumber>272e3&&CodaEventNumber<275e3"//155uA
    //  "cleandata && scandata1>0 && scandata2==1",
    //   "cleandata && scandata1>0 && scandata2==2",
    //  "cleandata && scandata1>0 && scandata2==3",
    //  "cleandata && scandata1>0 && scandata2==4",
    //  "cleandata && scandata1>0 && scandata2==5",
    //  "cleandata && scandata1>0 && scandata2==6",
    //  "cleandata && scandata1>0 && scandata2==7",
    //  "cleandata && scandata1>0 && scandata2==8 && Entry$>181e3",
    };
  }
  // Depreciated
  if (ana == "all" && 1==0) {
    beam_evtcut[5576] = { 
    // "CodaEventNumber>120*60*2.1&&CodaEventNumber<120*60*2.6",
    //   " CodaEventNumber>4e3&&CodaEventNumber<8e3", //20uA
    "CodaEventNumber>11e3&&CodaEventNumber<14.57e3",//40uA
    // "CodaEventNumber>40.8e3&&CodaEventNumber<41.9e3",//22uA 
    // "CodaEventNumber>43.2e3&&CodaEventNumber<46e3",//20uA
    // "CodaEventNumber>85.1e3&&CodaEventNumber<85.4e3",//11uA
    //"CodaEventNumber>113.4e3&&CodaEventNumber<115.5e3",//60uA
    // "CodaEventNumber>120.8e3&&CodaEventNumber<121.14e3",//10uA
    "CodaEventNumber>136.7e3&&CodaEventNumber<137.2e3",//90uA
    //  "CodaEventNumber>152.2e3&&CodaEventNumber<152.7e3",//120uA
    "CodaEventNumber>169e3&&CodaEventNumber<169.7e3",//145uA 
    "CodaEventNumber>192.4e3&&CodaEventNumber<193e3",//146uA
    "CodaEventNumber>201e3&&CodaEventNumber<203.6e3",//140uA
    "CodaEventNumber>272e3&&CodaEventNumber<275e3"//155uA
    //  "cleandata && scandata1>0 && scandata2==1",
    //   "cleandata && scandata1>0 && scandata2==2",
    //  "cleandata && scandata1>0 && scandata2==3",
    //  "cleandata && scandata1>0 && scandata2==4",
    //  "cleandata && scandata1>0 && scandata2==5",
    //  "cleandata && scandata1>0 && scandata2==6",
    //  "cleandata && scandata1>0 && scandata2==7",
    //  "cleandata && scandata1>0 && scandata2==8 && Entry$>181e3",
    };
  }
  beam_evtcut[5653] = { 
    "scandata2==1",
    "scandata2==2",
    "scandata2==3",
    "scandata2==4",
    "scandata2==5",
    "scandata2==6",
    "scandata2==7",
    "scandata2==8",
    "scandata2==9"
  };
  beam_evtcut[5778] = { 
    // points 1 and 2 get cut out by the usercut anyways... They are duplicate 140 and 150 uA points
    "scandata2==3",
    "scandata2==4",
    "scandata2==5",
    "scandata2==6",
    "scandata2==7",
    "scandata2==8",
    "scandata2==9"
  };
  beam_evtcut[6049] = { 
    "CodaEventNumber>10.1e3&&CodaEventNumber<11.4e3",//107
    "CodaEventNumber>28.6e3&&CodaEventNumber<28.83e3", //95
    "CodaEventNumber>50e3&&CodaEventNumber<52.8e3",    //107
    "CodaEventNumber>58.7e3&&CodaEventNumber<68.5e3",//87
    "CodaEventNumber>73.35e3&&CodaEventNumber<73.63e3",//35
    "CodaEventNumber>77.55e3&&CodaEventNumber<77.7e3",//40
    "CodaEventNumber>130e3&&CodaEventNumber<135e3",//25
    "CodaEventNumber>210e3&&CodaEventNumber<211e3",//120
    "CodaEventNumber>220e3&&CodaEventNumber<229.5e3",//125
    "CodaEventNumber>250e3&&CodaEventNumber<300e3"//140
  };
  beam_evtcut[6176] = { 
    "CodaEventNumber>40e3&&CodaEventNumber<46e3",
    "CodaEventNumber>156e3&&CodaEventNumber<163e3",
    "CodaEventNumber>290e3&&CodaEventNumber<315e3",
    "CodaEventNumber>371e3&&CodaEventNumber<390e3"
  };
  beam_evtcut[6196] = { 
    "scandata2==1",
    "scandata2==2",
    "scandata2==3",
    "scandata2==4",
    "scandata2==5",
    "scandata2==6",
    "scandata2==7",
    "scandata2==8",
    "scandata2==9"
  };
  beam_evtcut[6233] = { 
    "CodaEventNumber>100e3&&CodaEventNumber<103.5e3",//150 
    "CodaEventNumber>217e3&&CodaEventNumber<225e3",//35
    "CodaEventNumber>245e3&&CodaEventNumber<248e3",//45
    "CodaEventNumber>278.6e3&&CodaEventNumber<281e3",//85
    "CodaEventNumber>310e3&&CodaEventNumber<324e3",//85
    "CodaEventNumber>373e3&&CodaEventNumber<378e3",//105
    "CodaEventNumber>419.5e3&&CodaEventNumber<421e3",//120
    "CodaEventNumber>458.5e3&&CodaEventNumber<460e3",//140
    "CodaEventNumber>470.6e3&&CodaEventNumber<471.8e3",//135
    "CodaEventNumber>521.5e3&&CodaEventNumber<524.5e3",//145
    "CodaEventNumber>560e3&&CodaEventNumber<577e3"//140
  };
  beam_evtcut[6242] = { 
    // OLD 
    // //"CodaEventNumber>38e3&&CodaEventNumber<44.5e3", //150uA
    //"CodaEventNumber>110e3&&CodaEventNumber<121e3", //140uA
    //"CodaEventNumber>156e3&&CodaEventNumber<162e3", //130uA
    //"CodaEventNumber>212e3&&CodaEventNumber<222e3", //120uA
    //"CodaEventNumber>274e3&&CodaEventNumber<280e3", //110uA
    //"CodaEventNumber>354e3&&CodaEventNumber<364e3" //100uA
    // NEW 
    "scandata1==150&&CodaEventNumber<45100",
    "scandata1==140&&CodaEventNumber<79e3",
    "scandata1==140&&cleandata&&CodaEventNumber<100e3",
    "scandata1==140&&cleandata&&CodaEventNumber>110e3",
    "scandata1==130&&CodaEventNumber<172e3",
    "scandata1==120&&CodaEventNumber<225e3",
    "scandata1==110&&CodaEventNumber<247.5e3",
    "scandata1==110&&CodaEventNumber>272e3&&CodaEventNumber<286e3",
    "CodaEventNumber>311.5e3&&CodaEventNumber<314e3",//100uA
    "scandata1==100&&CodaEventNumber<368e3"//100uA
  };
  beam_evtcut[6442] = { 
    "CodaEventNumber>82e3&&CodaEventNumber<87e3", //150uA
    "CodaEventNumber>114e3&&CodaEventNumber<125e3", //140uA
    "CodaEventNumber>161e3&&CodaEventNumber<170e3" //130uA
  };
  beam_evtcut[6560] = { 
    "scandata1==150",
    "scandata1==140",
    "scandata1==130",
    "scandata1==120",
    "scandata1==110",
    "scandata1==100",
    "scandata1==90",
    "scandata1==80",
    "scandata1==70",
    "scandata1==60",
    "scandata1==50",
    "scandata1==40",
    "scandata1==30",
    "scandata1==20",
    "scandata1==10"
  };
  beam_evtcut[6563] = {
    "(CodaEventNumber>119.5e3&&CodaEventNumber<123.5e3)",
    "(CodaEventNumber>124.5e3&&CodaEventNumber<126.5e3)",
    "(CodaEventNumber>127e3&&CodaEventNumber<128.4e3)",
    "(CodaEventNumber>135e3&&CodaEventNumber<195e3)",
    "(CodaEventNumber>200e3&&CodaEventNumber<215e3)",
    "(CodaEventNumber>224e3&&CodaEventNumber<239e3)",
    "(CodaEventNumber>242e3&&CodaEventNumber<246e3)", //150
    //"(CodaEventNumber>253.5e3&&CodaEventNumber<258e3)",//150 noisey
    "(CodaEventNumber>266e3&&CodaEventNumber<281e3)",//150
    //"(CodaEventNumber>289e3&&CodaEventNumber<297.5e3)",//150 noisy
    //"(CodaEventNumber>309e3&&CodaEventNumber<317e3)",//150 noisy
    "(CodaEventNumber>324e3&&CodaEventNumber<326e3)",//150
    "(CodaEventNumber>331.5e3&&CodaEventNumber<333.5e3)",//45
    "(CodaEventNumber>340e3&&CodaEventNumber<346e3)",//45
    "(CodaEventNumber>355e3&&CodaEventNumber<362e3)"//45
    //"(CodaEventNumber>371e3)"    //150 noisy
  };
  beam_evtcut[6739] = { 
    "cleandata&&scandata1==20&&CodaEventNumber<113.2e3",
    "cleandata&&scandata1==50&&CodaEventNumber<209.5e3",
    "cleandata&&scandata1==101&&!(CodaEventNumber<135e3&&CodaEventNumber>130.2e3)",
    "cleandata&&scandata1==151",
    "scandata1==100&&CodaEventNumber>250e3&&bcm_an_us.hw_sum_raw/bcm_an_us.num_samples>10350",
  /*  "CodaEventNumber>136.95e3&&CodaEventNumber<137.25e3",//15uA
    "CodaEventNumber>145.2e3 && CodaEventNumber<145.5e3",//15uA
    "CodaEventNumber>206.45 && CodaEventNumber<206.75e3",//15uA
    "CodaEventNumber>211.43e3 && CodaEventNumber<211.75e3",//15uA
    "CodaEventNumber>164.4e3 && CodaEventNumber<165.05e3",//16uA
    "CodaEventNumber>202.7e3 && CodaEventNumber<203.02e3",//16uA
    "CodaEventNumber>124.65e3&&CodaEventNumber<124.95e3",//30uA
    "CodaEventNumber>278.55e3&&CodaEventNumber<278.85e3",//32uA
    "CodaEventNumber>270.7e3&&CodaEventNumber<271.0e3",//35uA
    "CodaEventNumber>283.17e3&&CodaEventNumber<283.45e3",//35uA
    "CodaEventNumber>359.7e3&&CodaEventNumber<360e3",//35uA
    "CodaEventNumber>370.8e3&&CodaEventNumber<371.05e3",//35uA
    //37 lots 
    "CodaEventNumber>137.35e3&&CodaEventNumber<137.65e3",//46uA
    "CodaEventNumber>145.6e3&&CodaEventNumber<145.9e3",//46uA
    "CodaEventNumber>203.1e3&&CodaEventNumber<203.4e3",//46uA
    "CodaEventNumber>206.8e3&&CodaEventNumber<207.1e3",//46uA
    "CodaEventNumber>119.1e3&&CodaEventNumber<119.45e3",//55uA
    "CodaEventNumber>215.86e3&&CodaEventNumber<216.1e3",//55uA
    "CodaEventNumber>124.9e3&&CodaEventNumber<125.34e3",//60uA
    "CodaEventNumber>119.5e3&&CodaEventNumber<119.83e3",//72uA
    "CodaEventNumber>137.7e3&&CodaEventNumber<138e3",//72uA
    "CodaEventNumber>145.95e3&&CodaEventNumber<146.27e3",//72uA
    "CodaEventNumber>203.45e3&&CodaEventNumber<203.75e3",//72uA
    //81uA
    //85uA
    "CodaEventNumber>216.85e3&&CodaEventNumber<217.45e3",//90uA
    "CodaEventNumber>320.7e3&&CodaEventNumber<323.7e3",//90uA
    "CodaEventNumber>217.47e3&&CodaEventNumber<218.05e3",//95uA
    "CodaEventNumber>218.1e3&&CodaEventNumber<218.68e3",//97uA
    "CodaEventNumber>218.7e3&&CodaEventNumber<221.5e3",//98uA
    "CodaEventNumber>221.6e3&&CodaEventNumber<222.2e3",//125uA
    "CodaEventNumber>222.25e3&&CodaEventNumber<222.8e3",//136
    "CodaEventNumber>263e3&&CodaEventNumber<263.3e3",  //136
    "CodaEventNumber>271.45e3&&CodaEventNumber<271.75e3", //136
    "CodaEventNumber>279.3e3&&CodaEventNumber<279.64e3", //136
    "CodaEventNumber>283.9e3&&CodaEventNumber<284.2e3", //136
    "CodaEventNumber>222.85e3&&CodaEventNumber<223.45e3",//141
    "CodaEventNumber>223.5e3&&CodaEventNumber<224.45e3",//144
    "CodaEventNumber>239e3&&CodaEventNumber<257e3",//149
    "CodaEventNumber>321e3&&CodaEventNumber<323.5e3",//90uA
    "CodaEventNumber>320e3&&CodaEventNumber<320.25e3",//37uA
    "CodaEventNumber>320.35e3&&CodaEventNumber<320.65e3"//84uA
    */
  };
  beam_evtcut[7121] = {
    "scandata1==151 && cleandata && Entry$<34e3",
    "scandata1==133 && cleandata && Entry$>44e3",
    "scandata1==110 && cleandata && Entry$<73e3",
    "scandata1==91 && cleandata",
    "scandata1==70 && cleandata && Entry$<103.2e3",
    "scandata1==52 && cleandata && Entry$>123.5e3",
    "scandata1==30 && cleandata",
    "scandata1==10 && cleandata && Entry$>164.5e3"
  };
  beam_evtcut[7122] = { 
    "cleandata&&scandata1==150",
    "cleandata&&scandata1==131 && CodaEventNumber<143.9e3",
    "cleandata&&scandata1==110",
    "cleandata&&scandata1==90 && CodaEventNumber<117.4e3",
    "cleandata&&scandata1==70 && CodaEventNumber>99.6e3 && CodaEventNumber<108e3",
    "cleandata&&scandata1==50&&CodaEventNumber<62.3e3",
    "cleandata&&scandata1==30",
    "cleandata&&scandata1==10"
  };
  beam_evtcut[7279] = {
     "scandata1==150 && cleandata",
     "scandata1==130 && cleandata && Entry$>52e3",
     "scandata1==110 && cleandata",
     "scandata1==90 && cleandata && Entry$>92.5e3",
     "scandata1==70 && cleandata",
     "scandata1==50 && cleandata && Entry$>136e3",
     "scandata1==30 && cleandata",
     "scandata1==10 && cleandata && Entry$>182e3"
  };
  beam_evtcut[7282] = { 
    "cleandata&&scandata1==150",
    "cleandata&&scandata1==130&&CodaEventNumber<86.9e3",
    "cleandata&&scandata1==110",
    "cleandata&&scandata1==90",
    "cleandata&&scandata1==70",
    "cleandata&&scandata1==50 && CodaEventNumber<160e3",
    "cleandata&&scandata1==30"
    // "cleandata&&scandata1==10",
  };
   beam_evtcut[7406] = { 
    "cleandata&&scandata1==150",
    "cleandata&&scandata1==130",
    "cleandata&&scandata1==120",
    "cleandata&&scandata1==110 && !(CodaEventNumber>116e3 && CodaEventNumber<120.2e3) && CodaEventNumber<121.5e3",
    "cleandata&&scandata1==100&&CodaEventNumber>23e3",
    "cleandata&&scandata1==90",
    "cleandata&&scandata1==80",
    "cleandata&&scandata1==70",
    "cleandata&&scandata1==60",
    "cleandata&&scandata1==50",
    "cleandata&&scandata1==40 && CodaEventNumber>51.4e3",
    "cleandata&&scandata1==30 && CodaEventNumber<71.1e3",
    "cleandata&&scandata1==20 && CodaEventNumber>61.1e3"
  };
  beam_evtcut[7407] = {
     "scandata1==150 && cleandata && Entry$>12e3",
     "scandata1==130 && cleandata",
     "scandata1==110 && cleandata && Entry$<72e3",
     "scandata1==90 && cleandata && Entry$>92e3",
     "scandata1==70 && cleandata && Entry$>122.5e3",
     "scandata1==50 && cleandata",
     "scandata1==30 && cleandata && Entry$>190.5e3",
     "scandata1==20 && cleandata && Entry$>182e3"
  };



  std::map <Int_t,std::vector<TCut>> pedestal_evtcut;// 
  pedestal_evtcut[2769] = { 
    "cleandata==1 && scandata2==1 && scandata1<50", // Scandata2 tag = current/10
    "cleandata==1 && scandata2==2 && scandata1<50",
    "cleandata==1 && scandata2==3 && scandata1<50",
    "cleandata==1 && scandata2==4 && scandata1<50",
    "cleandata==1 && scandata2==5 && scandata1<50",
    "cleandata==1 && scandata2==6 && scandata1<50",
    "cleandata==1 && scandata2==7 && scandata1<50",
    "cleandata==1 && scandata2==8 && scandata1<50",
  };
  pedestal_evtcut[2963] = {
    "CodaEventNumber>18e3&&CodaEventNumber<30e3",//50
    "CodaEventNumber>62e3&&CodaEventNumber<70e3",//50
    "CodaEventNumber>91e3&&CodaEventNumber<92.5e3",//40
    "CodaEventNumber>98e3&&CodaEventNumber<101e3",//40
    "CodaEventNumber>117e3&&CodaEventNumber<128e3",//40
    "CodaEventNumber>135e3&&CodaEventNumber<138e3",//30
    "CodaEventNumber>142e3&&CodaEventNumber<143e3",//30
    "CodaEventNumber>150e3&&CodaEventNumber<160e3",//30
    "CodaEventNumber>177e3&&CodaEventNumber<180e3"//20
  };
  pedestal_evtcut[2967] = {
    "Entry$>23.5e3 && Entry$<24e3",
    "Entry$>28.6e3 && Entry$<30e3",
    "Entry$>42.1e3 && Entry$<44e3",
    "Entry$>55e3 && Entry$<60e3",
    "Entry$>42.1e3 && Entry$<44e3",
    "Entry$>55e3 && Entry$<60e3"
  };
  pedestal_evtcut[3098] = {
     "Entry$>110.5e3 && Entry$<112.5e3",
     "Entry$>262e3 && Entry$<264e3",
     "Entry$>298.6e3 && Entry$<303.5e3",
     "Entry$>298.6e3 && Entry$<303.5e3",
     "Entry$>298.6e3 && Entry$<303.5e3",
     "Entry$>403e3 && Entry$<423e3",
     "Entry$>403e3 && Entry$<423e3",
     "Entry$>241e3 && Entry$<243.5e3",
  };
  pedestal_evtcut[3101] = {
     "Entry$<16e3",
     "Entry$>38e3 && Entry$<42.1e3",
     "Entry$>49.2e3 && Entry$<51.6e3",
     "Entry$>49.15e3 && Entry$<51.6e3", 
     "Entry$>49.15e3 && Entry$<51.6e3", 
     "Entry$>181e3",
     "Entry$>181e3"
  };
  pedestal_evtcut[3105] = {
    "CodaEventNumber>38.5e3&&CodaEventNumber<41e3",
    "CodaEventNumber>54e3&&CodaEventNumber<55.5e3",
    "CodaEventNumber>54e3&&CodaEventNumber<55.5e3",
    "CodaEventNumber>97e3&&CodaEventNumber<98e3"
  };
  pedestal_evtcut[3135] = {
    "CodaEventNumber>47.1e3&&CodaEventNumber<48.5e3", 
    "CodaEventNumber>75.2e3&&CodaEventNumber<76.5e3",
    "CodaEventNumber>100.5e3&&CodaEventNumber<102e3",
    "CodaEventNumber>103.72e3&&CodaEventNumber<105e3",
    "CodaEventNumber>110e3&&CodaEventNumber<111e3"
  };
  pedestal_evtcut[3138] = {
    "CodaEventNumber>10e3&&CodaEventNumber<14e3",
    "CodaEventNumber>16.8e3&&CodaEventNumber<18e3",
    "CodaEventNumber>22e3&&CodaEventNumber<23e3",
    "CodaEventNumber>70e3&&CodaEventNumber<71e3",
    "CodaEventNumber>76e3&&CodaEventNumber<77e3"
  };
  pedestal_evtcut[3395] = { 
    /*
    "cleandata==1 && scandata2==1 && scandata1<50",
    "cleandata==1 && scandata2==2 && scandata1<50",
    "cleandata==1 && scandata2==3 && scandata1<50",
    "cleandata==1 && scandata2==4 && scandata1<50",
    "cleandata==1 && scandata2==5 && scandata1<50",
    "cleandata==1 && scandata2==6 && scandata1<50",
    "cleandata==1 && scandata2==7 && scandata1<50",
    "cleandata==1 && scandata2==8 && scandata1<50",
    "cleandata==1 && scandata2==9 && scandata1<50",
    "cleandata==1 && scandata2==10 && scandata1<50",
    "cleandata==1 && scandata2==10 && scandata1<50"
      */
    "cleandata && scandata1==0 && (scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==2 || scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)",
    "cleandata && scandata1==0 && (scandata2==4 || scandata2==3)",
    "cleandata && scandata1==0 && (scandata2==5 || scandata2==4)",
    "cleandata && scandata1==0 && (scandata2==6 || scandata2==5)",
    "cleandata && scandata1==0 && (scandata2==7 || scandata2==6)",
    "cleandata && scandata1==0 && (scandata2==8 || scandata2==7)",
    "cleandata && scandata1==0 && (scandata2==9 || scandata2==8)",
    "cleandata && scandata1==0 && (scandata2==10 || scandata2==9)",
    "cleandata && scandata1==0 && (scandata2==11 || scandata2==10)"
  };
  pedestal_evtcut[3645] = { 
    "cleandata && scandata1==0 && (scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==2 || scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)",
    "cleandata && scandata1==0 && (scandata2==4 || scandata2==3)",
    "cleandata && scandata1==0 && (scandata2==5 || scandata2==4)",
    "cleandata && scandata1==0 && (scandata2==6 || scandata2==5)",
    "cleandata && scandata1==0 && (scandata2==7 || scandata2==6)",
    "cleandata && scandata1==0 && (scandata2==8 || scandata2==7)"
  };
  pedestal_evtcut[3735] = { 
    "cleandata && scandata1==0 && (scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==2 || scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)",
    "cleandata && scandata1==0 && (scandata2==4 || scandata2==3)",
    "cleandata && scandata1==0 && (scandata2==5 || scandata2==4)",
    "cleandata && scandata1==0 && (scandata2==6 || scandata2==5)",
    "cleandata && scandata1==0 && (scandata2==7)"
  };
  pedestal_evtcut[3749] = {
    "Entry$>19.6e3 && Entry$<21.3e3",    
    "Entry$>33e3 && Entry$<34.6e3",    
    "Entry$>50.3e3 && Entry$<55.3e3",
    "Entry$>65e3 && Entry$<80e3"   
  };
  pedestal_evtcut[3751] = {
    "scandata2==1 && (CodaEventNumber>86000&&CodaEventNumber<87100)",
    "scandata2==4 &&cleandata &&CodaEventNumber>121.5e3&&CodaEventNumber<123.3e3",
    "scandata2==4 && CodaEventNumber>131.6e3&&CodaEventNumber<133.2e3",
    "scandata2==7 &&CodaEventNumber>162.2e3&&CodaEventNumber<163.6e3",
    "scandata2==8 &&CodaEventNumber>177.8e3&&CodaEventNumber<179.4e3",
    "scandata2==9 &&CodaEventNumber>197.5e3"
  };
  pedestal_evtcut[4229] = {
    "cleandata && scandata1==0 && (scandata2==1)&&CodaEventNumber<62e3",
    "cleandata && scandata1<2 && (scandata2==2 || scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)",
    "cleandata && scandata1==0 && (scandata2==4 || scandata2==3)",
    "cleandata && scandata1==0 && (scandata2==5 || scandata2==4)",
    "cleandata && scandata1==0 && (scandata2==6 || scandata2==5)",
    "cleandata && scandata1==0 && (scandata2==7 || scandata2==6)&&CodaEventNumber<264e3",
    "cleandata && scandata1==0 && (scandata2==8 || scandata2==7)",
    "cleandata && scandata1==0 && (scandata2==9 || scandata2==8)",
    "cleandata && scandata1==0 && (scandata2==10 || scandata2==9)",
    "cleandata && scandata1==0 && (scandata2==10)"
  };
  pedestal_evtcut[4901] = {
    "cleandata && scandata1==0 && (scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==2 || scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)",
    "cleandata && scandata1==0 && (scandata2==4 || scandata2==3)",
    "cleandata && scandata1==0 && (scandata2==5 || scandata2==4)",
    "cleandata && scandata1==0 && (scandata2==5)",
    "cleandata && scandata1==0 && (scandata2==7)",
    "cleandata && scandata1==0 && (scandata2==8)"
  };
  pedestal_evtcut[5360] = {
    "CodaEventNumber>51.5e3&&CodaEventNumber<60e3",
    "CodaEventNumber>121e3&&CodaEventNumber<125e3",
    "CodaEventNumber>215e3&&CodaEventNumber<235e3",
    "CodaEventNumber>270e3&&CodaEventNumber<300e3"
  };
  pedestal_evtcut[5396] = {
     "CodaEventNumber>120*60*2.8&&CodaEventNumber<120*60*3.1",
     "CodaEventNumber>120*60*14.2&&CodaEventNumber<120*60*14.7",
     "CodaEventNumber>120*60*19&&CodaEventNumber<120*60*19.4",
     "CodaEventNumber>120*60*24&&CodaEventNumber<120*60*24.5"
  };
  pedestal_evtcut[5434] = {
    "cleandata && scandata1==0 && (scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==2 || scandata2==1)",
    "cleandata && scandata1==0 && (scandata2==3 || scandata2==2)",
    "cleandata && scandata1==0 && (scandata2==4 || scandata2==3)",
    "cleandata && scandata1==0 && (scandata2==5 || scandata2==4)",
    "cleandata && scandata1==0 && (scandata2==5)",
    "cleandata && scandata1==0 && (scandata2==7)",
    "cleandata && scandata1==0 && (scandata2==8 && Entry$>181e3)"
  };
  pedestal_evtcut[5566] = {
    "CodaEventNumber>34e3&&CodaEventNumber<38e3", //50uA ped
    "CodaEventNumber>45e3&&CodaEventNumber<47.4e3",//50uA ped
    "CodaEventNumber>52e3&&CodaEventNumber<60e3",//75uA ped
    "CodaEventNumber>77e3&&CodaEventNumber<78.5e3",   //40uA
    "CodaEventNumber>114e3&&CodaEventNumber<116.5e3", //100uA
    "CodaEventNumber>170e3&&CodaEventNumber<178e3",//70uA
    "CodaEventNumber>272e3&&CodaEventNumber<273.5e3",//65uA
    "CodaEventNumber>296e3&&CodaEventNumber<298.5e3",   //70uA
    "CodaEventNumber>409e3&&CodaEventNumber<415e3"   //80uA
  };
  pedestal_evtcut[5576] = {
    "CodaEventNumber>1e3&&CodaEventNumber<3.7e3",//20uA ped
    "CodaEventNumber>15e3&&CodaEventNumber<20e3",//40uA ped
    "CodaEventNumber>35e3&&CodaEventNumber<40e3",//22uA ped
    "CodaEventNumber>46.3e3&&CodaEventNumber<50e3",//20uA ped
    "CodaEventNumber>84e3&&CodaEventNumber<84.7e3",//11uA ped
    "CodaEventNumber>116e3&&CodaEventNumber<120e3",//60uA ped
    "CodaEventNumber>118e3&&CodaEventNumber<120.4e3",//10uA ped
    "CodaEventNumber>137.3e3&&CodaEventNumber<143e3",//90uA ped
    "CodaEventNumber>152.8e3&&CodaEventNumber<155.5e3",//120uA ped
    "CodaEventNumber>170e3&&CodaEventNumber<172e3",//145uA ped
    "CodaEventNumber>195e3&&CodaEventNumber<196e3",   //146 ped
    "CodaEventNumber>203.8e3&&CodaEventNumber<207e3",   //140uA
    "CodaEventNumber>276e3&&CodaEventNumber<279e3" //155 ped
  };
  pedestal_evtcut[6049] = {
    "CodaEventNumber>11.5e3&&CodaEventNumber<20e3",    //107
    "CodaEventNumber>28.9e3&&CodaEventNumber<30e3",   //95
    "CodaEventNumber>54e3&&CodaEventNumber<56e3",    //107
    "CodaEventNumber>70e3&&CodaEventNumber<72e3",//87
    "CodaEventNumber>74e3&&CodaEventNumber<76e3",    //35
    "CodaEventNumber>81e3&&CodaEventNumber<86e3",//40
    "CodaEventNumber>136e3&&CodaEventNumber<145e3",//25
    "CodaEventNumber>212e3&&CodaEventNumber<215e3",//120
    "CodaEventNumber>231e3&&CodaEventNumber<235e3",//125 after
    "CodaEventNumber>231e3&&CodaEventNumber<235e3"//140 before same ped
  };
  pedestal_evtcut[6176] = {
    "CodaEventNumber>53.5e3&&CodaEventNumber<56.5e3",
    "CodaEventNumber>169.5e3&&CodaEventNumber<173.5e3",
    "CodaEventNumber>270e3&&CodaEventNumber<274e3",
    "CodaEventNumber>351e3&&CodaEventNumber<360e3"
    //"CodaEventNumber>29e3&&CodaEventNumber<36e3",
    //"CodaEventNumber>143e3&&CodaEventNumber<146e3",
    //"CodaEventNumber>270e3&&CodaEventNumber<274e3",
    //"CodaEventNumber>351e3&&CodaEventNumber<360e3"
  };
  pedestal_evtcut[6233] = {
    "CodaEventNumber>104e3&&CodaEventNumber<115e3", //150
    "CodaEventNumber>226e3&&CodaEventNumber<227e3",//35
    "CodaEventNumber>250e3&&CodaEventNumber<253e3",//45
    "CodaEventNumber>282e3&&CodaEventNumber<288e3",//85
    "CodaEventNumber>325e3&&CodaEventNumber<335e3",//85
    "CodaEventNumber>379e3&&CodaEventNumber<389e3",//105
    "CodaEventNumber>422e3&&CodaEventNumber<428e3",//120
    "CodaEventNumber>461e3&&CodaEventNumber<464e3",//140
    "CodaEventNumber>472e3&&CodaEventNumber<482e3",//135
    "CodaEventNumber>526e3&&CodaEventNumber<528.5e3",//145
    "CodaEventNumber>578e3&&CodaEventNumber<588e3"//140
  };
  pedestal_evtcut[6242] = {
    //"CodaEventNumber>15e3&&CodaEventNumber<30e3", //150uA
    //"CodaEventNumber>60e3&&CodaEventNumber<70e3", //140uA
    //"CodaEventNumber>130e3&&CodaEventNumber<142e3", //130uA
    //"CodaEventNumber>180e3&&CodaEventNumber<195e3", //120uA
    //"CodaEventNumber>250e3&&CodaEventNumber<265e3", //110uA
    //"CodaEventNumber>320e3&&CodaEventNumber<330e3", //100uA
    // NEW
    "(CodaEventNumber>15e3&&CodaEventNumber<30e3)||(CodaEventNumber>45.5e3&&CodaEventNumber<70e3)", //150uA
    "(CodaEventNumber>45.5e3&&CodaEventNumber<70e3)||(CodaEventNumber>79.5e3&&CodaEventNumber<91.5e3)", //140uA
    "(CodaEventNumber>79.5e3&&CodaEventNumber<91.5e3)||(CodaEventNumber>100.5e3&&CodaEventNumber<103e3)",//140
    "(CodaEventNumber>100.5e3&&CodaEventNumber<103e3)||(CodaEventNumber>127e3&&CodaEventNumber<143e3)",//140
    "(CodaEventNumber>127e3&&CodaEventNumber<143e3)||(CodaEventNumber>176e3&&CodaEventNumber<198.5e3)", //130uA
    "(CodaEventNumber>176e3&&CodaEventNumber<198.5e3)||(CodaEventNumber>226e3&&CodaEventNumber<236e3)", //120uA
    "(CodaEventNumber>226e3&&CodaEventNumber<236e3)||(CodaEventNumber>248e3&&CodaEventNumber<267e3)", //110uA
    "(CodaEventNumber>248e3&&CodaEventNumber<267e3)||(CodaEventNumber>287e3&&CodaEventNumber<308e3)",//110
    "(CodaEventNumber>287e3&&CodaEventNumber<308e3)||(CodaEventNumber>314.5e3&&CodaEventNumber<316.5e3)",
    "(CodaEventNumber>340.5e3&&CodaEventNumber<343e3)||(CodaEventNumber>370e3&&CodaEventNumber<375e3)"//100uA new
      //"CodaEventNumber>320e3&&CodaEventNumber<330e3", //100uA bad pedestal
  };
  pedestal_evtcut[6442] = {
    "CodaEventNumber>74e3&&CodaEventNumber<76e3", //150uA
    "CodaEventNumber>102e3&&CodaEventNumber<109e3", //140uA
    "CodaEventNumber>155.5e3&&CodaEventNumber<158.5e3" //130uA
  };
  pedestal_evtcut[6560] ={
    "scandata2==1&&CodaEventNumber<205e3&&scandata1==0&&cleandata",    
    "scandata2==2&&CodaEventNumber<205e3&&scandata1==0&&cleandata",    
    "scandata2==3&&CodaEventNumber<205e3&&scandata1==0&&cleandata",    
    "scandata2==4&&CodaEventNumber<205e3&&scandata1==0&&cleandata",    
    "scandata2==5&&CodaEventNumber<205e3&&scandata1==0&&cleandata",    
    "scandata2==6&&CodaEventNumber<205e3&&scandata1==0&&cleandata",    
    "scandata2==7&&CodaEventNumber<205e3&&scandata1==0&&cleandata",    
    "scandata2==8&&CodaEventNumber<205e3&&scandata1==0&&cleandata",    
    "scandata2==7&&CodaEventNumber>205e3&&scandata1==0&&cleandata",  
    "scandata2==6&&CodaEventNumber>205e3&&scandata1==0&&cleandata",  
    "scandata2==5&&CodaEventNumber>205e3&&scandata1==0&&cleandata", 
    "scandata2==4&&CodaEventNumber>205e3&&scandata1==0&&cleandata",  
    "scandata2==3&&CodaEventNumber>205e3&&scandata1==0&&cleandata", 
    "scandata2==9&&CodaEventNumber>205e3&&scandata1==0&&cleandata",      
    "scandata2==10&&CodaEventNumber>205e3&&scandata1==0&&cleandata"   
  };
  pedestal_evtcut[7121] = {
    "scandata1==0 && CodaEventNumber>30e3 && CodaEventNumber<55e3 && cleandata",
    "scandata1==0 && CodaEventNumber>55e3 && CodaEventNumber<70e3 && cleandata",
    "scandata1==0 && CodaEventNumber>70e3 && CodaEventNumber<80e3 && cleandata",
    "scandata1==0 && CodaEventNumber>90e3 && CodaEventNumber<103e3 && cleandata",
    "scandata1==0 && CodaEventNumber>112e3 && CodaEventNumber<120e3 && cleandata",
    "scandata1==0 && CodaEventNumber>120e3 && CodaEventNumber<153e3 && cleandata",
    "scandata1==0 && CodaEventNumber>153e3 && CodaEventNumber<165e3 && cleandata",      
    "scandata1==0 && CodaEventNumber>165e3 && cleandata"   
  };
  pedestal_evtcut[7279] = {
     "scandata1==0 && scandata2==2 && cleandata",
     "scandata1==0 && scandata2==3 && cleandata && Entry$<69.5e3",
     "scandata1==0 && scandata2==4 && cleandata && Entry$<88e3",
     "scandata1==0 && scandata2==5 && cleandata",
     "scandata1==0 && scandata2==6 && cleandata",
     "scandata1==0 && scandata2==7 && cleandata",
     "scandata1==0 && scandata2==8 && cleandata",
     "scandata1==0 && scandata2==9 && cleandata"   
  };
  pedestal_evtcut[7407] = {
     "scandata1==0 && scandata2==1 && cleandata && Entry$>22e3",
     "scandata1==0 && scandata2==2 && cleandata",
     "scandata1==0 && scandata2==3 && cleandata",
     "scandata1==0 && scandata2==4 && cleandata",
     "scandata1==0 && scandata2==5 && cleandata",
     "scandata1==0 && scandata2==6 && cleandata",
     "Entry$>180e3 && Entry$< 186e3",
     "scandata1==0 && scandata2==7 && cleandata"   
  };

  if (beam_evtcut.count(run_num) == 0 || (ana=="unser" && pedestal_evtcut.count(run_num) == 0)){
    Printf("Error, run %d not included in macro",run_num);
    return;
  }
  const int ndata = beam_evtcut[run_num].size(); // sizeof(beam_evtcut)/sizeof(*beam_evtcut);
  if (ana=="unser" && ndata != pedestal_evtcut[run_num].size()) 
  {
    Printf("Error, incommensurate pedestal vs. beam on Unser scan data");
    return;
  }

  // Add in the default initialzed user cuts from the individual macros
  std::map <Int_t,TCut> user_cuts;
  user_cuts[2769] = "";
  user_cuts[2963] = "bcm_dg_us.Device_Error_Code==0";
  user_cuts[2965] = "!(scandata2==1&&cleandata==1&&bcm_an_us<40)&&!(scandata2==1&&cleandata==1&&CodaEventNumber<40000)&&!(scandata2==2&&cleandata==1&&CodaEventNumber<80500)";
  user_cuts[2967] = "";
  //user_cuts[3098] = "";
  user_cuts[3098] = "bcm_an_us>15&&cleandata==1&&scandata2!=1&&!(CodaEventNumber>313.4e3&&CodaEventNumber<318e3)";
  user_cuts[3101] = "";
  user_cuts[3105] = "";
  user_cuts[3106] = "bcm_dg_us.Device_Error_Code==0 && bcm_an_ds.Device_Error_Code==0";
  user_cuts[3135] = "bcm_dg_us.Device_Error_Code==0 && bcm_an_ds.Device_Error_Code==0";
  user_cuts[3138] = "!(CodaEventNumber>8000&&CodaEventNumber<20000)&&!(CodaEventNumber>69000&&CodaEventNumber<80000)";
  user_cuts[3395] = "";
  user_cuts[3424] = "";
  user_cuts[3429] = "ErrorFlag==0&&CodaEventNumber>9500";
  user_cuts[3430] = "cleandata&&CodaEventNumber>2700&&!(scandata2==2&&CodaEventNumber>29800)&&!(scandata2==3&&CodaEventNumber>35000)&&!(scandata2==7&&CodaEventNumber>111.7e3)&&bmwobj==-1";
  user_cuts[3597] = "bmwobj==-1&&!(cleandata&&scandata1==400&&(CodaEventNumber>112000&&CodaEventNumber<120000))";
  user_cuts[3645] = "!(scandata2==4&&scandata1>0&&unser<40)&&!(scandata2==2&&scandata1>0&&unser<60)&&!(scandata2==6&&unser<20)";
  user_cuts[3735] = "";
  user_cuts[3749] = "";
  user_cuts[3751] = "";
  if (ana == "all") {
    user_cuts[3751] = "!(scandata1==700&&CodaEventNumber>85800&&CodaEventNumber<89300)&&!(scandata1==550&&CodaEventNumber>121e3&&CodaEventNumber<126e3)&&!(scandata1==300&&CodaEventNumber>162e3)";
  }
  user_cuts[4229] = "";
  if (ana == "all") {
    user_cuts[4229] = "cleandata && scandata1>3 && !((scandata2==0)&&CodaEventNumber<26e3)&&!(scandata2==2&&unser<10)&&!(scandata2==7&&CodaEventNumber<285e3)&&scandata2!=1";
  }
  user_cuts[4309] = "cleandata&&!(cleandata&&scandata1==550&&CodaEventNumber<175e3)";
  user_cuts[4610] = "cleandata&&!(scandata1==700&&bcm_an_us<65)&&!(scandata1>300&&bcm_an_us<35)";
  user_cuts[4650] = "cleandata&&!(scandata2==3&&bcm_an_us<20)";
  user_cuts[4652] = "cleandata";
  user_cuts[4806] = "cleandata&&scandata1!=80&&bcm_an_us>5&&!(scandata1==300&&bcm_an_us<25)&&CodaEventNumber>40e3&&!(CodaEventNumber>45500&&CodaEventNumber<46000)&&!(CodaEventNumber>275e3&&CodaEventNumber<298e3)";
  user_cuts[4872] = "cleandata&&!(scandata1>600&&ErrorFlag!=0)&&!(scandata2==2&&bcm_an_us<75)&&!(scandata2==3&&CodaEventNumber<265e3)&&!(scandata2==8&&bcm_an_us<25)&&!(scandata2==10&&CodaEventNumber<492.5e3)&&CodaEventNumber<492e3&&!(CodaEventNumber>86e3&&CodaEventNumber<90e3)";
  user_cuts[4901] = "";
  user_cuts[5360] = "";
  user_cuts[5396] = "";
  // OLD user_cuts[5417] = "scandata1!=0 && cleandata";// && bpm4aX<1 && bpm4aX>-4 && bpm4aY<1.35 && bpm16Y>0.06 && !(CodaEventNumber>163e3&&CodaEventNumber<171e3) && !(CodaEventNumber>70e3&&CodaEventNumber<80e3) && !(CodaEventNumber>156e3&&CodaEventNumber<158e3) && !(CodaEventNumber>76e3 && CodaEventNumber<80e3) && bpm11X>0.075 && bpm11Y<-0.0755&&CodaEventNumber>150e3";
  user_cuts[5417] = "scandata1!=0 && cleandata && !(CodaEventNumber>163e3&&CodaEventNumber<171e3) && !(CodaEventNumber>70e3&&CodaEventNumber<80e3) && !(CodaEventNumber>156e3&&CodaEventNumber<158e3) && !(CodaEventNumber>76e3 && CodaEventNumber<80e3) &&CodaEventNumber>150e3";
  user_cuts[5434] = "";
  // OLD user_cuts[5436] = "cleandata&&scandata1!=0&&bpm4aX<-0.3&&bpm4aX>-1&&bpm12X<0&&!(CodaEventNumber>20e3&&CodaEventNumber<30e3)&&!(CodaEventNumber>110e3&&CodaEventNumber<122e3)&&!(CodaEventNumber>124.6e3&&CodaEventNumber<126e3)&&!(CodaEventNumber>140e3&&CodaEventNumber<150e3)&&bpm12Y>-1&&bpm16X<0.1&&!(CodaEventNumber>124.3e3&&CodaEventNumber<125e3)&&bpm1Y<1.45&&bpm1X>-1.95";
  user_cuts[5436] = "cleandata&&scandata1!=0&&!(CodaEventNumber>15.5e3&&CodaEventNumber<30e3)&&!(CodaEventNumber>110e3&&CodaEventNumber<122e3)&&!(CodaEventNumber>124.6e3&&CodaEventNumber<126.01e3)&&!(CodaEventNumber>140e3&&CodaEventNumber<150e3)&&!(CodaEventNumber>124.3e3&&CodaEventNumber<125e3)&&!(CodaEventNumber>171.5e3&&CodaEventNumber<173.5e3)&&!(CodaEventNumber>132.5e3&&CodaEventNumber<132.6e3)&&!(CodaEventNumber>54.5e3&&CodaEventNumber<55.1e3)&&!(CodaEventNumber>132e3&&CodaEventNumber<135e3)&&CodaEventNumber>16e3";
  //user_cuts[5474] = "cleandata && bpm4eY>0.78 && bpm12X<-1.2";
  user_cuts[5474] = "cleandata && !(CodaEventNumber<85e3&&CodaEventNumber>78e3)";
  user_cuts[5566] = "";
  user_cuts[5576] = "";
	if (ana == "all") {
    user_cuts[5576] = "((CodaEventNumber>4e3&&CodaEventNumber<8e3)||(CodaEventNumber>11e3&&CodaEventNumber<14.57e3)||(CodaEventNumber>40.8e3&&CodaEventNumber<41.9e3)||(CodaEventNumber>43.2e3&&CodaEventNumber<46e3)||(CodaEventNumber>85.1e3&&CodaEventNumber<85.4e3)||(CodaEventNumber>113.4e3&&CodaEventNumber<115.5e3)||(CodaEventNumber>136.7e3&&CodaEventNumber<137.2e3)||(CodaEventNumber>152.2e3&&CodaEventNumber<152.7e3)||(CodaEventNumber>169e3&&CodaEventNumber<169.7e3)||(CodaEventNumber>192.4e3&&CodaEventNumber<193e3)||(CodaEventNumber>201e3&&CodaEventNumber<203.6e3)||(CodaEventNumber>272e3&&CodaEventNumber<275e3))&&!(CodaEventNumber>60e3&&CodaEventNumber<120e3)&&bpm4eY>1&&bpm12X<-0.7&&CodaEventNumber>10e3&&!(CodaEventNumber>30e3&&CodaEventNumber<60e3)";
  }
  //user_cuts[5653] = "cleandata && !(CodaEventNumber>24600 && CodaEventNumber<25500)",
  // Here is the _more_cuts version that cuts out 3 beam motion points
  user_cuts[5653] = "cleandata && !(CodaEventNumber>24600 && CodaEventNumber<25500)&& !(CodaEventNumber>100e3 && CodaEventNumber<140e3)",
  //user_cuts[5778] = "cleandata && !(scandata2==1) && !(scandata2==2) && !(scandata2==10) && !(CodaEventNumber>99e3 && CodaEventNumber<102e3) && !(CodaEventNumber>108e3 && CodaEventNumber<109e3) && !(CodaEventNumber>145e3 && CodaEventNumber<148e3) && !(CodaEventNumber>155e3 && CodaEventNumber<156e3) && !(CodaEventNumber>150.3e3 && CodaEventNumber<151.4e3) && !(CodaEventNumber>157.5e3 && CodaEventNumber<158.0e3) && bpm4ecX>-0.6 && !(CodaEventNumber>147.9e3 && CodaEventNumber<148.1e3) && bpm1X>1.1";
  user_cuts[5778] = "cleandata && !(scandata2==1) && !(scandata2==2) && !(scandata2==10) && !(CodaEventNumber>99e3 && CodaEventNumber<102e3) && !(CodaEventNumber>108e3 && CodaEventNumber<109e3) && !(CodaEventNumber>145e3 && CodaEventNumber<148e3) && !(CodaEventNumber>155e3 && CodaEventNumber<156e3) && !(CodaEventNumber>150.3e3 && CodaEventNumber<151.4e3) && !(CodaEventNumber>157.5e3 && CodaEventNumber<158.0e3) && !(CodaEventNumber>147.9e3 && CodaEventNumber<148.1e3)";
  user_cuts[6049] = "";
  user_cuts[6176] = "";
  user_cuts[6196] = "cleandata &&  !(Entry$>119e3 && Entry$<121e3) && !(Entry$>130e3 && Entry$<132e3) && bpm4aX>-0.7 && bpm4aX<-0.43 && !(Entry$<56.3e3 && Entry$>55.9e3) && bpm16X<-0.02 &&bpm1X>-0.18";
  user_cuts[6233] = "";
  user_cuts[6242] = "";
  user_cuts[6442] = "";
  user_cuts[6560] = "cleandata&&!(scandata1==130&&CodaEventNumber>55700&&CodaEventNumber<65000)&&!(scandata1==110&&CodaEventNumber>124.6e3)&&!(scandata1==60&&CodaEventNumber>230e3&&CodaEventNumber<235.2e3)";
  user_cuts[6563] = "((CodaEventNumber>36e3&&CodaEventNumber<50e3)||(CodaEventNumber>93.5e3&&CodaEventNumber<100e3)||(CodaEventNumber>110e3&&CodaEventNumber<115e3)||(CodaEventNumber>119.5e3&&CodaEventNumber<123.5e3)||(CodaEventNumber>124.5e3&&CodaEventNumber<126.5e3)||(CodaEventNumber>127e3&&CodaEventNumber<128.4e3)||(CodaEventNumber>135e3&&CodaEventNumber<195e3)||(CodaEventNumber>200e3&&CodaEventNumber<215e3)||(CodaEventNumber>224e3&&CodaEventNumber<239e3)||(CodaEventNumber>242e3&&CodaEventNumber<246e3)||(CodaEventNumber>253.5e3&&CodaEventNumber<258e3)||(CodaEventNumber>266e3&&CodaEventNumber<281e3)||(CodaEventNumber>289e3&&CodaEventNumber<297.5e3)||(CodaEventNumber>309e3&&CodaEventNumber<317e3)||(CodaEventNumber>324e3&&CodaEventNumber<326e3)||(CodaEventNumber>331.5e3&&CodaEventNumber<333.5e3)||(CodaEventNumber>340e3&&CodaEventNumber<346e3)||(CodaEventNumber>355e3&&CodaEventNumber<362e3)||(CodaEventNumber>371e3))&&!(CodaEventNumber<115e3)";
  user_cuts[6739] = "cleandata";
  user_cuts[7121] = "";
  user_cuts[7122] = "cleandata && bpm12X>-1.9";
  user_cuts[7279] = "";
  user_cuts[7282] = "cleandata && bpm4aX<-0.35";
	user_cuts[7406] = "cleandata && bpm4eY>0.28";
  user_cuts[7407] = "";
  // Allow actual user cut from command line to override
  if (user_cut != "") {
    user_cuts[run_num] = user_cut;
  }
  if (user_cuts.count(run_num) == 0) {
    Printf("Error, no user_cut found for run %d",run_num);
    return;
  }

  std::map <Int_t,Double_t> uplimit;
  uplimit[2769] = 200.0;
  uplimit[2963] = 200.0;
  uplimit[2965] = 200.0;
  uplimit[2967] = 200.0;
  uplimit[3098] = 200.0;
  uplimit[3101] = 200.0;
  uplimit[3105] = 200.0;
  uplimit[3106] = 200.0;
  uplimit[3135] = 200.0;
  uplimit[3138] = 200.0;
  uplimit[3395] = 200.0;
  uplimit[3424] = 200.0;
  uplimit[3429] = 200.0;
  uplimit[3430] = 200.0;
  uplimit[3597] = 200.0;
  uplimit[3645] = 200.0;
  uplimit[3735] = 200.0;
  uplimit[3749] = 200.0;
  uplimit[3751] = 200.0;
  uplimit[4229] = 200.0;
  uplimit[4309] = 200.0;
  uplimit[4610] = 200.0;
  uplimit[4650] = 200.0;
  uplimit[4652] = 200.0;
  uplimit[4806] = 200.0;
  uplimit[4872] = 200.0;
  uplimit[4901] = 200.0;
  uplimit[5360] = 200.0;
  uplimit[5396] = 200.0;
  uplimit[5417] = 120.0;
  uplimit[5434] = 200.0;
  // uplimit[5436] = 150.0;
  uplimit[5436] = 145.0;
  uplimit[5474] = 175.0;
  uplimit[5566] = 200.0;
  uplimit[5576] = 200.0;
  uplimit[5653] = 180.0;
  uplimit[5778] = 175.0;
  uplimit[6049] = 175.0;
  uplimit[6176] = 175.0;
  uplimit[6196] = 175.0;
  uplimit[6233] = 200.0;
	uplimit[6242] = 200.0;
  uplimit[6442] = 200.0;
  uplimit[6560] = 200.0;
  uplimit[6563] = 200.0;
  uplimit[6739] = 200.0;
  uplimit[7121] = 200.0;
  uplimit[7122] = 200.0;
  uplimit[7279] = 200.0;
  uplimit[7282] = 200.0;
  uplimit[7406] = 200.0;
  uplimit[7407] = 200.0;

  std::map <Int_t,Double_t> lowlimit;
  lowlimit[2769] = 5.0;
  lowlimit[2963] = 5.0;
  lowlimit[2965] = 5.0;
  lowlimit[2967] = 2.0;
  lowlimit[3098] = 5.0;
  lowlimit[3101] = 5.0;
  lowlimit[3105] = 5.0;
  //lowlimit[3105] = 44.0;
  //lowlimit[3106] = 45.0;
  lowlimit[3106] = 5.0;
  lowlimit[3135] = 5.0;
  lowlimit[3138] = 5.0;
  //lowlimit[3138] = 37.0;
  lowlimit[3395] = 5.0;
  lowlimit[3424] = 5.0;
  lowlimit[3429] = 5.0;
  lowlimit[3430] = 15.0;
  lowlimit[3597] = 5.0;
  lowlimit[3645] = 5.0;
  lowlimit[3735] = 5.0;
  lowlimit[3749] = 5.0;
  lowlimit[3751] = 5.0;
  lowlimit[4229] = 5.0;
  lowlimit[4309] = 5.0;
  lowlimit[4610] = 5.0;
  lowlimit[4650] = 5.0;
  lowlimit[4652] = 5.0;
  lowlimit[4806] = 5.0;
  // FIXME see the hack if condition at the Pedestal Fit line
  //55.0; 55 uA is the low cut for main detector case
  lowlimit[4872] = 5.0;
  lowlimit[4901] = 5.0;
  lowlimit[5360] = 5.0;
  lowlimit[5396] = 5.0;
  lowlimit[5417] = 10.0;
  lowlimit[5434] = 5.0;
  // lowlimit[5436] = 30.0;
  lowlimit[5436] = 5.0;
  lowlimit[5474] = 5.0;
  //lowlimit[5474] = 20.0;
  lowlimit[5566] = 5.0;
  lowlimit[5576] = 5.0;
  lowlimit[5653] = 10.0;
  lowlimit[5778] = 10.0;
  lowlimit[6049] = 5.0;
  lowlimit[6176] = 5.0;
  lowlimit[6196] = 20.0;
  lowlimit[6233] = 5.0;
  lowlimit[6242] = 5.0;
  lowlimit[6442] = 5.0;
  lowlimit[6560] = 5.0;
  lowlimit[6563] = 5.0;
  lowlimit[6739] = 5.0;
  lowlimit[7121] = 5.0;
  lowlimit[7122] = 5.0;
  lowlimit[7279] = 5.0;
  lowlimit[7282] = 5.0;
  lowlimit[7406] = 5.0;
  lowlimit[7407] = 5.0;

  // Allow user to overwrite from command line
  if (user_uplimit != -100.0) {
    uplimit[run_num] = user_uplimit;
  }
  if (user_lowlimit != -100.0) {
    lowlimit[run_num] = user_lowlimit;
  }

  if (lowlimit.count(run_num) == 0 || uplimit.count(run_num) == 0){
    Printf("Error, run %d fit limits not included in macro",run_num);
    return;
  }

  std::map <Int_t,TString> user_bcms; 
  user_bcms[2965] ="bcm_an_us";
  user_bcms[3098] ="bcm_an_us";
  user_bcms[3105] ="bcm_an_us";
  user_bcms[3106] ="bcm_an_us";
  user_bcms[3138] ="bcm_an_us";
  //user_bcms[3138] ="bcm_an_ds3";
  user_bcms[3395] ="bcm_an_ds";
  user_bcms[3424] ="bcm_an_ds";
  user_bcms[3429] ="bcm_an_ds";
  user_bcms[3430] ="bcm_an_ds";
  user_bcms[3597] ="bcm_dg_ds";
  user_bcms[3645] ="bcm_dg_ds";
  user_bcms[3751] ="bcm_an_ds";
  user_bcms[4229] ="bcm_dg_ds";
  user_bcms[4309] ="bcm_dg_ds";
  user_bcms[4610] ="bcm_dg_ds";
  user_bcms[4650] ="bcm_dg_ds";
  user_bcms[4652] ="bcm_dg_ds";
  user_bcms[4806] ="bcm_dg_ds";
  user_bcms[4872] ="bcm_an_us";
  user_bcms[5417] ="bcm_an_us";
  user_bcms[5436] ="bcm_an_us";
  user_bcms[5474] ="bcm_an_us";
  user_bcms[5576] ="bcm_an_us";
  user_bcms[5653] ="bcm_an_us";
  user_bcms[5778] ="bcm_an_us";
  user_bcms[6196] ="bcm_an_us";
  user_bcms[6560] ="bcm_an_us";
  user_bcms[6563] ="bcm_an_us";
  //user_bcms[6731] ="bcm_an_us"; // Transverse running Ca40 2x2
  //user_bcms[6732] ="bcm_an_us"; // Transverse running Pb 4x4 
  user_bcms[6739] ="bcm_an_us";
  //user_bcms[6758] ="bcm_an_us"; // Transverse running Carbon 2x2
  user_bcms[7122] ="bcm_an_us";
  user_bcms[7282] ="bcm_an_us";
  user_bcms[7406] ="bcm_an_us";
  std::map <Int_t,Double_t> user_peds;
  user_peds[2965] = -510.43;
  user_peds[3098] = -510.43;
  user_peds[3105] = -821.75;//-510.43;
  user_peds[3106] = -821.75;//-510.43;
  user_peds[3138] = -821.75;//-510.43;
  //user_peds[3138] = -1226.03;//-510.43;
  user_peds[3395] = -387.81;
  user_peds[3424] = -387.81;
  user_peds[3429] = -387.81;
  user_peds[3430] = -387.81;
  user_peds[3597] = -210.26;
  user_peds[3645] = -500.22;
  user_peds[3751] = -537.44;
  user_peds[4229] = -351.06;
  user_peds[4309] = -351.06;
  user_peds[4610] = -351.06;
  user_peds[4650] = -351.06;
  user_peds[4652] = -351.06;
  user_peds[4806] = -351.06;
  user_peds[4872] = -650.95;
  user_peds[5417] = -597.00;
  user_peds[5436] = -597.00;
  user_peds[5474] = -597.00;
  user_peds[5576] = -645.63;
  user_peds[5653] = -645.63;
  user_peds[5778] = -645.63;
  //user_peds[6196] = -37.94; // "Original macro was wrong"
  user_peds[6196] = -539.12; // Most recent pedestal was -539.12 from run 6176
  //user_peds[6196] = -288.54; // ?
  user_peds[6560] = -636.04;
  user_peds[6563] = -636.04;
  user_peds[6739] = -636.04;
  user_peds[7122] = -640.65;
  user_peds[7282] = -651.7;
  user_peds[7406] = -621.09;
  std::map <Int_t,Double_t> user_norms;
  user_norms[2965] = 0.002693;
  user_norms[3098] = 0.002635;
  user_norms[3105] = 0.002635;
  user_norms[3106] = 0.002635;
  user_norms[3138] = 0.002635;//0.00082918740;
  //user_norms[3138] = 0.000799;
  user_norms[3395] = 0.002774;
  user_norms[3424] = 0.002774;
  user_norms[3429] = 0.002774;
  user_norms[3430] = 0.002774;
  user_norms[3597] = 0.002717;
  user_norms[3645] = 0.002338;
  user_norms[3751] = 0.006133;
  user_norms[4229] = 0.002318;
  user_norms[4309] = 0.002318;
  user_norms[4610] = 0.002318;
  user_norms[4650] = 0.002318;
  user_norms[4652] = 0.002318;
  user_norms[4806] = 0.002318;
  user_norms[4872] = 0.006184;
  user_norms[5417] = 0.008975;
  user_norms[5436] = 0.008975;
  user_norms[5474] = 0.008975;
  user_norms[5576] = 0.008924;
  user_norms[5653] = 0.008924;
  user_norms[5778] = 0.008924;
  //user_norms[6196] = 0.009289; // "Original" macro was wrong
  user_norms[6196] = 0.0089691; // Most recent gain was 0.0089691 from run 6176 
  //user_norms[6196] = 0.0091642; // ?
  user_norms[6560] = 0.008921;
  user_norms[6563] = 0.008921;
  user_norms[6739] = 0.008921;
  user_norms[7122] = 0.008923;
  user_norms[7282] = 0.008923;
  user_norms[7406] = 0.008934;

  if (ana == "all") {
    if (user_bcm != "") {
      user_bcms[run_num] = user_bcm;
    }
    if (user_ped != -1000000) {
      user_peds[run_num] = user_ped;
    }
    if (user_norm != -1000000) {
      user_norms[run_num] = user_norm;
    }

    if (user_bcms.count(run_num) == 0 || user_peds.count(run_num) == 0 || user_norms.count(run_num) == 0){
      Printf("Error, run %d pedestal info not included in macro",run_num);
      return;
    }
  }

  // Write out the label
  if (version == "old") {
    label = "RMS/sqrt(nentries) error bar, fit w.r.t. ";
  }
  if (version == "new") {
    label = "RMS error bar, fit w.r.t. ";
  }
  if (type == "orig") {
    label = label + "prompt calibrated ";
  }
  if (type == "global") {
    label = label + "global calibrated ";
  }
  if (ana == "unser") {
    label = label + "unser";
  }
  if (ana == "all") {
    label = label + user_bcms.at(run_num);
  }

  double bcm_mean[ndata]; 
  double bcm_error[ndata];
  double unser_mean[ndata]; 
  double unser_ped[ndata]; 
  double unser_error[ndata];
  double unserped_error[ndata];
  double bcm_res[ndata]; // residual
  double bcm_res_error[ndata]; // residual

  TString gfit_title;
  TString gres_title;
  TH1D *h_stat;
  double ped[nbcm];
  double gain[nbcm];
  double slope[nbcm];
  double p0err[nbcm];
  double p1err[nbcm];


  TGraphErrors *g_unser;
  //TMultiGraph *g_unser_t = new TMultiGraph();  
  TGraph *g_unser_t1;
  TGraph *g_unser_t2;
  TGraph *g_relative;
  TGraph *g_relative_t1;
  TGraph *g_relative_t2;
  TGraphErrors *g_res;
  TGraphErrors *g_fit;
  TGraphErrors *g_res_ref;
  TGraphErrors *g_fit_ref;
  TMultiGraph *mg_res;
  TMultiGraph *mg_fit;

  TF1 *f_fit = new TF1("f_fit","[1]*x+[0]",-10,10e3); // x : current
  f_fit->SetParName(0,"Pedestal");
  f_fit->SetParName(1,"Slope");
  double init_par[2] = { 1e3,1e-3};

  TString full_ped_cut = "";
  TCut my_cut = "";
  for(int i=0;i<ndata;i++){
    my_cut = beam_evtcut[run_num].at(i)+user_cuts[run_num];
    if (ana == "unser") {
      full_ped_cut = full_ped_cut + "||" + (TString)pedestal_evtcut[run_num].at(i);
      tree->Draw("unser",
          my_cut,"goff");
    }
    else if (ana == "all") {
      tree->Draw(Form("(%s.hw_sum_raw/%s.num_samples-%f)*%f",user_bcms.at(run_num).Data(),user_bcms.at(run_num).Data(),user_peds.at(run_num),user_norms.at(run_num)),
          my_cut,"goff");
    }
    h_stat =(TH1D*)gDirectory->FindObject("htemp");
    if (h_stat!=0) {
      unser_mean[i] = h_stat->GetMean();
      //unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      if (version == "new") {
        unser_error[i] = h_stat->GetRMS();///TMath::Sqrt(h_stat->GetEntries());
      }
      else if (version == "old") {
        unser_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
      }
    }
    if (ana == "unser") {
      my_cut = pedestal_evtcut[run_num].at(i);
      //FIXME needs to be specific per run
      tree->Draw("unser",
          my_cut,"goff");
      h_stat =(TH1D*)gDirectory->FindObject("htemp");
      if (h_stat!=0) {
        unser_ped[i]=h_stat->GetMean();
        unser_mean[i]  = unser_mean[i] - unser_ped[i]; // pedestal subtraction
        if (version == "new") 
        {
          unserped_error[i] = h_stat->GetRMS();///TMath::Sqrt(h_stat->GetEntries());
          unser_error[i]= sqrt(pow(unser_error[i],2)+pow(unserped_error[i],2));
        }
      }
    }
    else {
      //Printf("Not Unser analysis, no beam off assumed");
    }
    cout << unser_mean[i]
      << "+/-"
      << unser_error[i];
    if (version == "new") {
      cout << " ped "
      << unser_ped[i]
      << "+/-"
      << unserped_error[i]
      << endl;
    }
    else if (version == "old") {
      cout << endl;
    }
  }
  // QA Plots for Unser scans
  if (ana == "unser") {
    TVirtualPad* pad_buff;
    c_all->Clear();
    c_all->Divide(2,1);
    pad_buff = c_all->cd(1);
    g_unser = new TGraphErrors(ndata,unser_mean,unser_ped,unser_error,unserped_error);
    g_unser->SetMarkerStyle(33);
    g_unser->SetMarkerSize(0.7);
    g_unser->Draw("AP");
    g_unser->SetTitle(Form("Unser mean vs. Pedestal; UNSER (uA); UNSER Pedestal (uA)"));
    //g_unser->Fit("p0","QR");

    pad_buff = c_all->cd(2);
    tree->Draw(Form("%s:CodaEventNumber","unser"),"unser<15","");
    g_unser_t1 = (TGraph*)pad_buff->FindObject("Graph");
    if(g_unser_t1!=0){
      g_unser_t1->SetNameTitle("GraphAll",Form("Unser pedestal vs. time, red is cut out; UNSER Pedestal (uA); CodaEventNumber"));
      g_unser_t1->GetHistogram()->SetTitle(Form("Unser pedestal vs. time, red is cut out"));
      g_unser_t1->SetTitle(Form("Unser pedestal vs. time, red is cut out; UNSER Pedestal (uA); CodaEventNumber"));
      g_unser_t1->SetMarkerColor(kRed);
      //  g_unser_t1->SetMarkerSize(0.5);
    }
    tree->Draw(Form("%s:CodaEventNumber","unser"),Form("unser<15&&(1==0%s)",full_ped_cut.Data()),"same");
    g_unser_t2 = (TGraph*)pad_buff->FindObject("Graph");
    if(g_unser_t2!=0){
      g_unser_t2->SetNameTitle("GraphAll",Form("Unser pedestal vs. time, red is cut out; UNSER Pedestal (uA); CodaEventNumber"));
      g_unser_t2->GetHistogram()->SetTitle(Form("Unser pedestal vs. time, red is cut out"));
      g_unser_t2->SetTitle(Form("Unser pedestal vs. time, red is cut out; UNSER Pedestal (uA); CodaEventNumber"));
      g_unser_t2->SetMarkerColor(kBlue);
      //  g_unser_t2->SetMarkerSize(0.5);
    }
    c_all->cd();
    plot_title = Form("Run %d: %s", run_num, label.Data());
    TText *t1 = new TText(0.0,0.007,plot_title);
    t1->SetNDC();
    t1->SetTextSize(0.05);
    t1->Draw("same");
    c_all->SaveAs(Form("%s/run%d_verify_%s_ped_%s.png",outputDir.Data(),run_num,ana.Data(),version.Data()));
    //gSystem->Exec(Form("convert $(ls -rt %s/run%d*_unser_ped_%s.png) %s/run%d_unser_ped_%s_%s.pdf",outputDir.Data(),run_num,version.Data(),outputDir.Data(),run_num,version.Data(),type.Data()));
    //gSystem->Exec(Form("rm -f %s/run%d_unser_ped_%s.png",outputDir.Data(),run_num,version.Data()));
  }
  // Pedestal fits for Unser or ALL scans
  for(int ibcm=0;ibcm<nbcm;ibcm++){
    branch_name = Form("%s.hw_sum_raw/%s.num_samples",
        device_name[ibcm].Data(),device_name[ibcm].Data());

    TString full_beamon_cut = "(1==0";
    //TString full_beamon_cut = (TString)user_cuts[run_num] + "&& (1==0";
    for(int i=0;i<ndata;i++){
      my_cut = beam_evtcut[run_num].at(i)+user_cuts[run_num];
      full_beamon_cut = full_beamon_cut + "||" + (TString)beam_evtcut[run_num].at(i);
      tree->Draw(branch_name,
          my_cut,"goff");
      h_stat =(TH1D*)gDirectory->FindObject("htemp");
      if (h_stat!=0) {
        bcm_mean[i] = h_stat->GetMean();
        //bcm_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
        if (version == "new") {
          bcm_error[i] = h_stat->GetRMS();///TMath::Sqrt(h_stat->GetEntries());
        }
        else if (version == "old") {
          bcm_error[i] = h_stat->GetRMS()/TMath::Sqrt(h_stat->GetEntries());
        }
      }
    }
    full_beamon_cut = full_beamon_cut + ")";
    if (user_cuts[run_num] != "") { 
     full_beamon_cut = full_beamon_cut + " && " + (TString)user_cuts[run_num];
    }
    // QA Plots for ALL scans
    if (ana == "all") {
      TVirtualPad* pad_buff;
      c_all->Clear();
      c_all->Divide(2,1);
      pad_buff = c_all->cd(1);
      tree->Draw(Form("%s:%s",device_name[ibcm].Data(),user_bcms.at(run_num).Data()),full_beamon_cut.Data(),"");
      g_relative = (TGraph*)pad_buff->FindObject("Graph");
      if (g_relative!=0) {
        g_relative->SetMarkerStyle(33);
        g_relative->SetMarkerSize(0.7);
        //g_relative->Draw("AP");
        g_relative->SetNameTitle("GraphOne",Form("%s vs. %s; %s; %s (uA)",device_name[ibcm].Data(),user_bcms.at(run_num).Data(),device_name[ibcm].Data(),user_bcms.at(run_num).Data()));
        g_relative->GetHistogram()->SetTitle(Form("%s vs. %s",device_name[ibcm].Data(),user_bcms.at(run_num).Data()));
        g_relative->SetTitle(Form("%s vs. %s; %s; %s (uA)",device_name[ibcm].Data(),user_bcms.at(run_num).Data(),device_name[ibcm].Data(),user_bcms.at(run_num).Data()));
        //g_relative->Fit("p0","QR");
      }
      c_all->cd(2);
      pad_buff = c_all->cd(2);
      tree->Draw(Form("%s:CodaEventNumber",device_name[ibcm].Data()),full_beamon_cut.Data(),"");
      g_relative_t1 = (TGraph*)pad_buff->FindObject("Graph");
      if(g_relative_t1!=0){
        g_relative_t1->SetNameTitle("GraphAllDets",Form("%s vs. time, red is cut out; %s; CodaEventNumber",device_name[ibcm].Data(),device_name[ibcm].Data()));
        g_relative_t1->GetHistogram()->SetTitle(Form("%s vs. time, red is cut out",device_name[ibcm].Data()));
        g_relative_t1->SetTitle(Form("%s vs. time, red is cut out; %s; CodaEventNumber",device_name[ibcm].Data(),device_name[ibcm].Data()));
        g_relative_t1->SetMarkerColor(kBlue);
        //  g_relative_t1->SetMarkerSize(0.5);
      }
      tree->Draw(Form("%s:CodaEventNumber",device_name[ibcm].Data()),Form("!(%s)",full_beamon_cut.Data()),"same");
      g_relative_t2 = (TGraph*)pad_buff->FindObject("Graph");
      if(g_relative_t2!=0){
        g_relative_t2->SetNameTitle("GraphAllDets",Form("%s vs. time, red is cut out; %s; CodaEventNumber",device_name[ibcm].Data(),device_name[ibcm].Data()));
        g_relative_t2->GetHistogram()->SetTitle(Form("%s vs. time, red is cut out",device_name[ibcm].Data()));
        g_relative_t2->SetTitle(Form("%s vs. time, red is cut out; %s; CodaEventNumber",device_name[ibcm].Data(),device_name[ibcm].Data()));
        g_relative_t2->SetMarkerColor(kRed);
        //  g_relative_t2->SetMarkerSize(0.5);
      }
      c_all->cd();
      plot_title = Form("Run %d: %s with %s", run_num, device_name[ibcm].Data(), label.Data());
      TText *t1 = new TText(0.0,0.007,plot_title);
      t1->SetNDC();
      t1->SetTextSize(0.05);
      t1->Draw("same");
      c_all->SaveAs(Form("%s/run%d_verify_%s_%s_ped_%s.png",outputDir.Data(),run_num,device_name[ibcm].Data(),ana.Data(),version.Data()));
    }
    if (device_blacknumbers[run_num].at(ibcm) > 1) { continue; }
    c_bcm->Clear();
    c_bcm->Divide(2,1);
    c_bcm->cd(1);

    g_fit = new TGraphErrors(ndata,unser_mean,bcm_mean,unser_error,bcm_error);
    g_fit->SetMarkerStyle(33);
    g_fit->SetMarkerSize(0.7);
    g_fit->Draw("AP");
    if (ana == "unser") {
      gfit_title = Form("%s; UNSER(uA); Raw ADC per Sample",branch_name.Data());
    }
    if (ana == "all") {
      gfit_title = Form("%s; %s; Raw ADC per Sample",branch_name.Data(),user_bcms.at(run_num).Data());
    }
    g_fit->SetTitle(gfit_title.Data());
    f_fit->SetParameters(init_par);
    f_fit->SetLineColor(kRed);
    f_fit->SetLineWidth(1);

    // Pedestal Fit
    if(branch_name.Contains("ds10"))
    {
      // DS 10x is a mess
      g_fit->Fit("f_fit","QR","",30,100);
    }
    else
    {
      if (((run_num < 3583 && branch_name.Contains("dsl")) || (run_num<3733 && branch_name.Contains("dsr"))) && lowlimit[run_num]<35)
      {
        // DS main detectors get a limited high current range to fit, due to non-linear thicker quartz
        g_fit->Fit("f_fit","QR","",35,uplimit[run_num]); 
      }
      if (run_num == 4872 && (branch_name.Contains("usl") || branch_name.Contains("usr") || branch_name.Contains("dsl") || branch_name.Contains("dsr") || branch_name.Contains("atl") || branch_name.Contains("atr")) && lowlimit[run_num]<55)  
      {
        // Run 4872 has a 55 lower limit
        g_fit->Fit("f_fit","QR","",55,uplimit[run_num]); 
      }
      else
      {
        g_fit->Fit("f_fit","QR","",lowlimit[run_num],uplimit[run_num]);
      }
    }

    ped[ibcm] = f_fit->GetParameter(0);
    slope[ibcm]=f_fit->GetParameter(1);
    gain[ibcm] = 1/slope[ibcm];

    /* Moved to right after ROOT file print, before text file print
    if(ana == "all" && (device_name[ibcm]=="usl"||device_name[ibcm]=="usr"||device_name[ibcm]=="atl1"||device_name[ibcm]=="atl2"||device_name[ibcm]=="atr1"||device_name[ibcm]=="atr2"||device_name[ibcm]=="sam1"||device_name[ibcm]=="sam2"||device_name[ibcm]=="sam3"||device_name[ibcm]=="sam4"||device_name[ibcm]=="sam5"||device_name[ibcm]=="sam6"||device_name[ibcm]=="sam7"||device_name[ibcm]=="sam8")){
      gain[ibcm]=76.293e-6;
    }*/

    p0err[ibcm]=f_fit->GetParError(0);      
    p1err[ibcm]=f_fit->GetParError(1);      
    for(int i=0;i<ndata;i++){
      if (ana == "all") {
        bcm_res[i] = (bcm_mean[i]-ped[ibcm]) - unser_mean[i]*slope[ibcm];
        bcm_res_error[i] = sqrt(pow(bcm_error[i],2)+pow(unser_error[i]*slope[ibcm],2)+pow(p0err[ibcm],2)+pow(p1err[ibcm]*unser_mean[i],2)+pow(p0err[ibcm],2));
        //	bcm_res_error[i] = sqrt(pow(bcm_error[i] *gain[ibcm],2)+pow(p0err[ibcm]*gain[ibcm],2)+pow(p1err[ibcm]*(bcm_mean[i]-ped[ibcm])*pow(gain[ibcm],2),2)+pow(p0err[ibcm]*gain[ibcm],2));

        // bcm_res[i] = (bcm_mean[i]-ped[ibcm]) - unser_mean[i]*slope[ibcm];
        // bcm_res_error[i] = sqrt(pow(bcm_error[i],2)+pow(unser_error[i]*slope[ibcm],2));
        //
      }
      if (ana == "unser") {
        bcm_res[i] = (bcm_mean[i]-ped[ibcm])*gain[ibcm] - unser_mean[i];
        if (version == "new") {
          bcm_res_error[i] = sqrt(pow(bcm_error[i] *gain[ibcm],2)+pow(unser_error[i],2)+pow(p0err[ibcm]*gain[ibcm],2)+pow(p1err[ibcm]*(bcm_mean[i]-ped[ibcm])*pow(gain[ibcm],2),2)+pow(p0err[ibcm]*gain[ibcm],2));
          //	bcm_res_error[i] = sqrt(pow(bcm_error[i] *gain[ibcm],2)+pow(p0err[ibcm]*gain[ibcm],2)+pow(p1err[ibcm]*(bcm_mean[i]-ped[ibcm])*pow(gain[ibcm],2),2)+pow(p0err[ibcm]*gain[ibcm],2));
          // bcm_res[i] = (bcm_mean[i]-ped[ibcm]) - unser_mean[i]*slope[ibcm];
          // bcm_res_error[i] = sqrt(pow(bcm_error[i],2)+pow(unser_error[i]*slope[ibcm],2));
        }
        else if (version == "old") {
          bcm_res_error[i] = bcm_error[i] *gain[ibcm];
        }
      }
    }

    c_bcm->cd(2);
    g_res = new TGraphErrors(ndata,unser_mean,bcm_res,unser_error,bcm_res_error);
    g_res->SetMarkerStyle(33);
    g_res->SetMarkerSize(0.7);
    g_res->Draw("AP");
    if (ana == "unser") {
      gres_title = Form("%s;UNSER(uA); Residual(uA) ",branch_name.Data());
    }
    if (ana == "all") {
      gres_title = Form("%s;%s; Residual(counts) ",branch_name.Data(),user_bcms.at(run_num).Data());
    }
    g_res->SetTitle(gres_title.Data());
    //f_zero->Draw("same");
    // Residual fit over same range
    if(branch_name.Contains("ds10"))
      g_fit->Fit("f_fit","QR","",30,100);
    else
      g_res->Fit("f_zero","QR","",lowlimit[run_num],uplimit[run_num]);

    plot_title = Form("Run %d: %s with %s", run_num, device_name[ibcm].Data(), label.Data());
    c_bcm->cd();
    TText *t1 = new TText(0.0,0.007,plot_title);
    t1->SetNDC();
    t1->SetTextSize(0.05);
    t1->Draw("same");

    c_bcm->SaveAs(Form("%s/run%d_%s_%s_ped_fit_%dto%d_%s_witherr.png",outputDir.Data(),run_num,ana.Data(),device_name[ibcm].Data(),(Int_t)lowlimit[run_num],(Int_t)uplimit[run_num],version.Data()));
  } // End of BCMs loop
  gSystem->Exec(Form("convert $(ls -rt %s/run%d_verify*_%s_ped_%s.png) %s/run%d_%s_ped_%s_%s.pdf",outputDir.Data(),run_num,ana.Data(),version.Data(),outputDir.Data(),run_num,ana.Data(),version.Data(),type.Data()));
  gSystem->Exec(Form("rm -f %s/run%d_verify*_%s_ped_%s.png",outputDir.Data(),run_num,ana.Data(),version.Data()));

  gSystem->Exec(Form("convert $(ls -rt %s/run%d*_ped_fit_%dto%d_%s_witherr.png) %s/run%d_%s_pedestal_fit_%dto%d_%s_witherr_Caryn_%s.pdf",outputDir.Data(),run_num,(Int_t)lowlimit[run_num],(Int_t)uplimit[run_num],version.Data(),outputDir.Data(),run_num,ana.Data(),(Int_t)lowlimit[run_num],(Int_t)uplimit[run_num],version.Data(),type.Data()));
  gSystem->Exec(Form("rm -f %s/run%d_*_ped_fit_%dto%d_%s_witherr.png",outputDir.Data(),run_num,(Int_t)lowlimit[run_num],(Int_t)uplimit[run_num],version.Data()));

  PrintRootFile((Double_t)run_num,(Double_t)ndata,ana,version,type,device_name,device_blacknumbers[run_num],ped,p0err,gain);

  FILE *pFile;
  pFile = fopen(Form("%s/%s_pedestals_%s_%s_%d.txt",outputDir.Data(),ana.Data(),version.Data(),type.Data(),run_num),"w+");
  fprintf(pFile, "Run %d, Unser scaled pedestals\n",run_num);
  fprintf(pFile, "Device Name, Pedestal, Pedestal Error, Gain scale factor\n");
  printf("Run %d, Unser scaled pedestals\n",run_num);
  printf("Device Name, Pedestal, Gain scale factor\n");
  for(int ibcm=0;ibcm<nbcm;ibcm++){
    if(ana == "all" && (device_name[ibcm]=="usl"||device_name[ibcm]=="usr"||device_name[ibcm]=="atl1"||device_name[ibcm]=="atl2"||device_name[ibcm]=="atr1"||device_name[ibcm]=="atr2"||device_name[ibcm]=="sam1"||device_name[ibcm]=="sam2"||device_name[ibcm]=="sam3"||device_name[ibcm]=="sam4"||device_name[ibcm]=="sam5"||device_name[ibcm]=="sam6"||device_name[ibcm]=="sam7"||device_name[ibcm]=="sam8")){
      gain[ibcm]=76.293e-6;
    }    

    if (device_blacknumbers[run_num].at(ibcm) > 1) { continue; }
    if (device_blacknumbers[run_num].at(ibcm) == 0)
    {
      fprintf(pFile, "!");
      printf("!");
    }
    fprintf(pFile, "%s,%.2f,%.2f,%f\n",
        device_name[ibcm].Data(),
        ped[ibcm],p0err[ibcm],
        gain[ibcm]);
    printf("%s, %.2f, %f \n",
        device_name[ibcm].Data(),
        ped[ibcm],
        gain[ibcm]);
  }
  fclose(pFile);
  rootfile->Close();
}

void PrintRootFile(Double_t run_num, Double_t n_data, TString ana, TString version, TString type, std::vector<TString> device_names, std::vector<Int_t> validity, Double_t* pedestals, Double_t* pedestal_errors, Double_t* gains)
{
  // Get old file, old tree and set top branch address
  cout << "Clearing prior instances of run_number " << run_num << endl;
  TString outputFileName = Form("%s_%s_pedestals_CREX_%s.root",version.Data(),ana.Data(),type.Data());
  if (run_num<5000.0) {
    outputFileName = Form("%s_%s_pedestals_PREX_%s.root",version.Data(),ana.Data(),type.Data());
  }
  Bool_t newFile = gSystem->AccessPathName(outputFileName.Data());
  if (!newFile){
    TFile* oldfile = TFile::Open(outputFileName.Data());
    TTree *oldtree;
    oldfile->GetObject("ped", oldtree);
    const auto nentries = oldtree->GetEntries();

    TFile newfile(Form("localTmp_%d.root",(Int_t)run_num), "recreate");
    auto newtree = oldtree->CloneTree(0);
    TLeaf* runNumL = oldtree->GetLeaf("run_number");
    Double_t tmpRunNum = 0.0;
    // Remove prior copies of run_num indexed entries
    for (auto i : ROOT::TSeqI(nentries)) {
      runNumL->GetBranch()->GetEntry(i);
      tmpRunNum = runNumL->GetValue();
      if (tmpRunNum==run_num) continue;
      oldtree->GetEntry(i);
      newtree->Fill();
    }
    // Fill New data into the tree at the end
    //for (Int_t i = 0; i < sizeof(device_names)/sizeof(*device_names) ; i++)
    newtree->SetBranchAddress(Form("run_number"),&run_num);
    newtree->SetBranchAddress(Form("n_current_points"),&n_data);
    for (Int_t i = 0; i < device_names.size() ; i++) {
      if (validity.at(i) > 1) { continue; } // Ignore BPM entries
      newtree->SetBranchAddress(Form("%s_pedestal",device_names[i].Data()),&pedestals[i]);
      newtree->SetBranchAddress(Form("%s_pedestal_error",device_names[i].Data()),&pedestal_errors[i]);
      newtree->SetBranchAddress(Form("%s_gain",device_names[i].Data()),&gains[i]);
      newtree->SetBranchAddress(Form("%s_valid",device_names[i].Data()),&validity[i]);
    }
    newtree->Fill();

    // Write
    newtree->Write("ped",TObject::kOverwrite);
    newfile.Close();
    oldfile->Close();
    delete oldfile;
    gSystem->Exec(Form("mv localTmp_%d.root %s",(Int_t)run_num,outputFileName.Data()));
  }
  else {
    TFile newfile(outputFileName.Data(),"recreate");
    TTree * newtree = new TTree();
    newtree->Branch(Form("run_number"),&run_num);
    newtree->Branch(Form("n_current_points"),&n_data);
    for (Int_t i = 0; i < device_names.size() ; i++) {
      if (validity.at(i) > 1) { continue; } // Ignore BPM entries
      newtree->Branch(Form("%s_pedestal",device_names[i].Data()),&pedestals[i]);
      newtree->Branch(Form("%s_pedestal_error",device_names[i].Data()),&pedestal_errors[i]);
      newtree->Branch(Form("%s_gain",device_names[i].Data()),&gains[i]);
      newtree->Branch(Form("%s_valid",device_names[i].Data()),&validity[i]);
    }
    newtree->Fill();
    newtree->Write("ped",TObject::kOverwrite);
    newfile.Close();
  }
}

void LoadStyle(){
  gROOT->SetStyle("Plain");
  gStyle->SetStatH(0.15);
  gStyle->SetStatW(0.2);
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(1111);
  gStyle->SetStatX(0.55);
  gStyle->SetStatY(0.92);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetPadColor(39); 
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadRightMargin(0.01);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetLabelSize(0.035,"x");
  gStyle->SetLabelSize(0.035,"y");
  gStyle->SetTitleSize(0.05,"hxyz");
  gStyle->SetTitleOffset(1.5,"y");
  gROOT->ForceStyle();  
}
