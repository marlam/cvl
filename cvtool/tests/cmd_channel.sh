#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm
$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm
$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm
$CVTOOL create -f color -n 1 -w 99 -h 99 -c 0x000000 > 0.pnm
$CVTOOL merge -o merge.txt r.pnm g.pnm b.pnm > rgb.pnm
$CVTOOL merge -o merge.txt r.pnm 0.pnm 0.pnm > r00.pnm
$CVTOOL merge -o merge.txt 0.pnm g.pnm 0.pnm > 0g0.pnm
$CVTOOL merge -o merge.txt 0.pnm 0.pnm b.pnm > 00b.pnm
$CVTOOL create -f lum -n 1 -w 99 -h 99 -c 0x000000 > 0.pnm.lum
$CVTOOL create -f lum -n 1 -w 99 -h 99 -c 0xffffff > 1.pnm.lum
$CVTOOL merge -o merge.txt 1.pnm.lum 0.pnm.lum 0.pnm.lum > 100.pnm.lum
$CVTOOL merge -o merge.txt 0.pnm.lum 1.pnm.lum 0.pnm.lum > 010.pnm.lum
$CVTOOL merge -o merge.txt 0.pnm.lum 0.pnm.lum 1.pnm.lum > 001.pnm.lum

$CVTOOL channel -c r < rgb.pnm > x100.pnm.lum
cmp 100.pnm.lum x100.pnm.lum
$CVTOOL channel -c g < rgb.pnm > x010.pnm.lum
cmp 010.pnm.lum x010.pnm.lum
$CVTOOL channel -c b < rgb.pnm > x001.pnm.lum
cmp 001.pnm.lum x001.pnm.lum

$CVTOOL channel -r -c r < 100.pnm.lum > xr00.pnm
cmp r00.pnm xr00.pnm
$CVTOOL channel -r -c g < 010.pnm.lum > x0g0.pnm
cmp 0g0.pnm x0g0.pnm
$CVTOOL channel -r -c b < 001.pnm.lum > x00b.pnm
cmp 00b.pnm x00b.pnm

$CVTOOL convert -t float < rgb.pnm | $CVTOOL channel -c r | $CVTOOL convert -t uint8 > y100.pnm.lum
cmp 100.pnm.lum y100.pnm.lum
$CVTOOL convert -t float < rgb.pnm | $CVTOOL channel -c g | $CVTOOL convert -t uint8 > y010.pnm.lum
cmp 010.pnm.lum y010.pnm.lum
$CVTOOL convert -t float < rgb.pnm | $CVTOOL channel -c b | $CVTOOL convert -t uint8 > y001.pnm.lum
cmp 001.pnm.lum y001.pnm.lum

$CVTOOL channel -c 0 < rgb.pnm > x100.pnm.lum
cmp 100.pnm.lum x100.pnm.lum
$CVTOOL channel -c 1 < rgb.pnm > x010.pnm.lum
cmp 010.pnm.lum x010.pnm.lum
$CVTOOL channel -c 2 < rgb.pnm > x001.pnm.lum
cmp 001.pnm.lum x001.pnm.lum

$CVTOOL channel -r -c 0 < 100.pnm.lum > xr00.pnm
cmp r00.pnm xr00.pnm
$CVTOOL channel -r -c 1 < 010.pnm.lum > x0g0.pnm
cmp 0g0.pnm x0g0.pnm
$CVTOOL channel -r -c 2 < 001.pnm.lum > x00b.pnm
cmp 00b.pnm x00b.pnm

cmd_tests_cleanup
