#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t gray -n 1 -w 10 -h 10 -c blue  				> /dev/null 
$CVTOOL create -t rgb  -n 2 -w 1  -h 1  -c yellow				> /dev/null 
$CVTOOL create -t yuv  -n 3 -w 90 -h 4  -c orange -C 420jpeg -F 25:1 -A 16:9	> /dev/null 
$CVTOOL create -t yuv  -n 3 -w 99 -h 3  -c orange -C 444     -F 1:1  -A 4:3	> /dev/null 
$CVTOOL create -t yuv  -n 3 -w 90 -h 4  -c orange -C 420jpeg -F 0:0  -A 100:1	> /dev/null 

cmd_tests_cleanup
