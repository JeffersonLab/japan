void exampleMacro1(double scale=4){
  TF1 *f=new TF1("f1",Form("sin(x)/x*%f",scale),0.,10.);
  f->Draw();
}
