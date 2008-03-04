#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -n 1 -w 99 -h 99 -c 0xaa0000 > l.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x005500 > m.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x000022 > d.pnm 
$CVTOOL merge -o merge3.txt l.pnm m.pnm d.pnm > lmd.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x000000 > 0.pnm 
$CVTOOL merge -o merge3.txt 0.pnm 0.pnm 0.pnm > 000.pnm 

$CVTOOL min -3 -k 1 < lmd.pnm > x000.pnm
cmp 000.pnm x000.pnm

cmd_tests_cleanup
