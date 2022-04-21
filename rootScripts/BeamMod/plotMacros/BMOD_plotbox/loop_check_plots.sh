#!/bin/bash

while IFS=',' read -r run event; do
  cp -p Mul_Outlier_checks/plots/*Run_$run*.pdf Mul_Outlier_checks/save_plots/
  #evince Mul_Outlier_checks/plots/*Run_$run*.pdf
done < Mul_Outlier_checks/5sigma_count_multiples.out
#done < Mul_Outlier_checks/Cut_list_5sigmas.list
