import sys
import os
import ROOT as R
import pandas as pd


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
    print(self.df)
    return self.df.loc[:,['RunID','CycleID','Variable','coeff','error']]
  #function returns unique list of variables
  def returnlov(self):
    return self.df['Variable'].unique().tolist()
  
class plotobj:
  def __init__(self,name):
    self.name=name
    self.coeff=[]
    self.error=[]
  def updatelist(self,coeff,error):
    self.coeff.append(coeff)
    self.error.append(error)
    

filelist= ['res/prexRespin1_1473_dither.res','res/prexRespin1_1474_dither.res']
filecount=len(filelist);
pobj=[]

for f in range(0,filecount):
  bmod=bmoddata(filelist[f])
  df=bmoddata.returndf(bmod) 
  if filecount==0:
    list_var=bmoddata.returnlov(bmod)
    for l in range(0,len(list_var)):
      pobj.append(plotobj(list_var[l]))


  

#sensitivity_df= source_pd_df[source_pd_df['CycleID']<0]
#print(source_pd_df)
#print(runinfo)
#print(sensitivity_df)
