#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 20 -h 20 -c black > black.pnm		
$CVTOOL create -w 20 -h 20 -c red   > red.pnm		
$CVTOOL create -w 10 -h 10 -c red   > smallred.pnm	

$CVTOOL resize -w 10 -h 10 < red.pnm > xsmallred.pnm 
cmp smallred.pnm xsmallred.pnm 

$CVTOOL resize -w 20 -h 20 -c red < smallred.pnm > xred.pnm 
cmp red.pnm xred.pnm 

$CVTOOL resize -w 20 -h 20 -c black -x -100 -y 100 < smallred.pnm > xblack.pnm 
cmp black.pnm xblack.pnm 

cmd_tests_cleanup
