#!/bin/bash

# Test 002:
#
#    Run the setup script and check for the existence (and content) of some
#    important variables.
#

setupscript=SetupFiles/SET_ME_UP.bash

if [ ! -e ${setupscript} ] ; then
  echo "Setup script ${setupscript} could not be found."
  exit -1
fi

source ${setupscript} || exit -1

if [ -z "${QWANALYSIS}" ] ; then
  echo "Environment variable QWANALYSIS undefined."
  exit -1
fi

if [ -z "${QWSCRATCH}" ] ; then
  echo "Environment variable QWSCRATCH undefined."
  exit -1
fi

exit 0