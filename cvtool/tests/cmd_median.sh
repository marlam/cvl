#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm 
$CVTOOL merge -o merge1.txt r.pnm g.pnm b.pnm > rgb.pnm 

$CVTOOL median -k 1 < rgb.pnm > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

$CVTOOL median -3 -k 0 < rgb.pnm > yrgb.pnm 
cmp rgb.pnm yrgb.pnm 

$CVTOOL median -a -k 1 < rgb.pnm > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

$CVTOOL median -a -3 -k 1 < rgb.pnm > /dev/null

cmd_tests_cleanup
