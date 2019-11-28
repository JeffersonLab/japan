To Collect: 
1) login as apar@adaq3
2) cd into ~/PREX/prompt/collector
3) source ../../setup_japan.tcsh

WAC List Creation
1) create a list of run number you want to (see for example test.list)
2) use PVDB/RCND/RCDB commands/website
3) make a slug#.list

Collector:
1) run the command: ./collector -d ../results/ -l test.list (Note:replace test with your file name)
2) a root file named prexPrompt_test.root will be stored in the rootfiles directory
3) to produce plots stay in the collector directory and run ./aggregate 
4) you will be asked to enter a test part of the rootfile, enter it and hit Enter
5) a aggregate_plots_test.pdf file will be created in the plots directory.

Draw Post Pan:
1) cd into drawPostpan
2) [apar@adaq1 ~/PREX/prompt/collector/drawPostpan]$ root -l -b -q drawPostPan.C'("../rootfiles/prexPrompt_slug10.root","slug10-Left","list.txt")'
