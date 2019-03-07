void noStatBox()
{
    gStyle->SetOptStat(0);
    gROOT->ForceStyle();
    gPad->Close();
}
