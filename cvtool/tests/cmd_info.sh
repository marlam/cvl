#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t gray -n 1 -w 10 -h 10 -c blue > t1.pgm
echo "STREAMTYPE=pnm PIXELTYPE=gray WIDTH=10 HEIGHT=10" > t1.txt
$CVTOOL info -o xt1.txt < t1.pgm
cmp t1.txt xt1.txt

$CVTOOL create -t rgb  -n 2 -w 1  -h 1  -c yellow > t2.pnm
echo "STREAMTYPE=pnm PIXELTYPE=rgb WIDTH=1 HEIGHT=1" > t2.txt
$CVTOOL info -o xt2.txt < t2.pnm
cmp t2.txt xt2.txt

$CVTOOL create -t yuv  -n 3 -w 90 -h 4  -c orange -C 420jpeg -F 25:1 -A 16:9	> t3.y4m
echo "STREAMTYPE=y4m PIXELTYPE=yuv WIDTH=90 HEIGHT=4 INTERLACING=unknown CHROMA=420jpeg FRAMERATE=25:1 ASPECTRATIO=16:9" > t3.txt
$CVTOOL info -o xt3.txt < t3.y4m
cmp t3.txt xt3.txt

$CVTOOL create -t yuv  -n 3 -w 99 -h 3  -c orange -C 444     -F 1:1  -A 4:3	> t4.y4m
echo "STREAMTYPE=y4m PIXELTYPE=yuv WIDTH=99 HEIGHT=3 INTERLACING=unknown CHROMA=444 FRAMERATE=1:1 ASPECTRATIO=4:3" > t4.txt
$CVTOOL info -o xt4.txt < t4.y4m
cmp t4.txt xt4.txt

cmd_tests_cleanup
