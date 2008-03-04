#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 100 -h 100 -c red > box.pnm 

$CVTOOL shear --shear-x 45.0 -i none < box.pnm > /dev/null
$CVTOOL shear --shear-y 45.0 -i none < box.pnm > /dev/null
$CVTOOL shear --shear-x -12.3 --shear-y -39.1 < box.pnm > /dev/null
$CVTOOL shear --shear-x 0 --shear-y 0 -i none < box.pnm > xbox.pnm
cmp box.pnm xbox.pnm

cmd_tests_cleanup
