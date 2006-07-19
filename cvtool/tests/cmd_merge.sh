#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c yellow > 1.pnm 
$CVTOOL create -t rgb -n 2 -w 99 -h 99 -c yellow > 2.pnm 
$CVTOOL create -t rgb -n 4 -w 99 -h 99 -c yellow > 4.pnm 
$CVTOOL merge    -o a.txt 1.pnm 2.pnm 1.pnm > a.pnm 
cmp 4.pnm a.pnm 
$CVTOOL merge -s -o b.txt 1.pnm 2.pnm 1.pnm > b.pnm 
cmp 4.pnm b.pnm 

cmd_tests_cleanup
