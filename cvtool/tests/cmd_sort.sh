#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -t uint8 -f lum -w 100 -h 100 -c 0xffffff > a.pnm
$CVTOOL create -t uint8 -f lum -w 100 -h 100 -c 0x000000 > b.pnm
$CVTOOL combine -m topbottom a.pnm b.pnm > ab.pnm 
$CVTOOL combine -m topbottom b.pnm a.pnm > ba.pnm

$CVTOOL sort < ab.pnm > xba.pnm
cmp ba.pnm xba.pnm

cmd_tests_cleanup
