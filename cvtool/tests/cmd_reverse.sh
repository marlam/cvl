#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c red   > r.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c green > g.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c blue  > b.pnm 
$CVTOOL merge -o merge.txt r.pnm g.pnm b.pnm > rgb.pnm 

$CVTOOL reverse < rgb.pnm | $CVTOOL select 0 > xb.pnm 
cmp b.pnm xb.pnm 

$CVTOOL reverse < rgb.pnm | $CVTOOL reverse > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

cmd_tests_cleanup
