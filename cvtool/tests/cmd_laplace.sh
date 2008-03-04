#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 10 -h 10 -c red   > r.pnm 

$CVTOOL laplace -c 1.0 < r.pnm > xr.pnm
cmp r.pnm xr.pnm 

cmd_tests_cleanup
