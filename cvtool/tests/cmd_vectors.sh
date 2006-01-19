#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -n 2 -w 10 -h 10 > dd.pnm
$CVTOOL opticalflow bm-sad -k 1 -M 10 -D 0.1 -L 0.5 < dd.pnm > f2i.txt
echo "CVL FIELD 10 10" > f3.txt; i=0; while [ $i -lt 100 ]; do echo "0 0 0" >> f3.txt; i=$((i+1)); done

$CVTOOL vectors visualize -t 2i < f2i.txt > /dev/null
$CVTOOL vectors visualize -t 2i -x1 -y1 -X1 -Y1 -f10 < f2i.txt > /dev/null
$CVTOOL vectors visualize -t 2 -x1 -y1 -X1 -Y1 -f10 < f2i.txt > /dev/null
$CVTOOL vectors visualize -t 3 -x10 -y10 -X1 -Y1 -f10 < f3.txt > /dev/null

cmd_tests_cleanup
