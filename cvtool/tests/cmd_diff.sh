#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xff00ff > rb.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x000000 > 0.pnm 
$CVTOOL merge -o merge1.txt r.pnm g.pnm b.pnm > rgb.pnm 
$CVTOOL merge -o merge2.txt b.pnm g.pnm r.pnm > bgr.pnm 
$CVTOOL merge -o merge3.txt 0.pnm 0.pnm 0.pnm > 000.pnm 
$CVTOOL merge -o merge4.txt rb.pnm 0.pnm rb.pnm > rb0rb.pnm 

$CVTOOL diff -s -o d1.txt rgb.pnm rgb.pnm > x000.pnm 
cmp 000.pnm x000.pnm 
$CVTOOL diff -s rgb.pnm bgr.pnm > xrb0rb.pnm 2> d2.txt
cmp rb0rb.pnm xrb0rb.pnm 

$CVTOOL diff -s -o - rgb.pnm rgb.pnm > d3.txt
cmp d1.txt d3.txt

cmd_tests_cleanup
