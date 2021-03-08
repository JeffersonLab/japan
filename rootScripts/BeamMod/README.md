

bmodRMS.C is designed to perform the RMS check to determine if some slopes should be cut out
bmodStitch.C is designed to look for neighboring supercycles within a given run (not yet implementing neighbor runs) to combine cycles
runBmodAna.C is designed to do run averaging instead of cyclewise analysis ... normal bmodAna.C now implements this with a config flag

bmodAna.C does the cyclewise or runwise sensitivity, alpha, delta, beta calculations, and slope calculations. It can also do the RMS cuts and can be told which set of detectors to use in the global "flag" (instead of the "alldetsflag" which looks at all detectors included in the analysis)
