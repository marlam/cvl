#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 9 -h 9 -c 0xff0000 > r.pnm 
$CVTOOL create -w 9 -h 9 -c 0x00ff00 > g.pnm 
$CVTOOL create -w 9 -h 9 -c 0x0000ff > b.pnm 

$CVTOOL gamma -g 1.0 < r.pnm > xr.pnm
cmp r.pnm xr.pnm
$CVTOOL gamma -g 1.0 < g.pnm > xg.pnm
cmp g.pnm xg.pnm
$CVTOOL gamma -g 1.0 < b.pnm > xb.pnm
cmp b.pnm xb.pnm
$CVTOOL gamma -g 0.001 < r.pnm > /dev/null 
$CVTOOL gamma -g 100.1 < r.pnm > /dev/null 

cmd_tests_cleanup
