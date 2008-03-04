#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -t uint8 -f lum   -n 1 -w 10 -h 10 -c red	> /dev/null 
$CVTOOL create -t float -f lum   -n 2 -w 10 -h 10 -c green	> /dev/null 
$CVTOOL create -t uint8 -f color -n 3 -w 10 -h 10 -c 0xff00ff	> /dev/null 
$CVTOOL create -t float -f color -n 4 -w 10 -h 10 -c r1g2b3	> /dev/null 

cmd_tests_cleanup
