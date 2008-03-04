#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -n 1 -w 9 -h 9 -c red   > r.pnm 
$CVTOOL create -n 1 -w 9 -h 9 -c green > g.pnm 
$CVTOOL create -n 1 -w 9 -h 9 -c blue  > b.pnm 
$CVTOOL merge -o merge.txt r.pnm g.pnm b.pnm > rgb.pnm 

$CVTOOL select 0 < rgb.pnm > xr.pnm 
cmp r.pnm xr.pnm 
$CVTOOL select 1 < rgb.pnm > xg.pnm 
cmp g.pnm xg.pnm 
$CVTOOL select 2 < rgb.pnm > xb.pnm 
cmp b.pnm xb.pnm 
$CVTOOL select 0-2 < rgb.pnm > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 
$CVTOOL select -- 2 0-1 0- -2 - < rgb.pnm > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

$CVTOOL select --fps=1.0 0:00 < rgb.pnm > xr.pnm 
cmp r.pnm xr.pnm 
$CVTOOL select --fps=1.0 0:01 < rgb.pnm > xg.pnm 
cmp g.pnm xg.pnm 
$CVTOOL select --fps=1.0 0:02 < rgb.pnm > xb.pnm 
cmp b.pnm xb.pnm 
$CVTOOL select --fps=1.0 0:00:00.0-0:00:02.995 < rgb.pnm > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 
$CVTOOL select --fps=1.0 -- 2 0:00-1 0:0.0- -0:00:2 - < rgb.pnm > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

cmd_tests_cleanup
