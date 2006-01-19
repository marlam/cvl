#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t gray -w 9 -h 9 -c 0x111111 > 11.pgm 
$CVTOOL create -t gray -w 9 -h 9 -c 0xeeeeee > ee.pgm 
$CVTOOL create -t rgb  -w 9 -h 9 -c red   > r.pnm 
$CVTOOL create -t rgb  -w 9 -h 9 -c green > g.pnm 
$CVTOOL create -t rgb  -w 9 -h 9 -c blue  > b.pnm 
$CVTOOL combine r.pnm g.pnm b.pnm > rgb.pnm 

$CVTOOL invert < 11.pgm > xee.pgm 
cmp ee.pgm xee.pgm 

$CVTOOL invert < rgb.pnm | $CVTOOL invert > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

cmd_tests_cleanup
