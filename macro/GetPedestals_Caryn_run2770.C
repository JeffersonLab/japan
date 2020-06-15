#include "GetBPMPedestal_injector_Caryn.C"
#include "GetHallBPMPedestal_Caryn.C"
#include "GetHallBCMPedestal_Caryn.C"
#include "GetBCMPedestal_injector_Caryn.C"
#include "GetSAMPedestal_Caryn.C"
#include "GetMainDetPedestal_Caryn.C"
#include "GetHallCavPedestal_Caryn.C"

void GetPedestals_Caryn_run2770(int run_num = 1,TString ucut = "1"){
  //.x GetPedestals_Caryn_run2769.C(2769,"1")

  //To do a pedestal run, use scandata1 as monitor for current in injector. Use scandata2=1,2,3,4...10 to enumerate each current setting. Use scandata2=100,200....1000 to enumerate the beam off before and after each current setpoint for the unser pedestals. Currently maximum is 10 data points, suggest 10uA,20,30,40,50,60,65,70,75,80 so scandata1=104,206,301,409,510,631,652,702,753,805 for example. Fit 65,70,75uA (640<scandata1<760)...May wish to do pedestals with target in for main detectors and SAMs. This means raster is on.  

    TString rf_name =Form("$QW_ROOTFILES/prexALL_%d.000.root",run_num);
  TFile *rootfile = TFile::Open(rf_name);
  //run 2567

  TString injcut = Form("%s&&!(scandata2==349&&scandata2==4)&&scandata1!=567",ucut.Data());

  TString hallcut = Form("%s&&!(scandata2==349&&scandata2==4)",ucut.Data());


     GetBPMPedestal_injector_Caryn(run_num,"scandata1",injcut,500,800,1);
      GetBCMPedestal_injector_Caryn(run_num,"scandata1/10.",injcut,40,80,2);

        GetHallBPMPedestal_Caryn(run_num,"bcm_an_us",hallcut,50,80,3);
       GetHallBCMPedestal_Caryn(run_num,hallcut,"bcm_an_us",50,80,4);
   //   GetSAMPedestal_Caryn(run_num,hallcut,"bcm_an_us",50,80,5);
   //   GetMainDetPedestal_Caryn(run_num,hallcut,"bcm_an_su",50,80,6);


   rootfile->Close();


}
