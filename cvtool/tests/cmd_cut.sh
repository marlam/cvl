#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 10 -h 10 -c red > red.pnm 
$CVTOOL resize -w 20 -h 20 -c black < red.pnm > redblack.pnm 

$CVTOOL cut -l 5 -t 5 -w 10 -h 10 < redblack.pnm > xred.pnm 
cmp xred.pnm red.pnm 

cmd_tests_cleanup
