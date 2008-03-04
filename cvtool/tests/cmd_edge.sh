#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -h 1 -w 1 > dummy.pnm 

$CVTOOL edge -m sobel 				< dummy.pnm > /dev/null 
$CVTOOL edge -m canny -s 1.0 -l 0.1 -h 0.3	< dummy.pnm > /dev/null 

cmd_tests_cleanup
