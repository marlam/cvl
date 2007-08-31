#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 10 -h 10 -c red   > r.pnm 

$CVTOOL laplace -c 1.0 < r.pnm > xr.pnm
cmp r.pnm xr.pnm 

cmd_tests_cleanup
