#!/bin/bash
cd ~/PREX/prompt/bmodAna/
for (( i=181; i<=185; i++ )); do
  echo "Slug $slug, auto < > WAC"
  slug=$i
  root -l -b -q runBmodAna.C\(1,\"inputs/input_monitors_1X.txt\",\"run_list/slug${slug}.list\"\);
done
