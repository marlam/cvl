#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -f color -n 5 -w 10 -h 10 -c black	> tmp.pnm 
$CVTOOL foreach -n 2 "$CVTOOL invert" < tmp.pnm > tmpi.pnm 
$CVTOOL foreach -n 5 "$CVTOOL invert" < tmpi.pnm > tmpii.pnm 
cmp tmp.pnm tmpii.pnm 

cmd_tests_cleanup
