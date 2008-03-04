#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -t float -w 100 -h 100 -c 0x000000 > 0.pfs

$CVTOOL visualize scalar < 0.pfs > /dev/null
$CVTOOL visualize scalar -m 0 -M 1 < 0.pfs > /dev/null

$CVTOOL visualize vector2 --mode color  < 0.pfs > /dev/null
$CVTOOL visualize vector2 --mode needle < 0.pfs > /dev/null

cmd_tests_cleanup
