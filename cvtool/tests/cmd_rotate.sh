#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 10 -h 10 -c red   > r.pnm 
$CVTOOL create -w 10 -h 10 -c green > g.pnm 
$CVTOOL create -w 10 -h 10 -c blue  > b.pnm 
$CVTOOL combine r.pnm g.pnm b.pnm > rgb.pnm 
$CVTOOL flip < rgb.pnm | $CVTOOL flop > rgb180.pnm 

$CVTOOL rotate -i none -a 180 < rgb.pnm > xrgb180.pnm 
cmp rgb180.pnm xrgb180.pnm 

$CVTOOL rotate -i none -a 90 < rgb.pnm | $CVTOOL rotate -i none -a -90 > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

cmd_tests_cleanup
