#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t gray -w 9 -h 9 -c 0x000000 > 00.pgm 
$CVTOOL create -t gray -w 9 -h 9 -c 0x111111 > 11.pgm 
$CVTOOL create -t gray -w 9 -h 9 -c 0x222222 > 22.pgm 
$CVTOOL create -t gray -w 9 -h 9 -c 0x555555 > 55.pgm 
$CVTOOL create -t gray -w 9 -h 9 -c 0xaaaaaa > aa.pgm 
$CVTOOL create -t gray -w 9 -h 9 -c 0xffffff > ff.pgm 
$CVTOOL combine 00.pgm 11.pgm 22.pgm > i.pgm 
$CVTOOL combine 55.pgm aa.pgm ff.pgm > o.pgm 

$CVTOOL equalize < i.pgm > xo.pgm 
cmp o.pgm xo.pgm 

cmd_tests_cleanup
