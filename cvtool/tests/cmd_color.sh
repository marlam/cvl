#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c 0xff0000 > r.pnm 
$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c 0x00ff00 > g.pnm 
$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c 0x0000ff > b.pnm 
$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c 0x000000 > 0.pnm 

# 1. Hue
$CVTOOL color -h 120 < r.pnm > xg.pnm 
cmp g.pnm xg.pnm 
$CVTOOL color -h 240 < r.pnm > xb.pnm 
cmp b.pnm xb.pnm 

# 2. Saturation
$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c 127,127,127 > s.pnm 
$CVTOOL color -s -1 < r.pnm > xs.pnm 
cmp s.pnm xs.pnm 

# 3. Lightness
$CVTOOL create -t rgb -n 1 -w 9 -h 9 -c 0xffffff > l.pnm 
$CVTOOL color -l +15 < g.pnm > xl.pnm 
cmp l.pnm xl.pnm 

# 4. Contrast
$CVTOOL color -c -1 < r.pnm > /dev/null 
$CVTOOL color -c +1 < r.pnm > /dev/null 

# 5. Gamma
$CVTOOL color -g 0.001 < r.pnm > /dev/null 
$CVTOOL color -g 100.1 < r.pnm > /dev/null 
$CVTOOL color -g 1.0,1.0,1.0 < r.pnm > xr1.pnm
cmp r.pnm xr1.pnm
$CVTOOL color -g 1.0 < r.pnm > xr2.pnm
cmp r.pnm xr2.pnm


cmd_tests_cleanup
