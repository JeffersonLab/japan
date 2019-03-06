# PANGUIN

Simple program that reads a configuration file and plots from a file. Searches for trees and branch names to find the correct tree. Plots the result of a macro (n.b. your macro should only make one plot). 

If you have your QW_ROOTFILES env set it will search for a file with the correct run number in that folder assuming the format prexXXX_####.root, where XXX is the configuraiton (for now we have CH, INJ, ALL).

This has been tested to compile on adaq3 using cmake. 

## Use 
Running without arguments will load the macros/default.cfg macro and run that. 

### f option
```
./build/panguin -f path/to/nameOfAna.cfg
```
This will read in the configuration files and execute the commands within.

### r option
```
./build/panguin -r XXXX
```
  where XXXX is your run number. This will load the default.cfg macro (if you want to run your own macro use the -f option as well) and look for your run number as explained above.

### P option
```
./build/panguin -P
```
  This can(should) be used in conjuction with the previous options. It will use your config file (or the default) to print a file called summaryplots.pdf with plots generated from the rootfile it reads. 


## Online monitor
With the **watchfile** option enabled the GUI will reload the file every 30 s and will redraw the current canvas (for default usage please look at defaultOnline.cfg).

The process to run the online monitor goes as follows: 
a) Run the ET connected japan output:
```
cd mainJapanDir
build/qwparity --config prex.conf --add-config online_apar.conf
```
This will connect to the ET system and analyze events as they come through the ET system. It will update the output file every ~30s. The output file will be placed in the QW_ROOTFILES directory and will have the following pattern: ''prexALL_999999.adaq*.root''. This file can be accessed from the prompt or through panguin. 
b) Run panguin:
```
cd mainJapanDir/pangin
build/panguin -r 999999 -f macros/defaultOnline.cfg
```
The configuration can be changed with whatever you need but it must have the ''watchfile'' option set. 


