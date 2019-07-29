#!/bin/bash
#cd ~/PREX/prompt/Aggregator/wrapper/
../setup_camguin_dithering.sh
python loop-oneRun-dither.py -r ${1} -f input_dithering.txt -F 1 & # Do full runs
python loop-oneRun-dither.py -r ${1} -f input_dithering.txt -F 0 & # Do mini runs
