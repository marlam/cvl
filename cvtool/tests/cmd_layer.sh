#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x000000 > 0.pnm
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xffffff > 1.pnm

$CVTOOL layer --mode=min r.pnm g.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=min r.pnm 0.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=min r.pnm r.pnm > xr.pnm
cmp xr.pnm r.pnm

$CVTOOL layer --mode=max r.pnm g.pnm b.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=max r.pnm 0.pnm > xr.pnm
cmp xr.pnm r.pnm
$CVTOOL layer --mode=max 0.pnm 0.pnm > x0.pnm
cmp x0.pnm 0.pnm

$CVTOOL layer --mode=or r.pnm g.pnm b.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=or r.pnm 1.pnm 0.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=or r.pnm 0.pnm 0.pnm > xr.pnm
cmp xr.pnm r.pnm

cmd_tests_cleanup
