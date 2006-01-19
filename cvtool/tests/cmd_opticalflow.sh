#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -h 1 -w 1 > d.pnm 
$CVTOOL merge -o merge.txt d.pnm d.pnm > dd.pnm 

$CVTOOL opticalflow hs        -l 10.0 -n 50		< dd.pnm > f.txt     
$CVTOOL opticalflow hs     -b -l 10.0 -n 50		< dd.pnm > /dev/null 
$CVTOOL opticalflow lk        -k 3			< dd.pnm > /dev/null 
$CVTOOL opticalflow lk     -b -k 3			< dd.pnm > /dev/null 
$CVTOOL opticalflow clg       -l 10.0 -O 1.95 -n 50	< dd.pnm > /dev/null 
$CVTOOL opticalflow clg    -b -l 10.0 -O 1.95 -n 50	< dd.pnm > /dev/null 
$CVTOOL opticalflow bm-sad    -k 3 -M 10 -D 0.1 -L 0.5	< dd.pnm > /dev/null 
$CVTOOL opticalflow bm-sad -b -k 3 -M 10 -D 0.1 -L 0.5	< dd.pnm > /dev/null 
$CVTOOL opticalflow bm-asw    -k 3 -M 10 -c 7.0 -p 36.0	< dd.pnm > /dev/null 
$CVTOOL opticalflow bm-asw -b -k 3 -M 10 -c 7.0 -p 36.0	< dd.pnm > /dev/null 
$CVTOOL opticalflow cc -t 0 -w0.1 -o out.txt -f f.txt < f.txt > /dev/null 

cmd_tests_cleanup
