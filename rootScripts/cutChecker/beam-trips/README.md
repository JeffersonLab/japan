== How to use the Tree Skimmer-Trip Finder ==
The trip finder/tree skimmer (addTripCounter.C) does 3 things
1) It grabs the existing tree and makes an exact copy of a subset of it's branches - useful things like CodaEventNumber and ErrorFlag, but also devices. 
NOTE: I am not 100% sure that grabbing 2 struct-devices from the tree will work. I had bad luck the first time I tried, then I stopped doing 2 devices but I also I think fixed the struct definition to not fail. It needs to be followed up on
2) It adds in some new useful branches like "run_number" and all of the RCDB info (using the standard string->double conversions decided in the aggregator) from the RCDB (so you need to be running this on a computer with access to RCDB and set your RCDB access environment appropriately).
3) While looping over the tree, it will check if an event is a good event in all ErrorFlag bits other than the beam trip bits, and when that trip holdoff cut ends it will restart a counter SinceLastTripEnd demarkating the beginning of a new period of clean, post-trip data.
NOTE: If the beam has tripped but everything other than the trip is clean, this is the data that gets notated as a post-trip section. There are likely cases where as the trip holdoff cut ended it had a BPM burp cut or something, and these trip ends would NOT be counted in this analysis as a result (it's just too many things to consider at once, but it can be changed if desired).
NOTE: There are also SinceLastTripStart and UntilNextTripStart and UntilNextTripEnd variables, though their ability to distenguish between trip holdoffs and otherwise good data are significantly less clear. It is likely that these other variables are not logically implemented correctly, so please treat them with a grain of salt.
NOTE: If the analysis is too slow, we can remove the reverse tree-loop (UntilNextTripEnd/Start determinations) and also add the SinceLastTripEnd determination directly into the new-tree printing loop. It was left in 3 separate loops just for logical clarity and debugging.

Adding new devices to be included in the trimmed tree should be as easy as defining their corresponding structs (be careful!) and verifying that their data elements are successfully transfered through the tree copy (I suspect there is some more complicated technique of copying structs than what I have implemented, but what I have now definitely works for 1 bcm device's hw_sum channel, which is all I needed).

== How to compare exactly commensurate data sets and cuts ==
The data-set comparator (loopAq.C) does 2 things
1) It grabs two sets of data, one from prompt and another from respin1 (hardcoded for now, can be changed)
- Build's tree indices with "run_number" and "CodaEventNumber" as the major and minor indexing variables
- Loops over every single entry and if the entry is missing from one tree then it will skip this entry (i.e. if a CodaEventNumber cut was placed to remove some data from the tree entirely this would remove that data from both data sets)
- Adds the data from asym_bcm_an_us.hw_sum into histograms - that data from the prompt tree and respin1 tree, and using the cut from respin1 or prompt - so there are 4 combinations that are utilized, and it also separates into IHWP and Wien states (using the rcdb info), so you get 16 outputs ultimately, plus some merged combos too.
2) While it is doing that loop above, which generates the exactly commensurate datasets for Aq comparisons, it also counts the number of ErrorFlag that are !=0 in one data set but are == 0 in the other, and integrates the number of new data to respin1 vs. data newly cut out going into respin1, and makes some comparisons of the data sets. Further improvements may be made to determine the exact ErrorFlag bits that are active in the cut in vs. cut out cases, and also usage of SinceLastTripEnd may be done to verify that most of the new data cut into respin 1 does come from the shorter beam trip cut holdoff. Much is possible here.

To utilize this script properly it is best to already have the outputs of addTripCounter.C and then hadd together into one file (currently hardcoded in loopAq.C to be "test.root") that contains every single run's full tree in a single TTree.

== Others == 
There are a number of other macros and shell scripts for executing loops of these jobs and for making specific convenient plots. Most are adapted from simple command line executions or ROOT command line draw commands, so none are elegant.
