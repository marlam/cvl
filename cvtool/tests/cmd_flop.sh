#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 10 -h 10 -c red   > r.pnm 
$CVTOOL create -w 10 -h 10 -c green > g.pnm 
$CVTOOL create -w 10 -h 10 -c blue  > b.pnm 
$CVTOOL combine r.pnm g.pnm b.pnm > rgb.pnm 
$CVTOOL combine b.pnm g.pnm r.pnm > bgr.pnm 

$CVTOOL flop < r.pnm > xr.pnm 
cmp r.pnm xr.pnm 

$CVTOOL flop < rgb.pnm > xbgr.pnm 
cmp bgr.pnm xbgr.pnm 

cmd_tests_cleanup
