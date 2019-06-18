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
  This can be used in conjuction with the previous options. It will use your config file (or the default) to print a file called summaryplots.pdf with plots generated from the rootfile it reads. 

### V option
```
./build/panguin -v N
```
 This will run with a verbosity level of N (higher is more noisy).


## Online monitor
With the **watchfile** option enabled the GUI will reload the file every few seconds and will redraw the current canvas (for default usage please look at defaultOnline.cfg).

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

## Configuration file options
Examples of these options can be found in macros/defaul.cfg and macros/defaultOnline.cfg.

### Output drawing options
- **plotsdir** option allows you to set the directory where the plots will be created. Directory has to exist
- **plotFormat** option allows you to select the format of the plots (png, gif, pdf). Default is pdf.
### File selection
- **rootfile** followed by the path and name of the root file allows you to select a file to draw from
- **protorootfile** followed by the path and name of of the root file where the run number if replaced with 5x"X" (i.e. "XXXXX"). This allows you to select the run number with the -r option (see above)
- **goldenrootfile** followed by the path and name of the root file will select that file to draw as a comparison for the current file to more easily notice problems with the current run.
### watchfile option 
See online monitor above.
### Histogram binning options
- **2DbinsX** or **2DbinsY** followed by a number; for 2D histograms this option allows you to set the number of bins (default ROOT is 40 bins)
## Cuts
- **definecut** followed by a string (with no spaces and no quotation marks); this allows you to group any number of cuts (using the standard TTree cut syntax) and give it a simple name to be used later.
### Tab creation
- **newpage** followed by 2 numbers (N,M); creates a new tab with N number of columns and M numbers of rows;
- **title** followed by any number of strings (spaces can be included) allows you to set the title for the tab
### Drawing options
Panguin will search the Ttrees that it finds in the file you specified and will extract the branch names. It simplifies the drawing from a Ttree but follows the same basic logic.
- **var1D**; will make a 1D histogram of var1D (if found in any of the Ttrees)
- **var1D -type scat**; same as above but will draw with the scater option (for other options check the ROOT draw options)
- **var1D -type scat SomeCut**; same as above but will cut events that satisfy the SameCut (custom cut explained above)
- **var1D -type scat SomeCut -title "Pretty title"**; same as above but will set the title of the histogram
- **var1:var2**; draw a 2D histogram (above options can also be used)
- **var1:var2 CodaEventNumber>10**; same as above with the extra cut based on variables available in the Ttree;
- **macro someMacro.C**; for more complicated plots you can write your own macro and then output the result in the PANGUIN tab. The macro needs to create only one plot. 
### Non-standard GUI color
- **guicolor** followed by the string of a color like (white, red, blue) allows you to set the border color of the gui
### 
