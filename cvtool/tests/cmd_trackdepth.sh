#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t gray -w 10 -h 30 -c0x000000 > black.pgm
$CVTOOL create -t gray -w 10 -h 30 -c0xffffff > white.pgm
$CVTOOL combine -m lr white.pgm black.pgm black.pgm > depth0.pgm
$CVTOOL combine -m lr black.pgm white.pgm black.pgm > depth1.pgm
$CVTOOL combine -m lr black.pgm black.pgm white.pgm > depth2.pgm
$CVTOOL combine -m lr black.pgm black.pgm black.pgm > bbb.pgm
$CVTOOL combine -m lr white.pgm white.pgm white.pgm > www.pgm

$CVTOOL merge -o /dev/null bbb.pgm www.pgm bbb.pgm www.pgm bbb.pgm > video.pgm
$CVTOOL opticalflow bm-sad    -k1 -M1 -D0.1 -L0.5 < video.pgm > flow-fw.field
$CVTOOL opticalflow bm-sad -b -k1 -M1 -D0.1 -L0.5 < video.pgm > flow-bw.field
$CVTOOL trackdepth -n 3 -d 0     -f flow-fw.field -F flow-bw.field depth0.pgm                        > /dev/null
$CVTOOL trackdepth -n 3 -d   1   -f flow-fw.field -F flow-bw.field            depth1.pgm             > /dev/null
$CVTOOL trackdepth -n 3 -d     2 -f flow-fw.field -F flow-bw.field                       depth2.pgm  > /dev/null
$CVTOOL trackdepth -n 3 -d 0,1   -f flow-fw.field -F flow-bw.field depth0.pgm depth1.pgm             > /dev/null
$CVTOOL trackdepth -n 3 -d   1,2 -f flow-fw.field -F flow-bw.field            depth1.pgm depth2.pgm  > /dev/null
$CVTOOL trackdepth -n 3 -d 0,2   -f flow-fw.field -F flow-bw.field depth0.pgm            depth2.pgm  > /dev/null
$CVTOOL trackdepth -n 3 -d 0,1,2 -f flow-fw.field -F flow-bw.field depth0.pgm depth1.pgm depth2.pgm  > /dev/null

cmd_tests_cleanup
