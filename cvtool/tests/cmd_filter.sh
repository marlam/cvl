#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xff0000 > r.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x00ff00 > g.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x0000ff > b.pnm 
$CVTOOL merge -o merge1.txt r.pnm g.pnm b.pnm > rgb.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xaa5500 > 1.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x555555 > 2.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x0055aa > 3.pnm 
$CVTOOL merge -o merge2.txt 1.pnm 2.pnm 3.pnm > 123.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0xaa0000 > l.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x005500 > m.pnm 
$CVTOOL create -t rgb -n 1 -w 99 -h 99 -c 0x000022 > d.pnm 
$CVTOOL merge -o merge3.txt l.pnm m.pnm d.pnm > lmd.pnm 
$CVTOOL merge -o merge4.txt m.pnm d.pnm d.pnm > mdd.pnm 
$CVTOOL merge -o merge5.txt l.pnm l.pnm m.pnm > llm.pnm 

# mean
$CVTOOL filter mean -3 -k 1 < rgb.pnm > x123.pnm 
cmp 123.pnm x123.pnm 

# min
$CVTOOL filter min -3 -k 1 < lmd.pnm > xmdd.pnm
cmp mdd.pnm xmdd.pnm

# max
$CVTOOL filter max -3 -k 1 < lmd.pnm > xllm.pnm
cmp llm.pnm xllm.pnm

# median
$CVTOOL filter median -3 -k 1 < rgb.pnm > xrgb.pnm 
cmp rgb.pnm xrgb.pnm 

# wallis
$CVTOOL filter wallis -k 2 -m 210 -s 20 -g 7 -r 1 < rgb.pnm > /dev/null
$CVTOOL filter wallis -3 -k 1 -m 80 -s 5 -g 1 -r 0.5 < rgb.pnm > /dev/null

# gauss
$CVTOOL filter gauss -x 1 -y 2 -t 3 --sigma-x=0.5 --sigma-y=1.0 --sigma-t=1.5 < rgb.pnm > /dev/null 

cmd_tests_cleanup
