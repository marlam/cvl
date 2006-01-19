#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c red   > r.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c green > g.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c blue  > b.pnm 
$CVTOOL merge -o merge.txt r.pnm g.pnm > rg.pnm 
$CVTOOL merge -o merge.txt r.pnm g.pnm b.pnm > rgb.pnm 

$CVTOOL split -n 1 -t x%1N.pnm < rgb.pnm 
cmp r.pnm x0.pnm 
cmp g.pnm x1.pnm 
cmp b.pnm x2.pnm 

$CVTOOL split -n 1 -t xx%3N.pnm -b -s 9 < rgb.pnm 
cmp r.pnm xx009.pnm 
cmp g.pnm xx008.pnm 
cmp b.pnm xx007.pnm 

$CVTOOL split -n 2 -t xxx%1N.pnm -s 5 < rgb.pnm 
cmp rg.pnm xxx5.pnm 
cmp b.pnm xxx7.pnm 

cmd_tests_cleanup
