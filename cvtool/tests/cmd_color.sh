#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 9 -h 9 -c 0xff0000 > r.pnm 
$CVTOOL create -w 9 -h 9 -c 0x00ff00 > g.pnm 
$CVTOOL create -w 9 -h 9 -c 0x0000ff > b.pnm 
$CVTOOL create -w 9 -h 9 -c 0x000000 > 0.pnm 

# 1. Hue
$CVTOOL color -h 120 < r.pnm > xg.pnm 
cmp g.pnm xg.pnm 
$CVTOOL color -h 240 < r.pnm > xb.pnm 
cmp b.pnm xb.pnm 

# 2. Saturation
$CVTOOL create -w 9 -h 9 -c r128g128b128 > s1.pnm 
$CVTOOL create -w 9 -h 9 -c r127g127b127 > s2.pnm 
$CVTOOL color -s -1 < r.pnm > xs.pnm 
# FIXME: The result is either s1 or s2. How do I check this??
#cmp s1.pnm xs.pnm 

# 3. Lightness
$CVTOOL create -w 9 -h 9 -c 0xffffff > l.pnm 
$CVTOOL color -l +15 < g.pnm > xl.pnm 
cmp l.pnm xl.pnm 

# 4. Contrast
$CVTOOL color -c -1 < r.pnm > /dev/null 
$CVTOOL color -c +1 < r.pnm > /dev/null 

cmd_tests_cleanup
