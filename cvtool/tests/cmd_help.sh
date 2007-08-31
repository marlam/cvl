#!/usr/bin/env bash

. cmd_tests_common.sh

$CVTOOL help 		2> help.txt 	
$CVTOOL help help 	2> helphelp.txt	

cmd_tests_cleanup
