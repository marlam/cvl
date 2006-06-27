#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -n 2 -w 10 -h 10 > dd.pnm
$CVTOOL opticalflow bm-sad -k 1 -M 10 -D 0.1 -L 0.5 < dd.pnm > f2i.field

$CVTOOL vectors visualize -t 2i < f2i.field > /dev/null
$CVTOOL vectors visualize -t 2i -x1 -y1 -X1 -Y1 -f10 < f2i.field > /dev/null
#$CVTOOL vectors visualize -t 2 -x1 -y1 -X1 -Y1 -f10 < f2i.field > /dev/null
#$CVTOOL vectors visualize -t 3 -x10 -y10 -X1 -Y1 -f10 < f3.field > /dev/null

cmd_tests_cleanup
