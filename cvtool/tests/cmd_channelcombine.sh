#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm
$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm
$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm
$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0x000000 > 0.pnm
$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0xffffff > 1.pnm

$CVTOOL channelcombine r.pnm g.pnm b.pnm > xr.pnm
cmp r.pnm xr.pnm
$CVTOOL channelcombine g.pnm r.pnm b.pnm > xg.pnm
cmp g.pnm xg.pnm
$CVTOOL channelcombine g.pnm b.pnm r.pnm > xb.pnm
cmp b.pnm xb.pnm

$CVTOOL channelcombine r.pnm r.pnm r.pnm > x1.pnm
cmp 1.pnm x1.pnm
$CVTOOL channelcombine g.pnm g.pnm g.pnm > x0.pnm
cmp 0.pnm x0.pnm

cmd_tests_cleanup
