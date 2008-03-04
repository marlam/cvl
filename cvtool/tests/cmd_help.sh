#!/usr/bin/env bash

. $CVTOOL_TESTS_COMMON

cmd_tests_init

$CVTOOL help 		2> help.txt 	
$CVTOOL help help 	2> helphelp.txt	

cmd_tests_cleanup
