#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 7 -h 7 > black.ppm
$CVTOOL create -w 1 -h 1 -c r255g255b255 > dot.ppm
$CVTOOL blend -s dot.ppm -S -x 3 -y 3 < black.ppm > dotted.ppm
$CVTOOL merge -o merge.txt black.ppm black.ppm black.ppm dotted.ppm black.ppm black.ppm black.ppm > cube.ppm

$CVTOOL sedt < dotted.ppm > sedt-dotted.field
$CVTOOL sedt -3 < cube.ppm > sedt-cube.field

cmd_tests_cleanup
