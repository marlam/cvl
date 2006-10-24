#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t gray -w 1 -h 1 > d.pgm 
$CVTOOL create -t rgb  -w 1 -h 1 > c.pnm 

$CVTOOL dibr -d d.pgm -b 0 -p 0    --zps 0   -h none	< c.pnm >/dev/null 
$CVTOOL dibr -d d.pgm -b 1 -p -1   --zps 1   -h linear	< c.pnm >/dev/null 
$CVTOOL dibr -d d.pgm -b 2 -p +1   --zps 0.5 -h mean	< c.pnm >/dev/null 
$CVTOOL dibr -d d.pgm -b 3 -p 0.2  --zps 0.1 -h near	< c.pnm >/dev/null 
$CVTOOL dibr -d d.pgm -b 4 -p 0.5  --zps 0.0 -h far	< c.pnm >/dev/null 

cmd_tests_cleanup
