#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm 
$CVTOOL merge -o merge1.txt r.pnm g.pnm b.pnm > rgb.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0xaa5500 > 1.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x555555 > 2.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x0055aa > 3.pnm 
$CVTOOL merge -o merge2.txt 1.pnm 2.pnm 3.pnm > 123.pnm 

$CVTOOL mean -3 -k 1 < rgb.pnm > x123.pnm 
cmp 123.pnm x123.pnm 

$CVTOOL mean -k 1 < rgb.pnm > /dev/null
$CVTOOL mean -x 1 -y 1 < rgb.pnm > /dev/null

cmd_tests_cleanup
