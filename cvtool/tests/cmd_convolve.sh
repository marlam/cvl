#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm 
$CVTOOL merge -o merge1.txt r.pnm g.pnm b.pnm > rgb.pnm 

# do the same as the 3D mean filter with k=1
$CVTOOL create -n 1 -w 99 -h 99 -c 0xaa5500 > 1.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x555555 > 2.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x0055aa > 3.pnm 
$CVTOOL merge -o merge2.txt 1.pnm 2.pnm 3.pnm > 123.pnm 
$CVTOOL convolve -X 3:1,1,1 -Y 1,1,1 -T 3:1,1,1 < rgb.pnm > x123.pnm 
cmp 123.pnm x123.pnm 
$CVTOOL convolve -K 3x3x3:1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 < rgb.pnm > y123.pnm 
cmp 123.pnm y123.pnm 

cmd_tests_cleanup
