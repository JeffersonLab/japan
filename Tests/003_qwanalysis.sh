#!/bin/bash

# Test 003:
#
#   Run the analysis code without arguments and make sure it succeeds.  This
#   test is meant to catch problems in linked libraries and the environment
#   variables.
#

setupscript=SetupFiles/SET_ME_UP.bash

if [ ! -e ${setupscript} ] ; then
  echo "Setup script ${setupscript} could not be found."
  exit -1
fi

source ${setupscript} || exit -1

bin/qwparity || exit -1

exit 0
