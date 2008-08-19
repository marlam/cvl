#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 10 -h 10 -c red -t float > red.pfs 

$CVTOOL dwt -t dwt -D 2 -l 1 < red.pfs > dwt.pfs
$CVTOOL dwt -t idwt -D 2 -l 1 < dwt.pfs > xred.pfs
cmp red.pfs xred.pfs
$CVTOOL dwt -t soft-thresholding -D 2 -l 1 -T 0 < dwt.pfs > st.pfs
$CVTOOL dwt -t idwt -D 2 -l 1 < st.pfs > xxred.pfs
cmp red.pfs xxred.pfs

cmd_tests_cleanup
