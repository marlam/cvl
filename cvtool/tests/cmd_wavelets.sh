#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 10 -h 10 -c red > red.ppm
$CVTOOL convert -t float < red.ppm > red.pfs

$CVTOOL wavelets -t dwt -D 2 -l 1 < red.pfs > dwt.pfs
$CVTOOL wavelets -t idwt -D 2 -l 1 < dwt.pfs > xred.pfs
$CVTOOL convert -t uint8 < xred.pfs > xred.ppm
cmp red.ppm xred.ppm
$CVTOOL wavelets -t soft-thresholding -l 1 -T 0 < dwt.pfs > st.pfs
$CVTOOL wavelets -t hard-thresholding -l 1 -T 0 < dwt.pfs > ht.pfs
$CVTOOL wavelets -t idwt -D 2 -l 1 < st.pfs > xxred.pfs
$CVTOOL convert -t uint8 < xxred.pfs > xxred.ppm
cmp red.ppm xxred.ppm

cmd_tests_cleanup
