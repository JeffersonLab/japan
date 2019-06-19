import sys
import os
import ROOT as R
import pandas as pd


source=sys.argv[1]

runinfo = pd.read_csv(filepath_or_buffer=source, engine="python", delimiter='\t', skiprows=2,nrows=1)
source_pd_df= pd.read_csv(filepath_or_buffer=source, engine="python",delimiter='\t',header=0, skiprows=5)



sensitivity_df= source_pd_df[source_pd_df['CycleID']<0]
print(source_pd_df)
#print(runinfo)
#print(sensitivity_df)
