#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x000000 > 0.pnm
$CVTOOL merge -o merge.txt r.pnm g.pnm b.pnm > rgb.pnm
$CVTOOL merge -o merge.txt r.pnm 0.pnm 0.pnm > r00.pnm
$CVTOOL merge -o merge.txt 0.pnm g.pnm 0.pnm > 0g0.pnm
$CVTOOL merge -o merge.txt 0.pnm 0.pnm b.pnm > 00b.pnm

$CVTOOL channel -c r < rgb.pnm > xr00.pnm
cmp r00.pnm xr00.pnm
$CVTOOL channel -c g < rgb.pnm > x0g0.pnm
cmp 0g0.pnm x0g0.pnm
$CVTOOL channel -c b < rgb.pnm > x00b.pnm
cmp 00b.pnm x00b.pnm

cmd_tests_cleanup
