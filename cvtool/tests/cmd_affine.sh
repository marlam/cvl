#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 10 -h 10 -c red   > r.pnm
$CVTOOL create -w 10 -h 10 -c green > g.pnm
$CVTOOL create -w 10 -h 10 -c blue  > b.pnm
$CVTOOL combine r.pnm g.pnm b.pnm > rgb.pnm
$CVTOOL create -w 20 -h 20 -c red   > R.pnm
$CVTOOL create -w 20 -h 20 -c green > G.pnm
$CVTOOL create -w 20 -h 20 -c blue  > B.pnm
$CVTOOL combine R.pnm G.pnm B.pnm > RGB.pnm

$CVTOOL affine -m 1,0,0,1 < rgb.pnm > xrgb.pnm
cmp rgb.pnm xrgb.pnm

$CVTOOL affine -m 2,0,0,2 -i none < rgb.pnm > xRGB.pnm
cmp RGB.pnm xRGB.pnm

cmd_tests_cleanup
