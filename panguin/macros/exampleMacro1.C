void exampleMacro1(){
  TF1 *f=new TF1("f1","sin(x)/x",0.,10.);
  f->Draw();
}
