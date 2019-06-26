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

source SetupFiles/SET_ME_UP.bash

if [ ! -f ${QW_DATA}/parity_ALL_${run}.dat ] ; then
  wget http://hallaweb.jlab.org/12GeV/Moller/downloads/japan/parity_ALL_${run}.dat
  mv parity_ALL_${run}.dat ${QW_DATA}
fi
make -C build && build/qwparity -c prex.conf -r $run -e :2k

hallaweb=/group/halla/www/hallaweb/html/12GeV/Moller/downloads/japan/
branch=`git rev-parse --abbrev-ref HEAD`
if [ -d ${hallaweb} ] ; then
  mkdir -p ${hallaweb}/${branch}
  cp ${QW_ROOTFILES}/prexALL_${run}.root ${hallaweb}/${branch}/
fi
