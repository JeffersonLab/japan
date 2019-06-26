#!/bin/bash

function usage() {
  echo "Usage: $0 [-h] [-r run]"
}

if [ $# -lt 2 ] ; then
  usage
fi

while getopts “:h:r:” opt; do
  case $opt in
    h) usage && exit ;;
    r) run=$OPTARG ;;
    *) usage && exit ;;
  esac
done

go get go-hep.org/x/hep/groot/cmd/root-diff

source SetupFiles/SET_ME_UP.bash

if [ ! -f ${QW_DATA}/parity_ALL_${run}.dat ] ; then
  wget http://hallaweb.jlab.org/12GeV/Moller/downloads/japan/parity_ALL_${run}.dat
  mv parity_ALL_${run}.dat ${QW_DATA}
fi
make -C build && build/qwparity -c prex.conf -r $run -e :2k

if [ ! -f prexALL_${run}.root ] ; then
  wget http://hallaweb.jlab.org/12GeV/Moller/downloads/japan/develop/prexALL_${run}.root
fi
root-diff -k evt,mul prexALL_${run}.root ${QW_ROOTFILES}/prexALL_${run}.root
