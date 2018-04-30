#!/bin/bash

# Test 001:
#
#   This test performs a 'make distclean' and tries to build the entire
#   QwAnalysis framework from scratch.
#

make distclean || exit -1
make config || exit -1
make || exit -1

exit 0
