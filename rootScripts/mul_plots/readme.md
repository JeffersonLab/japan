# usage
  > ./mul_plot.sh -h
    to see how to run the script

  a few examples:
  * with run number: seperated run number or run range; this is the default mode
  > ./mul_plot.sh -n "name_prefix" run1 run2-run3
    this will produce a rootfile named "name_prefix_mul_tree.root" under results dir.
    which collect related data from run1 and runs from run2 to run3.

  * with slug number: seperated slug number or slug range, use -s switch
  > ./mul_plot.sh -n "name_prefix" -s slug1 slug2-slug3
    this will use all runs in input slugs

## rootfiles
  there are two trees in the resulted root file: reg and dit; the 'reg' tree store
  raw and reg data and the 'dit' tree collects dithering data. Currently, only 
  asym of main det. (and their combinations) are collected. If you need data about 
  other det. (monitors), then you can modify the code of "make_mul_tree.C" to add 
  needed branch (remember to compile modified file with `make` command).

  !!! Note: the cut condition for these two trees are different:
    for reg tree, the good condition is "ok_cut == 1"
    for dit tree, the good condition is "ErrorFlag == 0"

## plots
  currently, there are a few draw scripts to draw wanted plots:
  * draw_mul_plots: this command draw us (ds) plots without sign correction, so you can combine any runs you want
    > ./draw_mul_plots -n "name_prefix" path_to_rootfiles1 path_to_rootfiles2 ...
  * draw_single_arm:  this command does the same thing as draw_mul_plots, but it draws the left (right) plots
    > ./draw_single_arm -n "name_prefix" path_to_rootfiles1 path_to_rootfiles2 ...

  correspondingly, there areeeeeee sign corrected version of previous two command:
  * draw_mul_plots_sign_corrected:  
      as the name implies, this command applies sign correction to data, currently, this command only works 
      with slug data, but not any run data
    > ./draw_mul_plots_sign_corrected -n "name_prefix" slug1 slug2
      it reads data from reselts dir. by assuming root file path as "results/slugN_mul_tree.root", so if you 
      have rootfiles stored in some other place, you need to make a link in the results dir. and change its 
      name to proper format to make the command work
  * draw_single_arm_sign_corrected
      left (right) version

  To make sign correction, it depends on a default input of "/adaqfs/home/apar/weibin/analysis/slug_sign.list"
  which specify the sign for each slug. 

## compilation
  > make


# Notes
## single arm running: 
  * slug8: 3453-3507
  * slug9: 3524-3541
  * slug10: 3543-3563
  * slug11: 3566-3576
  * slug20: 3718-3722
  * slug30: 3863-3881
  * slug43: 4067-4080
  * slug74: 4677-4695


## bpm11X and bpm12X
For runs before 3583, we use bpm12X; for runs after 3583, we use 11X, so we need to take care of that.
In make_mul_tree.C, if bpm12X is used, I will convert it to bpm11X, so that we have a uniform interface for postpan

## dithering
slug0-3 has no dithering data


## postpan
!!! Note the difference between regression of all slug data and the collection of postpan data of all runs in a slug
there is a little difference.
