#!/bin/bash
while IFS= read -r line ; do 
  root -l -b -q addTripCounter.C'('$line',"prompt")'
  root -l -b -q addTripCounter.C'('$line',"respin1")'
done < $1
