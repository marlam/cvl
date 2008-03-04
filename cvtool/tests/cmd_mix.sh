#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -n 1 -w 99 -h 99 -c r000g000b000 > 000.pnm
$CVTOOL create -n 1 -w 99 -h 99 -c r100g100b100 > 100.pnm
$CVTOOL create -n 1 -w 99 -h 99 -c r200g200b200 > 200.pnm

$CVTOOL mix -w 1,0,0 000.pnm 100.pnm 200.pnm > x000.pnm
cmp x000.pnm 000.pnm
$CVTOOL mix -w 0,1,0 000.pnm 100.pnm 200.pnm > x100.pnm
cmp x100.pnm 100.pnm
$CVTOOL mix -w 0,0,1 000.pnm 100.pnm 200.pnm > x200.pnm
cmp x200.pnm 200.pnm

$CVTOOL mix -w 4711 200.pnm > x200.pnm
cmp x200.pnm 200.pnm

$CVTOOL mix -w 1,1 000.pnm 200.pnm > x100.pnm
cmp x100.pnm 100.pnm

$CVTOOL mix -w 1,1,1 000.pnm 100.pnm 200.pnm > x100.pnm
cmp x100.pnm 100.pnm

cmd_tests_cleanup
