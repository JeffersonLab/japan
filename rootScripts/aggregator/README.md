# Continuous Aggregate Monitor Graphical User INterface: CAMGUIN
A set of methods useful for reading a root tree, doing some analysis, and printing those results into an updatable ROOT file. This is a fully functioning macro whose methods stand alone in PANGUIN and in ROOT in general as well, and it represents several possible ways of accessing and manipulating ROOT output. 

## Usage 

Example script for how to use environment variables and PANGUIN to execute the CAMGUIN macros on data files

`source [your copy of setup_japan.tcsh]`

`source setup_camguin.tcsh`

`./panguin_wrapper -f analyze.cfg -r 1374 -n 3 -P`

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
