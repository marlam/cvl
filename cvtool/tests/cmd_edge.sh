#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -h 1 -w 1 > dummy.pnm 

$CVTOOL edge sobel 			< dummy.pnm > /dev/null 
$CVTOOL edge canny -s 1.0 -l 10 -h 30	< dummy.pnm > /dev/null 

cmd_tests_cleanup
