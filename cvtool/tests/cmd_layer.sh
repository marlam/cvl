#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm
$CVTOOL create -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm
$CVTOOL create -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm
$CVTOOL create -n 1 -w 99 -h 99 -c 0x000000 > 0.pnm
$CVTOOL create -n 1 -w 99 -h 99 -c 0xffffff > 1.pnm

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

$CVTOOL layer --mode=median r.pnm g.pnm b.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=median r.pnm 0.pnm > xr.pnm
cmp xr.pnm r.pnm
$CVTOOL layer --mode=median 0.pnm 0.pnm > x0.pnm
cmp x0.pnm 0.pnm

$CVTOOL layer --mode=or r.pnm g.pnm b.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=or r.pnm 1.pnm 0.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=or r.pnm 0.pnm 0.pnm > xr.pnm
cmp xr.pnm r.pnm

$CVTOOL layer --mode=and r.pnm g.pnm b.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=and r.pnm 1.pnm 0.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=and r.pnm 1.pnm r.pnm > xr.pnm
cmp xr.pnm r.pnm

$CVTOOL layer --mode=xor r.pnm r.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=xor 1.pnm 0.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=xor 0.pnm 0.pnm 0.pnm > x0.pnm
cmp x0.pnm 0.pnm

$CVTOOL layer --mode=diff r.pnm g.pnm b.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=diff 1.pnm 1.pnm > x0.pnm
cmp x0.pnm 0.pnm

$CVTOOL layer --mode=add r.pnm g.pnm b.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=add 0.pnm 1.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=add 1.pnm 1.pnm > x1.pnm
cmp x1.pnm 1.pnm

$CVTOOL layer --mode=xadd r.pnm r.pnm > xr.pnm
cmp xr.pnm r.pnm
$CVTOOL layer --mode=xadd 1.pnm 1.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=xadd 0.pnm 0.pnm > x0.pnm
cmp x0.pnm 0.pnm

$CVTOOL layer --mode=sub r.pnm r.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=sub 1.pnm 0.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=sub 0.pnm 1.pnm > x0.pnm
cmp x0.pnm 0.pnm

$CVTOOL layer --mode=xsub r.pnm r.pnm > /dev/null
$CVTOOL layer --mode=xsub 1.pnm 0.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=xsub 0.pnm 1.pnm > x0.pnm
cmp x0.pnm 0.pnm

$CVTOOL layer --mode=mul r.pnm g.pnm b.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=mul 1.pnm 1.pnm > x1.pnm
cmp x1.pnm 1.pnm
$CVTOOL layer --mode=mul 0.pnm r.pnm g.pnm b.pnm > x0.pnm
cmp x0.pnm 0.pnm

$CVTOOL layer --mode=div 0.pnm r.pnm > x0.pnm
cmp x0.pnm 0.pnm
$CVTOOL layer --mode=div r.pnm r.pnm > /dev/null
$CVTOOL layer --mode=div r.pnm g.pnm > /dev/null

cmd_tests_cleanup
