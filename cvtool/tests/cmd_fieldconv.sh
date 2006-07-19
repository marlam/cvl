#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 7 -h 7 -t gray > black.pgm
$CVTOOL fieldconv -i frame -o gray < black.pgm > xblack.pgm
cmp black.pgm xblack.pgm

$CVTOOL create -w 7 -h 7 -t rgb -c r10g20b20 > color.ppm
$CVTOOL fieldconv -i frame -o rgb < color.ppm > xcolor.ppm
cmp color.ppm xcolor.ppm

cmd_tests_cleanup
