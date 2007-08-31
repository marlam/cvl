#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 10 -h 10 -c red > r.pnm 
$CVTOOL gauss -k 2 < r.pnm > g.pnm

$CVTOOL unsharpmask -u g.pnm -c 0.75 < r.pnm > xr.pnm
cmp r.pnm xr.pnm 

cmd_tests_cleanup
