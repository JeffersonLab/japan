#!/bin/bash

# Framework for regression testing.
#
#    Executable scripts in the test directory with format [0-9][0-9][0-9]_*.sh
#    are executed.  Success is indicated by a zero return value, failure by a
#    non-zero return value.  Regression tests should explicitly return a value
#    with 'exit [n]'.
#

testdir="Tests"
tests=`ls ${testdir}/[0-9][0-9][0-9]_*.sh`

# Loop over all tests
for test in ${tests} ; do

  # Check whether test is executable
  if [ ! -x $test ] ; then
    echo "$test is not executable."
    echo "Regression testing aborted."
    exit -1
  fi

  echo "Running $test..."
  if $test ; then
    echo "$test succeeded."
  else
    echo "$test failed."
    exit -1
  fi

done

echo "All regression tests were successful."
