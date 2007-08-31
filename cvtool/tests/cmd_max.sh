#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -n 1 -w 99 -h 99 -c 0xaa0000 > l.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x005500 > m.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x000022 > d.pnm 
$CVTOOL merge -o merge3.txt l.pnm m.pnm d.pnm > lmd.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0xaa5500 > X.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0xaa5522 > Y.pnm 
$CVTOOL create -n 1 -w 99 -h 99 -c 0x005522 > Z.pnm 
$CVTOOL merge -o merge3.txt X.pnm Y.pnm Z.pnm > XYZ.pnm 

$CVTOOL max -3 -k 1 < lmd.pnm > xXYZ.pnm
cmp XYZ.pnm xXYZ.pnm

cmd_tests_cleanup
