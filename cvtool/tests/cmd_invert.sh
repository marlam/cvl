#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -f lum   -w 9 -h 9 -c 0x111111 > 11.pnm 
$CVTOOL create -f lum   -w 9 -h 9 -c 0xeeeeee > ee.pnm 
$CVTOOL create -f color -w 9 -h 9 -c red   > r.pnm 
$CVTOOL create -f color -w 9 -h 9 -c green > g.pnm 
$CVTOOL create -f color -w 9 -h 9 -c blue  > b.pnm 
$CVTOOL combine r.pnm g.pnm b.pnm > rgb.pnm 

$CVTOOL invert < 11.pnm > xee.pnm 
cmp ee.pnm xee.pnm 

$CVTOOL invert < rgb.pnm | $CVTOOL invert > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

$CVTOOL convert -t float < 11.pnm | $CVTOOL invert | $CVTOOL convert -t uint8 > yee.pnm
cmp ee.pnm yee.pnm

$CVTOOL convert -t float < rgb.pnm | $CVTOOL invert | $CVTOOL invert | $CVTOOL convert -t uint8 > yrgb.pnm
cmp rgb.pnm yrgb.pnm

cmd_tests_cleanup
