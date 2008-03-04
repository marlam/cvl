#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL create -w 10 -h 10 -c red   > r.pnm 
$CVTOOL create -w 10 -h 10 -c green > g.pnm 
$CVTOOL create -w 10 -h 10 -c blue  > b.pnm 
$CVTOOL combine r.pnm g.pnm b.pnm > rgb.pnm 
$CVTOOL create -w 20 -h 20 -c red   > R.pnm 
$CVTOOL create -w 20 -h 20 -c green > G.pnm 
$CVTOOL create -w 20 -h 20 -c blue  > B.pnm 
$CVTOOL combine R.pnm G.pnm B.pnm > RGB.pnm 

$CVTOOL scale -h 20 -i none < rgb.pnm > x1RGB.pnm 
cmp RGB.pnm x1RGB.pnm 

$CVTOOL scale -w 60 -i none < rgb.pnm > x2RGB.pnm 
cmp RGB.pnm x2RGB.pnm 

$CVTOOL scale -f 2.0 -i none < rgb.pnm > x3RGB.pnm 
cmp RGB.pnm x3RGB.pnm 

$CVTOOL scale -x 2.0 -y 1.0 -i none < rgb.pnm | $CVTOOL scale -x 1.0 -y 2.0 -i none > x4RGB.pnm 
cmp RGB.pnm x4RGB.pnm 

cmd_tests_cleanup
