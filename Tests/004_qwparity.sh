#!/bin/bash

# Test 004:
#
#   Run the mock data generator and the analysis and make sure it succeeds.
#

setupscript=SetupFiles/SET_ME_UP.bash

if [ ! -e ${setupscript} ] ; then
  echo "Setup script ${setupscript} could not be found."
  exit -1
fi

source ${setupscript} || exit -1

OUT=`mktemp -t qwparity.XXXXXX.out`
build/qwmockdatagenerator -r 10 -e :10000 --config qwparity.conf --detectors mock_detectors.map | grep -i -v time | tee $OUT || exit -1
diff $OUT Tests/004_qwmockdatagenerator.ref || exit -1

OUT=`mktemp -t qwparity.XXXXXX.out`
build/qwparity -r 10 -e :10000 --config qwparity.conf --detectors mock_detectors.map | grep -i -v time | grep -v `hostname` | grep -v "Processing event" | tee $OUT || exit -1
diff $OUT Tests/004_qwparity.ref || exit -1

exit 0
