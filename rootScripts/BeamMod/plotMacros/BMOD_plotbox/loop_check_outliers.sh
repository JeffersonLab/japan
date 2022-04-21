#!/bin/bash

while IFS=',' read -r run event; do
  root -l -b -q checkOutlierRun.C'('$run','$event')'
done < Mul_Outlier_checks/5sigma_count_pruned.out
#done < Mul_Outlier_checks/fastplot.out
