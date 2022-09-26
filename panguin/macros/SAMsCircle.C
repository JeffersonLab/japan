void SAMsCircle(){
  TPad *a1 = new TPad("a1", "a1", 0,0,1,1);
  a1->Draw();

  Float_t r1 = 0.3, r2 = 0.3;
  Float_t delta = 0.05*r1;
  Float_t size = 0.1;
  TEllipse * el = new TEllipse(0.5, 0.5, r1, r2);
  TLatex * t1 = new TLatex(0.5, 0.5+r2-6*delta, "1");
  t1->SetTextSize(size);
  t1->SetTextColor(kRed);
  TLatex * t2 = new TLatex(0.5+r1/2, 0.5+r2/2, "2");
  t2->SetTextSize(size);
  t2->SetTextColor(kRed);
  TLatex * t3 = new TLatex(0.5+r1-size/2.5, 0.5-size/2, "3");
  t3->SetTextSize(size);
  t3->SetTextColor(kRed);
  TLatex * t4 = new TLatex(0.5+r1/2, 0.5-r2/2-5*delta, "4");
  t4->SetTextSize(size);
  t4->SetTextColor(kRed);
  TLatex * t5 = new TLatex(0.5, 0.5-r2+delta, "5");
  t5->SetTextSize(size);
  t5->SetTextColor(kRed);
  TLatex * t6 = new TLatex(0.5-r1/2-size/5, 0.5-r2/2-6*delta, "6");
  t6->SetTextSize(size);
  t6->SetTextColor(kRed);
  TLatex * t7 = new TLatex(0.5-r1+delta, 0.5-size/2, "7");
  t7->SetTextSize(size);
  t7->SetTextColor(kRed);
  TLatex * t8 = new TLatex(0.5-r1/2-size/5, 0.5+r2/2+delta, "8");
  t8->SetTextSize(size);
  t8->SetTextColor(kRed);

  a1->cd();
  el->Draw();
  t1->Draw();
  t2->Draw();
  t3->Draw();
  t4->Draw();
  t5->Draw();
  t6->Draw();
  t7->Draw();
  t8->Draw();
}
