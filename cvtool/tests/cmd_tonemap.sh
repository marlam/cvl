#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL create -w 10 -h 10 -c red   > r.pnm 

$CVTOOL tonemap -m durand02 --sigma-spatial=0.3 --sigma-luminance=0.4 --base-contrast=3 < r.pnm > /dev/null
$CVTOOL tonemap -m drago03 --max-display-luminance=200.0 --bias=0.84 < r.pnm > /dev/null

cmd_tests_cleanup
