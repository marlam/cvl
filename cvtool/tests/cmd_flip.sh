#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 10 -h 10 -c red   > r.pnm 
$CVTOOL create -w 10 -h 10 -c green > g.pnm 
$CVTOOL create -w 10 -h 10 -c blue  > b.pnm 
$CVTOOL combine -m topbottom r.pnm g.pnm b.pnm > rgb.pnm 
$CVTOOL combine -m topbottom b.pnm g.pnm r.pnm > bgr.pnm 

$CVTOOL flip < r.pnm > xr.pnm 
cmp r.pnm xr.pnm 

$CVTOOL flip < rgb.pnm > xbgr.pnm 
cmp bgr.pnm xbgr.pnm 

cmd_tests_cleanup
