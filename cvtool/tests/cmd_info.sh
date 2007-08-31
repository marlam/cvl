#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t uint8 -f lum   -n 1 -w  10 -h  10 -c blue	> t1.pnm
$CVTOOL create -t float -f lum   -n 2 -w   1 -h   1 -c blue	> t2.pfs
$CVTOOL create -t uint8 -f color -n 3 -w 100 -h 100 -c blue	> t3.pnm
$CVTOOL create -t float -f color -n 4 -w  10 -h  10 -c blue	> t4.pfs

echo "STREAM=pnm CHANNELS=1 FORMAT=luminance TYPE=uint8 WIDTH=10 HEIGHT=10"	>  t1.txt
echo "STREAM=pfs CHANNELS=1 FORMAT=luminance TYPE=float WIDTH=1 HEIGHT=1"	>  t2.txt
echo "STREAM=pfs CHANNELS=1 FORMAT=luminance TYPE=float WIDTH=1 HEIGHT=1"	>> t2.txt
echo "STREAM=pnm CHANNELS=3 FORMAT=color TYPE=uint8 WIDTH=100 HEIGHT=100"	>  t3.txt
echo "STREAM=pnm CHANNELS=3 FORMAT=color TYPE=uint8 WIDTH=100 HEIGHT=100"	>> t3.txt
echo "STREAM=pnm CHANNELS=3 FORMAT=color TYPE=uint8 WIDTH=100 HEIGHT=100"	>> t3.txt
echo "STREAM=pfs CHANNELS=3 FORMAT=color TYPE=float WIDTH=10 HEIGHT=10"		>  t4.txt
echo "STREAM=pfs CHANNELS=3 FORMAT=color TYPE=float WIDTH=10 HEIGHT=10"		>> t4.txt
echo "STREAM=pfs CHANNELS=3 FORMAT=color TYPE=float WIDTH=10 HEIGHT=10"		>> t4.txt
echo "STREAM=pfs CHANNELS=3 FORMAT=color TYPE=float WIDTH=10 HEIGHT=10"		>> t4.txt
echo "STREAM=pfs CHANNELS=3 FORMAT=color TYPE=float WIDTH=10 HEIGHT=10"		>  t4s.txt

$CVTOOL info -o xt1.txt < t1.pnm
cmp t1.txt xt1.txt
$CVTOOL info -o xt2.txt < t2.pfs
cmp t2.txt xt2.txt
$CVTOOL info -o xt3.txt < t3.pnm
cmp t3.txt xt3.txt
$CVTOOL info -o xt4.txt < t4.pfs
cmp t4.txt xt4.txt

$CVTOOL info -S -o xt4s.txt < t4.pfs
cmp t4s.txt xt4s.txt

$CVTOOL info -S -s -o dummy.txt < t3.pnm

cmd_tests_cleanup
