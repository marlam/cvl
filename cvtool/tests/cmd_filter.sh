#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm 
$CVTOOL merge -o merge1.txt r.pnm g.pnm b.pnm > rgb.pnm 

# 1. average
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xaa5500 > 1.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x555555 > 2.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x0055aa > 3.pnm 
$CVTOOL merge -o merge2.txt 1.pnm 2.pnm 3.pnm > 123.pnm 
$CVTOOL filter average -3 -k 1 < rgb.pnm > x123.pnm 
cmp 123.pnm x123.pnm 

# 2. median
$CVTOOL filter median -3 -k 1 < rgb.pnm > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

# 3. gauss
$CVTOOL filter gauss -x 1 -y 2 -t 3 --sigma-x=0.5 --sigma-y=1.0 --sigma-t=1.5 < rgb.pnm > /dev/null 

cmd_tests_cleanup
