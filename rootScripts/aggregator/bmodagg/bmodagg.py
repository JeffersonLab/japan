import sys
import os
import ROOT as R
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from array import array

class bmoddata:
  #parses Tao's csv file output and initializes a bmod structure storing the information
  def __init__(self, source):
    self.source=source
    self.runinfo=pd.read_csv(filepath_or_buffer=source, engine="python", delimiter='\t', skiprows=2,nrows=1)
    self.df=pd.read_csv(filepath_or_buffer=source, engine="python",delimiter='\t',header=0, skiprows=5)
    self.df.insert(0,'RunID',self.runinfo.iloc[0][0])
    self.df['Variable']=self.df.DV_name+'_vs_'+self.df.IV_name
    self.df=self.df.drop(["#comment","DV_name","IV_name","unit","status"],axis=1) #Getting rid of unnecesary columns may be
  #function returns slope table if parameter specified is 'slope' or returns sensitivity
  def returndf(self): #return the data frame with coefficient and values
    return self.df.loc[:,['RunID','CycleID','Variable','coeff','error']]
  #function returns unique list of variables
  def returnlov(self):
    return self.df['Variable'].unique().tolist()
  
class plotobj:
  def __init__(self,name):
    self.name=name
    self.run=[]
    self.cycle=[]
    self.coeff=[]
    self.error=[]
  def updatelist(self,run,cycle,coeff,error):
    self.run=self.run+run
    self.cycle=self.cycle+cycle
    self.coeff=self.coeff+coeff
    self.error=self.error+error
    

filelist= ['res/prexRespin1_1473_dither.res','res/prexRespin1_1474_dither.res']
filecount=len(filelist);
pobj=[]

for f in range(0,filecount):
  bmod=bmoddata(filelist[f])
  df=bmoddata.returndf(bmod) 

  if f==0:
    varlist=bmoddata.returnlov(bmod)
    varcount=len(varlist)
    for l in range(0,varcount):
      pobj.append(plotobj(varlist[l]))

  for l in range(0,varcount):
    df_varcut=df[df.Variable==varlist[l]]
    plotobj.updatelist(pobj[l],df_varcut['RunID'].tolist(), df_varcut['CycleID'].tolist(), df_varcut['coeff'].tolist(),df_varcut['error'].tolist())

output=R.TFile("bmod_agg_summary.root","recreate")

for l in range(0,varcount):
    n=len(pobj[l].run)
    c=R.TCanvas(pobj[l].name, pobj[l].name, 800,600)
    graph=R.TGraphErrors(n,array('f',pobj[l].run), array('f',pobj[l].coeff), array('f',[0]*n), array('f',pobj[l].error)  )
    graph.SetTitle(pobj[l].name+" vs run/cycle")
    graph.Draw("AP")
    c.Print("fig/"+pobj[l].name+".png")
    #graph.Write(pobj[l].name)

    
output.Close()





  

#sensitivity_df= source_pd_df[source_pd_df['CycleID']<0]
#print(source_pd_df)
#print(runinfo)
#print(sensitivity_df)
