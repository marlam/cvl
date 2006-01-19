#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t gray -w 1 -h 3 -c0x000000 > black.pgm
$CVTOOL create -t gray -w 1 -h 3 -c0xffffff > white.pgm
$CVTOOL combine -m lr white.pgm black.pgm black.pgm > depth0.pgm
$CVTOOL combine -m lr black.pgm white.pgm black.pgm > depth1.pgm
$CVTOOL combine -m lr black.pgm black.pgm white.pgm > depth2.pgm

cat > flow-fw.txt <<EOF
CVL FIELD 3 3
1 0
0 0
0 0
1 0
0 0
0 0
1 0
0 0
0 0
CVL FIELD 3 3
0 0
1 0
0 0
0 0
1 0
0 0
0 0
1 0
0 0
EOF
cat > flow-bw.txt <<EOF
CVL FIELD 3 3
0 0
-1 0
0 0
0 0
-1 0
0 0
0 0
-1 0
0 0
CVL FIELD 3 3
0 0
0 0
-1 0
0 0
0 0
-1 0
0 0
0 0
-1 0
EOF

$CVTOOL trackdepth -n 3 -d 0     -f flow-fw.txt -F flow-bw.txt depth0.pgm                        > /dev/null
$CVTOOL trackdepth -n 3 -d   1   -f flow-fw.txt -F flow-bw.txt            depth1.pgm             > /dev/null
$CVTOOL trackdepth -n 3 -d     2 -f flow-fw.txt -F flow-bw.txt                       depth2.pgm  > /dev/null
$CVTOOL trackdepth -n 3 -d 0,1   -f flow-fw.txt -F flow-bw.txt depth0.pgm depth1.pgm             > /dev/null
$CVTOOL trackdepth -n 3 -d   1,2 -f flow-fw.txt -F flow-bw.txt            depth1.pgm depth2.pgm  > /dev/null
$CVTOOL trackdepth -n 3 -d 0,2   -f flow-fw.txt -F flow-bw.txt depth0.pgm            depth2.pgm  > /dev/null
$CVTOOL trackdepth -n 3 -d 0,1,2 -f flow-fw.txt -F flow-bw.txt depth0.pgm depth1.pgm depth2.pgm  > /dev/null

cmd_tests_cleanup
