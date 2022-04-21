#!/bin/bash

./eig_segmentRunwise.sh ../results/contiguous-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles/rcdb_burstwise_eigenvectors_sorted_sorted.root
./eig_segmentRunwise.sh ../results/contiguous-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles/rcdb_burstwise_eigenvectors_sorted.root
./eig_segmentRunwise.sh ../results/contiguous-AT-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles/rcdb_segment_eigenvectors_sorted_sorted.root
./eig_segmentRunwise.sh ../results/contiguous-AT-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles/rcdb_segment_eigenvectors_sorted.root

./eig_segmentRunwise.sh ../results/contiguous-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_seg_avg/respin2-rcdb-parts-slowc-segments-seg_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_err_eigen_seg_avg/respin2-rcdb-parts-slowc-segments-seg_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-AT-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_seg_avg/respin2-rcdb-parts-slowc-segments-seg_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-AT-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_err_eigen_seg_avg/respin2-rcdb-parts-slowc-segments-seg_avg_sens-eigen_reg_lagr.root

./eig_segmentRunwise.sh ../results/contiguous-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_part_avg/respin2-rcdb-parts-slowc-segments-part_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_err_eigen_part_avg/respin2-rcdb-parts-slowc-segments-part_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-AT-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_part_avg/respin2-rcdb-parts-slowc-segments-part_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-AT-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_err_eigen_part_avg/respin2-rcdb-parts-slowc-segments-part_avg_sens-eigen_reg_lagr.root

./eig_segmentRunwise.sh ../results/contiguous-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_part_b_avg/respin2-rcdb-parts-slowc-segments-part_b_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_err_eigen_part_b_avg/respin2-rcdb-parts-slowc-segments-part_b_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-AT-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_eigen_part_b_avg/respin2-rcdb-parts-slowc-segments-part_b_avg_sens-eigen_reg_lagr.root
./eig_segmentRunwise.sh ../results/contiguous-AT-segmentRuns_respin2.list /lustre19/expphy/volatile/halla/parity/crex-respin2/LagrangeOutput/rootfiles_err_eigen_part_b_avg/respin2-rcdb-parts-slowc-segments-part_b_avg_sens-eigen_reg_lagr.root
