#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

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

$CVTOOL channelextract -c r < rgb.pnm > x100.pnm.lum
cmp 100.pnm.lum x100.pnm.lum
$CVTOOL channelextract -c g < rgb.pnm > x010.pnm.lum
cmp 010.pnm.lum x010.pnm.lum
$CVTOOL channelextract -c b < rgb.pnm > x001.pnm.lum
cmp 001.pnm.lum x001.pnm.lum

$CVTOOL convert -t float < rgb.pnm | $CVTOOL channelextract -c r | $CVTOOL convert -t uint8 > y100.pnm.lum
cmp 100.pnm.lum y100.pnm.lum
$CVTOOL convert -t float < rgb.pnm | $CVTOOL channelextract -c g | $CVTOOL convert -t uint8 > y010.pnm.lum
cmp 010.pnm.lum y010.pnm.lum
$CVTOOL convert -t float < rgb.pnm | $CVTOOL channelextract -c b | $CVTOOL convert -t uint8 > y001.pnm.lum
cmp 001.pnm.lum y001.pnm.lum

$CVTOOL channelextract -c 0 < rgb.pnm > x100.pnm.lum
cmp 100.pnm.lum x100.pnm.lum
$CVTOOL channelextract -c 1 < rgb.pnm > x010.pnm.lum
cmp 010.pnm.lum x010.pnm.lum
$CVTOOL channelextract -c 2 < rgb.pnm > x001.pnm.lum
cmp 001.pnm.lum x001.pnm.lum

cmd_tests_cleanup
