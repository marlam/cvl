#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c red   > r.pnm 
$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c green > g.pnm 
$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c blue  > b.pnm 
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

$CVTOOL convert -o y4m -C 444 -F 1:1 < r.pnm   > r.y4m 
$CVTOOL convert -o y4m -C 444 -F 1:1 < g.pnm   > g.y4m 
$CVTOOL convert -o y4m -C 444 -F 1:1 < b.pnm   > b.y4m 
$CVTOOL convert -o y4m -C 444 -F 1:1 < rgb.pnm > rgb.y4m 

$CVTOOL select 0:00 < rgb.y4m > xr.y4m 
cmp r.y4m xr.y4m 
$CVTOOL select 0:01 < rgb.y4m > xg.y4m 
cmp g.y4m xg.y4m 
$CVTOOL select 0:02 < rgb.y4m > xb.y4m 
cmp b.y4m xb.y4m 
$CVTOOL select 0:00:00.0-0:00:02.995 < rgb.y4m > xrgb.y4m 
cmp rgb.y4m xrgb.y4m 
$CVTOOL select -- 2 0:00-1 0:0.0- -0:00:2 - < rgb.y4m > xrgb.y4m 
cmp rgb.y4m xrgb.y4m 

cmd_tests_cleanup
