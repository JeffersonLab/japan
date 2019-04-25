# Continuous Aggregate Monitor Graphical User INterface: CAMGUIN
A set of methods useful for reading a root tree, doing some analysis, and printing those results into an updatable ROOT file. This is a fully functioning macro whose methods stand alone in PANGUIN and in ROOT in general as well, and it represents several possible ways of accessing and manipulating ROOT output. 

## Usage 

Example script for how to use environment variables and PANGUIN to execute the CAMGUIN macros on data files

Panguin Interface:
`source [your copy of setup_japan.tcsh]`

`source setup_camguin.tcsh`

`./panguin_wrapper -f analyze.cfg -r 1374 -n 1 -P`
`./postpan_wrapper -r 1374`


Using ROOT directly:
  ./wrapper.sh -r 1296 -f input.txt -n 1
  
## What is happening

* The Panguin Wrapper script intelligently sets environment variables ROOT can access (since PANGUIN doesn't share them)
* Panguin simply executes the "camguin.C" macro over and over again with different channels and analysis requests being made.
* The macro opens the requested ROOT file itself (using similar methods to PANGUIN) and reads the desired tree, branch, leaf, and cut information
* The data is fed into a histogram or looped over, and further analysis is done in analysis methods (default cuts = (ErrorFlag == 0 && [userData].Device_Error_Code == 0))

## Results 

The results of analysis methods are written into a re-writable ROOT file "run_aggregator.root"
* The aggregate ROOT file will contain "run_number", "n_runs" (useful for further aggregating many runs into one result), and the name of the analyzed outputs (e.g. "rms_asym_bcm_an_ds3", the first "_" can be changed later if wanted)
* It can be treated like any other ROOT file, and its contents are all flat, at the same level
* If an analysis that was done before is not done for a new run, or if a new type of analysis result is generated and not executed on prior runs then placeholder -999999 values are stored
* All values (including "run_number" and "n_runs") are doubles, so more interesting kinds of information can be stored in high or low bits later if desired
* The postpan reader uses the file format developed by Tao and prints the contents into individual leaves and is better to run in pure macro form with the postpan wrapper script

## Specific Cases of the camguin.C master macro in action


* To run the post-pan output files do:
root -L camguin.C'("meanrms","reg","reg_asym_sam2","reg_asym_sam2","1",0,"defaultHist")'
* To run a standard Japan output file do:
root -L camguin.C'("meanrms","mul","asym_sam2","hw_sum","defaultCuts",0,"defaultHist")'
* To store the post-pan output coefficients from the textfiles do:
root -L camguin.C'("postpan")' 
