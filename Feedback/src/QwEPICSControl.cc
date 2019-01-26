#include "QwEPICSControl.h"

QwEPICSControl::QwEPICSControl()
{
  Int_t status;
  status = ca_search("IGLdac3:ao_7", &fIDHall_A_IA);//IA Cell Setpoint  Hall A  
  status = ca_pend_io(10);
  status = ca_search("IGLdac3:ao_4", &fIDHall_C_IA);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  //updated PC values
  status = ca_search("C1068_QDAC01", &fIDPockels_Cell_plus);//PC (PITA) POS
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC02", &fIDPockels_Cell_minus);//PC (PITA) NEG 
  status = ca_pend_io(10);

  //new Hall C IA variables
  status = ca_search("C1068_QDAC11", &fIDHall_C_IA_A0);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC12", &fIDHall_C_IA_A1);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC13", &fIDHall_C_IA_A2);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC14", &fIDHall_C_IA_A3);//IA Cell Setpoint  Hall C
  status = ca_pend_io(10);

  //new Hall A IA variables
  status = ca_search("C1068_QDAC03", &fIDHall_A_IA_A0);//IA Cell Setpoint  Hall A
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC04", &fIDHall_A_IA_A1);//IA Cell Setpoint  Hall A
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC05", &fIDHall_A_IA_A2);//IA Cell Setpoint  Hall A
  status = ca_pend_io(10);
  status = ca_search("C1068_QDAC06", &fIDHall_A_IA_A3);//IA Cell Setpoint  Hall A
  status = ca_pend_io(10);

  //  Helicity magnet controls
  fHelMagNames[0] = "MHE0L01V";
  fHelMagNames[1] = "MHE0L02H";
  fHelMagNames[2] = "MHE0L03V";
  fHelMagNames[3] = "MHE0L03H";
  fHelicityNames[0] = "Even";
  fHelicityNames[1] = "Odd";
  /*
  status = ca_search("hel_even_1",&fIDHelMag[0][0]); //Even helicity setpoint for MHE0L01V
  status = ca_pend_io(10);
  status = ca_search("hel_odd_1", &fIDHelMag[0][1]);  //Odd helicity setpoint for MHE0L01V
  status = ca_pend_io(10);
  status = ca_search("hel_even_2",&fIDHelMag[1][0]); //Even helicity setpoint for MHE0L02H
  status = ca_pend_io(10);
  status = ca_search("hel_odd_2", &fIDHelMag[1][1]);  //Odd helicity setpoint for MHE0L02H
  status = ca_pend_io(10);
  status = ca_search("hel_even_3",&fIDHelMag[2][0]); //Even helicity setpoint for MHE0L03V
  status = ca_pend_io(10);
  status = ca_search("hel_odd_3", &fIDHelMag[2][1]);  //Odd helicity setpoint for MHE0L03V
  status = ca_pend_io(10);
  status = ca_search("hel_even_4",&fIDHelMag[3][0]); //Even helicity setpoint for MHE0L03H
  status = ca_pend_io(10);
  status = ca_search("hel_odd_4", &fIDHelMag[3][1]);  //Odd helicity setpoint for MHE0L03H
  status = ca_pend_io(10);

  */

  //Feedback status EPICS variables
  status = ca_search("qw:ChargeFeedback", &fFeedbackStatus);//
  status = ca_pend_io(10);
  status = ca_search("qw:ChargeAsymmetry", &fChargeAsymmetry);//
  status = ca_pend_io(10);
  status = ca_search("qw:ChargeAsymmetryError", &fChargeAsymmetryError);//
  status = ca_pend_io(10);
  status = ca_search("qw:ChargeAsymmetryWidth", &fChargeAsymmetryWidth);//
  status = ca_pend_io(10);
  
  ca_search("IGL1I00DI24_24M", &fHalfWavePlateStatus);
  status = ca_pend_io(10);
  //Hall A parameters
  status = ca_search("qw:HAChargeAsymmetry", &fHAChargeAsymmetry);//
  status = ca_pend_io(10);
  status = ca_search("qw:HAChargeAsymmetryError", &fHAChargeAsymmetryError);//
  status = ca_pend_io(10);
  status = ca_search("qw:HAChargeAsymmetryWidth", &fHAChargeAsymmetryWidth);//
  status = ca_pend_io(10);
  
  //Target paremetes
  status = ca_search("qw:TargetDiffX", &fTargetXDiff);
  status = ca_search("qw:TargetDiffXError", &fTargetXDiffError);
  status = ca_search("qw:TargetDiffXWidth", &fTargetXDiffWidth);

  status = ca_search("qw:TargetDiffXP", &fTargetXPDiff);
  status = ca_search("qw:TargetDiffXPError", &fTargetXPDiffError);
  status = ca_search("qw:TargetDiffXPWidth", &fTargetXPDiffWidth);

  status = ca_search("qw:TargetDiffY", &fTargetYDiff);
  status = ca_search("qw:TargetDiffYError", &fTargetYDiffError);
  status = ca_search("qw:TargetDiffYWidth", &fTargetYDiffWidth);

  status = ca_search("qw:TargetDiffYP", &fTargetYPDiff);
  status = ca_search("qw:TargetDiffYPError", &fTargetYPDiffError);
  status = ca_search("qw:TargetDiffYPWidth", &fTargetYPDiffWidth);

  //3C12 paremetes
  status = ca_search("qw:3C12DiffX", &f3C12XDiff);
  status = ca_search("qw:3C12DiffXError", &f3C12XDiffError);
  status = ca_search("qw:3C12DiffXWidth", &f3C12XDiffWidth);

  status = ca_search("qw:3C12DiffY", &f3C12YDiff);
  status = ca_search("qw:3C12DiffYError", &f3C12YDiffError);
  status = ca_search("qw:3C12DiffYWidth", &f3C12YDiffWidth);

  status = ca_search("qw:3C12EffQ", &f3C12YQ);
  status = ca_search("qw:3C12EffQError", &f3C12YQError);
  status = ca_search("qw:3C12EffQWidth", &f3C12YQWidth);

  status = ca_search("qw:BCM8", &fBCM8Yield);
  status = ca_search("qw:BCM78DDAsymmetry", &fBCM8DDAsymmetry);
  status = ca_search("qw:BCM78DDAsymmetryError", &fBCM8DDAsymmetryError);
  status = ca_search("qw:BCM78DDAsymmetryWidth", &fBCM8DDAsymmetryWidth);


  status = ca_search("qw:USLumiAsymmetry", &fUSLumiSumAsymmetry);
  status = ca_search("qw:USLumiAsymmetryError", &fUSLumiSumAsymmetryError);
  status = ca_search("qw:USLumiAsymmetryWidth", &fUSLumiSumAsymmetryWidth);





};

QwEPICSControl::~QwEPICSControl()
{  

};

