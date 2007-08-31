#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm 
$CVTOOL merge -o merge1.txt r.pnm g.pnm b.pnm > rgb.pnm 

$CVTOOL gauss -k1 < rgb.pnm > /dev/null 
$CVTOOL gauss -x 1 -y 2 --sigma-x=0.5 --sigma-y=1.0 < rgb.pnm > /dev/null 
$CVTOOL gauss -3 -k1 < rgb.pnm > /dev/null 
$CVTOOL gauss -x 1 -y 2 -t 3 --sigma-x=0.5 --sigma-y=1.0 --sigma-t=1.5 < rgb.pnm > /dev/null 

cmd_tests_cleanup
