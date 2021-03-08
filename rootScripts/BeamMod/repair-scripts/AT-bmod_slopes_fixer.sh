#!/bin/bash

prefix=""
list="../results/segmentRun.list"
if [[ $# -ge 1 ]] ; then
  segmentlist=$1
fi
if [[ $# -ge 2 ]] ; then
  prunelist=$2
fi
if [[ $# -ge 3 ]] ; then
  prunecyclelist=$3
fi

#cd repair-scripts

./segmentRunwise.sh $segmentlist _13746 NONE AT-
./segmentRunwise.sh $segmentlist _13746 _A AT-
./segmentRunwise.sh $segmentlist _13746 _B AT-
./segmentRunwise.sh $segmentlist _13746 _C AT-
./segmentRunwise.sh $segmentlist _13746 _D AT-

./segmentRunwise.sh $segmentlist _13726 NONE AT-
./segmentRunwise.sh $segmentlist _13726 _A AT-
./segmentRunwise.sh $segmentlist _13726 _B AT-
./segmentRunwise.sh $segmentlist _13726 _C AT-
./segmentRunwise.sh $segmentlist _13726 _D AT-

./segmentRunwise.sh $segmentlist _15746 NONE AT-
./segmentRunwise.sh $segmentlist _15746 _A AT-
./segmentRunwise.sh $segmentlist _15746 _B AT-
./segmentRunwise.sh $segmentlist _15746 _C AT-
./segmentRunwise.sh $segmentlist _15746 _D AT-


./pruneRunwise.sh $prunelist _13746 NONE AT-
./pruneRunwise.sh $prunelist _13746 _A AT-
./pruneRunwise.sh $prunelist _13746 _B AT-
./pruneRunwise.sh $prunelist _13746 _C AT-
./pruneRunwise.sh $prunelist _13746 _D AT-

./pruneRunwise.sh $prunelist _13726 NONE AT-
./pruneRunwise.sh $prunelist _13726 _A AT-
./pruneRunwise.sh $prunelist _13726 _B AT-
./pruneRunwise.sh $prunelist _13726 _C AT-
./pruneRunwise.sh $prunelist _13726 _D AT-

./pruneRunwise.sh $prunelist _15746 NONE AT-
./pruneRunwise.sh $prunelist _15746 _A AT-
./pruneRunwise.sh $prunelist _15746 _B AT-
./pruneRunwise.sh $prunelist _15746 _C AT-
./pruneRunwise.sh $prunelist _15746 _D AT-
