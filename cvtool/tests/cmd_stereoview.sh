#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -t rgb  -w 2 -h 1 > stereo.pnm 

$CVTOOL stereoview anaglyph --color=on  -g red-cyan  < stereo.pnm > /dev/null 
$CVTOOL stereoview anaglyph --color=off -g red-green < stereo.pnm > /dev/null 
$CVTOOL stereoview anaglyph --color=0   -g red-blue  < stereo.pnm > /dev/null 

$CVTOOL stereoview 3d-display -f lr			< stereo.pnm > /dev/null 
$CVTOOL stereoview 3d-display -f tb -w 100 -h 100	< stereo.pnm > /dev/null 
$CVTOOL stereoview 3d-display -f ci			< stereo.pnm > /dev/null 
$CVTOOL stereoview 3d-display -f ri			< stereo.pnm > /dev/null 

cmd_tests_cleanup
