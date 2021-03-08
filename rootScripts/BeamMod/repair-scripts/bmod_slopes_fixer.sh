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

./segmentRunwise.sh $segmentlist _13746
./segmentRunwise.sh $segmentlist _13746 _A
./segmentRunwise.sh $segmentlist _13746 _B
./segmentRunwise.sh $segmentlist _13746 _C
./segmentRunwise.sh $segmentlist _13746 _D

./segmentRunwise.sh $segmentlist _13726  
./segmentRunwise.sh $segmentlist _13726 _A
./segmentRunwise.sh $segmentlist _13726 _B
./segmentRunwise.sh $segmentlist _13726 _C
./segmentRunwise.sh $segmentlist _13726 _D

./segmentRunwise.sh $segmentlist _15746  
./segmentRunwise.sh $segmentlist _15746 _A
./segmentRunwise.sh $segmentlist _15746 _B
./segmentRunwise.sh $segmentlist _15746 _C
./segmentRunwise.sh $segmentlist _15746 _D


./pruneRunwise.sh $prunelist _13746  
./pruneRunwise.sh $prunelist _13746 _A
./pruneRunwise.sh $prunelist _13746 _B
./pruneRunwise.sh $prunelist _13746 _C
./pruneRunwise.sh $prunelist _13746 _D

./pruneRunwise.sh $prunelist _13726  
./pruneRunwise.sh $prunelist _13726 _A
./pruneRunwise.sh $prunelist _13726 _B
./pruneRunwise.sh $prunelist _13726 _C
./pruneRunwise.sh $prunelist _13726 _D

./pruneRunwise.sh $prunelist _15746  
./pruneRunwise.sh $prunelist _15746 _A
./pruneRunwise.sh $prunelist _15746 _B
./pruneRunwise.sh $prunelist _15746 _C
./pruneRunwise.sh $prunelist _15746 _D


./pruneCyclewise.sh $prunecyclelist _13746  
./pruneCyclewise.sh $prunecyclelist _13746 _A
./pruneCyclewise.sh $prunecyclelist _13746 _B
./pruneCyclewise.sh $prunecyclelist _13746 _C
./pruneCyclewise.sh $prunecyclelist _13746 _D

./pruneCyclewise.sh $prunecyclelist _13726  
./pruneCyclewise.sh $prunecyclelist _13726 _A
./pruneCyclewise.sh $prunecyclelist _13726 _B
./pruneCyclewise.sh $prunecyclelist _13726 _C
./pruneCyclewise.sh $prunecyclelist _13726 _D

./pruneCyclewise.sh $prunecyclelist _15746  
./pruneCyclewise.sh $prunecyclelist _15746 _A
./pruneCyclewise.sh $prunecyclelist _15746 _B
./pruneCyclewise.sh $prunecyclelist _15746 _C
./pruneCyclewise.sh $prunecyclelist _15746 _D
